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

int main() {
	setlocale(LC_ALL, "Russian");
	double val = 0;
	cout << "Добро пожаловать в программу 'калькулятор'! Введите 'h', чтобы вызвать справку" << endl;
	try {
		define_name("pi", 3.1415926535, true);
		define_name("e", 2.7182818284, true);

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
