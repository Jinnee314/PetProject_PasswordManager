#include "PasswordManager.hpp"
#include "parser.hpp"

#include <iterator>
#include <fstream>
#include <algorithm>

#include <cryptopp/modes.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>

bool operator==(const Record& l, const Record& r)
{
	return tie(l.name, l.login, l.password, l.description) == tie(r.name, r.login, r.password, r.description);
}

bool operator!=(const Record& l, const Record& r)
{
	return tie(l.name, l.login, l.password, l.description) != tie(r.name, r.login, r.password, r.description);
}

std::ostream& operator<<(std::ostream& out, const Record& rec)
{
	out << "Name: " << rec.name << "\n"
		<< "Description:\n";
	for (const auto& str : splitLongStringForOut(rec.description))
	{
		out << str << '\n';
	}

	return out;
}

PasswordManager::DataIter PasswordManager::getIterByNumber(size_t number)
{
	size_t half = data.size() / 2;
	return number > half ?
		std::prev(end(data), data.size() - number) : std::next(begin(data), number);
}

void PasswordManager::createKeyAndIv(std::string masterKey)
{
	using namespace CryptoPP;

	std::string hash;
	SHA384 sha384;

	//вычисление хэш значения и запись его в строку hash
	StringSource ss(
		masterKey, true,
		new HashFilter(
			sha384,
			new StringSink(hash)
		)
	);

	masterKey.clear();
	
	std::copy(next(begin(hash), aesKey.size()), end(hash), begin(cbcIv));
	hash.resize(aesKey.size());
	std::copy(begin(hash), end(hash), begin(aesKey));
}

void PasswordManager::createStringFromData()
{
	fileData.clear();

	for (auto& [key, rec] : data)
	{
		fileData += std::move(rec.name) + " " + std::move(rec.login) + " " + std::move(rec.password) + " " + std::move(rec.description) + ";";
	}
}

void PasswordManager::createDataFromString()
{
	auto records = splitString(fileData, ';');

	for (auto& record : records)
	{
		auto parts = splitString(record, ' ');
		data[std::string{ parts[0] }] = {
			std::string( std::move(parts[0]) ),
			std::string( std::move(parts[1]) ),
			std::string( std::move(parts[2]) ),
			parts.size() == 4 ? std::string( std::move(parts[3]) ) : ""
		};
	}
}

void PasswordManager::writeDataInFile()
{
	std::ofstream out(fileStorage);

	if (!out.is_open())
	{
		throw std::runtime_error("File not open");
	}

	out << fileData;
	out.close();
}

void PasswordManager::decrypt()
{
	using namespace CryptoPP;

	std::string openText;

	//Расшифровываем строку fileData и записываем результат в строку openText
	CBC_Mode<AES>::Decryption decryptAES(aesKey.data(), aesKey.size(), cbcIv.data());
	StringSource ss(
		fileData, true,
		new StreamTransformationFilter(
			decryptAES,
			new StringSink(openText)
		)
	);

	fileData = std::move(openText);
}

void PasswordManager::encrypt()
{
	using namespace CryptoPP;

	std::string cipher;

	//Зашифровываем строку fileData и записываем результат в строку cipher
	CBC_Mode<AES>::Encryption encrypt(aesKey.data(), aesKey.size(), cbcIv.data());
	StringSource ss(
		fileData, true,
		new StreamTransformationFilter(
			encrypt,
			new StringSink(cipher)
		)
	);

	fileData = std::move(cipher);
}

PasswordManager::~PasswordManager()
{
	if(!data.empty())
	{
		createStringFromData();

		encrypt();

		writeDataInFile();
	}
	else
	{
		std::ofstream out(fileStorage);
		out.close();
	}
}

void PasswordManager::readDataFromFile(std::filesystem::path file)
{
	fileStorage = std::move(file);

	if (std::filesystem::file_size(fileStorage) == 0)
	{
		return;
	}

	std::ifstream in(fileStorage);

	if (!in.is_open())
	{
		throw std::runtime_error("File not open");
	}


	for (std::string line; std::getline(in, line); )
	{
		fileData += line;
	}

	in.close();
}

void PasswordManager::decryptData(std::string masterKey)
{
	createKeyAndIv(std::move(masterKey));

	decrypt();

	createDataFromString();
}

void PasswordManager::addRecord(std::string name, std::string login, std::string password, std::string description)
{
	if (name.empty()) return;

	Record rec{ move(name), move(login), move(password), move(description) };
	addRecord(std::move(rec));
}

void PasswordManager::addRecord(Record rec)
{
	data.emplace(std::string_view(rec.name), std::move(rec));
}

void PasswordManager::deleteRecordByName(std::string name)
{
	if (name.empty())
	{
		return;
	}
	if (name == currRec->first)
	{
		deleteCurrent();
		return;
	}
	data.erase(name);
}

void PasswordManager::deleteRecordByNumber(size_t number)
{
	if (number >= data.size())
	{
		return;
	}

	data.erase(getIterByNumber(number));
}

void PasswordManager::deleteCurrent()
{
	data.erase(currRec);
	currRec = end(data);
}

void PasswordManager::changeNameRecord(std::string newName)
{
	if (newName.empty())
	{
		return;
	}
	auto tmp = data.extract(currRec);
	tmp.key() = newName;
	currRec = data.insert(std::move(tmp)).position;
	currRec->second.name = std::move(newName);
}

void PasswordManager::changeLoginRecord(std::string newLogin)
{
	currRec->second.login = std::move(newLogin);
}

void PasswordManager::changePasswordRecord(std::string newPass)
{
	currRec->second.password = std::move(newPass);
}

void PasswordManager::changeDescriptionRecord(std::string newDes)
{
	currRec->second.description = std::move(newDes);
}

std::vector<std::string_view> PasswordManager::getNames() const
{
	std::vector<std::string_view> res;
	res.reserve(data.size());

	for (const auto& [key, value] : data)
	{
		res.push_back(key);
	}

	return res;
}

Record PasswordManager::getRecordByName(std::string name)
{
	currRec = data.find(name);
	if (currRec == end(data))
	{
		return Record{};
	}

	return currRec->second;
}

Record PasswordManager::getRecordByNumber(size_t number)
{
	if (number >= data.size())
	{
		return Record{};
	}
	currRec = getIterByNumber(number);

	return currRec->second;
}

std::string PasswordManager::getCurrentRecordName()
{
	return currRec->first;
}

std::string PasswordManager::getCurrentRecordLogin()
{
	return currRec->second.login;
}

std::string PasswordManager::getCurrentRecordPassword()
{
	return currRec->second.password;
}

std::string PasswordManager::getCurrentRecordDescription()
{
	return currRec->second.description;
}

size_t PasswordManager::numRecords() const
{
	return data.size();
}

void PasswordManager::clearData()
{
	data.clear();
	currRec = data.begin();
}
