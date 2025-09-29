#include <sstream>
#include <iostream>

#include "Parser.h"
#include "Syntax.h"

// Entrypoint
Program* Parser::doParsing(std::vector<Token> const& tokenStream)
{
    auto itr = tokenStream.begin();
    end_ = tokenStream.end();
    Program* p = parseProgram(itr);
    if (p->statements.size() == 0) {
        throw SyntaxError{ "ERROR: empty program!" };
    }
    return p;
}

static void unexpectedTokenError(Token const& found, std::string const& expected) {
    std::stringstream temp;
    temp << "Unexpected token ERROR: " << found << ". Expected " << expected << " instead.";
    throw SyntaxError{ temp.str() };
}

// Program
Program* Parser::parseProgram(std::vector<Token>::const_iterator& itr) {
    Program* p = new Program();

	// Ciclo finché non sono alla fine o arrivo a ENDMARKER
    while (itr != end_ && itr->tag != Token::ENDMARKER) {
        Statement* s = parseStatement(itr);
        p->statements.push_back(s);
    }

    if (itr == end_ || itr->tag != Token::ENDMARKER) {
        throw SyntaxError{ "ERROR: Program must end with ENDMARKER" };
    }

    safe_next(itr); // Consumo ENDMARKER
    return p;
}

// Statement che può essere Compound (se c'è if o while) o semplice
Statement* Parser::parseStatement(std::vector<Token>::const_iterator& itr) {
    
    while (itr->tag == Token::NEWLINE) safe_next(itr); // salto le righe vuote

    if (itr == end_ || itr->tag == Token::ENDMARKER) return nullptr;
    if (itr->tag == Token::IF || itr->tag == Token::WHILE) {
        return parseCompoundStatement(itr);
    }
    else {
        return parseSimpleStatement(itr);
    }
}

Statement* Parser::parseSimpleStatement(std::vector<Token>::const_iterator& itr) {
    if (itr->tag == Token::ID) {
        std::string id = itr->word;
        safe_next(itr);
        
        // Inizializzazione lista
        if (itr->tag == Token::EQ) {
            safe_next(itr);
            if (itr->tag == Token::LIST) { // id = list()
                safe_next(itr); // consumo list
                if (itr->tag != Token::LP) unexpectedTokenError(*itr, "'('");
                safe_next(itr); // LP
                if (itr->tag != Token::RP) unexpectedTokenError(*itr, "')'");
                safe_next(itr); // RP
                if (itr->tag != Token::NEWLINE && itr->tag != Token::ENDMARKER && itr->tag != Token::DEDENT)
                    unexpectedTokenError(*itr, "NEWLINE, DEDENT or ENDMARKER");
                if (itr->tag == Token::NEWLINE)
                    safe_next(itr);
                return new listInit(id);
            }
            else {
                // Caso con id = <expr> newline
                Expression* e = parseExpression(itr);
                if (itr->tag != Token::NEWLINE && itr->tag != Token::ENDMARKER && itr->tag != Token::DEDENT)
                    unexpectedTokenError(*itr, "NEWLINE, DEDENT or ENDMARKER");
                if (itr->tag == Token::NEWLINE)
                    safe_next(itr);
				// Faccio return nuova Definition
                return new Definition(new Variable(id), e);
            }
        }
        else if (itr->tag == Token::DOT) {
            // Caso <id>.append( <expr> ) newline
            safe_next(itr);
            if (itr->tag != Token::APPEND) unexpectedTokenError(*itr, "APPEND");
            safe_next(itr); // consumo append
            if (itr->tag != Token::LP) unexpectedTokenError(*itr, "'('");
            safe_next(itr); // LP
            Expression* e = parseExpression(itr);
            if (itr->tag != Token::RP) unexpectedTokenError(*itr, "')'");
            safe_next(itr); // RP
            if (itr->tag != Token::NEWLINE && itr->tag != Token::ENDMARKER && itr->tag != Token::DEDENT)
                unexpectedTokenError(*itr, "NEWLINE, DEDENT or ENDMARKER");
            if (itr->tag == Token::NEWLINE)
                safe_next(itr);
            return new listAppend(id, e);
        }
        else {
            unexpectedTokenError(*itr, "'= <expr> ', = list() or '.append(<expr>)'");
        }
    }
    
    // Caso BREAK newline
    else if (itr->tag == Token::BREAK) {
        safe_next(itr);
        if (itr->tag != Token::NEWLINE && itr->tag != Token::ENDMARKER && itr->tag != Token::DEDENT)
            unexpectedTokenError(*itr, "NEWLINE, DEDENT or ENDMARKER");
        if (itr->tag == Token::NEWLINE)
            safe_next(itr);
        return new Break();
    }

    // Caso CONTINUE newline
    else if (itr->tag == Token::CONTINUE) {
        safe_next(itr);
        if (itr->tag != Token::NEWLINE && itr->tag != Token::ENDMARKER && itr->tag != Token::DEDENT)
            unexpectedTokenError(*itr, "NEWLINE, DEDENT or ENDMARKER");
        if (itr->tag == Token::NEWLINE)
            safe_next(itr);
        return new Continue();
    }
	
    // Caso PRINT ( <expr> ) newline
    else if (itr->tag == Token::PRINT) {
        safe_next(itr);
        if (itr->tag != Token::LP) unexpectedTokenError(*itr, "'('");
        safe_next(itr); // LP
        Expression* e = parseExpression(itr);
        if (itr->tag != Token::RP) unexpectedTokenError(*itr, "')'");
        safe_next(itr); // RP
        if (itr->tag != Token::NEWLINE && itr->tag != Token::ENDMARKER && itr->tag != Token::DEDENT)
            unexpectedTokenError(*itr, "NEWLINE, DEDENT or ENDMARKER");
        if (itr->tag == Token::NEWLINE)
            safe_next(itr);
        return new Print(e);
    }
    else {
        // Altrimenti errore
        unexpectedTokenError(*itr, "ID, break, continue or print");
        return nullptr;
    }
}


