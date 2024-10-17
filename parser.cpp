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

// �������, ������� ������ ������ � �������� � ����������� ��� ����� ������� ������.
// ������� ���� - ��������� ����������, � ���������� � �������� ���������� ���������
// �������������� �������.
CommandWithArgs parseCommandWithArgs(const string& str)
{
	auto tokens = splitString(str, ' ');

	const auto& comm = tokens[0];
	CommandWithArgs res;

	// ����� �� ������ ������ ��������, ������� ��� �� �����
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
		if (tokens.size() > 1 && tokens[i][0] == '-' && tokens[i][1] == '-') // ��������� �����
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
		else if (pairFlagAndArg) // ���� �� ��� ����, � ������ �� ����, ������, ��� �������� ����� (� ���� ����������)
		{
			pairFlagAndArg = false;
			res.second.back().second = move(tokens[i]);
		}
		else // ���� �� �� ���� �����, ������ ��� ������ ��������
		{
			res.second.push_back({ FlagsArg::Default, move(tokens[i]) });
		}		
	}

	return res;
}