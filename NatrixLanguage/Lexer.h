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
	[[nodiscard]] static std::string TokenToString(TokenType type);

	static const std::unordered_map<std::string, TokenType> Other;
	static const std::unordered_map<std::string, TokenType> Operators;
	static const std::unordered_map<std::string, TokenType> Keywords;
};

struct TextPosition final {
	size_t Line = 1;
	size_t Charaster = 1;
};

struct Token final {
	Token(TokenType type, std::string value, TextPosition position, size_t length) noexcept;

	TokenType Type;
	std::string Value;
	size_t Length = 0;
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

	Token SetSeek(size_t seek) noexcept;

private:
	size_t m_MaxOperatorLength = 0;
	std::unordered_set<std::string> m_Operators;
	std::unordered_set<char> m_OperatorChars;
	std::string m_InputText;
	std::vector<size_t> m_NewLineSeeks;
	size_t m_Seek = SEEK_EOF;
	size_t m_LastTokenSize = 0;

private:
	void Advance() noexcept;
	void Advance(size_t offset) noexcept;
	char GetNextChar() noexcept;
	char GetCurrentChar() const noexcept;
	char Peek(size_t offset) const noexcept;

	Token CreateToken(TokenType type, std::string value, size_t length);

	std::string ParseString(char chr);
	std::string ParseOperator(char chr);
	std::string ParseName(char chr);
	std::string ParseNumber(char chr);

	bool IsCorrectForName(char chr) const noexcept;
};