#pragma once
#include <iostream>
#include <string>
#include <filesystem>
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
	std::filesystem::path wayToStorages;
	std::map<std::string, Record> data;

public:
	explicit PasswordManager(std::filesystem::path wayToStorage);

	bool AddRecord(
		const std::string& name,
		const std::string& login,
		const std::string& password,
		const std::string& description = "");
	bool AddRecord(const Record& rec);
	//bool AddRecord(Record rec);

	std::vector<std::string> getNames() const;
	std::map<std::string, Record> getRecords() const;

	void deleteRecordByName(const std::string& name);
	void deleteRecordByNumber(size_t number);

	Record& getRecordByName(const std::string& name) const;
	Record& getRecordByNumber(size_t number) const;

	Record& getRecord(const std::string& input) const;	

	void clearData();
};

