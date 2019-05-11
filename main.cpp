#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

const char number = '8';					// '8'  обозначает число
const char quit = 'q';						// t.kind == quit означает, что t - лексема выхода
const char print = ';';						// t.kind == print означает, что t - лексема печати
const string prompt = "> "; 					// приглашение
const string result = "= ";					// Используется для указания на то, что далее следует результат
const char let = 'l';						
const string declkey = "let";
const char name = 'a';						// t.kind == number означает, что t - переменная
const char cons = 'c';						// t.kind == what_const означает, что следующая объявленная переменная является константой
const string what_const = "const";				
const char help = 'h';						// t.kind == help означает, что t - лексема вызова справочной информации

void error(string s) {
	throw runtime_error(s);
}

class Token {
public:
	char kind;
	double value;
	string name;
	bool c;
	Token() {}
	
	Token(char ch) : kind{ ch } {}

	Token(char ch, double val) : kind{ ch }, value{ val } {}

	Token(char ch, string name, bool c) : kind{ ch }, name{ name }, c { c } {}
};

class Token_stream {
public:
	Token_stream() : full{ false } {}
	Token get();
	void putback(Token t);
	void ignore(char c);
private:
	bool full{ false };
	Token buffer;
};

void Token_stream::putback(Token t) {
	if (full) error("put back() : буфер заполнен");
	buffer = t;
	full = true;
}

Token Token_stream::get() {
// Чтение символов из cin и составление Token 							
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
	case '(': case ')':
	case '{': case '}':
	case '+': case '-':
	case '!':
	case '*': case '/': case '%':
	case '=':
		return Token{ ch };						// Каждый символ представляет сам себя 

	case '.':								// Число с плавающей точкой может начинаться с точки
	case 'О': case '1': case '2':
	case '3': case '4': case '5':
	case '6': case '7': case '8':
	case '9':
	{
		cin.putback(ch);						// Вернуть цифру во входной поток
		double val;
		cin >> val;							// Считать число с плавающей точкой
		return Token{ number, val };
	}
	default:
		if (isalpha(ch)) {						// Попытка распознать имя переменной или ключевое слово
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

Token_stream ts;

double expression();

int factorial(double num) {
	int factorial = 1;
	if (num == 0) return 1;
	int x = (int)num;
	for (int i = 1; i <= x; i++) factorial = factorial*i;
	return factorial;
}

double statement(bool c = false);

class Variable {								// класс, описывающий переменные
public:
	string name;
	double value;
	bool what_const = false;
	Variable(string name, double val, bool c) { this->name = name; this->value = val; this->what_const = c; }
};

vector<Variable> var_table;

double get_value(string s) {
	for (const Variable &v : var_table)
		if (v.name == s) return v.value;
	error("get: неопределенная переменая " + s);
	return 0;
}
void set_value(string s, double d, bool c) {
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

double primary() {								// числа, скобки, унарные + и -, переменные
	Token t = ts.get();
	switch (t.kind)
	{
	case '{':
	{
		double d = expression();
		t = ts.get();
		if (t.kind != '}') {
			error("требуется '}'");
		}
		return d;
	}
	case '(':
	{
		double d = expression();
		t = ts.get();
		if (t.kind != ')') {
			error("требуется ')'");
		}
		return d;
	}
	case number:
		char c;
		cin >> c;
		if (c == '!') return factorial(t.value);			 //вычисление факториала
		cin.putback(c);
		return t.value;
	case '-':
		return -primary();
	case '+':
		return primary();
	case name:
		return get_value(t.name);
	default:
		break;
	}
	error("primary: неопознанное первичное выражение");
	return 0;
}

double term() {									// *, /, %
	double left = primary();
	Token t = ts.get();
	while (true) {
		switch (t.kind) {
		case '*':
			left *= primary();
			t = ts.get();
			break;
		case '/':
		{
			double d = primary();
			if (d == 0) error("Деление на нуль");
			left /= d;
			t = ts.get();
			break;
		}
		case '%':
		{
			double d = primary();
			if (d == 0) error("%: деление на нуль");
			left = left - d*(int)(left/d);
			t = ts.get();
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}
 
double expression() {							      // + или -
	double left = term();
	Token t = ts.get();
	while (true) {
		switch (t.kind) {
		case '+':
			left += term();
			t = ts.get();
			break;
		case '-':
			left -= term();
			t = ts.get();
			break;
		default:
			ts.putback(t);
			return left;
		}
	}
}


bool is_declared(string var) {							// проверяет, существует ли переменная или нет
	for (const Variable &v : var_table)
		if (v.name == var) return true;
	return false;
}
double define_name(string var, double val, bool c = false) {			// добавление новой переменной в вектор
	if (is_declared(var)) {
		set_value(var, val, c);
	}
	else var_table.push_back(Variable(var, val, c));
	return val;
}

double declaration(bool c = false) {
	Token t = ts.get();
	if (t.kind != name) error("в объявлении ожидается имя переменной");
	string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("пропущен символ = в объявлении " + var_name);

	double d = expression();
	define_name(var_name, d, c);
	return d;
}

double statement(bool c) {
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
			double x = statement();
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

void calculate() {							// цикл вычисления выражения
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
		cout << result << statement() << endl;
	}
	catch (exception &e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}


int main() {
	setlocale(LC_ALL, "Russian");
	double val = 0;
	cout << "Добро пожаловать в программу 'калькулятор'! Введите 'h', чтобы вызвать справку" << endl;
	try {
		define_name("pi", 3.1415926535);
		define_name("e", 2.7182818284);

		calculate();
		system("Pause");
		return 0;
	}
	catch (exception &e) {
		cerr << e.what() << endl;
		cout << "Чтобы закрыть окно, введите символ ~" << endl;
		char ch;
		for (char ch; cin >> ch;)
			if (ch == '~') return 1;
		return 1;
	}
	catch (...) {
		cerr << "исключение" << endl;
		cout << "Чтобы закрыть окно, введите символ ~" << endl;
		char ch;
		for (char ch; cin >> ch;)
			if (ch == '~') return 1;
		return 2;
	}
	return 0;
}
