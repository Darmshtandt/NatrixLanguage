#include "ParserV1.h"

#include <cassert>

#include "SyntaxError.h"

// ============================================================================
//	BaseParser
// ============================================================================
BaseParser::BaseParser(Lexer& lexer) noexcept :
	m_CurrentToken(lexer.GetNextToken()),
	m_NextToken(lexer.GetNextToken()),
	m_pLexer(&lexer)
{
}

void BaseParser::Consume(TokenType type) {
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

void BaseParser::Next() {
	if (m_CurrentToken.Type == TokenType::_EOF)
		throw SyntaxError("Invalid syntax");

	m_CurrentToken = m_NextToken;
	if (m_NextToken.Type != TokenType::_EOF)
		m_NextToken = m_pLexer->GetNextToken();
}

// ============================================================================
//	ParserValue
// ============================================================================
ParserValue::ParserValue(Lexer& lexer) noexcept :
	BaseParser(lexer)
{
	m_pParserOperator = std::make_unique<ParserOperator>(lexer);
}

// Value ::= Symbol | TypeValue | Operator_rvalue
std::string ParserValue::Value() {
	if (m_CurrentToken.Type == TokenType::Symbol)
		return Symbol();

	static const std::unordered_set types = {
		TokenType::IntVal, TokenType::FloatVal, TokenType::BoolVal, TokenType::StrVal
	};

	if (types.contains(m_CurrentToken.Type))
		return TypeValue();
	return m_pParserOperator->Operator();
}

// Symbol ::= Symbol ('(' Arguments ')')?
std::string ParserValue::Symbol() {
	Consume(TokenType::Symbol);
	assert(0);

	std::string retult;
	if (m_CurrentToken.Type == TokenType::BracketL) {
		Consume(TokenType::BracketL);
		retult = Arguments();
		Consume(TokenType::BracketR);
		assert(0);
	}
	return retult;
}

// TypeValue = IntVal | FloatVal | BoolVal | StrVal
std::string ParserValue::TypeValue() {
	switch (m_CurrentToken.Type) {
	case TokenType::IntVal:
	case TokenType::FloatVal:
	case TokenType::BoolVal:
	case TokenType::StrVal:
		Next();
		return m_CurrentToken.Value;
	}

	throw ValueError("Unknown value type");
}

// Arguments :: = (Var(',' Var)*) | \0
std::string ParserValue::Arguments() {
	assert(0);
}

// ============================================================================
//	ParserOperator
// ============================================================================
ParserOperator::ParserOperator(Lexer& lexer) noexcept :
	BaseParser(lexer)
{
	m_pParserValue = std::make_unique<ParserValue>(lexer);
}

// Operator ::= Operator_assignment
std::string ParserOperator::Operator() {
	return Operator_assignment();
}

// Operator_assignment ::= (Symbol '=')? Operator_rvalue
std::string ParserOperator::Operator_assignment() {
	return Operator_rvalue();
}

// Operator_rvalue = Operator_logicalor
std::string ParserOperator::Operator_rvalue() {
	return Operator_logicalor();
}

// Operator_logicalor ::= Operator_logicaland ('||' Operator_logicaland)*
std::string ParserOperator::Operator_logicalor() {
	return Operator_logicaland();
}

// Operator_logicaland ::= Operator_bitor ('&&' Operator_bitor)*
std::string ParserOperator::Operator_logicaland() {
	return Operator_bitor();
}

// Operator_bitor ::= Operator_bitxor ('|' Operator_bitxor)*
std::string ParserOperator::Operator_bitor() {
	return Operator_bitxor();
}

// Operator_bitxor ::= Operator_bitand ('^' Operator_bitand)*
std::string ParserOperator::Operator_bitxor() {
	return Operator_bitand();
}

// Operator_bitand ::= Operator_eq ('&' Operator_eq)*
std::string ParserOperator::Operator_bitand() {
	return Operator_eq();
}

// Operator_eq ::= Operator_ltgt
std::string ParserOperator::Operator_eq() {
	return Operator_ltgt();
}

// Operator_ltgt ::= Operator_bitshift
std::string ParserOperator::Operator_ltgt() {
	return Operator_bitshift();
}

// Operator_bitshift ::= Operator_unary
std::string ParserOperator::Operator_bitshift() {
	return Operator_unary();
}

// Operator_unary ::= Operator_mult (('+' | '-') Operator_mult)*
std::string ParserOperator::Operator_unary() {
	return Operator_mult();
}

// Operator_mult ::= Operator_prefix (('*' | '/' | '%') Operator_prefix)*
std::string ParserOperator::Operator_mult() {
	return Operator_prefix();
}

// Operator_prefix ::= ('++' | '--' | '+' | '-' | '!' | '~') Symbol | Operator_primary
std::string ParserOperator::Operator_prefix() {
	switch (m_CurrentToken.Type) {
	case TokenType::Inc:
	case TokenType::Dec:
	case TokenType::Add:
	case TokenType::Sub:
	case TokenType::LogicalNot:
	case TokenType::BitNot:
		Next();
		std::string value;
		Consume(TokenType::Symbol);
		assert(0);
		return value;
	}

	return Operator_primary();
}

// Operator_primary ::= Value | '(' Operator ')' | Symbol ('++' | '--')
std::string ParserOperator::Operator_primary() {
	if (m_CurrentToken.Type == TokenType::BracketL) {
		Next();
		const std::string result = Operator();
		Consume(TokenType::BracketR);
		return result;
	}

	if (m_CurrentToken.Type == TokenType::Symbol) {
		if (m_NextToken.Type == TokenType::Inc || m_NextToken.Type == TokenType::Dec) {
			std::string value;
			Next();
			Next();
			assert(0);
			return value;
		}
	}

	return m_pParserValue->Value();
}

// ============================================================================
//	ParserKeyword
// ============================================================================
ParserKeyword::ParserKeyword(Lexer& lexer) noexcept:
	BaseParser(lexer),
	m_ParserOperator(lexer)
{
}

// Var ::= 'var' Symbol ('=' Operator_rvalue)? ';'
void ParserKeyword::Var() {
	Consume(TokenType::Var);
	Consume(TokenType::Symbol);

	if (m_CurrentToken.Type == TokenType::Assignment) {
		Next();
		std::string value = m_ParserOperator.Operator();
		assert(0);
	}

	Consume(TokenType::Semicolon);
}

// If ::= 'if' '(' Operator ')' AnyCode ('else' AnyCode)?
void ParserKeyword::If() {
}

// For ::= 'for' '(' Var ';' Operator ';' Expression ')' AnyCode | '(;' Operator ';' Expression ')' AnyCode
void ParserKeyword::For() {
}

// While ::= 'while' '(' Operator ')' AnyCode
void ParserKeyword::While() {
}

// Func ::= 'func' Symbol '(' Arguments ')' CodeBlock
void ParserKeyword::Func() {
}

// Return ::= 'return' Value ';'
void ParserKeyword::Return() {
}

// ============================================================================
//	ParserLanguage
// ============================================================================
ParserLanguage::ParserLanguage(Lexer& lexer) noexcept:
	BaseParser(lexer),
	m_ParserOperator(lexer),
	m_ParserKeyword(lexer)
{
}

// Code ::= Symbol | Var | If | For | While | Func | Return
void ParserLanguage::Code() {
	switch (m_CurrentToken.Type) {
	case TokenType::Symbol:
	case TokenType::If:
	case TokenType::For:
	case TokenType::While:
	case TokenType::Func:
	case TokenType::Return:
		assert(0);

	case TokenType::Var:
		return m_ParserKeyword.Var();
	}

	throw SyntaxError("Invalid syntax");
}

// CodeBlock ::= '{' Code '}'
void ParserLanguage::CodeBlock() {
}

// AnyCode ::= Code | CodeBlock
void ParserLanguage::AnyCode() {
}