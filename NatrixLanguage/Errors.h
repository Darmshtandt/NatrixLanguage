#pragma once

#include <exception>
#include <unordered_map>

using ErrorCode = unsigned int;

constexpr ErrorCode LEXER_ERROR = 1000;
constexpr ErrorCode PARSER_ERROR = 2000;

enum {
	LERROR_INVALID_SYNTAX = LEXER_ERROR,
	LERROR_INCORRECT_VALUE,

	PERROR_INVALID_SYNTAX = PARSER_ERROR,
	PERROR_NUMBER_ARGUMENTS,
	PERROR_UNKOWN_TOKEN,
	PERROR_CANNOT_TO_RVALUE,
	PERROR_OPERATOR_NOT_APPLICABLE,
	PERROR_INCORRECT_VALUE,
	PERROR_UNKOWN_TYPE,
};

inline static const std::unordered_map<ErrorCode, char const* const> g_ErrorMap = {
	{ 0, "Unknown error" },
	{ LERROR_INVALID_SYNTAX, "Invalid syntax" },
	{ LERROR_INCORRECT_VALUE, "Incorrect value" },

	{ PERROR_INVALID_SYNTAX, "Invalid syntax" },
	{ PERROR_NUMBER_ARGUMENTS, "Incorrect number of arguments" },
	{ PERROR_UNKOWN_TOKEN, "Unknown token" },
	{ PERROR_CANNOT_TO_RVALUE, "Cannot apply operator to rvalue" },
	{ PERROR_OPERATOR_NOT_APPLICABLE, "The operator is not applicable to this value" },
	{ PERROR_INCORRECT_VALUE, "Incorrect value" },
	{ PERROR_UNKOWN_TYPE, "Unknown type" },
};

__forceinline char const* const GetErrorText(ErrorCode code) noexcept {
	if (g_ErrorMap.contains(code))
		return g_ErrorMap.at(code);
	return g_ErrorMap.at(0);
}

class SyntaxError : public std::exception {
public:
	explicit SyntaxError(ErrorCode code) :
		std::exception(GetErrorText(code)),
		Code(code)
	{
	}
	explicit SyntaxError(char const* const msg) :
		std::exception(msg),
		Code(0)
	{
	}

	const ErrorCode Code;
};