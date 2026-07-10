#pragma once

#include "FuncStruct.h"

class SymbolTable {
public:
	SymbolTable() noexcept = default;
	SymbolTable(SymbolTable&& table) noexcept;

	void CreateVar(const std::string& name);
	void CreateVar(const std::string& name, const NppValue& value);
	void SetVarValue(const std::string& name, const NppValue& value);
	NppValue* GetVarValuePtr(const std::string& name);

	void CreateFunction(std::string name, NppFunc func);
	void CreateFunction(std::string name, NppFuncArgs&& args, std::vector<Token>&& body);

	VariableStruct CallFunction(const std::string& name, const NppFuncArgs& args);
	NppFunc* GetFunctionPtr(const std::string& name);

private:
	std::unordered_map<std::string, NppValue> m_VarMap;
	std::unordered_map<std::string, FuncStruct> m_FuncMap;
	std::unordered_map<std::string, NppFunc> m_ImportedFuncMap;
};

using SharedSymbolTable = std::shared_ptr<SymbolTable>;