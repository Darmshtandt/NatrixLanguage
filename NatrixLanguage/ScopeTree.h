#pragma once

#include "SymbolTable.h"

class ScopeTree {
public:
	explicit ScopeTree(ScopeTree* pParent);
	~ScopeTree() noexcept = default;

	[[nodiscard]] std::shared_ptr<ScopeTree> CreateScope();
	void PopScope() noexcept;

	[[nodiscard]] const ScopeTree* Last() const noexcept;
	[[nodiscard]] SymbolTable* Table() const noexcept;

private:
	std::list<std::shared_ptr<ScopeTree>> m_Nodes;
	std::unique_ptr<SymbolTable> m_Table;
	ScopeTree* m_pParent;
};