Statement* Parser::parseCompoundStatement(std::vector<Token>::const_iterator& itr)
{
    // Compound statement iniziano con IF / WHILE
    if (itr->tag == Token::IF)
        return parseIfStatement(itr);

    else if (itr->tag == Token::WHILE)
		return parseWhileStatement(itr);
}

ifStatement* Parser::parseIfStatement(std::vector<Token>::const_iterator& itr) {
    ifStatement* ifSt = new ifStatement;
    safe_next(itr); // consumo IF/ELIF

    ifSt->condition = parseExpression(itr);

    if (itr == end_ || itr->tag != Token::COLON) unexpectedTokenError(*itr, "COLON");
    safe_next(itr);
    if (itr == end_ || itr->tag != Token::NEWLINE) unexpectedTokenError(*itr, "NEWLINE");
    safe_next(itr);
    if (itr == end_ || itr->tag != Token::INDENT) unexpectedTokenError(*itr, "INDENT");
    safe_next(itr);

    while (itr != end_ && itr->tag != Token::DEDENT) {
        while (itr->tag == Token::NEWLINE) safe_next(itr);
        if (itr->tag == Token::DEDENT) break;
        Statement* st = parseStatement(itr);
        if (!st) break;
        ifSt->block.push_back(st);
    }

    if (itr == end_) unexpectedTokenError(*itr, "DEDENT");
    safe_next(itr); // consumo DEDENT

    // Gestione ELIF
    if (itr != end_ && itr->tag == Token::ELIF) {
        ifSt->elifBlock = parseIfStatement(itr);
    }
    // Gestione ELSE
    else if (itr != end_ && itr->tag == Token::ELSE) {
        safe_next(itr);
        if (itr == end_ || itr->tag != Token::COLON) unexpectedTokenError(*itr, "COLON");
        safe_next(itr);
        if (itr == end_ || itr->tag != Token::NEWLINE) unexpectedTokenError(*itr, "NEWLINE");
        safe_next(itr);
        if (itr == end_ || itr->tag != Token::INDENT) unexpectedTokenError(*itr, "INDENT");
        safe_next(itr);

        while (itr != end_ && itr->tag != Token::DEDENT) {
            while (itr->tag == Token::NEWLINE) safe_next(itr);
            if (itr->tag == Token::DEDENT) break;
            Statement* st = parseStatement(itr);
            if (!st) break;
            ifSt->elseBlock.push_back(st);
        }

        if (itr == end_) unexpectedTokenError(*itr, "DEDENT");
        safe_next(itr); // consumo DEDENT
    }

    return ifSt;
}


