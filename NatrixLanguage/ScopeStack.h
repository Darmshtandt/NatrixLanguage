#pragma once

#include "SymbolTable.h"

class ScopeStack {
public:
	ScopeStack();

	void CreateLocalScope();
	void ReleaseLocalScope();

	void CreateVar(const std::string& name);
	void CreateVar(const std::string& name, const NppValue& value);

	void SetVarValue(const std::string& name, const NppValue& value);
	NppValue& GetVarValue(const std::string& name);

	void CreateFunction(std::string name, NppFunc func);
	VariableStruct1 CallFunction(const std::string& name, const NppFuncArgs& args);

	__forceinline SymbolTable& GetGlobalTable() noexcept;

private:
	std::vector<SymbolTable> m_SymbolTables;

private:
	__forceinline SymbolTable& TopScope() noexcept;
	NppValue* FindVariable(const std::string& name) noexcept;
	NppFunc* FindFunction(const std::string& name) noexcept;
};
