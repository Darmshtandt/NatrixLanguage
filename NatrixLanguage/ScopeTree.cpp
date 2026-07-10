#include "ScopeTree.h"
#include <cassert>

ScopeTree::ScopeTree(ScopeTree* pParent) :
	m_pParent(pParent),
	m_Table(new SymbolTable)
{
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