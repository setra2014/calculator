#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

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

class complex {
private:
	double re;
	double im;
public:
	complex(double re = 0, double im = 0) { this->re = re; this->im = im; }
	double get_re() { return this->re; }
	double get_im() { return this->im; }
	void set_re(double v) { re = v; }
	void set_im(double v) { im = v; }
	complex &operator+(complex value) {
		this->re += value.re;
		this->im += value.im;
		return *this;
	}
	complex &operator-(complex value) {
		this->re -= value.re;
		this->im -= value.im;
		return *this;
	}
	complex &operator*(complex value) {
		double a = this->re * value.re - this->im * value.im;
		this->im = this->im * value.re + this->re * value.im;
		this->re = a;
		return *this;
	}
	complex &operator/(complex value) {
		if (value.re != 0 && value.im != 0) error("Операция деления на комплексное число не определена");
		else if(value.re != 0) {
			this->re /= value.re;
			this->im /= value.re;
			return *this;
		}
		else if (value.im != 0) {
			double a = this->im / value.im;
			this->im = -this->re / value.im;
			this->re = a;
			return *this;
		}
	}
	complex &operator-() {
		this->re *= -1;
		this->im *= -1;
		return *this;
	}
	friend std::ostream &operator<<(std::ostream &out, complex v);
};

std::ostream &operator<<(std::ostream &out, complex v) {
	out << v.get_re() << ' ' << '+' << ' ' << 'i' << v.get_im();
	return out;
}

class Token {
public:
	char kind;
	complex value;
	string name;
	bool c;
	Token() {}
	
	Token(char ch) : kind{ ch } {}

	Token(char ch, complex val) : kind{ ch }, value{ val } {}

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
		complex val(c, 0);
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

Token_stream ts;

complex expression();

int factorial(complex num) {
	if (num.get_im() == 0) {
		int factorial = 1;
		if (num.get_re() == 0) return 1;
		int x = (int)num.get_re();
		for (int i = 1; i <= x; i++) factorial = factorial*i;
		return factorial;
	}
	else error("Факториал комплексного числа неопределен");
}

complex statement(bool c = false);

class Variable {
public:
	string name;
	complex value;
	bool what_const = false;
	Variable(string name, complex val, bool c) { this->name = name; this->value = val; this->what_const = c; }
};

vector<Variable> var_table;

complex get_value(string s) {
	for (const Variable &v : var_table)
		if (v.name == s) return v.value;
	error("get: неопределенная переменая " + s);
}

void set_value(string s, complex d, bool c) {
	for (Variable &v : var_table)
		if (v.name == s) {
			if (v.what_const) {
				error("set: нельзя переопределять константу");
			}
			v.what_const = c;
			v.value = v.value + c;
			cout << s << " = " << v.value << endl;
			return;
		}
	error("set: неопределенная переменная" + s);
}

complex primary() {								// числа и скобки
	Token t = ts.get();
	switch (t.kind)
	{
	case '{':
	{
		complex d = expression();
		t = ts.get();
		if (t.kind != '}') {
			error("требуется '}'");
		}
		return d;
	}
	case '(':
	{
		complex d = expression();
		t = ts.get();
		if (t.kind != ')') {
			error("требуется ')'");
		}
		return d;
	}
	case image:
		t.value.set_im(primary().get_re());
		return t.value;
	case number:
		char c;
		cin >> c; 
		if (c == '!') return factorial(t.value);
		cin.putback(c);
		return t.value;
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

complex term() {									// *, /, %
	complex left = primary();
	Token t = ts.get();
	while (true) {
		switch (t.kind) {
		case '*':
			left = left * primary();
			t = ts.get();
			break;
		case '/':
		{
			complex d = primary();
			if (d.get_re() == 0 && d.get_im() == 0) error("Деление на нуль");
			left = left / d;
			t = ts.get();
			break;
		}
		case '%':
		{
			complex d = primary();
			if (d.get_im() !=0 && d.get_re() == 0 && left.get_im() != 0 && left.get_re() == 0) error("%: деление на нуль или комплексное число");
			left.set_re(fmod(left.get_re(), d.get_re()));
			t = ts.get();
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

complex expression() {							// + или -
	complex left = term();
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
		default:
			ts.putback(t);
			return left;
		}
	}
}


bool is_declared(string var) {
	for (const Variable &v : var_table)
		if (v.name == var) return true;
	return false;
}
complex define_name(string var, complex val, bool c = false) {
	if (is_declared(var)) {
		set_value(var, val, c);
	}
	else var_table.push_back(Variable(var, val, c));
	return val;
}

complex declaration(bool c = false) {
	Token t = ts.get();
	if (t.kind != name) error("в объявлении ожидается имя переменной");
	string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("пропущен символ = в объявлении " + var_name);

	complex d = expression();
	define_name(var_name, d, c);
	return d;
}

complex statement(bool c) {
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
			complex x = statement();
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
