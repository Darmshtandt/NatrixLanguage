#include "SymbolTable.h"
#include "Errors.h"
#include <cassert>
#include <stdexcept>


SymbolTable::SymbolTable(SymbolTable&& table) noexcept:
	m_VarMap(std::move(table.m_VarMap)),
	m_ImportedFuncMap(std::move(table.m_ImportedFuncMap)) {
}

void SymbolTable::CreateVar(const std::string& name) {
	if (m_VarMap.contains(name))
		throw SyntaxError(PERROR_SYMBOL_NOT_EXISTS, ("Variable \"" + name + "\" already exists").c_str());

	m_VarMap[name];
}
void SymbolTable::CreateVar(const std::string& name, const NppValue& value) {
	if (m_VarMap.contains(name))
		throw SyntaxError(PERROR_SYMBOL_NOT_EXISTS, ("Variable \"" + name + "\" already exists").c_str());
	m_VarMap[name] = value;
}

void SymbolTable::SetVarValue(const std::string& name, const NppValue& value) {
	if (!m_VarMap.contains(name))
		throw SyntaxError(PERROR_SYMBOL_NOT_EXISTS, ("Variable \"" + name + "\" not exists").c_str());
	m_VarMap[name] = value;
}

NppValue* SymbolTable::GetVarValuePtr(const std::string& name) {
	if (!m_VarMap.contains(name))
		return nullptr;
	return &m_VarMap[name];
}


void SymbolTable::CreateFunction(std::string name, NppFunc func) {
	if (!func)
		throw std::runtime_error("Incorrect function");

	if (m_ImportedFuncMap.contains(name))
		throw SyntaxError(PERROR_SYMBOL_NOT_EXISTS, ("Function \"" + name + "\" already exists").c_str());
	m_ImportedFuncMap[std::move(name)] = std::move(func);
}

void SymbolTable::CreateFunction(std::string name, NppFuncArgs&& args, std::vector<Token>&& body) {
	assert(0);
}

VariableStruct SymbolTable::CallFunction(const std::string& name, const NppFuncArgs& args) {
	if (!m_ImportedFuncMap.contains(name))
		throw SyntaxError(PERROR_SYMBOL_NOT_EXISTS, ("Unknown function: \"" + name + "\"").c_str());
	return m_ImportedFuncMap[name](args);
}

NppFunc* SymbolTable::GetFunctionPtr(const std::string& name) {
	if (!m_ImportedFuncMap.contains(name))
		return nullptr;
	return &m_ImportedFuncMap[name];
}