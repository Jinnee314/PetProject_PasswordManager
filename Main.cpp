#include "PasswordManager.hpp"
#include "parser.hpp"

#include <iostream>
#include <string>
#include <iterator>
#include <sstream>
#include <iomanip>
#include <set>

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

// Проверка корректности аргументов для создания новой записи
// Для вывода некоторой информации и остановки выполнения использую
// исключения.
void isRightArgsForAdd(const vector<Arg>& args)
{
	if (args.size() > 4)
	{
		throw std::invalid_argument("Invalid number of arguments");
	}

	int checkDefault = 0;
	bool nameCorrect = false;
	for (const auto& [flag, val] : args)
	{
		if (flag == FlagsArg::Error || flag == FlagsArg::Num)
		{
			throw std::invalid_argument("Incorrect argument flag " + string{ val } + ";");
		}
		if (flag == FlagsArg::Default)
		{
			++checkDefault;
		}
		if (flag == FlagsArg::Name && val != "")
		{
			nameCorrect = true;
		}
	}

	if (checkDefault != 0 && args.size() != checkDefault)
	{
		throw std::invalid_argument("Incorrect argument flags;");
	}

	if (checkDefault != 0 && checkDefault < 3)
	{
		throw std::invalid_argument("Incorrect number of arguments;");
	}

	if (!nameCorrect)
	{
		throw std::invalid_argument("There must be a --name flag");
	}
}

Record recordFromArgs(const vector<Arg>& args)
{
	isRightArgsForAdd(args);

	Record resRec;

	if (args[0].first == FlagsArg::Default)
	{
		resRec.name = args[0].second;
		resRec.login = args[1].second;
		resRec.password = args[2].second;
		if (args.size() == 4)
		{
			resRec.description = args[4].second;
		}
	}
	else
	{
		for (size_t i = 0; i < args.size(); ++i)
		{
			switch (args[i].first)
			{
			case FlagsArg::Name:
				resRec.name = args[i].second;
				break;
			case FlagsArg::Login:
				resRec.login = args[i].second;
				break;
			case FlagsArg::Pass:
				resRec.password = args[i].second;
				break;
			case FlagsArg::Des:
				resRec.description = args[i].second;
				break;
			}
		}
	}

	return resRec;
}

void addNewRecord(PasswordManager& ps, const vector<Arg>& args)
{
	Record newRec{ "", "", "", "" };
	bool save = true;
	if (!args.empty())
	{
		try
		{
			newRec = move(recordFromArgs(args));
		}
		catch (const std::exception& e)
		{
			cout << e.what() << "\n";
			return;
		}
	}
	else
	{
		string answer = "";
		bool repeat = true;

		while(repeat)
		{
			while (newRec.name.empty())
			{
				cout << "Enter name: ";
				cin >> newRec.name;
			}
			cout << "\nEnter login: ";
			cin >> newRec.login;
			cout << "\nEnter password: ";
			cin >> newRec.password;
			cout << "\nEnter description: ";
			cin >> newRec.description;

			cout << "New record:\n";
			cout << newRec << "\n";

			answer = "";
			while (answer != "y" && answer != "yes" && answer != "Yes" && answer != "YES"
				&& answer != "n" && answer != "no"  && answer != "No"  && answer != "NO")
			{
				cout << "Save? (y\\n): ";
				cin >> answer;
			}
			save = (answer == "y" || answer == "yes" || answer == "Yes" || answer == "YES");
			if (save)
			{
				repeat = false;
			}
			else
			{
				answer = "";
				while (answer != "y" && answer != "yes" && answer != "Yes" && answer != "YES"
					&& answer != "n" && answer != "no" && answer != "No" && answer != "NO")
				{
					cout << "Repeat? (y\\n): ";
					cin >> answer;
				}
				repeat = (answer == "y" || answer == "yes" || answer == "Yes" || answer == "YES");
			}
		}
	}
	
	if (save)
	{
		ps.AddRecord(move(newRec));
	}
}