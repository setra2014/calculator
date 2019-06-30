#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <complex>
#include "variable.h"
#include "calculate.h"
#include "constName.h"
#include "token.h"
#include "token_stream.h"

using namespace std;
using namespace ConstName;

void Token_stream::putback(Token t) {
	if (full) error("put back() : буфер заполнен");
	buffer = t;
	full = true;
}

Token Token_stream::get() {
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
		return Token{ ch };

	case '.':								// Число с плавающей точкой может начинаться с точки
	case 'О': case '1': case '2':
	case '3': case '4': case '5':
	case '6': case '7': case '8':
	case '9':
	{
		cin.putback(ch);					// Вернуть цифру во входной поток
		double c;
		cin >> c;							// Считать число с плавающей точкой
		complex <double> val(c, 0);
		return Token{ number, val };
	}
	default:
		if (isalpha(ch)) {
			string s;
			s += ch;
			bool c = false;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s += ch;
			cin.putback(ch);
			if (s == what_const) {
				c = true;
				return Token{ cons };
			}
			if (s == declkey)
				return Token{ let };
			return Token{ name, s, c };
		}
		error("Неверная лексема");
	}
	return Token{ 'e' };
}

void Token_stream::ignore(char c) {
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	// проверяем входные данные
	char ch = 0;
	while (cin >> ch)
		if (ch == c) return;
}
