#include "PasswordManager.hpp"
#include "parser.hpp"
#include "Tests.hpp"

#include <iostream>
#include <string>
#include <iterator>
#include <sstream>
#include <iomanip>
#include <set>
#include <future>
#include <optional>
#include <charconv>
#include <conio.h>
#include <Windows.h>

using namespace std;

void showHelp();
void showNameSavedRecords(const PasswordManager& ps);
void getRecord(PasswordManager& ps, const vector<Arg>& args);
void addNewRecord(PasswordManager& ps, const vector<Arg>& args);
void deleteRecord(PasswordManager& ps, const vector<Arg>& args);

// Нужна для того, чтобы скрывать символы ключа звёздочками.
string getMasterKey();

int main()
{
	PasswordManager ps;
	std::string masterKey;

	{
		auto fut = std::async(&PasswordManager::readDataFromFile, &ps, "testStorage");

		masterKey = getMasterKey();

		fut.get();
	}

	while (ps.decryptData(move(masterKey)) == 1)
	{
		cout << "Wrong master key. Try again.\n";
		masterKey = getMasterKey();
	}

	showNameSavedRecords(ps);

	cout << "\n" << "Write help to get information about the commands.\n";

	string commandWithArgs;
	bool end = false;
	while (!end)
	{
		cout << "\nEnter command:\n";
		getline(cin, commandWithArgs);
		if (commandWithArgs.empty())
		{
			continue;
		}
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
			if (args.empty())
			{
				showNameSavedRecords(ps);
			}
			else
			{
				getRecord(ps, args);
			}
			break;
		case Command::End:
			end = true;
			break;
		default:
			cout << "Undefine command. Try again.\n";
			break;
		}
	}

	return 0;
}

void showHelp()
{
}

void showNameSavedRecords(const PasswordManager& ps)
{
	auto names = ps.getNames();
	if (names.empty())
	{
		cout << "There are no saved records.\n";
		return;
	}

	cout << "Saved records:\n";
	for (size_t i = 0; i < names.size(); ++i)
	{
		std::cout << i + 1 << ". " << names[i] << ";\n";
	}
}

bool isRightArgsForAdd(const vector<Arg>& args)
{
	if (args.empty() || args.size() > 4)
	{
		cout << "Invalid number of arguments;\n";
		return false;
	}

	bool nameCorrect = false;
	for (const auto& [flag, val] : args)
	{
		if (flag != FlagsArg::Name && flag != FlagsArg::Login && flag != FlagsArg::Pass && flag != FlagsArg::Des)
		{
			cout << "Incorrect argument flag;\n";
			return false;
		}
		if (flag == FlagsArg::Name && val != "")
		{
			nameCorrect = true;
		}
	}

	if (!nameCorrect)
	{
		cout << "There must be a --name flag with some value;\n";
		return false;
	}

	return true;
}

Record recordFromArgs(vector<Arg> args)
{
	if (!isRightArgsForAdd(args))
	{
		return Record{};
	}

	Record resRec;

	for (auto& [flag, val] : args)
	{
		val.remove_prefix(val.find_first_not_of(' '));
		val.remove_suffix(val.size() - 1 - val.find_last_not_of(' '));
		switch (flag)
		{
		case FlagsArg::Name:
			resRec.name = move(val);
			break;
		case FlagsArg::Login:
			resRec.login = move(val);
			break;
		case FlagsArg::Pass:
			resRec.password = move(val);
			break;
		case FlagsArg::Des:
			resRec.description = move(val);
			break;
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

string getMasterKey()
{
	string masterKey;
	cout << "Enter master key: ";
	char ch = 0;               // Переменная для символа.
	while (true)                // Создание бесконечного цикла.
	{
		ch = static_cast<char>(_getch());        // Помещаем код нажатой клавиши в переменную.
		if (ch == 13)         // Установка Enter на прерывание цикла.
		{
			break;         // Прерывание цикла.
		}
		if (ch == 27)   // Установка Esc на закрытие консоли.
		{
			exit(0);      // Выход из консоли.
		}
		if (ch == 8)     // Установка Backspace на удаление символов.
		{
			/*Если строка pass не является пустой, то из неё
			можно удалять  последний символ (Иначе закрывалась консоль.)*/
			if (!masterKey.empty())
			{
				/*
				Смещение курсора на одну позицию  в лево вывод пробела и
				снова смещение курсора влево, то есть при нажатии Backspace
				символ будет стираться, а курсор перемещается.
				*/
				cout << (char)8 << ' ' << char(8);
				masterKey.pop_back();
			}
		}
		else
		{
			cout << '*';
			masterKey += ch;
		}
	}
	cout << "\n";
	return masterKey;
}