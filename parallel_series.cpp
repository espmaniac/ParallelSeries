#include <iostream>
#include <cstring>
#include <cstdint>
#include <cmath>
#include "parallel_series.hpp"

static bool isChar(uint8_t key) {
	return (key >= 'a' && key <= 'z' || key >= 'A' && key <= 'Z');
}

static bool isNumber(uint8_t key) {
	return (key >= '0' && key <= '9');
}

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
				int8_t exponent = 0;

				for (uint8_t i = 0; i < (sizeof(PREFIXES) / sizeof(*PREFIXES)); ++i) {
					METRIC_PREFIX prefix = PREFIXES[i];
					if (
						strcmp(prefix.symbol, lexer.token.value.c_str()) == 0
						||
						strcmp(prefix.name, lexer.token.value.c_str()) == 0
					) {
						exponent = prefix.exponent;
						break;
					}
				}

				if (exponent != 0)
					result *= pow(10, exponent);
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

Star ParallelSeries::deltaStarTransform(Delta delta) {
	/* also known as
	 mesh -> star
	 pi -> star
	 pi -> T
	 delta -> wye
	*/

	Star output = {0.0, 0.0, 0.0};

	if (onSeries == nullptr || onParallel == nullptr) {
		error("[deltaStarTransform] onSeries or onParallel or both are nullptr");
		return output;
	}
	
	double deltaSeries = onSeries(delta.a, onSeries(delta.b, delta.c));

	output.a = (delta.a * delta.c) / deltaSeries;

	output.b = (delta.b * delta.c) / deltaSeries;

	output.c = (delta.a * delta.b) / deltaSeries;

	/* CHECK

	// (a + b) = (c || (a + b))
	double a_series_b = onParallel(delta.c, onSeries(delta.a, delta.b));

	// (b + c) = (a || (b + c))
	double b_series_c = onParallel(delta.a, onSeries(delta.b, delta.c));

	// (c + a) = (b || (c + a));
	double c_series_a = onParallel(delta.b, onSeries(delta.a, delta.c));

	*/

	return output;
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