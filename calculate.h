#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <complex>
#include "variable.h"
using namespace std;

complex <double> expression();
complex <double> primary();								// числа и скобки
complex <double> term();									// *, /, %

int factorial(double num);

complex <double> statement(bool c);

complex <double> get_value(string s);
void set_value(string s, complex <double> d, bool c);
bool is_declared(string var);

complex <double> define_name(string var, complex <double> val, bool c = false);
complex <double> declaration(bool c = false);
complex <double> statement(bool c = false);

void clean_up_mess();

void calculate();
void error(string s);
