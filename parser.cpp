#include "parser.hpp"
#include <stdexcept>

using namespace std;


vector<string_view> splitString(string_view str, string_view delim)
{
	vector<string_view> words;

	// Удаляем лишние разделители из начала строки
	str.remove_prefix(str.find_first_not_of(delim));

	while (!str.empty())
	{
		auto endWord = str.find(delim);
		words.push_back(str.substr(0, endWord));
		str.remove_prefix(endWord == str.npos ? str.size() : endWord + delim.size());
	}

	return words;
}

std::vector<std::string_view> splitLongStringForOut(std::string_view str)
{
	std::vector<std::string_view> res;

	while (!str.empty())
	{
		if(str.size() > WIDTH_OUT_STRING)
		{
			auto endStr = str.find_last_of(' ', WIDTH_OUT_STRING);
			res.push_back(str.substr(0, endStr));
			str.remove_prefix(
				endStr == str.npos ? str.size() : endStr + 1
			);
		}
		else
		{
			res.push_back(move(str));
			break;
		}
	}

	return res;
}

CommandWithArgs parseCommandWithArgs(const string& str)
{
	if (str.empty())
	{
		return CommandWithArgs{};
	}

	CommandWithArgs res;
	string_view tmp(str);

	auto endComm = tmp.find(' ');
	auto comm = tmp.substr(0, endComm); // Выделяем команду

	auto convertedComm = convertCommand.find(comm);
	if (convertedComm == convertCommand.end())
	{
		res.first = Command::Error;
		return res;
	}
	res.first = convertedComm->second;

	// Если команда без аргументов
	if (endComm == tmp.npos) 
	{
		return res;
	}

	tmp.remove_prefix(endComm + 1);
	
	// Переменная обозначает, является ли первое слово строки - флагом
	bool firstFlag = false; 
	
	// Если самое первое слово большой строки - флаг, нужно это запомнить
	if (tmp[0] == '-' && tmp[1] == '-')
	{
		firstFlag = true;
	}

	// Разбиваем строку на подстроки вида "ФЛАГ АРГУМЕНТ".
	auto args = splitString(move(tmp), "--"); 

	for (auto& arg : args)
	{
		// Если есть флаг в начале, отделяем его от аргумента и заносим в res
		if (firstFlag)
		{
			auto endFlag = arg.find(' ');
			auto flag = arg.substr(0, endFlag); // получаем флаг
			arg.remove_prefix(endFlag == arg.npos ? arg.size() : endFlag + 1); // убираем флаг из строки

			auto convertedArg = convertArg.find(flag); // Получаем соответствующее значение перечисления
			if (convertedArg == convertArg.end()) // Если такого флага нет, то выдаём ошибку и заканчиваем работу
			{
				res.second.push_back({ FlagsArg::Error, move(flag) });
				return res;
			}
			res.second.push_back({ convertedArg->second, move(arg)}); // сохраняем флаг и его значение
		}
		// Если флага в начале нет, то сохраняем строку с флагом Default
		else
		{
			res.second.push_back({ FlagsArg::Default, move(arg) });
		}
		// Если в начале общей строки флага не было, но функцию splitString выше разбила строку на несколько,
		// значит, флаги есть дальше и нужно их правильно обработать.
		firstFlag = true; 
	}

	return res;
}