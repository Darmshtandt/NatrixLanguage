#pragma once

#include "ScopeStack.h"

#include <list>
#include <memory>
#include <string>

class CallStack {
	struct CallFrame final {
		std::shared_ptr<FuncStruct> pFunc = nullptr;
		size_t ReturnSeek;
	};

public:
	CallStack(CallStack&& stack) noexcept;

	void Call(const std::shared_ptr<FuncStruct>& pFunc, size_t returnSeek);
	size_t Return() noexcept;

private:
	std::list<CallFrame> m_Frames;
};