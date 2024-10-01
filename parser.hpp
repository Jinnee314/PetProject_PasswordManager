#pragma once
#include <utility>
#include <string>
#include <vector>
#include <string_view>
#include <map>

enum class Command {
	Add, Del, Show, Change, Help, Error
};

enum class FlagsArg {
	Name, Login, Pass, Des, Num, Default, Error
};

using Arg = std::pair<FlagsArg, std::string_view>;
using CommandWithArgs = std::pair<Command, std::vector<Arg>>;

const std::map<std::string_view, Command> convertCommand
{
	{"add", Command::Add},
	{"del", Command::Del},
	{"show", Command::Show},
	{"change", Command::Change},
	{"help", Command::Help}
};

const std::map<std::string_view, FlagsArg> convertArg
{
	{"--name", FlagsArg::Name},
	{"--login", FlagsArg::Login},
	{"--pass", FlagsArg::Pass},
	{"--des", FlagsArg::Des},
	{"--num", FlagsArg::Num}
};

std::vector<std::string_view> splitWords(std::string_view str);

// ������� ��� ��������� ������� ������ ���, ����� � ����� ���� ������
// ������� � ������� �� ������ �� ����������� ������� ������ (� ����� ������ 75)
std::vector<std::string_view> splitLongStringForOut(std::string_view str);

// �������, ������� ������ ������ � �������� � ����������� ��� ����� ������� ������.
// ������� ���� - ��������� ����������, � ���������� � �������� ���������� ���������
// �������������� �������.
CommandWithArgs parseCommandWithArgs(const std::string& str);