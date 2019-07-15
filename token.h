#pragma once

template <typename T>
class Token {
public:
	char kind;
	T value;
	string name;
	bool c;
	Token() {}

	Token(char ch) : kind{ ch } {}

	Token(char ch, T val) : kind{ ch }, value{ val } {}

	Token(char ch, string name, bool c) : kind{ ch }, name{ name }, c{ c } {}
};
