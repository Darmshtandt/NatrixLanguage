#include "SymbolTable.h"
#include "Errors.h"

#include <stdexcept>


VariableStruct1::VariableStruct1(std::string name, NppValue value) noexcept :
	VarName(std::move(name)),
	ValueStruct(std::move(value))
{
}

void SymbolTable::CreateVar(const std::string& name) {
	if (m_VarMap.contains(name))
		throw SyntaxError(("Variable \"" + name + "\" already exists").c_str());

	m_VarMap[name];
}
void SymbolTable::CreateVar(const std::string& name, const NppValue& value) {
	if (m_VarMap.contains(name))
		throw SyntaxError(("Variable \"" + name + "\" already exists").c_str());
	m_VarMap[name] = value;
}

void SymbolTable::SetVarValue(const std::string& name, const NppValue& value) {
	if (!m_VarMap.contains(name))
		throw SyntaxError(("Variable \"" + name + "\" not exists").c_str());
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

	if (m_FuncMap.contains(name))
		throw SyntaxError(("Function \"" + name + "\" already exists").c_str());
	m_FuncMap[std::move(name)] = std::move(func);
}

VariableStruct1 SymbolTable::CallFunction(const std::string& name, const NppFuncArgs& args) {
	if (!m_FuncMap.contains(name))
		throw SyntaxError(("Unknown function: \"" + name + "\"").c_str());
	return m_FuncMap[name](args);
}

NppFunc* SymbolTable::GetFunctionPtr(const std::string& name) {
	if (!m_FuncMap.contains(name))
		return nullptr;
	return &m_FuncMap[name];
}