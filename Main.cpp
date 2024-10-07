#include "PasswordManager.hpp"
#include "parser.hpp"

#include <iostream>
#include <string>
#include <iterator>
#include <sstream>
#include <iomanip>
#include <set>
#include <future>
#include <optional>
#include <charconv>
#include <Windows.h>

using namespace std;

void showHelp();
void showNameSavedRecords(const PasswordManager& ps);
void getRecord(PasswordManager& ps, const vector<Arg>& args);
void addNewRecord(PasswordManager& ps, const vector<Arg>& args);
void deleteRecord(PasswordManager& ps, const vector<Arg>& args);

int main()
{
	PasswordManager ps;
	std::string masterKey;

	{
		auto fut = std::async(&PasswordManager::readDataFromFile, &ps, "testStorage");

		cout << "Enter master key: ";

		HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
		DWORD mode = 0;
		GetConsoleMode(hStdin, &mode);
		SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));

		cin >> masterKey;

		fut.get();
	}

	ps.decryptData(move(masterKey));

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
			getRecord(ps, args);
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
		ps.addRecord(move(newRec));
	}
}

std::optional<size_t> to_size_t(const std::string_view& input)
{
	size_t out;
	const std::from_chars_result result = std::from_chars(input.data(), input.data() + input.size(), out);
	if (result.ec == std::errc::invalid_argument || result.ec == std::errc::result_out_of_range)
	{
		return std::nullopt;
	}
	return out;
}

void changeRecordsFields(PasswordManager& ps, const vector<Arg>& args)
{
	if (args.empty())
	{
		cout << "Error. There are not enough arguments.\n"
			<< "Try \"change --flag new_value\"\n";
		return;
	}

	for (const auto& [type, val] : args)
	{
		switch (type)
		{
		case FlagsArg::Name:
			ps.changeNameRecord(string{ val });
			break;
		case FlagsArg::Login:
			ps.changeLoginRecord(string{ val });
			break;
		case FlagsArg::Pass:
			ps.changePasswordRecord(string{ val });
			break;
		case FlagsArg::Des:
			ps.changeDescriptionRecord(string{ val });
			break;
		}
	}
}

void showRecordInf(PasswordManager& ps, const vector<Arg>& args)
{
	if (args.empty())
	{
		cout << "Login: " << ps.getCurrentRecordLogin() << "\n";
		cout << "Password: " << ps.getCurrentRecordPassword() << "\n";
		return;
	}

	if (args[0].first == FlagsArg::All)
	{
		cout << "Name: " << ps.getCurrentRecordName() << "\n"
			<< "Login: " << ps.getCurrentRecordLogin() << "\n"
			<< "Password: " << ps.getCurrentRecordPassword() << "\n"
			<< "Description: " << ps.getCurrentRecordDescription() << "\n";
		return;
	}
	if (args[0].first == FlagsArg::Login)
	{
		cout << "Login: " << ps.getCurrentRecordLogin() << "\n";
		return;
	}
	if (args[0].first == FlagsArg::Pass)
	{
		cout << "Password: " << ps.getCurrentRecordPassword() << "\n";
	}
}

void getRecord(PasswordManager& ps, const vector<Arg>& args)
{
	if (args[0].first != FlagsArg::Num && args[0].first != FlagsArg::Default && args[0].first != FlagsArg::Name)
	{
		cout << "Invalid arguments type. Try again.\n";
		return;
	}

	Record curr;
	if (args[0].first == FlagsArg::Num)
	{
		auto number = to_size_t(args[0].second);
		if (!number || number.value() - 1 > ps.numRecords() || number.value() - 1 < 0)
		{
			cout << "Invalid argument. Try again.\n";
			return;
		}
		curr = ps.getRecordByNumber(number.value() - 1);
		if (curr == Record{})
		{
			cout << "There is no record with this number.\n";
			return;
		}
	}
	else
	{
		curr = ps.getRecordByName(string{ args[0].second });
		if (curr == Record{})
		{
			cout << "There is no record with that name.\n";
			return;
		}
	}

	cout << curr;

	string commandWithArgs;
	bool end = false;
	while (!end)
	{
		cout << "\nEnter command for record:\n";
		getline(cin, commandWithArgs);
		auto [comm, commArgs] = parseCommandWithArgs(commandWithArgs);

		switch (comm)
		{
		case Command::Change:
			changeRecordsFields(ps, commArgs);
			break;
		case Command::Del:
			ps.deleteCurrent();
			end = true;
			break;
		case Command::Show:
			showRecordInf(ps, commArgs);
			break;
		case Command::End:
			end = true;
			break;
		default:
			cout << "Undefine command. Try again.\n";
			break;
		}
	}
}

void deleteRecord(PasswordManager& ps, const vector<Arg>& args)
{
	if (args.empty())
	{
		cout << "Error. There are not enough arguments.\n" 
			<< "Try \"delete --name records_name\" or \"delete --num records_number\"\n";
		return;
	}
	if (args[0].first != FlagsArg::Num && args[0].first != FlagsArg::Default && args[0].first != FlagsArg::Name)
	{
		cout << "Invalid arguments type. Try again.\n";
		return;
	}
	if (args[0].first == FlagsArg::Num)
	{
		auto number = to_size_t(args[0].second);
		if (!number || number.value() - 1 > ps.numRecords() || number.value() - 1 < 0)
		{
			cout << "Invalid argument. Try again.\n";
			return;
		}
		ps.deleteRecordByNumber(number.value() - 1);
	}

	ps.deleteRecordByName(string{ args[0].second });
}
