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
	cout << "Добро пожаловать в программу 'калькулятор'! Введите 'h', чтобы вызвать справку" << endl;
	try {
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
