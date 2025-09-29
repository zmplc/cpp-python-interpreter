#pragma once

#include <vector>

#include "Token.h"
#include "Syntax.h"
#include "Exception.h"

class Parser {

public:
	Parser() = default;
	~Parser() = default;
	Parser(const Parser&) = delete;
	Parser& operator=(const Parser&) = delete;

	Program* doParsing(std::vector<Token> const& tokenStream);

private:
	std::vector<Token>::const_iterator end_;

	// Parse per program
	Program* parseProgram(std::vector<Token>::const_iterator& itr);

	// Parse per statement
	Statement* parseStatement(std::vector<Token>::const_iterator& itr);
	Statement* parseSimpleStatement(std::vector<Token>::const_iterator& itr);
	Statement* parseCompoundStatement(std::vector<Token>::const_iterator& itr);

	// Parse per if e while
	ifStatement* parseIfStatement(std::vector<Token>::const_iterator& itr);
	whileStatement* parseWhileStatement(std::vector<Token>::const_iterator& itr);

	// Parse per espressioni
	Expression* parseExpression(std::vector<Token>::const_iterator& itr);
	Expression* parseJoin(std::vector<Token>::const_iterator& itr);
	Expression* parseEquality(std::vector<Token>::const_iterator& itr);
	Expression* parseRel(std::vector<Token>::const_iterator& itr);
	Expression* parseNumExpr(std::vector<Token>::const_iterator& itr);
	Expression* parseTerm(std::vector<Token>::const_iterator& itr);
	Expression* parseUnary(std::vector<Token>::const_iterator& itr);
	Expression* parseFactor(std::vector<Token>::const_iterator& itr);
	Expression* parseLoc(std::vector<Token>::const_iterator& itr);

	Definition* parseDefinition(std::vector<Token>::const_iterator& itr);

	Variable* parseVariable(std::vector<Token>::const_iterator& itr);
	Constant* parseConstant(std::vector<Token>::const_iterator& itr);

	void safe_next(std::vector<Token>::const_iterator& itr) const {
		if (itr != end_) {
			++itr;
		}
		else {
			throw SyntaxError{ "Premature end of input!" };
		}
	}

};