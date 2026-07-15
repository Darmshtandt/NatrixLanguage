#include "ScopeTree.h"
#include <cassert>

ScopeTree::ScopeTree(ScopeTree* pParent) :
	m_pParent(pParent),
	m_Table(new SymbolTable)
{
}

void ScopeTree::SetVarValue(const std::string& name, const NppValue& value) {
	NppValue* pVar = FindVariable(name);
	if (!pVar)
		throw SyntaxError(PERROR_UNKOWN_IDENTIFIER);
	(*pVar) = value;
}
NppValue& ScopeTree::GetVarValue(const std::string& name) {
	NppValue* pVar = FindVariable(name);
	if (!pVar)
		throw SyntaxError(PERROR_UNKOWN_IDENTIFIER);
	return *pVar;
}

VariableStruct ScopeTree::CallFunction(const std::string& name, const NppFuncArgs& args) {
	NppFunc func = FindFunction(name);
	if (!func)
		throw SyntaxError(PERROR_UNKOWN_IDENTIFIER);
	return func(args);
}

std::shared_ptr<ScopeTree> ScopeTree::CreateScope() {
	return m_Nodes.emplace_back(new ScopeTree(this));
}
void ScopeTree::PopScope() noexcept {
	assert(!m_Nodes.empty());
	m_Nodes.pop_back();
}

const ScopeTree* ScopeTree::Last() const noexcept {
	if (!m_Nodes.empty())
		return m_Nodes.back()->Last();
	return this;
}
SymbolTable* ScopeTree::Table() const noexcept {
	return m_Table.get();
}

NppValue* ScopeTree::FindVariable(const std::string& name) noexcept {
	NppValue* value = m_Table->GetVarValuePtr(name);
	if (value)
		return value;
	if (m_pParent)
		return m_pParent->FindVariable(name);
	return nullptr;
}

NppFunc ScopeTree::FindFunction(const std::string& name) noexcept {
	NppFunc func = m_Table->GetFunction(name);
	if (func)
		return func;
	if (m_pParent)
		return m_pParent->FindFunction(name);
	return nullptr;
}