whileStatement* Parser::parseWhileStatement(std::vector<Token>::const_iterator& itr) {
    // I blocchi WHILE sono formati da - while <expr> : NEWLINE INDENT <statements> DEDENT
    whileStatement* whileSt = new whileStatement;
    safe_next(itr);
    whileSt->condition = parseExpression(itr);

    if (itr->tag != Token::COLON) 
        unexpectedTokenError(*itr, ":");
    safe_next(itr);
    if (itr->tag != Token::NEWLINE)
        unexpectedTokenError(*itr, "NEWLINE");
    safe_next(itr);
    if (itr->tag != Token::INDENT)
        unexpectedTokenError(*itr, "INDENT");

    safe_next(itr);

    while (itr->tag != Token::DEDENT) {

        // Skippo linee vuote
        if (itr->tag == Token::NEWLINE) {
            safe_next(itr);
            continue;
        }

        Statement* st = parseStatement(itr);
        whileSt->block.push_back(st);
    }
    // Consumo DEDENT
    safe_next(itr);
    return whileSt;
}


// Definzione
Definition* Parser::parseDefinition(std::vector<Token>::const_iterator& itr) {
    Variable* v = parseVariable(itr);
    if (itr->tag != Token::EQ) {
		unexpectedTokenError(*itr, "EQ");
    }
    safe_next(itr);
    Expression* e = parseExpression(itr);

    // Dopo l'espressione consumo il NEWLINE
    if (itr->tag != Token::NEWLINE) {
        unexpectedTokenError(*itr, "NEWLINE");
    }
    safe_next(itr);

    return new Definition{ v, e };
}

Expression* Parser::parseExpression(std::vector<Token>::const_iterator& itr) {
    // Caso <join>
    Expression* left = parseJoin(itr);

    // Ciclo finché trovo l’operatore OR
    while (itr->tag == Token::OR) {
        safe_next(itr); // consumo token OR
        Expression* right = parseJoin(itr);
        left = new orExpr(left, right);
    }

    return left;
}

Expression* Parser::parseJoin(std::vector<Token>::const_iterator& itr) {
    // Parso il primo equality
    Expression* left = parseEquality(itr);

    // Finché trovo AND faccio AndExpr
    while (itr != end_ && itr->tag == Token::AND) {
        int op = itr->tag;
        safe_next(itr); // consumo token AND
        Expression* right = parseEquality(itr);
        left = new andExpr(left, right);
    }

    return left;
}

Expression* Parser::parseEquality(std::vector<Token>::const_iterator& itr) {
    // Parso il primo operatore relazionale
    Expression* left = parseRel(itr);

    // Finché trovo == oppure != faccio RelExpression
    while (itr != end_ && (itr->tag == Token::EQEQ || itr->tag == Token::NEQ)) {
        int op = itr->tag;
        safe_next(itr); // consumo l'operatore
        Expression* right = parseRel(itr);
        left = new relExpression(op, left, right);
    }

    return left;
}

Expression* Parser::parseRel(std::vector<Token>::const_iterator& itr) {
	// Parso il primo numExpr
    Expression* left = parseNumExpr(itr);

	// Finché trovo <, <=, >, >= faccio RelExpression
    while (itr != end_ &&
        (itr->tag == Token::LT || itr->tag == Token::LTE ||
            itr->tag == Token::GT || itr->tag == Token::GTE)) {
        int op = itr->tag;
		safe_next(itr); // consumo l'operatore
        Expression* right = parseNumExpr(itr);
        left = new relExpression(op, left, right);
    }

    return left;
}

