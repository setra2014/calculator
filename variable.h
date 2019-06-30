#pragma once
#include <iostream>
#include <string>
#include <complex>
using namespace std;

class Variable {
public:
	string name;
	complex <double> value;
	bool what_const = false;
	Variable(string name, complex <double> val, bool c) { this->name = name; this->value = val; this->what_const = c; }
};
