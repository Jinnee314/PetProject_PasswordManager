#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <string_view>
#include <map>
#include <array>
#include <optional>

#include <cryptopp/aes.h>

struct Record
{
	std::string name, login, password, description;
};


// ��� ���������� ���������� ��� ���������� ������ 
// ���� ������.
bool operator==(const Record& l, const Record& r);
bool operator!=(const Record& l, const Record& r);

// ���������� ��������� ������ ��� ��������� Record.
// ������� ������ ��� � ��������. �������� �� ���� 
// WIDTH_OUT_STRING (�� ����� parser.hpp).
std::ostream& operator<<(std::ostream& out, const Record& rec);

// ������ ������ ��������
const size_t WIDTH_OUT_STRING = 75;

// ������� ������ des � ������ �� ������� WIDTH_OUT_STRING (75)
void outDescription(std::ostream& out, std::string_view des);

class PasswordManager
{
private:
	using DataStorage = std::map<std::string, Record>;
	using DataIter = DataStorage::iterator;
	
	// ���� �� �����-���������. � ������ ���� ��������� ��� ������ ����� ���������
	// ������ � ����������. ����� ��� �������� � ����������� ���������� ���������.
	std::filesystem::path fileStorage;

	// ��������� �������. ������ ��������� ����� ����� ������.
	DataStorage data;
	
	// ������� ������. ��������� ������, ������� ������ �������� 
	// getRecordByName, getRecordByNumber. ������������ ��� ������� 
	// ������ � ������������ ��� �������.
	DataIter currRec;

	// ������, � ������� ������������ ������ �� ����� � �� �������
	// ������ ������������ � ����.
	std::string fileData;

	// ���� ���������� � ������ �������������
	std::array<unsigned char, CryptoPP::AES::MAX_KEYLENGTH> aesKey;
	std::array<unsigned char, CryptoPP::AES::BLOCKSIZE> cbcIv;

	// ��� ������ ����� ����������� � ������� SHA256
	std::string hashMasterKey;

	// ����, ��������� � ���, ��� ���������� ������ ���� ��� �����.
	bool rightMasterKey;

	// ��������� ��������� �� ������.
	// �������� ������ �� ����� �� ������� ������� ����������� �� ���������� �������.
	DataIter getIterByNumber(size_t number);

	// ��������� �� ������ ����� ��� �������� � ������� SHA384 � �� ���� ��������
	// ���� ���������� (������ 256 ��� ����) � ������ ������������� (���������� 128 ��� ����).
	void createKeyAndIv(std::string masterKey);

	// �� ������ fileData ��������� ������� ������� data.
	// � ������ ������ ��������� �������� ';', � ���� ����� ������ ��������� ���������.
	void createStringFromData();

	// �� ������� data ������ ������ � ��������� ��������� � fileData.
	// � ������ ������ ��������� �������� ';', � ���� ����� ������ ��������� ���������.
	void createDataFromString();

	// ���������� ������ fileData � ���� fileStorage, �������� � �������� ������.
	void writeDataInFile();


	// ��������������/������������� ������ �� ������ fileData � ������� AES256
	// � ���������� ��������� � �� ��.
	void decrypt();
	void encrypt();

	// ��������� ��� �� ������ � ������� SHA256 � ���������� ���������
	std::string getHashSHA256(const std::string& str);

	// ���������, �� ��������� �� ����� ������������. �������� �������� �� ���,
	// ��� � ������ ������������� ������ ������� ������ ������������� � �������� �������
	bool checkMasterKey(const std::string& masterKey);

public:
	PasswordManager() = default;

	// � ����������� ������������� ������ � ���������� �� � ���� fileStorage.
	// ���� data ����� ��-�� ����, ��� ������������ ������ ��� ������, ������� ����
	// �� ������ �������.
	// ���� data ����� ��-�� ����, ��� �� ��� ����� ���������� ������ ����, �� ������
	// �� ������.
	~PasswordManager();

	// ���������� ������ �� �����. �������� � � ��������� ������ 
	// ��-�� ����, ��� ���������� ����� ������ ����������� �����������
	// ����, ��� ������������ ������ ������ ������.
	// ��������� ������ � ������ fileData.
	// ��������� ���� �� ����� � storageFile.
	// ���� ���� ����, �� ������ ��������� ���� �� ����� � storageFile.
	void readDataFromFile(std::filesystem::path file);

	// ��������� ������������ ����������� �����. ���� ���� �� ���������� ���������� 1.
	// ���� ���� ���� �, �������������, �������� ����� ���� �������, �� ��������� �� 
	// ����� ��� c ������� SHA256 � ��������� � hashMasterKey.
	// ��������� ���� ���������� � ������ �������������. �������������� ������, 
	// ���������� �� �����, � ��������� �� ��� ������� ������� data. ����������� 
	// currRec �������� data.end()
	// ���� ������ fileData �����, �� ������ ��������� ���� ���������� � ������ �������������.
	// ��� ���������� ��������� ���������� 0.
	int decryptData(std::string masterKey);


	 /*
		������ ���������� ������.
	 */
	
	// ������������ �����, � ��� �������� �����, � ����� ��������� ��
	// ������������. ������ ������ �� ��������� ����� � �������� ������
	// ������� ���������� ������.
	void addRecord(
		std::string name,
		std::string login,
		std::string password,
		std::string description = "");

	// ���������� ������ � ������� data.
	bool addRecord(Record rec);


	/*
		������ �������� �������.	
	*/
	
	// ������� ������ �� �����.
	// ���� ������ � ����� ����� ���, ������ �� ������.
	void deleteRecordByName(std::string name);

	// ������� ������ �� ������.
	// ���� ������ � ����� ����� ���, ������ �� ������
	void deleteRecordByNumber(size_t number);

	// ������� ������ �� currRec.
	// ����� �������� currRec = data.end()
	void deleteCurrent();


	/*
		������ ��������� ����� ������� ������.
	*/

	// ��� ��������� ����� ������ �� ������ ������ ����������
	// ������� � ����� ������
	void changeNameRecord(std::string newName);
	void changeLoginRecord(std::string newLogin);
	void changePasswordRecord(std::string newPass);
	void changeDescriptionRecord(std::string newDes);


	/*
		������ ��������� ����������.
	*/

	// ���������� ��� ����� ������� �� data.
	std::vector<std::string_view> getNames() const;

	// ��������� ������ �� �����. ���� ������ � ����� ���, ����������
	// ������ ������.
	std::optional<Record> getRecordByName(std::string name);

	// ��������� ������ �� ������. ���� ������ ��� ����� ������� ���, 
	// ���������� ������ ������.
	std::optional<Record> getRecordByNumber(size_t number);
	
	const std::string& getCurrentRecordName() const;
	const std::string& getCurrentRecordLogin() const;
	const std::string& getCurrentRecordPassword() const;
	const std::string& getCurrentRecordDescription() const;


	// ����� ��������� �������� ���������� �������
	size_t numRecords() const;

	// ����� ������� ������� data.
	void clearData();
};