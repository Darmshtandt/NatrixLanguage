#include "Lexer.h"
#include "Errors.h"

#include <cassert>

std::string Unescape(const std::string& text) {
	std::string result;
	result.reserve(text.size());

	const std::unordered_map<char, char> escapesMap = {
		{'n','\n'}, {'t','\t'}, {'r','\r'}, {'a','\a'},
		{'b','\b'}, {'f','\f'}, {'v','\v'}, {'\\','\\'},
		{'\'','\''},{'"','"'}
	};

	for (size_t i = 0; i < text.size(); ++i) {
		if (text[i] != '\\' || i + 1 >= text.size()) {
			result += text[i];
			continue;
		}

		const char nextChr = text[++i];
		result += (escapesMap.contains(nextChr))
			? escapesMap.at(nextChr)
			: nextChr;
	}

	return result;
}

Token::Token(TokenType type, std::string value, TextPosition position) noexcept :
	Type(type),
	Value(std::move(value)),
	Position(position)
{
}

const std::unordered_map<std::string, TokenType> LanguageMap::Operators = {
	{ "+", TokenType::Add },
	{ "-", TokenType::Sub },
	{ "*", TokenType::Mult },
	{ "/", TokenType::Div },
	{ "%", TokenType::Mod },
	{ "++", TokenType::Inc },
	{ "--", TokenType::Dec },

	{ "&", TokenType::BitAnd },
	{ "|", TokenType::BitOr },
	{ "~", TokenType::BitNot },

	{ "&&", TokenType::LogicalAnd },
	{ "||", TokenType::LogicalOr },
	{ "!", TokenType::LogicalNot },

	{ "==", TokenType::Equality },
	{ "!=", TokenType::Inequality },

	{ "(", TokenType::BracketL },
	{ ")", TokenType::BracketR },
	{ "{", TokenType::CodeBlockL },
	{ "}", TokenType::CodeBlockR },

	{ "=", TokenType::Assignment },

	{ "//", TokenType::SingleComment },
	{ "/*", TokenType::MultiComment },
};

const std::unordered_map<std::string, TokenType> LanguageMap::Keywords = {
	{ "null", TokenType::Null },
	{ "true", TokenType::BoolVal },
	{ "false", TokenType::BoolVal },

	{ "if", TokenType::If },
	{ "else", TokenType::Else },
	{ "for", TokenType::For },
	{ "while", TokenType::While },

	{ "var", TokenType::Var },
	{ "func", TokenType::Func },
	{ "return", TokenType::Return },
};


Lexer::Lexer(std::string inputText) :
	m_InputText(std::move(inputText))
{
	if (!m_InputText.empty())
		m_Seek = SEEK_START;

	m_NewLineSeeks.push_back(0);
	for (size_t i = 0; i < m_InputText.length(); ++i) {
		if (m_InputText[i] == '\n')
			m_NewLineSeeks.push_back(i + 1);
	}

	for (auto [op, _] : LanguageMap::Operators) {
		for (char chr : op)
			m_OperatorChars.insert(chr);
	}
}

