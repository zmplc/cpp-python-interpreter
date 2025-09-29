#pragma once

#include <stdexcept>
#include <string>

struct LexicalError : std::runtime_error {
	LexicalError(const char* msg) : std::runtime_error(msg) {}
	LexicalError(std::string msg) : std::runtime_error(msg.c_str()) {}
};

struct SyntaxError : std::runtime_error {
	SyntaxError(const char* msg) : std::runtime_error(msg) {}
	SyntaxError(std::string msg) : std::runtime_error(msg.c_str()) {}
};

struct EvaluationError : std::runtime_error {
	EvaluationError(const char* msg) : std::runtime_error(msg) {}
	EvaluationError(std::string msg) : std::runtime_error(msg.c_str()) {}
};

struct BreakThrowable : std::runtime_error {
	BreakThrowable() : std::runtime_error("Break thrown") {}
};

struct ContinueThrowable : std::runtime_error {
	ContinueThrowable() : std::runtime_error("Continue thrown") {}
};