#pragma once

#include "Lexer.h"
#include "ScopeStack.h"

class Parser {
public:
	explicit Parser(const Lexer& lexer);

	void ParseAll();

	void AnyCode();
	void CodeBlock();
	void Code();

	void Var();
	void If();
	void While();
	void For();

	VariableStruct1 Value();
	VariableStruct1 TypeValue();
	VariableStruct1 Symbol();
	NppFuncArgs PresentArguments();

	VariableStruct1 Operator();
	VariableStruct1 Operator_Assignment();
	VariableStruct1 Operator_rValue();

	VariableStruct1 Operator_Equality();

	VariableStruct1 Operator_Unary();
	VariableStruct1 Operator_MultDivMod();
	VariableStruct1 Operator_Prefix();
	VariableStruct1 Operator_Primary();

	void SkipAnyCode();
	void SkipCodeBlock();
	void SkipCode();

	TextPosition GetTextPosition() const noexcept;
	ScopeStack& GetScopeStack() noexcept;

private:
	Lexer m_Lexer;
	Token m_CurrentToken;
	ScopeStack m_ScopeStack;

private:
	void Consume(TokenType type);
	void Next();

	bool IsValue() const noexcept;
	bool IsTypeValue() const noexcept;
};