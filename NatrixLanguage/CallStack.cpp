#include "CallStack.h"

#include <cassert>

CallStack::CallStack(CallStack&& stack) noexcept:
	m_Frames(std::move(stack.m_Frames)) {
}

void CallStack::Call(const std::shared_ptr<FuncStruct>& pFunc, size_t returnSeek) {
	m_Frames.emplace_back(pFunc, returnSeek);
}

size_t CallStack::Return() noexcept {
	const size_t seek = m_Frames.back().ReturnSeek;
	m_Frames.pop_back();
	return seek;
}