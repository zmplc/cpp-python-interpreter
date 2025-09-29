#pragma once

#include <string>

struct Token {

    static constexpr int LP = 0;  // (
    static constexpr int RP = 1;  // )
    static constexpr int LBRACK = 2;  // [
    static constexpr int RBRACK = 3;  // ]
    static constexpr int COLON = 4;  // :
    static constexpr int COMMA = 5;  // ,
    static constexpr int DOT = 6;  // .
    static constexpr int EQ = 7;  // =

	// Operatori aritmetici
    static constexpr int ADD = 8;  // +
    static constexpr int SUB = 9;  // -
    static constexpr int MUL = 10;  // *
    static constexpr int DIV = 11;  // /
    static constexpr int INTDIV = 12;  // //

    // Operatori confronto
    static constexpr int LT = 13;  // <
    static constexpr int LTE = 14;  // <=
    static constexpr int GT = 15;  // >
    static constexpr int GTE = 16;  // >=
    static constexpr int EQEQ = 17;  // ==
    static constexpr int NEQ = 18;  // !=

	// Operatori booleani
    static constexpr int AND = 19;  // and
    static constexpr int OR = 20;  // or
    static constexpr int NOT = 21;  // not

    // Keywords
    static constexpr int IF = 22;
    static constexpr int ELIF = 23;
    static constexpr int ELSE = 24;
    static constexpr int WHILE = 25;
    static constexpr int BREAK = 26;
    static constexpr int CONTINUE = 27;
    static constexpr int LIST = 28;
    static constexpr int APPEND = 29;
    static constexpr int PRINT = 30;
    static constexpr int TRUE = 31;
    static constexpr int FALSE = 32;

	// Identificatori e costanti
    static constexpr int ID = 33;
    static constexpr int CONST = 34;

    // Structural tokens
    static constexpr int NEWLINE = 35;
    static constexpr int INDENT = 36;
    static constexpr int DEDENT = 37;
    static constexpr int ENDMARKER = 38;

    // Mapping
    static constexpr const char* id2word[]{
        "(", ")", "[", "]", ":", ",", ".", "=",
        "+", "-", "*", "/", "//",
        "<", "<=", ">", ">=", "==", "!=",
        "and", "or", "not",
        "if", "elif", "else", "while",
        "break", "continue", "list", "append", "print",
        "True", "False",
        "ID", "CONST",
        "NEWLINE", "INDENT", "DEDENT", "ENDMARKER"
    };

    static constexpr const char* tag2string[]{
        "LP", "RP", "LBRACK", "RBRACK", "COLON", "COMMA", "DOT", "ASSIGN",
        "ADD", "SUB", "MUL", "DIV", "INTDIV",
        "LT", "LTE", "GT", "GTE", "EQEQ", "NEQ",
        "AND", "OR", "NOT",
        "IF", "ELIF", "ELSE", "WHILE",
        "BREAK", "CONTINUE", "LIST", "APPEND", "PRINT",
        "TRUE", "FALSE",
        "ID", "CONST",
        "NEWLINE", "INDENT", "DEDENT", "ENDMARKER"
    };

    Token(int t, const char* w, int l = 0, int c = 0)
        : tag{ t }, word{ w }, line{ l }, column{ c } {
    }

    Token(int t, std::string w, int l = 0, int c = 0)
        : tag{ t }, word{ std::move(w) }, line{ l }, column{ c } {
    }

    ~Token() = default;
    Token(const Token&) = default;
    Token& operator=(const Token&) = default;

    int tag;
    std::string word;
	int line;       // numero linea (per errore)
	int column;     // numero colonna (per errore)
};

// Overload << per debug
std::ostream& operator<<(std::ostream& os, const Token& t);