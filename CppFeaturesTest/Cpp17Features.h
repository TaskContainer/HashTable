#pragma once

/*
� VS2017 ���������� ��������� ������������ ������ ���������� � ������������ ��� std::experimental::
�� ��������� C++17 ��� ������ ���������� � ������������ ��� std::
��������, ���������� <filesystem> ��������� � std::experimental::filesystem, � �� � std::filesystem::
*/

// �� ������ ������ (���� � ������� ��� ������) ������ ������������ std::experimental::
namespace std::experimental {}

// ������������� ���������� std::experimental � std::
namespace std { using namespace experimental; }

// ������, ��������, ���������� <filesystem> ����� ���������� � std::filesystem::