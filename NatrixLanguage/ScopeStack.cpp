#include "ScopeStack.h"

#include <cassert>
#include <stdexcept>

ScopeStack::ScopeStack() {
	m_SymbolTables.emplace_back(new SymbolTable);
}

ScopeStack::ScopeStack(ScopeStack&& stack) noexcept:
	m_SymbolTables(std::move(stack.m_SymbolTables)) {
}

void ScopeStack::CreateLocalScope() {
	m_SymbolTables.emplace_back(new SymbolTable);
}
void ScopeStack::ReleaseLocalScope() {
	if (m_SymbolTables.size() == 1)
		throw std::runtime_error("Local scope is empty");
	m_SymbolTables.pop_back();
}

void ScopeStack::CreateVar(const std::string& name) {
	Top()->CreateVar(name);
}
void ScopeStack::CreateVar(const std::string& name, const NppValue& value) {
	Top()->CreateVar(name, value);
}

void ScopeStack::SetVarValue(const std::string& name, const NppValue& value) {
	Top()->SetVarValue(name, value);
}
NppValue& ScopeStack::GetVarValue(const std::string& name) {
	NppValue* pValue = FindVariable(name);
	if (!pValue)
		throw SyntaxError(PERROR_SYMBOL_NOT_EXISTS, ("Variable \"" + name + "\" not exists").c_str());
	return *pValue;
}

VariableStruct ScopeStack::CallFunction(const std::string& name, const NppFuncArgs& args) {
	NppFunc func = FindFunction(name);
	if (!func)
		throw SyntaxError(PERROR_SYMBOL_NOT_EXISTS, ("Function \"" + name + "\" not exists").c_str());
	return func(args);
}

const std::shared_ptr<SymbolTable>& ScopeStack::GetGlobalTable() const noexcept {
	return m_SymbolTables.front();
}
const std::shared_ptr<SymbolTable>& ScopeStack::Top() const noexcept {
	return m_SymbolTables.back();
}

NppValue* ScopeStack::FindVariable(const std::string& name) noexcept {
	for (auto it = m_SymbolTables.rbegin(); it != m_SymbolTables.rend(); ++it) {
		NppValue* pValue = (*it)->GetVarValuePtr(name);
		if (pValue)
			return pValue;
	}
	return nullptr;
}

NppFunc ScopeStack::FindFunction(const std::string& name) noexcept {
	for (auto it = m_SymbolTables.rbegin(); it != m_SymbolTables.rend(); ++it) {
		NppFunc func = (*it)->GetFunction(name);
		if (func)
			return func;
	}
	return nullptr;
}