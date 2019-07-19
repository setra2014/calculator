#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <complex>
#include "constName.h"
#include "variable.h"
#include "token_stream.h"
#include "token.h"
#include <typeinfo>

using namespace std;
using namespace ConstName;

complex<double> get_value(string s, vector<Variable<complex<double>>> &var_table) {							//возвращает значение переменной
	for (const Variable <complex<double>> &v : var_table)
		if (v.name == s) return v.value;
	error("get: неопределенная переменая " + s);
}
void set_value(string s, complex<double> d, bool c, vector<Variable<complex<double>>> &var_table) {					//создает переменную
	for (Variable <complex<double>> &v : var_table)
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

int factorial(int num) {
	int factorial = 1;
	if (num == 0) return 1;
	for (int i = 1; i <= num; i++) factorial = factorial*i;
	return factorial;

}
template <typename T> T expression(Token_stream <T> &ts, vector<Variable<complex<double>>> &var_table);
template <> complex<double> expression(Token_stream <complex<double>> &ts, vector<Variable<complex<double>>> &var_table);
template <> complex<int> expression(Token_stream <complex<int>> &ts, vector<Variable<complex<double>>> &var_table);

template <typename T> T primary(Token_stream <T> &ts, vector<Variable<complex<double>>> &var_table) {								// числа, скобки, переменные, унарные операторы
	Token <T> t = ts.get();
	switch (t.kind)
	{
	case '{':
	{
		T d = expression<T>(ts, var_table);
		t = ts.get();
		if (t.kind != '}') {
			error("требуется '}'");
		}
		return d;
	}
	case '(':
	{
		T d = expression<T>(ts, var_table);
		t = ts.get();
		if (t.kind != ')') {
			error("требуется ')'");
		}
		return d;
	}
	case number: {
		return t.value;
	}
	case '-':
		return -primary<T>(ts, var_table);
	case '+':
		return primary<T>(ts, var_table);
	default:
		break;
	}
}
template <> complex<int> primary(Token_stream <complex<int>> &ts, vector<Variable<complex<double>>> &var_table) {								// числа, скобки, переменные, унарные операторы
	Token <complex<int>> t = ts.get<complex<int>>();
	switch (t.kind)
	{
	case '{':
	{
		complex<int> d = expression<complex<int>>(ts, var_table);
		t = ts.get<complex<int>>();
		if (t.kind != '}') {
			error("требуется '}'");
		}
		return d;
	}
	case '(':
	{
		complex<int> d = expression<complex<int>>(ts, var_table);
		t = ts.get<complex<int>>();
		if (t.kind != ')') {
			error("требуется ')'");
		}
		return d;
	}
	case number: {
		return t.value;
	}
	case image: {
		Token_stream <int> td;
		int im = primary<int>(td, var_table);
		int re = t.value.real();
		complex <int> NewT(re, im);
		return NewT;
	}
	case '-':
		return -primary<complex<int>>(ts, var_table);
	case '+':
		return primary<complex<int>>(ts, var_table);
	default:
		break;
	}
}

template <> complex<double> primary(Token_stream <complex<double>> &ts, vector<Variable<complex<double>>> &var_table) {								// числа, скобки, переменные, унарные операторы
	Token <complex<double>> t = ts.get<complex<double>>();
	switch (t.kind)
	{
	case '{':
	{
		complex<double> d = expression<complex<double>>(ts, var_table);
		t = ts.get<complex<double>>();
		if (t.kind != '}') {
			error("требуется '}'");
		}
		return d;
	}
	case '(':
	{
		complex<double> d = expression<complex<double>>(ts, var_table);
		t = ts.get<complex<double>>();
		if (t.kind != ')') {
			error("требуется ')'");
		}
		return d;
	}
	case number: {
		return t.value;
	}
	case image: {
		Token_stream <double> td;
		double im = primary<double>(td, var_table);
		double re = t.value.real();
		complex <double> NewT(re, im);
		return NewT;
	}
	case '-':
		return -primary<complex<double>>(ts, var_table);
	case '+':
		return primary<complex<double>>(ts, var_table);
	case 'a':
		return get_value(t.name, var_table);
	default:
		break;
	}
}


template <typename T> T term(Token_stream <T> &ts, vector<Variable<complex<double>>> &var_table) {									// *, /, %
	T left = primary<T>(ts, var_table);
	Token <T> t = ts.get();
	while (true) {
		switch (t.kind) {
		case '*':
			left = left * primary<T>(ts, var_table);
			t = ts.get();
			break;
		case '/':
		{
			T d = primary<T>(ts, var_table);
			if (d == 0) error("Деление на нуль");
			left = left / d;
			t = ts.get();
			break;
		}
		case '%':
		{
			T d = primary<T>(ts, var_table);
			if (d == 0) error("%: деление на нуль или комплексное число");
			left = fmod(left, d);
			t = ts.get();
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

template <> complex<int> term(Token_stream <complex<int>> &ts, vector<Variable<complex<double>>> &var_table) {									// *, /, %
	complex<int> left = primary<complex<int>>(ts, var_table);
	Token <complex<int>> t = ts.get<complex<int>>();
	while (true) {
		switch (t.kind) {
		case '*':
			left = left * primary<complex<int>>(ts, var_table);
			t = ts.get<complex<int>>();
			break;
		case '/':
		{
			complex<int> d = primary<complex<int>>(ts, var_table);
			if (d.real() == 0 && d.imag() == 0) error("Деление на нуль");
			left = left / d;
			t = ts.get<complex<int>>();
			break;
		}
		case '%':
		{
			error("Остаток от деления на комплексное число не определен");
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

template <> complex<double> term(Token_stream <complex<double>> &ts, vector<Variable<complex<double>>> &var_table) {									// *, /, %
	complex<double> left = primary<complex<double>>(ts, var_table);
	Token <complex<double>> t = ts.get<complex<double>>();
	while (true) {
		switch (t.kind) {
		case '*':
			left = left * primary<complex<double>>(ts, var_table);
			t = ts.get<complex<double>>();
			break;
		case '/':
		{
			complex<double> d = primary<complex<double>>(ts, var_table);
			if (d.real() == 0 && d.imag() == 0) error("Деление на нуль");
			left = left / d;
			t = ts.get<complex<double>>();
			break;
		}
		case '%':
		{
			error("Остаток от деления на комплексное число не определен");
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

template <typename T> T expression(Token_stream <T> &ts, vector<Variable<complex<double>>> &var_table) {									// +, -, !
	T left = term<T>(ts, var_table);
	Token <T> t = ts.get();
	while (true) {
		switch (t.kind) {
		case '+':
			left = left + term<T>(ts, var_table);
			t = ts.get();
			break;
		case '-':
			left = left - term<T>(ts, var_table);
			t = ts.get();
			break;
		case '!': {
			string type = typeid(left).name();
			string t = "int";
			if(type == t) return factorial(left);
			else error("Факториал неопределен");
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}
template <> complex<int> expression(Token_stream <complex<int>> &ts, vector<Variable<complex<double>>> &var_table) {									// +, -, !
	complex<int> left = term<complex<int>>(ts, var_table);
	Token <complex<int>> t = ts.get<complex<int>>();
	while (true) {
		switch (t.kind) {
		case '+':
			left = left + term<complex<int>>(ts, var_table);
			t = ts.get<complex<int>>();
			break;
		case '-':
			left = left - term<complex<int>>(ts, var_table);
			t = ts.get<complex<int>>();
			break;
		case '!': {
			error("Факториал комплексного числа неопределен!");
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}
template <> complex<double> expression(Token_stream <complex<double>> &ts, vector<Variable<complex<double>>> &var_table) {									// +, -, !
	complex<double> left = term<complex<double>>(ts, var_table);
	Token <complex<double>> t = ts.get<complex<double>>();
	while (true) {
		switch (t.kind) {
		case '+':
			left = left + term<complex<double>>(ts, var_table);
			t = ts.get<complex<double>>();
			break;
		case '-':
			left = left - term<complex<double>>(ts, var_table);
			t = ts.get<complex<double>>();
			break;
		case '!': {
			error("Факториал комплексного числа неопределен!");
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

template <typename T> void clean_up_mess(Token_stream <T> &ts) {
	ts.ignore(print);
}

bool is_declared(string var, vector<Variable<complex<double>>> var_table) {								//проверка существования переменной
	for (const Variable <complex<double>> &v : var_table)
		if (v.name == var) return true;
	return false;
}
complex<double> define_name(string var, complex<double> val, bool c, vector<Variable<complex<double>>> &var_table) {		//создание переменной
	if (is_declared(var, var_table)) {
		set_value(var, val, c, var_table);
	}
	else {
		Variable <complex <double>> v(var, val, c);
		var_table.push_back(v);
	}
	return val;
}
complex<double> declaration(bool c, Token_stream <complex<double>> &ts, vector<Variable<complex<double>>> &var_table) {
	Token <complex<double>> t = ts.get<complex<double>>();
	if (t.kind != name) error("в объявлении ожидается имя переменной");
	string var_name = t.name;

	Token <complex<double>> t2 = ts.get<complex<double>>();
	if (t2.kind != '=') error("пропущен символ = в объявлении " + var_name);

	complex<double> d = expression(ts, var_table);
	define_name(var_name, d, c, var_table);
	return d;
}

template <typename T> T statement(bool c, Token_stream <T> &ts, vector<Variable<complex<double>>> &var_table) {							//ключевые слова
	Token <T> t = ts.get();
	switch (t.kind) {
	default:
		ts.putback(t);
		return expression<T>(ts, var_table);
	}
}
template <> complex<double> statement(bool c, Token_stream <complex<double>> &ts, vector<Variable<complex<double>>> &var_table) {							//ключевые слова
	Token <complex<double>> t = ts.get<complex<double>>();
	switch (t.kind) {
	case let:
		return declaration(c, ts, var_table);
	case cons:
		return statement(true, ts, var_table);
	case name:
	{
		char c;
		cin >> c;
		switch (c)
		{
		case '=':
		{
			string n = t.name;
			complex<double> x = statement<complex<double>>(false, ts, var_table);
			define_name(n, x, t.c, var_table);
			return x;
		}
		default:
			cin.putback(c);
			break;
		}

	}
	default:
		ts.putback(t);
		return expression<complex<double>>(ts, var_table);
	}
}

int analysis() {
	char a;
	cin >> a;
	if (a == '.') {
		int an = analysis();
		if (an == 2 || an == 3) {
			cin.putback(a);
			return 3;
		}
		else {
			cin.putback(a);
			return 1;
		}
	}
	else if (a == image) {
		int an = analysis();
		if (an == 1 || an == 3) {
			cin.putback(a);
			return 3;
		}
		else {
			cin.putback(a);
			return 2;
		}
	}
	else if (isalpha(a) && a != image) {
		cin.putback(a);
		return 3;
	}
	else if (a == ';') {
		cin.putback(a);
		return 0;
	}
	else {
		int i = analysis();
		cin.putback(a);
		return i;
	}
}

void calculate() {
	vector<Variable<complex<double>>> var_table;
	Token_stream <double> ts;
	while (cin)
		try {
		cout << prompt;
		int an = analysis();
		Token <double> t = ts.get<double>();
		while (t.kind == print) t = ts.get<double>();
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
		if (t.kind == image) cin.putback(t.kind);
		if(t.kind == name) cin.putback(t.name[0]);
		
		switch (an)
		{
		case 0:
		{
			Token_stream <int> tsint;
			Token<int> tint(t.kind, (int)t.value);
			tsint.putback(tint);
			int i = statement<int>(false, tsint, var_table);
			cout << result << i << endl;
			break;
		}
		case 1:
		{
			Token_stream <double> tsdouble;
			tsdouble.putback(t);
			double d = statement<double>(false, tsdouble, var_table);
			cout << result << d << endl;
			break;
		}
		case 2:
		{
			Token_stream <int> tsint;
			Token <int> ti(t.kind, (int)t.value);
			complex<int> dd((int)t.value, 0);
			if (t.kind == image) { 
				int im = primary<int>(tsint, var_table); 
				complex<int> dr(0, im); 
				dd = dr; 
			}

			Token_stream <complex<int>> tscd;
			Token <complex<int>> td(ti.kind, dd);
			tscd.putback(td);
			complex <int> cd = statement<complex<int>>(false, tscd, var_table);
			cout << result << cd.real() << " + i(" << cd.imag() << ')' << endl;
			break;
		}
		case 3:
		{
			Token_stream <complex<double>> tscd;
			if (!isalpha(t.kind)) {
				complex<double> d(t.value);
				Token<complex<double>>td(t.kind, d);
				tscd.putback(td);
			}
			complex<double> dd(0, 0);

			Token<complex<double>> td1 = tscd.get<complex<double>>();
			tscd.putback(td1);
			complex <double> cd = statement<complex<double>>(false, tscd, var_table);
			cout << result << cd.real() << " + i(" << cd.imag() << ')' << endl;
			break;
}
		default:
			break;
		}
	}
	catch (exception &e) {
		cerr << e.what() << endl;
		clean_up_mess(ts);
	}
}
