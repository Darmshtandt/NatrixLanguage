#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

enum class TokenType : unsigned {
	Semicolon,
	Symbol,
	Comma,
	SingleComment,
	MultiComment,

	// Keywords
	Null,

	If,
	Else,
	For,
	While,

	Var,
	Func,
	Return,

	IntVal,
	FloatVal,
	BoolVal,
	StrVal,

	// Arithmetic
	Add,
	Sub,
	Mult,
	Div,
	Mod,
	Inc,
	Dec,

	// Bitwise
	BitAnd,
	BitOr,
	BitNot,

	// Logical
	LogicalAnd,
	LogicalOr,
	LogicalNot,

	// Equality
	Equality,
	Inequality,

	// Assignments
	Assignment,

	// Brackets
	BracketL,
	BracketR,
	CodeBlockL,
	CodeBlockR,

	_EOF,
};

struct LanguageMap {
	static const std::unordered_map<std::string, TokenType> Operators;
	static const std::unordered_map<std::string, TokenType> Keywords;
};

struct TextPosition final {
	int Line = 1;
	int Charaster = 1;
};

struct Token final {
	explicit Token(TokenType type, std::string value, TextPosition position) noexcept;

	TokenType Type;
	std::string Value;
	TextPosition Position;
};

class Lexer final {
public:
	enum Seek : size_t {
		SEEK_START = 0,
		SEEK_EOF = static_cast<size_t>(-1)
	};

public:
	explicit Lexer(std::string inputText);

	Token GetNextToken();
	TextPosition GetTextPosition() const noexcept;
	size_t GetSeek() const noexcept;

	void SetSeek(size_t seek) noexcept;

private:
	std::unordered_set<char> m_OperatorChars;
	std::string m_InputText;
	std::vector<size_t> m_NewLineSeeks;
	size_t m_Seek = SEEK_EOF;

private:
	void Advance() noexcept;
	char GetNextChar() noexcept;
	char GetCurrentChar() const noexcept;

	Token CreateToken(TokenType type, std::string value);

	std::string ParseString(char chr);
	std::string ParseOperator(char chr);
	std::string ParseName(char chr);
	std::string ParseNumber(char chr);

	bool IsCorrectForName(char chr) const noexcept;
};