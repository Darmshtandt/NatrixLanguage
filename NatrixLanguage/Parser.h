#pragma once

#include "Lexer.h"
#include "ScopeStack.h"
#include "ScopeTree.h"

class Parser {
public:
	explicit Parser(const Lexer& lexer);

	void ParseAll();

	void AnyCode();
	void CodeBlock();
	void Code();

	void Var();
	void Func();
	void If();
	void While();
	void For();

	VariableStruct Value();
	VariableStruct TypeValue();
	VariableStruct Symbol();
	NppFuncArgs PresentArguments();

	VariableStruct Operator();
	VariableStruct Operator_Assignment();
	VariableStruct Operator_rValue();

	VariableStruct Operator_Equality();

	VariableStruct Operator_Unary();
	VariableStruct Operator_MultDivMod();
	VariableStruct Operator_Prefix();
	VariableStruct Operator_Primary();

	void SkipAnyCode();
	void SkipCodeBlock();
	void SkipCode();

	TextPosition GetTextPosition() const noexcept;
	ScopeStack& GetScopeStack() noexcept;
	ScopeTree& GetScopeTree() noexcept;

private:
	Lexer m_Lexer;
	Token m_CurrentToken;
	ScopeStack m_ScopeStack;
	ScopeTree m_ScopeTree;

private:
	void Consume(TokenType type);
	void Next();

	bool IsValue() const noexcept;
	bool IsTypeValue() const noexcept;
};
