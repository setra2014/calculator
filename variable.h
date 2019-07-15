#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <complex>
#include "constName.h"

using namespace std;
using namespace ConstName;

template <typename T> class Variable {
public:
	string name;
	T value;
	bool what_const = false;
	Variable(string name, T val, bool c) { this->name = name; this->value = val; this->what_const = c; }
};
