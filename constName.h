#pragma once
#include <string>
using namespace std;

namespace ConstName {
	const char number = '8';					// '8'  обозначает число
	const char quit = 'q';						// t.kind == quit означает, что t - лексема выхода
	const char print = ';';						// // t.kind == print означает, что t - лексема печати
	const string prompt = "> ";
	const string result = "= ";					// Используется для указания на то, что далее следует результат
	const char let = 'l';
	const char name = 'a';
	const string declkey = "let";
	const char cons = 'c';
	const string what_const = "const";
	const char help = 'h';
	const char image = 'i';
	void error(string s) {
		throw runtime_error(s);
	}
}
