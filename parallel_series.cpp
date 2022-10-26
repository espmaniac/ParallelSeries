#include <iostream>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <map>
#include "parallel_series.hpp"

static bool isChar(uint8_t key) {
	return (key >= 'a' && key <= 'z' || key >= 'A' && key <= 'Z');
}

static bool isNumber(uint8_t key) {
	return (key >= '0' && key <= '9');
}

static const std::map<std::pair<const char*, const char*>, int8_t> METRIC_PREFIXES = { 
	// character case is not important
	// {SHORT PREFIX, FULL PREFIX}, value
	{{"t", "tera"}, 12},
	{{"g", "giga"}, 9},
	{{"m", "mega"}, 6},
	{{"k", "kilo"}, 3},
	{{"m", "milli"}, -3},
	{{"u", "micro"}, -6},
	{{"n", "nano"}, -9},
	{{"p", "pico"}, -12}
};

ParallelSeries::ParallelSeries() : expr(""), onSeries(nullptr), onParallel(nullptr) {
	reset();
}

ParallelSeries::~ParallelSeries() {}

double ParallelSeries::primary() {
	getToken();

	switch (lexer.token.type) {
		case LP: {
			double parens = solve();
			if (lexer.token.type != RP)
				error(") expected");
			getToken();	
			return parens;
		}

		case NUMBER: {
			Lexer save = lexer;
			double result = strtod(lexer.token.value.c_str(), nullptr);
			
			getToken();

			if (lexer.token.type == CHARS) {
				int8_t value = 0;
				for (const auto &prefix : METRIC_PREFIXES) {
					if ( //  Compare Strings without Case Sensitivity
						strcasecmp(lexer.token.value.c_str(), prefix.first.first) == 0
						||
						strcasecmp(lexer.token.value.c_str(), prefix.first.second) == 0) {
						value = prefix.second;
						break;
					}
				}
				if (value != 0)
					result *= pow(10, value);
				else
					error("unknown metric");

			} else 
				lexer = save;
			
			return result;
		}

		case '-': { // unary
			return -primary();
		}

		case '+': { // unary
			return primary();
		}

		default: {
			lexer.busy = true;
			error("primary expected");
			return 0;
		}

	}
}

double ParallelSeries::solve() {
	double left = primary();
	getToken();

	for(;!lexer.fail;) {
		switch(lexer.token.type) {
			case SERIES: {
				if (onSeries == nullptr) {
					error("onSeries = nullptr");
					return 0;
				}
				left = onSeries(left, solve());
				getToken();
				break;
			}
			case PARALLEL: {
				if (onParallel == nullptr) {
					error("onParallel = nullptr");
					return 0;
				}
				left = onParallel(left, primary());
				getToken();
				break;
			}

			default: {
				lexer.busy = true;
				return left;
			}
		}
	}
	return 0;
}

void ParallelSeries::reset() {
	lexer.index = 0;
	lexer.token.type = -1;
	lexer.token.value = "";
	lexer.busy = false;
	lexer.fail = false;
}

void ParallelSeries::getToken() {
	if (lexer.busy || lexer.fail) {
		lexer.busy = false;
		return;
	}

	lexer.token.value.clear();
	lexer.token.type = -1;
	
	for (uint8_t c = expr[lexer.index]; lexer.index < expr.size(); c = expr[++lexer.index]) {

		if (c == '\0' || c == '\n' || c == '\t' || c == ' ') continue;

		else if(isChar(c)) {
			lexer.token.type = CHARS;
			for (; isChar(c); c = expr[++lexer.index])
				lexer.token.value += c;
		}

		else if (isNumber(c)) {
			lexer.token.type = NUMBER;
			for (; isNumber(c); c = expr[++lexer.index])
				lexer.token.value += c;

			if ((c == '.' || c ==',') && isNumber(expr[lexer.index + 1])) {
				// the strtod function can ignore the comma
				lexer.token.value += '.'; // '.' || ','
				c = expr[++lexer.index]; // next number

				for (; isNumber(c); c = expr[++lexer.index])
					lexer.token.value += c;
			}
		}

		else if (c == '-' || c == SERIES || c == LP || c == RP) {
			lexer.token.type = c;
			++lexer.index;
		}

		else if ((c == '|' || c == '/') && expr[lexer.index + 1] == c) {
			lexer.token.type = PARALLEL;
			lexer.index += 2;
		}

		else {
			std::string msg = "bad token: ";
			msg += c;
			error(msg);
		}
		return;
	}

}

void ParallelSeries::error(std::string msg) {
	lexer.fail = true;
	std::cerr << "Error: " << msg << "" << std::endl;
}