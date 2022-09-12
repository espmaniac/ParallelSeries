#pragma once

#include <string>
#include <stack>

class ParallelSeries {
public:
	ParallelSeries();
	~ParallelSeries();

	double solve();

	std::string expr; // expression

	double (*onParallel)(double, double);
	double (*onSeries)(double, double);

private:

	double primary();

	void getToken();

	void error(std::string);
	
	// lexer token types
	enum {
		NUMBER = 1,
		SERIES = '+', PARALLEL = 2,
		LP = '(', RP = ')'
	};

	struct Lexer {
		size_t index;
		struct Token {
			uint8_t type;
			std::string value;
		} token;
		bool fail;
		bool busy;
	} lexer;
};