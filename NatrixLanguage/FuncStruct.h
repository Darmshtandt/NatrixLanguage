#pragma once

#include <functional>
#include "ValueStruct.h"
#include <memory>

class ScopeTree;
using NppFuncArgs = std::vector<VariableStruct>;
using NppFunc = std::function<VariableStruct(const NppFuncArgs&)>;


struct FuncStruct {
	std::shared_ptr<ScopeTree> Scope;
	NppFuncArgs Args;
	size_t FuncSeek;
};