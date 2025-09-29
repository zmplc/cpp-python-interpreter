#pragma once

#include "Syntax.h"

class Visitor {
public:
	virtual void visit(Program const& p) = 0;
	virtual void visit(Definition const& d) = 0;
	virtual void visit(Expression const& o) = 0;
	virtual void visit(Variable const& v) = 0;
	virtual void visit(Constant const& c) = 0;

    virtual void visit(ifStatement const& i) = 0;
    virtual void visit(whileStatement const& w) = 0;

    virtual void visit(Break const& b) = 0;
    virtual void visit(Continue const& c) = 0;
    virtual void visit(Print const& p) = 0;
    virtual void visit(listInit const& l) = 0;
    virtual void visit(listAppend const& l) = 0;

    virtual void visit(orExpr const& e) = 0;
    virtual void visit(andExpr const& e) = 0;
    virtual void visit(relExpression const& e) = 0;
    virtual void visit(mathExpression const& e) = 0;
    virtual void visit(unaryExpression const& e) = 0;
    virtual void visit(listAccess const& e) = 0;
};