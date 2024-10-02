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
	Record& currRec;

public:
	explicit PasswordManager(std::filesystem::path wayToStorage);

	bool AddRecord(
		const std::string& name,
		const std::string& login,
		const std::string& password,
		const std::string& description = "");
	bool AddRecord(const Record& rec);
	//bool AddRecord(Record rec);

	void deleteRecordByName(const std::string& name);
	void deleteRecordByNumber(size_t number);

	void changeNameRecord(std::string&& newName);
	void changeLoginRecord(std::string&& newLogin);
	void changePasswordRecord(std::string&& newPass);
	void changeDescriptionRecord(std::string&& newDes);

	std::vector<std::string> getNames() const;
	const Record& getRecordByName(const std::string& name) const;
	const Record& getRecordByNumber(size_t number) const;

	size_t numRecords() const;

	void clearData();
};

