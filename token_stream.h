#pragma once
#include "token.h"

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
