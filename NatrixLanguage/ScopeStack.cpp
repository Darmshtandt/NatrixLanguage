#include "ScopeStack.h"

#include <cassert>
#include <stdexcept>

ScopeStack::ScopeStack() {
	m_SymbolTables.emplace_back();
}

ScopeStack::ScopeStack(ScopeStack&& stack) noexcept:
	m_SymbolTables(std::move(stack.m_SymbolTables)) {
}

void ScopeStack::CreateLocalScope() {
	m_SymbolTables.emplace_back();
}
void ScopeStack::ReleaseLocalScope() {
	if (m_SymbolTables.size() == 1)
		throw std::runtime_error("Local scope is empty");
	m_SymbolTables.pop_back();
}

void ScopeStack::CreateVar(const std::string& name) {
	TopScope()->CreateVar(name);
}
void ScopeStack::CreateVar(const std::string& name, const NppValue& value) {
	TopScope()->CreateVar(name, value);
}

void ScopeStack::SetVarValue(const std::string& name, const NppValue& value) {
	TopScope()->SetVarValue(name, value);
}
NppValue& ScopeStack::GetVarValue(const std::string& name) {
	NppValue* pValue = FindVariable(name);
	if (!pValue)
		throw SyntaxError(PERROR_SYMBOL_NOT_EXISTS, ("Variable \"" + name + "\" not exists").c_str());
	return *pValue;
}

void ScopeStack::CreateFunction(std::string name, NppFunc func) {
	TopScope()->CreateFunction(std::move(name), std::move(func));
}

void ScopeStack::CreateFunction(std::string name, NppFuncArgs&& args, std::vector<Token>&& body) {
	TopScope()->CreateFunction(std::move(name), std::move(args), std::move(body));
}

VariableStruct1 ScopeStack::CallFunction(const std::string& name, const NppFuncArgs& args) {
	NppFunc* pFunc = FindFunction(name);
	if (!pFunc)
		throw SyntaxError(PERROR_SYMBOL_NOT_EXISTS, ("Function \"" + name + "\" not exists").c_str());
	return (*pFunc)(args);
}

const std::shared_ptr<SymbolTable>& ScopeStack::GetGlobalTable() const noexcept {
	return m_SymbolTables.front();
}
const std::shared_ptr<SymbolTable>& ScopeStack::TopScope() const noexcept {
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

NppFunc* ScopeStack::FindFunction(const std::string& name) noexcept {
	for (auto it = m_SymbolTables.rbegin(); it != m_SymbolTables.rend(); ++it) {
		NppFunc* pFunc = (*it)->GetFunctionPtr(name);
		if (pFunc)
			return pFunc;
	}
	return nullptr;
}