#pragma once
#include <utility>
#include <string>
#include <vector>
#include <string_view>
#include <unordered_map>

enum class Command {
	Add, Del, Show, Change, Help, Save, End, Error
};

enum class FlagsArg {
	Name, Login, Pass, Des, Num, Default, All, Error
};

using Arg = std::pair<FlagsArg, std::string_view>;
using CommandWithArgs = std::pair<Command, std::vector<Arg>>;

// ������� ��� �������� ����� � �������� ������������ Command
const std::unordered_map<std::string_view, Command> convertCommand
{
	{"add", Command::Add},
	{"del", Command::Del},
	{"delete", Command::Del},
	{"show", Command::Show},
	{"change", Command::Change},
	{"help", Command::Help},
	{"end", Command::End},
	{"exit", Command::End},
	{"save", Command::Save}
};

// ������� ��� �������� ����� � �������� ������������ FlagsArg.
const std::unordered_map<std::string_view, FlagsArg> convertArg
{
	{"name", FlagsArg::Name},
	{"login", FlagsArg::Login},
	{"pass", FlagsArg::Pass},
	{"des", FlagsArg::Des},
	{"num", FlagsArg::Num},
	{"all", FlagsArg::All}
};

// ������� ��������� ������ �� ��������� � ������������ delim.
std::vector<std::string_view> splitString(std::string_view str, std::string_view delim);

// �������, ������� ������ ������ � �������� � ����������� ��� ����� ������� ������.
// ������� ���� - ��������� ����������, � ���������� � �������� ���������� ���������
// �������������� �������.
CommandWithArgs parseCommandWithArgs(const std::string& str);