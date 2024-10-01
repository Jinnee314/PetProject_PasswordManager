#pragma once
#include <utility>
#include <string>
#include <vector>
#include <string_view>
#include <map>

enum class Command {
	Add, Del, Show, Change, Help, Error
};

enum class Flags {
	Name, Login, Pass, Des, Num, Default, Error
};

using Arg = std::pair<Flags, std::string_view>;
using CommandWithArgs = std::pair<Command, std::vector<Arg>>;

const std::map<std::string_view, Command> convertCommand
{
	{"add", Command::Add},
	{"del", Command::Del},
	{"show", Command::Show},
	{"change", Command::Change},
	{"help", Command::Help}
};

const std::map<std::string_view, Flags> convertArg
{
	{"--name", Flags::Name},
	{"--login", Flags::Login},
	{"--pass", Flags::Pass},
	{"--des", Flags::Des},
	{"--num", Flags::Num}
};

std::vector<std::string_view> splitWords(std::string_view str);

// ������� ��� ��������� ������� ������ ���, ����� � ����� ���� ������
// ������� � ������� �� ������ �� ����������� ������� ������ (� ����� ������ 75)
std::vector<std::string_view> splitLongStringForOut(std::string_view str);

// �������, ������� ������ ������ � �������� � ����������� ��� ����� ������� ������.
// ������� ���� - ��������� ����������, � ���������� � �������� ���������� ���������
// �������������� �������.
CommandWithArgs parseCommandWithArgs(const std::string& str);