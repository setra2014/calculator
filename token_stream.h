#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <complex>
#include "calculate.h"
#include "constName.h"
#include "Variable.h"
#include "Token.h"

using namespace std;
using namespace ConstName;
template <typename T>
class Token_stream {
public:
	Token_stream() : full{ false } {}
	template <typename T> Token <T> get() {
		if (full) {
			full = false;
			return buffer;
		}
		char ch;
		cin >> ch;

		switch (ch) {
		case print:
		case quit:
		case help:
		case image:
		case '(': case ')':
		case '{': case '}':
		case '+': case '-':
		case '!':
		case '*': case '/': case '%':
		case '=':
			return Token<T>(ch);
		case '.':
		case 'О': case '1': case '2':
		case '3': case '4': case '5':
		case '6': case '7': case '8':
		case '9':
		{
			cin.putback(ch);
			T c;
			cin >> c;
			return Token<T>(number, c);
		}
		default: {
			if (isalpha(ch)) {
				string s;
				s += ch;
				bool c = false;
				while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s += ch;
				cin.putback(ch);
				if (s == what_const) {
					c = true;
					return Token<T>{ cons };
				}
				if (s == declkey)
					return Token<T>{ let };
				return Token<T>{ name, s, c };
			}
		}
				 error("Неверная лексема");

		}
	}

	template <typename T> void putback(Token <T> t) {
		if (full) error("put back() : буфер заполнен");
		buffer = t;
		full = true;
	}
	template <typename T> void ignore(char c) {
		if (full && c == buffer.kind) {
			full = false;
			return;
		}
		full = false;

		// проверяем входные данные
		char ch;
		while (cin >> ch)
			if (ch == c) return;
	}
	void clear() {
		full = false;
		if(buffer.kind == number)
			cin.putback(buffer.value);
	}
private:
	bool full{ false };
	Token <T> buffer;
};
