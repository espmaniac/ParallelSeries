#pragma once

#include <string>

struct Delta {
	double a, b, c;
};

struct Star {
	double a, b, c;
};

struct METRIC_PREFIX {
	const char *symbol;
	const char *name;
	int8_t exponent; // base 10
};

const METRIC_PREFIX PREFIXES[] = {
	{
		.symbol = "T",
		.name = "tera",
		.exponent = 12
	},
	{
		.symbol = "G",
		.name = "giga",
		.exponent = 9
	},
	{
		.symbol = "M",
		.name = "mega",
		.exponent = 6
	},
	{
		.symbol = "k",
		.name = "kilo",
		.exponent = 3
	},
	{
		.symbol = "m",
		.name = "milli",
		.exponent = -3
	},
	{
		.symbol = "u",
		.name = "micro",
		.exponent = -6
	},
	{
		.symbol = "n",
		.name = "nano",
		.exponent = -9
	},
	{
		.symbol = "p",
		.name = "pico",
		.exponent = -12
	},
};

class ParallelSeries {
public:
	ParallelSeries();
	~ParallelSeries();

	double solve();

	void reset();

	std::string expr; // expression

	double (*onParallel)(double, double);
	double (*onSeries)(double, double);

	Star deltaStarTransform(Delta);

private:

	double primary();

	void getToken();

	void error(std::string);
	
	// lexer token types
	enum {
		CHARS = 0, NUMBER = 1,
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