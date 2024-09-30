#pragma once
#include <string>
#include <filesystem>
#include <map>

struct Record
{
	std::string name, login, password, description;
};

class PasswordManager
{
private:
	std::filesystem::path wayToStorage;
	std::map<std::string, Record> data;

public:
	PasswordManager(std::filesystem::path wayToStorage);

	void AddRecord(
		const std::string& name,
		const std::string& login,
		const std::string& password,
		const std::string& description = "");

	void showRecords();

	void deleteRecordByName(const std::string& name);
	void deleteRecordByNumber(size_t number);

	void getRecordByName(const std::string& name);
	void getRecordByNumber(size_t number);

	void getRecord(const std::string& input);	
};

