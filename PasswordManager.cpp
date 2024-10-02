#include "PasswordManager.hpp"
#include "parser.hpp"

#include <iostream>

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

	for (const auto& [key,  value] : data)
	{
		res.push_back(key);
	}

	return res;
}

size_t PasswordManager::numRecords() const
{
	return data.size();
}
