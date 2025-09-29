#pragma once

#include <vector>
#include <fstream>

#include "Token.h"
#include "Exception.h"

// Funzione object per tokenizzare uno stream di input
class Lexer {
public:
	Lexer() = default;
	~Lexer() = default;
	Lexer(Lexer const&) = delete;
	Lexer& operator=(Lexer const&) = delete;

	std::vector<Token> operator()(std::ifstream& inputFile) {
		std::vector<Token> inputTokens;
		tokenizeInputFile(inputFile, inputTokens);
		return inputTokens;
	}

private:
	std::string tokenizeConstant(std::ifstream& inputFile, std::stringstream& temp);
	void tokenizeInputFile(std::ifstream& inputFile, std::vector<Token>& inputTokens);
};