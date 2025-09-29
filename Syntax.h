#pragma once

#include <vector>
#include <string>

class Visitor;

struct Statement {
	virtual void accept(Visitor& visitor) const = 0;
};

struct Program {
	Program() = default;
	~Program() {
		for (auto s : statements) delete s;
	}

	void accept(Visitor& visitor) const;

	std::vector<Statement*> statements;
};

struct Expression : public Statement {

	virtual void accept(Visitor& visitor) const = 0;

	Expression* left_ = nullptr;
	Expression* right_ = nullptr;
	int opCode_ = 0;
};

// ifStatement
struct ifStatement : public Statement {

	void accept(Visitor& visitor) const override;

	Expression* condition = nullptr;
	std::vector<Statement*> block;
	std::vector<Statement*> elseBlock;
	ifStatement* elifBlock = nullptr;
};

// whileStatement
struct whileStatement : public Statement {
	
	void accept(Visitor& visitor) const override;

	Expression* condition = nullptr;
	std::vector<Statement*> block;
};

// Break
struct Break : public Statement {
	void accept(Visitor& visitor) const override;
};

// 

struct Continue : public Statement {
	void accept(Visitor& visitor) const override;
};

// Print
struct Print : public Statement {
	Print(Expression* expr) : expr_{ expr } {}
	~Print() { delete expr_; }

	void accept(Visitor& visitor) const override;

	Expression* expr_;
};

// listAppend -> aggiungo un elemento in fondo alla lista, nel mio caso <expr>
struct listAppend : public Statement {
	listAppend(std::string id, Expression* expr) : id_{ id }, expr_{ expr } {}
	~listAppend() { delete expr_; }

	void accept(Visitor& visitor) const override;

	std::string id_;
	Expression* expr_;
};

// listInit -> creo una nuova lista
struct listInit : public Statement {
	listInit(std::string id) : id_{ id } {}
	void accept(Visitor& visitor) const override;
	std::string id_;
};

struct Variable : public Expression {
	Variable(std::string id) : id_{ id } { }
	~Variable() = default;

	void accept(Visitor& visitor) const;

	std::string id_;
};

struct Constant : public Expression {
	Constant(int num) : num_{ num } { }
	~Constant() = default;

	void accept(Visitor& visitor) const;

	int num_;

};

struct Definition : public Statement {
	Definition(Variable* v, Expression* e) :
		variable_{v}, expression_{e} { }
	~Definition() {
		delete variable_;
		delete expression_;
	}

	void accept(Visitor& visitor) const override;

	Variable* variable_;
	Expression* expression_;
};

struct orExpr : public Expression {
	orExpr(Expression* l, Expression* r) : left_{ l }, right_{ r } {}

	~orExpr() {
		delete left_;
		delete right_;
	}

	void accept(Visitor& visitor) const override; 

	Expression* left_;
	Expression* right_;
};

struct andExpr : public Expression {
	andExpr(Expression* l, Expression* r) : left_{ l }, right_{ r } {}
	~andExpr() {
		delete left_;
		delete right_;
	}

	void accept(Visitor& visitor) const override;

	Expression* left_;
	Expression* right_;
};

// Per ==, !=, <, <=, >, >= (ovvero gli operatori di confronto)
struct relExpression : public Expression {
	relExpression(int opCode, Expression* l, Expression* r) :
		opCode_{ opCode }, left_{ l }, right_{ r } {
	}

	~relExpression() {
		delete left_;
		delete right_;
	}

	void accept(Visitor& visitor) const override;

	Expression* left_;
	Expression* right_;
	int opCode_;
};

// Per operatori aritmetici +, -, *, // (la divisione non intera non è definita nel progetto)
struct mathExpression : public Expression {
	mathExpression(int opCode, Expression* l, Expression* r) :
		opCode_{ opCode }, left_{ l }, right_{ r } {
	}

	~mathExpression() {
		delete left_;
		delete right_;
	}

	void accept(Visitor& visitor) const override;

	int opCode_; // ADD, SUB, MUL, INTDIV
	Expression* left_;
	Expression* right_;
};

// Per operatori unary: not, -
struct unaryExpression : public Expression {
	unaryExpression(int opCode, Expression* operand) :
		opCode_{ opCode }, operand_{ operand } {
	}

	~unaryExpression() {
		delete operand_;
	}

	void accept(Visitor& visitor) const override;

	int opCode_; // NOT, SUB (per <unary>)
	Expression* operand_;
};

// Lista per id[ <expr> ]
struct listAccess : public Expression {
	listAccess(const std::string& id, Expression* idx) :
		id_{ id }, index_{ idx } {
	}

	~listAccess() {
		delete index_;
	}

	void accept(Visitor& visitor) const override;

	std::string id_;
	Expression* index_;
};

