#include "PasswordManager.hpp"
#include "parser.hpp"

#include <iostream>
#include <iterator>

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

Record PasswordManager::getRecordByName(std::string_view name) const
{
	auto keyAndRecord = data.find(name);
	if (keyAndRecord == end(data))
	{
		return Record{};
	}

	currRec = keyAndRecord->second;
	return currRec;

}

Record PasswordManager::getRecordByNumber(size_t number) const
{
	if (number >= data.size())
	{
		return Record{};
	}

	size_t half = data.size() / 2;

	currRec = number > half ? 
		std::prev(end(data), number - half)->second : std::next(begin(data), number)->second;

	return currRec;
}

size_t PasswordManager::numRecords() const
{
	return data.size();
}

void PasswordManager::AddRecord(std::string name, std::string login, std::string password, std::string description)
{
	if (name.empty()) return;
	
	Record rec{ move(name), move(login), move(password), move(description) };
	AddRecord(std::move(rec));
}

void PasswordManager::AddRecord(Record rec)
{
	data.insert({ std::string_view(rec.name), std::move(rec) });
}
