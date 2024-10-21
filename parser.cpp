#include "parser.hpp"
#include <stdexcept>

using namespace std;


vector<string_view> splitString(string_view str, string_view delim)
{
	vector<string_view> words;

	// ������� ������ ����������� �� ������ ������
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
	auto comm = tmp.substr(0, endComm); // �������� �������

	auto convertedComm = convertCommand.find(comm);
	if (convertedComm == convertCommand.end())
	{
		res.first = Command::Error;
		return res;
	}
	res.first = convertedComm->second;

	// ���� ������� ��� ����������
	if (endComm == tmp.npos) 
	{
		return res;
	}

	tmp.remove_prefix(endComm + 1);
	
	// ���������� ����������, �������� �� ������ ����� ������ - ������
	bool firstFlag = false; 
	
	// ���� ����� ������ ����� ������� ������ - ����, ����� ��� ���������
	if (tmp[0] == '-' && tmp[1] == '-')
	{
		firstFlag = true;
	}

	// ��������� ������ �� ��������� ���� "���� ��������".
	auto args = splitString(move(tmp), "--"); 

	for (auto& arg : args)
	{
		// ���� ���� ���� � ������, �������� ��� �� ��������� � ������� � res
		if (firstFlag)
		{
			auto endFlag = arg.find(' ');
			auto flag = arg.substr(0, endFlag); // �������� ����
			arg.remove_prefix(endFlag == arg.npos ? arg.size() : endFlag + 1); // ������� ���� �� ������

			auto convertedArg = convertArg.find(flag); // �������� ��������������� �������� ������������
			if (convertedArg == convertArg.end()) // ���� ������ ����� ���, �� ����� ������ � ����������� ������
			{
				res.second.push_back({ FlagsArg::Error, move(flag) });
				return res;
			}
			res.second.push_back({ convertedArg->second, move(arg)}); // ��������� ���� � ��� ��������
		}
		// ���� ����� � ������ ���, �� ��������� ������ � ������ Default
		else
		{
			res.second.push_back({ FlagsArg::Default, move(arg) });
		}
		// ���� � ������ ����� ������ ����� �� ����, �� ������� splitString ���� ������� ������ �� ���������,
		// ������, ����� ���� ������ � ����� �� ��������� ����������.
		firstFlag = true; 
	}

	return res;
}