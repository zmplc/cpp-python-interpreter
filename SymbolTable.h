#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "Exception.h"

class SymbolTable {

public:
	SymbolTable() = default;
	~SymbolTable() = default;

	SymbolTable(const SymbolTable& other) = delete;
	SymbolTable& operator=(const SymbolTable& other) = delete;

	// Variabili scalari
	void setValue(std::string const& key, int value) {
		map[key] = value;
	}

	// Get di una variabile scalare
	int getValue(std::string const& key) const {
		auto itr = map.find(key);
		if (itr == map.end()) {
			std::stringstream temp;
			temp << "ERROR: Undeclared identifier: " << key;
			throw EvaluationError{ temp.str() };
		}
		return (*map.find(key)).second;
	}

	// Liste
	void setList(std::string const& key) {
		listMap[key] = std::vector<int>{};
	}

	// Aggiunge un elemento alla lista
	void appendToList(std::string const& key, int value) {
		auto itr = listMap.find(key);
		if (itr == listMap.end()) {
			std::stringstream temp;
			temp << "ERROR: Undeclared identifier: " << key;
			throw EvaluationError{ temp.str() };
		}
		(*listMap.find(key)).second.push_back(value);
	}

	// Get di un elemento dalla lista
	int getListValue(std::string const& key, int index) const {
		auto itr = listMap.find(key);
		if (itr == listMap.end()) {
			std::stringstream temp;
			temp << "ERROR: Undeclared identifier: " << key;
			throw EvaluationError{ temp.str() };
		}

		if (index < 0 || index >= (int)(*listMap.find(key)).second.size()) {
			std::stringstream temp;
			temp << "ERROR: Index out of bounds: " << key << "List size: " << (*listMap.find(key)).second.size();
			throw EvaluationError{ temp.str() };
		}
		return (*listMap.find(key)).second[index];
	}

private:
	// Mappa per variabili scalari
	std::unordered_map<std::string, int> map;
	// Mappa per liste
	std::unordered_map<std::string, std::vector<int>> listMap;
};