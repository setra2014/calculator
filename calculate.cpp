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

Token_stream ts;
vector<Variable> var_table;

void error(string s) {
	throw runtime_error(s);
}

int factorial(double num) {
	int factorial = 1;
	if (num == 0) return 1;
	int x = (int)num;
	for (int i = 1; i <= x; i++) factorial = factorial*i;
	return factorial;

}

complex <double> get_value(string s) {							//возвращает значение переменной
	for (const Variable &v : var_table)
		if (v.name == s) return v.value;
	error("get: неопределенная переменая " + s);
}

void set_value(string s, complex <double> d, bool c) {					//создает переменную
	for (Variable &v : var_table)
		if (v.name == s) {
			if (v.what_const) {
				error("set: нельзя переопределять константу");
			}
			v.what_const = c;
			v.value = d;
			cout << s << " = " << v.value << endl;
			return;
		}
	error("set: неопределенная переменная" + s);
}

complex <double> primary() {								// числа, скобки, переменные, унарные операторы
	Token t = ts.get();
	switch (t.kind)
	{
	case '{':
	{
		complex <double> d = expression();
		t = ts.get();
		if (t.kind != '}') {
			error("требуется '}'");
		}
		return d;
	}
	case '(':
	{
		complex <double> d = expression();
		t = ts.get();
		if (t.kind != ')') {
			error("требуется ')'");
		}
		return d;
	}
	case image: {
		double im = primary().real();
		double re = t.value.real();
		complex <double> NewT(re, im);
		return NewT;
	}
	case number: {
		return t.value;
	}
	case '-':
		return -primary();
	case '+':
		return primary();
	case 'a':
		return get_value(t.name);
	default:
		break;
	}
}

complex <double> term() {									// *, /, %
	complex <double> left = primary();
	Token t = ts.get();
	while (true) {
		switch (t.kind) {
		case '*':
			left = left * primary();
			t = ts.get();
			break;
		case '/':
		{
			complex <double> d = primary();
			if (d.real() == 0 && d.imag() == 0) error("Деление на нуль");
			left = left / d;
			t = ts.get();
			break;
		}
		case '%':
		{
			complex <double> d = primary();
			if (d.imag() != 0 && d.real() == 0 && left.imag() != 0 && left.real() == 0) error("%: деление на нуль или комплексное число");
			left = fmod(left.real(), d.real());
			t = ts.get();
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

complex <double> expression() {									// +, -, !
	complex <double> left = term();
	Token t = ts.get();
	while (true) {
		switch (t.kind) {
		case '+':
			left = left + term();
			t = ts.get();
			break;
		case '-':
			left = left - term();
			t = ts.get();
			break;
		case '!': {
			if (left.imag() == 0) return factorial(left.real());
			else {
				error("Факториал комплексного числа неопределен");
			}
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}


bool is_declared(string var) {								//проверка существования переменной
	for (const Variable &v : var_table)
		if (v.name == var) return true;
	return false;
}
complex <double> define_name(string var, complex <double> val, bool c) {		//создание переменной
	if (is_declared(var)) {
		set_value(var, val, c);
	}
	else var_table.push_back(Variable(var, val, c));
	return val;
}

complex <double> declaration(bool c) {
	Token t = ts.get();
	if (t.kind != name) error("в объявлении ожидается имя переменной");
	string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("пропущен символ = в объявлении " + var_name);

	complex <double> d = expression();
	define_name(var_name, d, c);
	return d;
}

complex <double> statement(bool c) {							//ключевые слова
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration(c);
	case cons:
		return statement(true);
	case name:
	{
		char c;
		cin >> c;
		switch (c)
		{
		case '=':
		{
			complex <double> x = statement();
			define_name(t.name, x, t.c);
			return x;
		}
		default:
			cin.putback(c);
			break;
		}

	}
	default:
		ts.putback(t);
		return expression();
	}
}


void clean_up_mess() {
	ts.ignore(print);
}

void calculate() {
	while (cin)
		try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == help) {
			cout << "q - выход" << endl
				<< "h - помошь" << endl
				<< "Калькулятор поддерживает операторы суммы (+), разности (-),\nпроизведения (*), деления (/), остатка от деления (%),\nвычисления факториала (!), работу со скобками () и {}" << endl
				<< "Имеется возможность создавать собственные переменные, используя ключевое слово 'let' " << endl
				<< "Для указания того, что переменная является константой, использовать ключевое слово 'const'" << endl
				<< "Для изменения значений переменных использовать оператор присваивания (=)" << endl;
			continue;
		}
		if (t.kind == quit) return;
		ts.putback(t);
		complex <double> s = statement();
		cout << result << s.real();
		if (s.imag() != 0) cout << " + i(" << s.imag() << ")";
		cout << endl;
	}
	catch (exception &e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}
