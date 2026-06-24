#include "Parser.h"

#include <cassert>
#include <stdexcept>

#include "Errors.h"

Parser::Parser(const Lexer& lexer) :
	m_Lexer(lexer),
	m_CurrentToken(m_Lexer.GetNextToken())
{
}

void Parser::ParseAll() {
	while (m_CurrentToken.Type != TokenType::_EOF)
		AnyCode();
}

void Parser::Consume(TokenType type) {
	if (m_CurrentToken.Type != type) {
		std::string errorMsg;
		errorMsg = "The token \"";
		errorMsg += std::to_string(static_cast<int>(type));
		errorMsg += "\" was expected, but \"";
		errorMsg += std::to_string(static_cast<int>(m_CurrentToken.Type));
		errorMsg += "\" was found.";

		throw SyntaxError(errorMsg.c_str());
	}

	Next();
}

void Parser::Next() {
	if (m_CurrentToken.Type == TokenType::_EOF)
		throw SyntaxError(PERROR_INVALID_SYNTAX);

	m_CurrentToken = m_Lexer.GetNextToken();
}

bool Parser::IsValue() const noexcept {
	if (m_CurrentToken.Type == TokenType::Symbol)
		return true;
	return IsTypeValue();
}

bool Parser::IsTypeValue() const noexcept {
	switch (m_CurrentToken.Type) {
	case TokenType::IntVal:
	case TokenType::FloatVal:
	case TokenType::BoolVal:
	case TokenType::StrVal:
		return true;
	}
	return false;
}


// AnyCode ::= Code | CodeBlock
void Parser::AnyCode() {
	if (m_CurrentToken.Type == TokenType::CodeBlockL)
		return CodeBlock();
	Code();
}

// CodeBlock ::= '{' (Code)* '}'
void Parser::CodeBlock() {
	Consume(TokenType::CodeBlockL);
	m_ScopeStack.CreateLocalScope();

	while (m_CurrentToken.Type != TokenType::_EOF && m_CurrentToken.Type != TokenType::CodeBlockR)
		AnyCode();

	m_ScopeStack.ReleaseLocalScope();
	Consume(TokenType::CodeBlockR);
}

// Code ::= (Var | Symbol) ';' | If | For | While
// | Func | Return
void Parser::Code() {
	switch (m_CurrentToken.Type) {
	case TokenType::Var:
		Var();
		Consume(TokenType::Semicolon);
		return;

	case TokenType::Symbol:
		Symbol();
		Consume(TokenType::Semicolon);
		return;

	case TokenType::If:
		return If();

	case TokenType::For:
		return For();

	case TokenType::While:
		return While();
	}

	std::string errorMsg;
	errorMsg = "Unknown token: \"";
	errorMsg += std::to_string(static_cast<int>(m_CurrentToken.Type));
	errorMsg += "\"";

	throw SyntaxError(errorMsg.c_str());
}


// Var ::= 'var' Symbol ('=' Operator_rValue)?
void Parser::Var() {
	Consume(TokenType::Var);

	const std::string varName = m_CurrentToken.Value;
	Consume(TokenType::Symbol);

	if (m_CurrentToken.Type == TokenType::Assignment) {
		Next();
		m_ScopeStack.CreateVar(varName, Operator_rValue().ValueStruct);
	}
	else {
		m_ScopeStack.CreateVar(varName);
	}
}

// If ::= ('if' '(' Operator ')' AnyCode ('else' AnyCode)?)*
void Parser::If() {
	Consume(TokenType::If);

	Consume(TokenType::BracketL);
	const bool hasTrue = Operator().ValueStruct.IsTrue();
	Consume(TokenType::BracketR);

	if (hasTrue)
		AnyCode();
	else
		SkipAnyCode();

	if (m_CurrentToken.Type != TokenType::Else)
		return;

	Next();

	if (hasTrue) {
		do {
			SkipAnyCode();
		}
		while (m_CurrentToken.Type == TokenType::Else);
	}
	else if (m_CurrentToken.Type == TokenType::If) {
		If();
	}
	else {
		AnyCode();
	}
}

// While ::= 'while' '(' Operator ')' AnyCode
void Parser::While() {
	Consume(TokenType::While);
	Consume(TokenType::BracketL);

	const size_t beginCycleSeek = m_Lexer.GetSeek();
	while (true) {
		const bool hasTrue = Operator().ValueStruct.IsTrue();
		Consume(TokenType::BracketR);

		if (!hasTrue) {
			SkipAnyCode();
			return;
		}

		AnyCode();
		m_Lexer.SetSeek(beginCycleSeek);
	}
}

