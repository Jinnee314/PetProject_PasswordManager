#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <string_view>
#include <map>

struct Record
{
	std::string name, login, password, description;
};

bool operator==(const Record& l, const Record& r);

bool operator!=(const Record& l, const Record& r);

std::ostream& operator<<(std::ostream& out, const Record& rec);

class PasswordManager
{
private:
	using DataStorage = std::map<std::string, Record>;
	using DataIter = DataStorage::iterator;

	std::filesystem::path wayToStorages;
	DataStorage data;
	DataIter currRec;

	// Получение итератора по номеру.
	// Проверка номера на выход за пределы размера 
	// возлагаются на вызывающую функцию.
	DataIter getIterByNumber(size_t number);

public:
	explicit PasswordManager(std::filesystem::path wayToStorage);
	~PasswordManager();

	void addRecord(
		std::string name,
		std::string login,
		std::string password,
		std::string description = "");
	void addRecord(Record rec);

	void deleteRecordByName(std::string name);
	void deleteRecordByNumber(size_t number);

	void changeNameRecord(std::string newName);
	void changeLoginRecord(std::string newLogin);
	void changePasswordRecord(std::string newPass);
	void changeDescriptionRecord(std::string newDes);

	std::vector<std::string_view> getNames() const;
	Record getRecordByName(std::string name);
	Record getRecordByNumber(size_t number);

	size_t numRecords() const;

	void clearData();
};

