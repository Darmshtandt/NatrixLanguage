#pragma once

#include <memory>
#include "SymbolTable.h"

class ScopeStack {
public:
	ScopeStack();
	ScopeStack(ScopeStack&& stack) noexcept;

	void CreateLocalScope();
	void ReleaseLocalScope();

	void CreateVar(const std::string& name);
	void CreateVar(const std::string& name, const NppValue& value);

	void SetVarValue(const std::string& name, const NppValue& value);
	NppValue& GetVarValue(const std::string& name);

	VariableStruct CallFunction(const std::string& name, const NppFuncArgs& args);

	[[nodiscard]] const SharedSymbolTable& GetGlobalTable() const noexcept;
	[[nodiscard]] const SharedSymbolTable& Top() const noexcept;

private:
	std::list<SharedSymbolTable> m_SymbolTables;

private:
	NppValue* FindVariable(const std::string& name) noexcept;
	NppFunc* FindFunction(const std::string& name) noexcept;
};