Expression* Parser::parseNumExpr(std::vector<Token>::const_iterator& itr) {
    // Parso il primo term
    Expression* left = parseTerm(itr);

    // Finché trovo + oppure - faccio mathExpression
    while (itr != end_ && (itr->tag == Token::ADD || itr->tag == Token::SUB)) {
        int op = itr->tag;
        safe_next(itr); // consumo l'operatore
        Expression* right = parseTerm(itr);
        left = new mathExpression(op, left, right);
    }

    return left;
}

Expression* Parser::parseTerm(std::vector<Token>::const_iterator& itr) {
    // Parso il primo unary
    Expression* left = parseUnary(itr);

	// Finché trovo * oppure // faccio mathExpression
    while (itr != end_ && (itr->tag == Token::MUL || itr->tag == Token::INTDIV)) {
        int op = itr->tag;
        safe_next(itr); // consumo l'operatore
        Expression* right = parseUnary(itr);
        left = new mathExpression(op, left, right);
    }

    return left;
}

Expression* Parser::parseUnary(std::vector<Token>::const_iterator& itr) {
    if (itr == end_) {
        throw SyntaxError{ "Unexpected end of input in unary expression" };
    }

	// Caso con NOT <unary> | - <unary>
    if (itr->tag == Token::NOT || itr->tag == Token::SUB) {
        int op = itr->tag;
        safe_next(itr); // consumo l'operatore

        Expression* expr = parseUnary(itr);
        return new unaryExpression(op, expr);
    }

    // Caso con <factor>
    return parseFactor(itr);
}

Expression* Parser::parseFactor(std::vector<Token>::const_iterator& itr) {
    if (itr == end_) {
        throw SyntaxError{ "Unexpected end of input in factor" };
    }

    if (itr->tag == Token::LP) {
        // Caso ( <expr> )
        safe_next(itr); // consumo "("
        Expression* expr = parseExpression(itr);

        if (itr == end_ || itr->tag != Token::RP) {
            unexpectedTokenError(*itr, ")");
        }
        safe_next(itr); // consumo ")"
        return expr;
    }
    else if (itr->tag == Token::ID) {
        // Caso con <loc> (che a sua volta può essere id o id[ <expr> ] -> chiamo parseLoc)
        return parseLoc(itr);
    }
	// Caso con num (CONST)
    else if (itr->tag == Token::CONST) {
        return parseConstant(itr);
    }
    // Caso con True o False
    else if (itr->tag == Token::TRUE || itr->tag == Token::FALSE) {
        bool value = (itr->tag == Token::TRUE);
        safe_next(itr);
        return new Constant(value ? 1 : 0); // valori booleani, metto 1 oppure 0 per TRUE || FALSE
    }
    else {
        // Altro -> errore
        unexpectedTokenError(*itr, "ID, CONST, True, False or '('");
        return nullptr;
    }
}

Expression* Parser::parseLoc(std::vector<Token>::const_iterator& itr) {
    // <loc> deve iniziare con un ID per definizione altrimenti errore
    if (itr->tag != Token::ID) {
        unexpectedTokenError(*itr, "ID");
    }

    std::string varName = itr->word;
    safe_next(itr);

    // Caso con id[ <expr> ] -> listAccess
    if (itr != end_ && itr->tag == Token::LBRACK) {
        safe_next(itr); // consumo "["
        Expression* index = parseExpression(itr);

        if (itr == end_ || itr->tag != Token::RBRACK) {
            unexpectedTokenError(*itr, "]");
        }
        safe_next(itr); // consumo "]"

		// Faccio listAccess con varName e index
        return new listAccess(varName, index);
    }

	// Altrimenti ho solo che <loc> è un id
    return new Variable(varName);
}


// Variabili e costanti
Variable* Parser::parseVariable(std::vector<Token>::const_iterator& itr) {
    Variable* v = new Variable{ itr->word };
    safe_next(itr);
    return v;
}

Constant* Parser::parseConstant(std::vector<Token>::const_iterator& itr) {
    std::stringstream temp;
    temp << itr->word;
    int num;
    temp >> num;
    Constant* c = new Constant{ num };
    safe_next(itr);
    return c;
}