#pragma once

#include "ValueStruct.h"
#include <functional>

struct VariableStruct1 final {
	VariableStruct1() noexcept = default;
	VariableStruct1(std::string name, NppValue value) noexcept;

	NppValue ValueStruct;
	std::string VarName;
};

using NppFuncArgs = std::vector<VariableStruct1>;
using NppFunc = std::function<VariableStruct1(const NppFuncArgs&)>;

class SymbolTable {
public:
	void CreateVar(const std::string& name);
	void CreateVar(const std::string& name, const NppValue& value);
	void SetVarValue(const std::string& name, const NppValue& value);
	NppValue* GetVarValuePtr(const std::string& name);

	void CreateFunction(std::string name, NppFunc func);
	VariableStruct1 CallFunction(const std::string& name, const NppFuncArgs& args);
	NppFunc* GetFunctionPtr(const std::string& name);

private:
	std::unordered_map<std::string, NppValue> m_VarMap;
	std::unordered_map<std::string, NppFunc> m_FuncMap;
};