#include "parser.hpp"
#include <stdexcept>

using namespace std;


vector<string_view> splitString(string_view str, char delim)
{
	vector<string_view> words;
	str.remove_prefix(str.find_first_not_of(delim));

	while (!str.empty())
	{
		auto endWord = str.find(delim);
		words.push_back(str.substr(0, endWord));
		str.remove_prefix(endWord == str.npos ? str.size() : endWord);
	}

	return words;
}

// ‘ункци€, котора€ парсит строку с командой и аргументами дл€ более удобной работы.
// √лавна€ цель - правильно распарсить, а логичность и верность аргументов провер€ют
// обрабатывающие функции.
CommandWithArgs parseCommandWithArgs(const string& str)
{
	auto tokens = splitString(str, ' ');

	const auto& comm = tokens[0];
	CommandWithArgs res;

	// чтобы не т€нуть дальше итератор, который нам не нужен
	{
		auto itConv = convertCommand.find(comm);
		if (itConv == convertCommand.end())
		{
			res.first = Command::Error;
			return res;
		}
		res.first = itConv->second;
	}

	if (tokens.size() == 1)
	{
		return res;
	}

	bool pairFlagAndArg = false;
	for (size_t i = 1; i < tokens.size(); i++)
	{
		if (tokens.size() > 1 && tokens[i][0] == '-' && tokens[i][1] == '-') // обработка флага
		{
			auto itConv = convertArg.find(tokens[i]);
			if (itConv == convertArg.end())
			{
				res.second.push_back({ FlagsArg::Error, move(tokens[i])});
				return res;
			}
			res.second.push_back({ itConv->second, "" });
			pairFlagAndArg = true;
		}
		else if (pairFlagAndArg) // если до был флаг, а сейчас не флаг, значит, это аргумент флага (в силу реализации)
		{
			pairFlagAndArg = false;
			res.second.back().second = move(tokens[i]);
		}
		else // если до не было флага, значит это просто аргумент
		{
			res.second.push_back({ FlagsArg::Default, move(tokens[i]) });
		}		
	}

	return res;
}