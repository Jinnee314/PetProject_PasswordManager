#include "PasswordManager.hpp"
#include "parser.hpp"

#include <iostream>
#include <iterator>
#include <cryptopp/modes.h>
#include <cryptopp/base64.h>

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

void PasswordManager::decrypt()
{
	using namespace CryptoPP;

	std::string openText;

	//�������������� ������ fileData � ���������� ��������� � ������ openText
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
		data.erase(currRec);
		currRec = end(data);
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

size_t PasswordManager::numRecords() const
{
	return data.size();
}

void PasswordManager::clearData()
{
	data.clear();
	currRec = data.begin();
}
