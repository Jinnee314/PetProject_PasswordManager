#include "PasswordManager.hpp"
#include "parser.hpp"

#include <iostream>
#include <string>
#include <iterator>
#include <sstream>
#include <iomanip>

using namespace std;

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
		case Command::Help:
			showHelp();
			break;
		case Command::Add:
			addNewRecord(ps, args);
			break;
		case Command::Del:
			deleteRecord(ps, args);
			break;
		case Command::Show:
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