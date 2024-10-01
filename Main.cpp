#include "PasswordManager.hpp"

#include <iostream>
#include <string>
#include <iterator>
#include <sstream>
#include <iomanip>

using namespace std;

enum class TypeCommand{
	Add, Delete, Show, Change, Help
};

enum class TypeArgs{
	Name, Login, Password, Description, Number
};

using Arg = pair<TypeArgs, string>;
using CommandWithArgs = pair<TypeCommand, vector<Arg>>;

vector<string_view> splitWords(string_view str);
vector<string_view> splitLongStringForOut(string_view str);
CommandWithArgs parseCommandWithArgs(const string& str);

void showHelp();
void showNameSavedRecords(const PasswordManager& ps);
void showRecord(PasswordManager& ps, const vector<Arg>& args);
void addNewRecord(PasswordManager& ps, const vector<Arg>& args);
void deleteRecord(PasswordManager& ps, const vector<Arg>& args);

int main()
{
	PasswordManager ps("testStorage");

	showNameSavedRecords(ps);

	cout << "\n" << "Write help to get information about the commands.\nEnter command:\n";

	string commandWithArgs;
	while (getline(cin, commandWithArgs))
	{
		auto [comm, args] = parseCommandWithArgs(commandWithArgs);

		switch (comm)
		{
		case TypeCommand::Help:
			showHelp();
			break;
		case TypeCommand::Add:
			addNewRecord(ps, args);
			break;
		case TypeCommand::Delete:
			deleteRecord(ps, args);
			break;
		case TypeCommand::Show:
			showRecord(ps, args);
			break;
		default:
			break;
		}
	}

	return 0;
}

void showNameSavedRecords(const PasswordManager& ps)
{
	auto names = ps.getNames();
	for (size_t i = 0; i < names.size(); ++i)
	{
		std::cout << i << ". " << names[i] << "\n";
	}
}