// For ::= 'for' '(' (Var)? ';' (Operator)? ';' (Operator)? ')' AnyCode
void Parser::For() {
	Consume(TokenType::For);
	Consume(TokenType::BracketL);

	m_ScopeStack.CreateLocalScope();
	if (m_CurrentToken.Type == TokenType::Var)
		Var();
	else if (m_CurrentToken.Type != TokenType::Semicolon)
		Operator();
	Consume(TokenType::Semicolon);

	const size_t conditionSeek = m_Lexer.GetSeek();
	bool isTrue = true;
	if (m_CurrentToken.Type != TokenType::Semicolon)
		isTrue = Operator().ValueStruct.IsTrue();
	Consume(TokenType::Semicolon);

	const size_t expressionSeek = m_Lexer.GetSeek();
	int numBrackets = 0;
	while (m_CurrentToken.Type != TokenType::BracketR || numBrackets > 0) {
		if (m_CurrentToken.Type == TokenType::BracketL)
			++numBrackets;
		else if (m_CurrentToken.Type == TokenType::BracketR)
			--numBrackets;
		Next();
	}
	Next();

	if (!isTrue) {
		SkipAnyCode();
		return;
	}

	const size_t beginCycleSeek = m_Lexer.GetSeek();
	while (isTrue) {
		AnyCode();

		const size_t endCycleSeek = m_Lexer.GetSeek();

		m_Lexer.SetSeek(expressionSeek);
		if (m_CurrentToken.Type != TokenType::Semicolon)
			Operator();

		m_Lexer.SetSeek(conditionSeek);
		if (m_CurrentToken.Type != TokenType::Semicolon)
			isTrue = Operator().ValueStruct.IsTrue();

		m_Lexer.SetSeek(isTrue ? beginCycleSeek : endCycleSeek);
	}

	m_ScopeStack.ReleaseLocalScope();
}

// ValueStruct ::= TypeValue | Symbol
VariableStruct1 Parser::Value() {
	if (IsTypeValue())
		return TypeValue();
	return Symbol();
}

// TypeValue = IntVal | FloatVal | BoolVal | StrVal
VariableStruct1 Parser::TypeValue() {
	VariableStruct1 value;
	switch (m_CurrentToken.Type) {
	case TokenType::IntVal:
		value = { "", NppValue(std::stoi(m_CurrentToken.Value)) };
		Next();
		return value;
		
	case TokenType::FloatVal:
		value = { "", NppValue(std::stof(m_CurrentToken.Value)) };
		Next();
		return value;

	case TokenType::BoolVal:
		value = { "", NppValue(m_CurrentToken.Value != "false" || m_CurrentToken.Value != "0") };
		Next();
		return value;

	case TokenType::StrVal:
		value = { "", NppValue(m_CurrentToken.Value) };
		Next();
		return value;
	}

	std::string errorMsg;
	errorMsg = "Unknown value: Token \"";
	errorMsg += std::to_string(static_cast<int>(m_CurrentToken.Type));
	errorMsg += "\"; value \"";
	errorMsg += m_CurrentToken.Value;
	errorMsg += "\"";

	throw SyntaxError(errorMsg.c_str());
}

// Symbol ::= Symbol ('(' PresentArguments ')')?
VariableStruct1 Parser::Symbol() {
	const std::string name = m_CurrentToken.Value;
	Consume(TokenType::Symbol);

	if (m_CurrentToken.Type == TokenType::BracketL) {
		Next();
		const NppFuncArgs args = PresentArguments();
		Consume(TokenType::BracketR);

		return m_ScopeStack.CallFunction(name, args);
	}

	return VariableStruct1(name, m_ScopeStack.GetVarValue(name));
}

// PresentArguments ::= \0 | (ValueStruct (',' ValueStruct)*)
NppFuncArgs Parser::PresentArguments() {
	if (!IsTypeValue() && m_CurrentToken.Type != TokenType::Symbol)
		return { };

	NppFuncArgs args;
	args.push_back(Value());

	while (m_CurrentToken.Type == TokenType::Comma) {
		Next();
		args.push_back(Value());
		Consume(TokenType::Symbol);
	}

	return args;
}


// Operator ::= Operator_assignment
VariableStruct1 Parser::Operator() {
	return Operator_Assignment();
}

// Operator_assignment ::= Operator_rValue ('=' Operator)?
VariableStruct1 Parser::Operator_Assignment() {
	VariableStruct1 var = Operator_rValue();
	if (m_CurrentToken.Type != TokenType::Assignment)
		return var;

	if (var.VarName.empty())
		throw SyntaxError(PERROR_OPERATOR_NOT_APPLICABLE);

	Next();
	VariableStruct1 rValue = Operator_rValue();
	m_ScopeStack.SetVarValue(var.VarName, rValue.ValueStruct);
	return VariableStruct1(var.VarName, rValue.ValueStruct);
}

// Operator_rValue = Operator_logicalor
VariableStruct1 Parser::Operator_rValue() {
	return Operator_Equality();
}

// Operator_logicalor ::= Operator_logicaland ('||' Operator)*
// Operator_logicaland ::= Operator_bitor ('&&' Operator)*
// Operator_bitor ::= Operator_bitxor ('|' Operator)*
// Operator_bitxor ::= Operator_bitand ('^' Operator)*
// Operator_bitand ::= Operator_Equality ('&' Operator)*

