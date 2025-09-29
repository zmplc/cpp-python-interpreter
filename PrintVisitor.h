#pragma once

#include <iostream>

#include "Visitor.h"
#include "Syntax.h"
#include "Token.h"

class PrintVisitor : public Visitor {

public:
	PrintVisitor(std::ostream& con) : console_{ con } { }
	~PrintVisitor() = default;

	void visit(Program const& p) override {
		for (Statement* statement : p.statements) {
			statement->accept(*this);
			console_ << std::endl;
		}
	}
	void visit(Definition const& d) override {
		d.variable_->accept(*this);
		console_ << " " << Token::id2word[Token::EQ] << " ";
		d.expression_->accept(*this);
	}
	void visit(Expression const& o) override {
		console_ << Token::id2word[Token::LP];
		o.left_->accept(*this);
		console_ << " " << Token::id2word[o.opCode_] << " ";
		o.right_->accept(*this);
		console_ << Token::id2word[Token::RP];
	}
	void visit(Variable const& v) override {
		console_ << v.id_;
	}
	void visit(Constant const& c) override {
		console_ << c.num_;
	}

	// AGGIUNTE DEL 27/09
    void visit(Break const& b) override {
        console_ << "break";
    }

    void visit(Continue const& c) override {
        console_ << "continue";
    }

    void visit(Print const& p) override {
        console_ << "print(";
        p.expr_->accept(*this);
        console_ << ")";
    }

    void visit(listInit const& l) override {
        console_ << l.id_ << " = list()";
    }

    void visit(listAppend const& l) override {
        console_ << l.id_ << ".append(";
        l.expr_->accept(*this);
        console_ << ")";
    }

    void visit(orExpr const& e) override {
        e.left_->accept(*this);
        console_ << " or ";
        e.right_->accept(*this);
    }

    void visit(andExpr const& e) override {
        e.left_->accept(*this);
        console_ << " and ";
        e.right_->accept(*this);
    }

    void visit(relExpression const& e) override {
		console_ << "(";
        e.left_->accept(*this);
        console_ << " " << Token::tag2string[e.opCode_] << " ";
        e.right_->accept(*this);
		console_ << ")";
    }

    void visit(mathExpression const& e) override {
        e.left_->accept(*this);
        console_ << " " << Token::tag2string[e.opCode_] << " ";
        e.right_->accept(*this);
        
    }

    void visit(unaryExpression const& e) override {
        
        if (e.opCode_ == Token::NOT) std::cout << "not ";
        else if (e.opCode_ == Token::SUB) std::cout << "-";
        e.operand_->accept(*this);
        
    }

    void visit(listAccess const& e) override {
        console_ << e.id_ << "[";
        e.index_->accept(*this);
        console_ << "]";
    }

    void visit(ifStatement const& i) override {
        console_ << "if ";
        i.condition->accept(*this);
        console_ << ":" << std::endl;
        for (Statement* st : i.block) {
            console_ << "    "; // indentazione di 4 spazi
            st->accept(*this);
            console_ << std::endl;
        }
        if (i.elifBlock != nullptr) {
            i.elifBlock->accept(*this);
        }
        if (!i.elseBlock.empty()) {
            console_ << "else:" << std::endl;
            for (Statement* st : i.elseBlock) {
                console_ << "    "; // indentazione di 4 spazi
                st->accept(*this);
                console_ << std::endl;
            }
        }
	}

    void visit(whileStatement const& w) override {
        console_ << "while ";
        w.condition->accept(*this);
        console_ << ":" << std::endl;
        for (Statement* st : w.block) {
            console_ << "    "; // indentazione di 4 spazi
            st->accept(*this);
            console_ << std::endl;
        }
	}

private:
	std::ostream& console_;

};