Token Lexer::GetNextToken() {
	char chr = GetCurrentChar();
	while (chr) {
		if (isspace(chr)) {
			chr = GetNextChar();
			continue;
		}

		if (isdigit(chr)) {
			const std::string part1 = ParseNumber(chr);
			if (chr == '.') {
				const std::string part2 = ParseNumber(GetNextChar());
				return CreateToken(TokenType::FloatVal, part1 + '.' + part2);
			}
			return CreateToken(TokenType::IntVal, part1);
		}

		if (IsCorrectForName(chr)) {
			const std::string name = ParseName(chr);

			if (LanguageMap::Keywords.contains(name)) {
				if (name == "true" || name == "false")
					return CreateToken(LanguageMap::Keywords.at(name), name);
				return CreateToken(LanguageMap::Keywords.at(name), "");
			}
			return CreateToken(TokenType::Symbol, name);
		}

		switch (chr) {
		case ';':
			Advance();
			return CreateToken(TokenType::Semicolon, "");

		case ',':
			Advance();
			return CreateToken(TokenType::Comma, "");

		case '"':
			return CreateToken(TokenType::StrVal, ParseString(chr));
		}

		if (m_OperatorChars.contains(chr)) {
			std::string op = ParseOperator(chr);
			while (!LanguageMap::Operators.contains(op)) {
				op.erase(op.end() - 1);
				--m_Seek;

				if (op.empty())
					break;
			}

			if (op == "//") {
				while (chr != '\n')
					chr = GetNextChar();
				chr = GetNextChar();
				continue;
			}

			if (op == "/*") {
				while (true) {
					if (chr == '*') {
						if ((chr = GetNextChar()) == '/')
							break;
					}
					else {
						chr = GetNextChar();
					}
				}
				chr = GetNextChar();
				continue;
			}

			if (!op.empty())
				return CreateToken(LanguageMap::Operators.at(op), "");
		}

		throw SyntaxError(LERROR_INVALID_SYNTAX);
	}

	return CreateToken(TokenType::_EOF, "");
}

TextPosition Lexer::GetTextPosition() const noexcept {
	TextPosition pos = { };
	switch (m_Seek) {
	case SEEK_START:
		return pos;

	case SEEK_EOF:
		const int penultLineSeek = *(m_NewLineSeeks.end() - 1);
		pos.Line = m_NewLineSeeks.size();
		pos.Charaster = m_InputText.length() - penultLineSeek;
		return pos;
	}

	for (int i = 0; i < m_NewLineSeeks.size(); ++i) {
		if (m_Seek <= m_NewLineSeeks[i]) {
			pos.Line = i;
			pos.Charaster = m_Seek - m_NewLineSeeks[i - 1];
			break;
		}
	}
	return pos;
}

size_t Lexer::GetSeek() const noexcept {
	return m_Seek;
}

void Lexer::SetSeek(size_t seek) noexcept {
	if (seek >= m_InputText.length()) {
		m_Seek = SEEK_EOF;
		return;
	}

	m_Seek = (seek > 0) ? seek : SEEK_START;
}

void Lexer::Advance() noexcept {
	if (m_Seek == SEEK_EOF)
		return;

	++m_Seek;
	if (m_Seek >= m_InputText.length())
		m_Seek = SEEK_EOF;
}

char Lexer::GetNextChar() noexcept {
	Advance();

	if (m_Seek == SEEK_EOF)
		return '\0';
	return m_InputText[m_Seek];
}

char Lexer::GetCurrentChar() const noexcept {
	if (m_Seek == SEEK_EOF)
		return '\0';
	return m_InputText[m_Seek];
}

Token Lexer::CreateToken(TokenType type, std::string value) {
	return Token(type, std::move(value), GetTextPosition());
}

std::string Lexer::ParseString(char chr) {
	assert(chr == '"');

	std::string text;
	while (chr = GetNextChar(), chr && chr != '"')
		text += chr;

	Advance();
	return Unescape(text);
}

std::string Lexer::ParseOperator(char chr) {
	assert(m_OperatorChars.contains(chr));

	std::string op;
	while (chr && m_OperatorChars.contains(chr)) {
		op += chr;
		chr = GetNextChar();
	}
	return op;
}

std::string Lexer::ParseName(char chr) {
	assert(!isdigit(chr));

	std::string word;
	while (chr && IsCorrectForName(chr)) {
		word += chr;
		chr = GetNextChar();
	}
	return word;
}

std::string Lexer::ParseNumber(char chr) {
	if (!isdigit(chr))
		throw SyntaxError(LERROR_INCORRECT_VALUE);

	std::string number;
	bool hasDot = false;
	while (chr && isdigit(chr)) {
		number += chr;
		chr = GetNextChar();
	}
	return number;
}

bool Lexer::IsCorrectForName(char chr) const noexcept {
	if (isalnum(chr))
		return true;

	constexpr const char* correctChars = "_$";
	return strcmp(correctChars, &chr) == 0;
}