// Operator_Equality ::= Operator_ltgt (('==' | '!=') Operator)*
VariableStruct1 Parser::Operator_Equality() {
	VariableStruct1 left = Operator_Unary();
	if (m_CurrentToken.Type != TokenType::Equality && m_CurrentToken.Type != TokenType::Inequality)
		return left;

	const bool isOperatorEquality = m_CurrentToken.Type == TokenType::Equality;
	Next();

	const VariableStruct1 right = Operator();
	const bool isValuesEquals = (left.ValueStruct <=> right.ValueStruct) == 0;

	if (isValuesEquals == isOperatorEquality)
		return VariableStruct1("", NppValue(true));
	return VariableStruct1("", NppValue(false));
}

// Operator_ltgt ::= Operator_bitshift
// Operator_bitshift ::= Operator_unary

// Operator_unary ::= Operator_MultDivMod (('+' | '-') Operator)*
VariableStruct1 Parser::Operator_Unary() {
	VariableStruct1 left = Operator_MultDivMod();
	if (m_CurrentToken.Type != TokenType::Add && m_CurrentToken.Type != TokenType::Sub)
		return left;

	const bool isAdd = m_CurrentToken.Type == TokenType::Add;
	Next();

	const VariableStruct1 right = Operator();
	if (isAdd)
		return VariableStruct1("", left.ValueStruct + right.ValueStruct);
	return VariableStruct1("", left.ValueStruct - right.ValueStruct);
}

// Operator_MultDivMod ::= Operator_Prefix (('*' | '/' | '%') Operator)*
VariableStruct1 Parser::Operator_MultDivMod() {
	VariableStruct1 var = Operator_Prefix();

	switch (m_CurrentToken.Type) {
	case TokenType::Mult:
		Next();
		var.ValueStruct *= Operator().ValueStruct;
		break;

	case TokenType::Div:
		Next();
		var.ValueStruct /= Operator().ValueStruct;
		break;

	case TokenType::Mod:
		Next();
		var.ValueStruct %= Operator().ValueStruct;
		break;
	}

	return var;
}

// Operator_Prefix ::= ('++' | '--' | '+' | '-' | '!' | '~')? Operator_Primary
VariableStruct1 Parser::Operator_Prefix() {
	bool hasPrefix = false;
	bool isRequiredVarName = false;

	switch (m_CurrentToken.Type) {
	case TokenType::Inc:
	case TokenType::Dec:
		isRequiredVarName = true;

	case TokenType::Add:
	case TokenType::Sub:
	case TokenType::LogicalNot:
	case TokenType::BitNot:
		hasPrefix = true;
		break;
	}

	if (!hasPrefix)
		return Operator_Primary();

	const TokenType op = m_CurrentToken.Type;
	Next();
	VariableStruct1 var = Operator_Primary();

	if (isRequiredVarName && var.VarName.empty())
		throw SyntaxError(PERROR_CANNOT_TO_RVALUE);

	switch (op) {
	case TokenType::Inc:
		var.ValueStruct = ++m_ScopeStack.GetVarValue(var.VarName);
		return var;

	case TokenType::Dec:
		var.ValueStruct = --m_ScopeStack.GetVarValue(var.VarName);
		return var;

	case TokenType::Add:
		return var;

	case TokenType::Sub:
		var.ValueStruct = -var.ValueStruct;
		return var;

	case TokenType::LogicalNot:
		var.ValueStruct = !var.ValueStruct;
		return var;

	case TokenType::BitNot:
		var.ValueStruct = ~var.ValueStruct;
		return var;
	}

	assert(0);
}

// Operator_Primary ::= Value | '(' Operator ')' | Operator ('++' | '--')
VariableStruct1 Parser::Operator_Primary() {
	if (m_CurrentToken.Type == TokenType::BracketL) {
		Next();
		const VariableStruct1 result = Operator();
		Consume(TokenType::BracketR);
		return result;
	}

	VariableStruct1 var = Value();
	if (m_CurrentToken.Type != TokenType::Inc && m_CurrentToken.Type != TokenType::Dec)
		return var;

	if (var.VarName.empty())
		throw SyntaxError(PERROR_CANNOT_TO_RVALUE);

	switch (m_CurrentToken.Type) {
	case TokenType::Inc:
		++m_ScopeStack.GetVarValue(var.VarName);
		break;
	case TokenType::Dec:
		--m_ScopeStack.GetVarValue(var.VarName);
		break;
	}
	return var;
}


void Parser::SkipAnyCode() {
	if (m_CurrentToken.Type == TokenType::CodeBlockL)
		SkipCodeBlock();
	else
		SkipCode();
}
void Parser::SkipCodeBlock() {
	Consume(TokenType::CodeBlockL);
	while (m_CurrentToken.Type != TokenType::CodeBlockR)
		SkipCode();
	Consume(TokenType::CodeBlockR);
}
void Parser::SkipCode() {
	while (m_CurrentToken.Type != TokenType::Semicolon)
		Next();
	Consume(TokenType::Semicolon);
}

TextPosition Parser::GetTextPosition() const noexcept {
	return m_Lexer.GetTextPosition();
}

ScopeStack& Parser::GetScopeStack() noexcept {
	return m_ScopeStack;
}