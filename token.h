#pragma once
#include <string>
#include <complex>
using namespace std;

class Token {
public:
	char kind;
	complex <double> value;
	string name;
	bool c;
	Token() {}

	Token(char ch) : kind{ ch } {}

	Token(char ch, complex <double> val) : kind{ ch }, value{ val } {}

	Token(char ch, string name, bool c) : kind{ ch }, name{ name }, c{ c } {}
};
