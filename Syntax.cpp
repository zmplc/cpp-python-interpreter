#include "Syntax.h"
#include "Visitor.h"

void Program::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void ifStatement::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void whileStatement::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void Break::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void Continue::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void Print::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void listAppend::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void listInit::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void Definition::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void Expression::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void Variable::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void Constant::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void orExpr::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void andExpr::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void relExpression::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void mathExpression::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void unaryExpression::accept(Visitor& visitor) const {
	visitor.visit(*this);
};

void listAccess::accept(Visitor& visitor) const {
	visitor.visit(*this);
};