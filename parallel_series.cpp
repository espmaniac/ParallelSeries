#include <iostream>
#include <cstdint>
#include "parallel_series.hpp"

static bool isChar(uint8_t key) {
	return (key >= 'a' && key <= 'z' || key >= 'A' && key <= 'Z');
}

static bool isNumber(uint8_t key) {
	return (key >= '0' && key <= '9');
}

ParallelSeries::ParallelSeries() : expr(""), onSeries(nullptr), onParallel(nullptr) {
	lexer.index = 0;
	lexer.token.type = -1;
	lexer.token.value = "";
	lexer.busy = false;
	lexer.fail = false;
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
			return strtod(lexer.token.value.c_str(), nullptr);
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


void ParallelSeries::getToken() {
	if (lexer.busy || lexer.fail) {
		lexer.busy = false;
		return;
	}

	lexer.token.value.clear();
	lexer.token.type = -1;
	
	for (uint8_t c = expr[lexer.index]; lexer.index < expr.size(); c = expr[++lexer.index]) {

		if (c == '\0' || c == '\n' || c == '\t' || c == ' ') continue;

		else if (isNumber(c) || c == '.' || c ==',') {
			lexer.token.type = NUMBER;
			for (/*lexer.token = c*/; (isNumber(c) || c == '.' || c ==','); c = expr[++lexer.index])
				lexer.token.value += c;
			//--lexer.index;
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