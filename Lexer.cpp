#include "Lexer.h"
#include "Exception.h"

#include <sstream>
#include <vector>
#include <string>
#include <iostream>

void printTokens(const std::vector<Token>& tokens) {
    for (const auto& tk : tokens) {
        std::cout << tk << std::endl;
    }
}

std::string Lexer::tokenizeConstant(std::ifstream& inputFile, std::stringstream& temp) {
    char ch = inputFile.get();
    while (ch >= '0' && ch <= '9') {
        temp << ch;
        ch = inputFile.get();
    }
    inputFile.unget();
    return temp.str();
}

void Lexer::tokenizeInputFile(std::ifstream& inputFile, std::vector<Token>& inputTokens) {
    char ch{};
    unsigned int rowCount{ 1 };
    std::vector<int> indents{ 0 };  // stack per indentation
    bool newLine = true;    // nuova line

    while (inputFile.get(ch)) {
        // Skippo newline, spazi
        if (ch == '\n') {
            inputTokens.push_back(Token{ Token::NEWLINE, "\\n" });
            rowCount += 1;
            newLine = true;
            continue;
        }
        else if (ch == ' ' || ch == '\t') {
            if (!newLine) continue;
        }
        // Skippo ; perché presenti in alcuni vettori di test anche se non descritti nel linguaggio
        if (ch == ';') continue;

        // Indentation
        if (newLine) {
            int countSpaces = 0;
            while (ch == ' ' || ch == '\t') {
                countSpaces += (ch == ' ') ? 1 : 4;
                inputFile.get(ch);
            }

            if (countSpaces > indents.back()) {
                indents.push_back(countSpaces);
                inputTokens.push_back(Token{ Token::INDENT, "INDENT" });
            }
            else {
                while (countSpaces < indents.back()) {
                    indents.pop_back();
                    inputTokens.push_back(Token{ Token::DEDENT, "DEDENT" });
                }
                if (countSpaces != indents.back()) {
					printTokens(inputTokens);
                    throw LexicalError("ERROR: Inconsistent indentation at line " + std::to_string(rowCount));
                }
            }
            newLine = false;
        }

        if (ch == '(') inputTokens.push_back(Token{ Token::LP, "(" });
        else if (ch == ')') inputTokens.push_back(Token{ Token::RP, ")" });
        else if (ch == '[') inputTokens.push_back(Token{ Token::LBRACK, "[" });
        else if (ch == ']') inputTokens.push_back(Token{ Token::RBRACK, "]" });
        else if (ch == ':') inputTokens.push_back(Token{ Token::COLON, ":" });
        else if (ch == ',') inputTokens.push_back(Token{ Token::COMMA, "," });
        else if (ch == '.') inputTokens.push_back(Token{ Token::DOT, "." });
        else if (ch == '+') inputTokens.push_back(Token{ Token::ADD, "+" });
        else if (ch == '-') inputTokens.push_back(Token{ Token::SUB, "-" });
        else if (ch == '*') inputTokens.push_back(Token{ Token::MUL, "*" });
        // Divisione intera
        else if (ch == '/') {
            if (inputFile.peek() == '/') {
                inputFile.get();
                inputTokens.push_back(Token{ Token::INTDIV, "//" });
            }
        }
        else if (ch == '=') {
            char next = inputFile.peek();
            if (next == '=') {
                inputFile.get();
                inputTokens.push_back(Token{ Token::EQEQ, "==" });
            }
            else {
                inputTokens.push_back(Token{ Token::EQ, "=" });
            }
        }

        // Operatori di confronto
        else if (ch == '<') {
            char next = inputFile.peek();
            if (next == '=') {
                inputFile.get();
                inputTokens.push_back(Token{ Token::LTE, "<=" });
            }
            else {
                inputTokens.push_back(Token{ Token::LT, "<" });
            }
        }
        else if (ch == '>') {
            char next = inputFile.peek();
            if (next == '=') {
                inputFile.get();
                inputTokens.push_back(Token{ Token::GTE, ">=" });
            }
            else {
                inputTokens.push_back(Token{ Token::GT, ">" });
            }
        }
        else if (ch == '!') {
            char next = inputFile.get();
            if (next == '=') {
                inputTokens.push_back(Token{ Token::NEQ, "!=" });
            }
            else {
                printTokens(inputTokens);
                throw LexicalError("ERROR: Unexpected character '!' at line " + std::to_string(rowCount));
            }
        }

        else if (ch >= '0' && ch <= '9') {
            std::stringstream temp;
            temp << ch;
            tokenizeConstant(inputFile, temp);
            inputTokens.push_back(Token{ Token::CONST, temp.str() });
        }

        // Indentificatori/keywords

        else if (std::isalpha(ch)) {
            std::stringstream temp;
            // isAlpha è true se ch è a-zA-Z
			// isAlnum è true se ch è a-zA-Z0-9
            temp << ch;
            do {
                std::isalnum(ch);
				ch = inputFile.get();
				if (std::isalnum(ch)) temp << ch;
                
            } while (std::isalnum(ch));
            
            inputFile.unget();

            std::string word = temp.str();
            int tag = Token::ID;

            // Keywords
            if (word == "if") tag = Token::IF;
            else if (word == "elif") tag = Token::ELIF;
            else if (word == "else") tag = Token::ELSE;
            else if (word == "while") tag = Token::WHILE;
            else if (word == "break") tag = Token::BREAK;
            else if (word == "continue") tag = Token::CONTINUE;
            else if (word == "list") tag = Token::LIST;
            else if (word == "append") tag = Token::APPEND;
            else if (word == "print") tag = Token::PRINT;
            else if (word == "True") tag = Token::TRUE;
            else if (word == "False") tag = Token::FALSE;
            else if (word == "and") tag = Token::AND;
            else if (word == "or") tag = Token::OR;
            else if (word == "not") tag = Token::NOT;

            inputTokens.push_back(Token{ tag, word });
        }

        // Stray character -> lexical error in Exception.h
        else {
            throw LexicalError("ERROR: Stray character '" + std::string(1, ch) + "' at line " + std::to_string(rowCount));
        }
    }

    // End of File
    while (indents.size() > 1) {
        indents.pop_back();
        inputTokens.push_back(Token{ Token::DEDENT, "DEDENT" });
    }

    inputTokens.push_back(Token{ Token::ENDMARKER, "ENDMARKER" });

}
