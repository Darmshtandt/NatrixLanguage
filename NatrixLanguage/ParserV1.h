#pragma once

#include <memory>
#include "Lexer.h"

class BaseParser {
public:
	explicit BaseParser(Lexer& lexer) noexcept;

	void Consume(TokenType type);
	void Next();

protected:
	Token m_CurrentToken;
	Token m_NextToken;

private:
	Lexer* m_pLexer;
};

class ParserOperator;

class ParserValue : public BaseParser {
public:
	explicit ParserValue(Lexer& lexer) noexcept;

	std::string Value();
	std::string Symbol();
	std::string TypeValue();
	std::string Arguments();

private:
	std::unique_ptr<ParserOperator> m_pParserOperator;
};

class ParserOperator : public BaseParser {
public:
	explicit ParserOperator(Lexer& lexer) noexcept;

	std::string Operator();
	std::string Operator_assignment();
	std::string Operator_rvalue();
	std::string Operator_logicalor();
	std::string Operator_logicaland();
	std::string Operator_bitor();
	std::string Operator_bitxor();
	std::string Operator_bitand();
	std::string Operator_eq();
	std::string Operator_ltgt();
	std::string Operator_bitshift();
	std::string Operator_unary();
	std::string Operator_mult();
	std::string Operator_prefix();
	std::string Operator_primary();

private:
	std::unique_ptr<ParserValue> m_pParserValue;
};

class ParserKeyword : public BaseParser {
public:
	explicit ParserKeyword(Lexer& lexer) noexcept;

	void Var();
	void If();
	void For();
	void While();
	void Func();
	void Return();

private:
	ParserOperator m_ParserOperator;
};

class ParserLanguage : public BaseParser {
public:
	explicit ParserLanguage(Lexer& lexer) noexcept;

	void Code();
	void CodeBlock();
	void AnyCode();

private:
	ParserOperator m_ParserOperator;
	ParserKeyword m_ParserKeyword;
};