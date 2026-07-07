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

	void CreateFunction(std::string name, NppFunc func);
	void CreateFunction(std::string name, NppFuncArgs&& args, std::vector<Token>&& body);
	VariableStruct1 CallFunction(const std::string& name, const NppFuncArgs& args);

	[[nodiscard]] const std::shared_ptr<SymbolTable>& GetGlobalTable() const noexcept;
	[[nodiscard]] const std::shared_ptr<SymbolTable>& TopScope() const noexcept;

private:
	std::list<std::shared_ptr<SymbolTable>> m_SymbolTables;

private:
	NppValue* FindVariable(const std::string& name) noexcept;
	NppFunc* FindFunction(const std::string& name) noexcept;
};