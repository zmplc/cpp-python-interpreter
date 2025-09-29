#pragma once

#include <iostream>

#include "Visitor.h"
#include "SymbolTable.h"

class EvaluationVisitor : public Visitor {
	
public:
    EvaluationVisitor(SymbolTable& st, std::ostream& con)
        : symbolTable_{ st }, console_{ con } {
    }

    ~EvaluationVisitor() = default;

    // Program
    void visit(Program const& p) override {
        for (Statement* statement : p.statements) {
            try {
                statement->accept(*this);
            }
            // Qui siamo fuori dal loop while, quindi ignoro break/continue trovati come da istruzioni
            catch (BreakThrowable& b) {}
			catch (ContinueThrowable& c) {}
        }
    }

    // Definition
    void visit(Definition const& d) override {
        int value = evaluateExpression(*d.expression_);
        symbolTable_.setValue(d.variable_->id_, value);
    }

    // Expression
    void visit(Expression const& o) override {
        throw std::runtime_error("ERROR: Expression visit should not be called.");
	}

	// ifStatement (vale per if, elif, else)
    void visit(ifStatement const& i) override {
        if (evaluateExpression(*i.condition)) {
            for (auto* st : i.block) st->accept(*this);
        }
        else if (i.elifBlock) {
            i.elifBlock->accept(*this);
        }
        else {
            for (auto* st : i.elseBlock) st->accept(*this);
        }
    }

	// whileStatement (se trovo break/continue faccio BreakThrowable/ContinueThrowable)
    void visit(whileStatement const& w) override {
        while (evaluateExpression(*w.condition)) {
            try {
                for (auto* st : w.block) {
                    st->accept(*this);
                }
            }
            catch (BreakThrowable& b) {
                break;
            }
            catch (ContinueThrowable& c) {
                continue;
			}
        }
    }

    // Break
    void visit(Break const& b) override {
        throw BreakThrowable{};
	}

    // Continue
    void visit(Continue const& c) override {
        throw ContinueThrowable{};
    }
    
    // Print
    void visit(Print const& p) override {
        int value = evaluateExpression(*p.expr_);
        console_ << value << std::endl;
    }

    
    // orExpr
    void visit(orExpr const& e) override {
        int l = evaluateExpression(*e.left_);
		if (l != 0) { lastValue_ = 1; return; }  // non valuto r per short-circuit
        int r = evaluateExpression(*e.right_);
        lastValue_ = (l || r);

    }

    // andExpr
    void visit(andExpr const& e) override {
        int l = evaluateExpression(*e.left_);
        if (l == 0) { lastValue_ = 0; return; }  // non valuto r per short-circuit
        int r = evaluateExpression(*e.right_);
        lastValue_ = (l && r);

    }

    // relExpression
    void visit(relExpression const& e) override {
        int l = evaluateExpression(*e.left_);
        int r = evaluateExpression(*e.right_);
        switch (e.opCode_) {
        case Token::LT:  lastValue_ = (l < r); break;
        case Token::LTE: lastValue_ = (l <= r); break;
        case Token::GT:  lastValue_ = (l > r); break;
        case Token::GTE: lastValue_ = (l >= r); break;
        case Token::EQEQ: lastValue_ = (l == r); break;
        case Token::NEQ:  lastValue_ = (l != r); break;
        default: throw std::runtime_error("ERROR: Unknown relational operator.");
        }
    }

    // mathExpression
    void visit(mathExpression const& e) override {
        int l = evaluateExpression(*e.left_);
        int r = evaluateExpression(*e.right_);
        switch (e.opCode_) {
        case Token::ADD: lastValue_ = l + r; break;
        case Token::SUB: lastValue_ = l - r; break;
        case Token::MUL: lastValue_ = l * r; break;
        case Token::INTDIV:
            if (r == 0) throw std::runtime_error("ERROR: Division by zero.");
            lastValue_ = l / r;
            break;
        default: throw std::runtime_error("ERROR: Unknown math operator.");
        }
    }

    // unaryExpression
    void visit(unaryExpression const& e) override {
        int val = evaluateExpression(*e.operand_);
        if (e.opCode_ == Token::SUB) lastValue_ = -val;
        else if (e.opCode_ == Token::NOT) lastValue_ = (val == 0);
        else throw std::runtime_error("ERROR: Unknown unary operator.");
    }

    // Variable
    void visit(Variable const& v) override {
        lastValue_ = symbolTable_.getValue(v.id_);
    }

    // Constant
    void visit(Constant const& c) override {
        lastValue_ = c.num_;
    }

	// Prendo l'ultimo valore calcolato
    int getValue() const { return lastValue_; }

	// ListInit
    void visit(listInit const& l) override {
        symbolTable_.setList(l.id_);
	}

	// listAppend
    void visit(listAppend const& l) override {
        int value = evaluateExpression(*l.expr_);
        symbolTable_.appendToList(l.id_, value);
	}

    // listAccess
    void visit(listAccess const& e) override {
        int index = evaluateExpression(*e.index_);
        lastValue_ = symbolTable_.getListValue(e.id_, index);
	}

private:
    SymbolTable& symbolTable_;
    std::ostream& console_;
    int lastValue_ = 0;

	// Faccio l'espressione e ritorno il valore calcolato
    int evaluateExpression(Expression const& expr) {
        expr.accept(*this);
        return lastValue_;
    }
};