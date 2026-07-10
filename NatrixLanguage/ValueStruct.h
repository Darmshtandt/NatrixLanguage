#pragma once

#include "Lexer.h"
#include "Errors.h"

#include <variant>


class NppValue final {
public:
	using Value = std::variant<
		std::monostate,
		int, float, bool,
		std::string>;

public:
	NppValue() noexcept = default;
	NppValue(Value&& value) noexcept;

	bool IsTrue() const noexcept;
	std::string ToString() const;

	NppValue& operator += (const NppValue& other);
	NppValue& operator -= (const NppValue& other);
	NppValue& operator *= (const NppValue& other);
	NppValue& operator /= (const NppValue& other);
	NppValue& operator %= (const NppValue& other);

	NppValue operator + (const NppValue& other) const;
	NppValue operator - (const NppValue& other) const;
	NppValue operator * (const NppValue& other) const;
	NppValue operator / (const NppValue& other) const;
	NppValue operator % (const NppValue& other) const;

	int operator <=> (const NppValue& other) const;

	NppValue operator - () const;
	NppValue operator ~ () const;
	NppValue operator ! () const;

	NppValue& operator ++ ();
	NppValue& operator -- ();

	const Value& Get() const noexcept;

private:
	Value m_Value = std::monostate{};

private:
	template<class Op>
	static int CompareOperation(const Value& a, const Value& b, Op op) {
		return std::visit(
			[&] (auto&& x, auto&& y) -> int {
				using A = std::decay_t<decltype(x)>;
				using B = std::decay_t<decltype(y)>;

				if constexpr (std::is_arithmetic_v<A> && std::is_arithmetic_v<B>)
					return op(x, y);
				if constexpr (std::is_same_v<A, B> && std::is_same_v<A, std::string>)
					return op(x, y);

				throw SyntaxError(PERROR_OPERATOR_NOT_APPLICABLE);
			},
			a, b);
	}

	template<class Op>
	static Value BinaryOperation(const Value& a, const Value& b, Op op) {
		return std::visit(
			[&] (auto&& x, auto&& y) -> Value {
				using A = std::decay_t<decltype(x)>;
				using B = std::decay_t<decltype(y)>;

				if constexpr (std::is_arithmetic_v<A> && std::is_arithmetic_v<B>)
					return op(x, y);

				throw SyntaxError(PERROR_OPERATOR_NOT_APPLICABLE);
			},
			a, b);
	}

	template<class Op>
	static Value UnaryOperation(const Value& v, Op op) {
		return std::visit([&] (auto&& x) -> Value {
			using T = std::decay_t<decltype(x)>;
			if constexpr (std::is_arithmetic_v<T>)
				return op(x);
			throw SyntaxError(PERROR_OPERATOR_NOT_APPLICABLE);
			}, v);
	}
};

struct VariableStruct final {
	VariableStruct() noexcept = default;
	VariableStruct(std::string name, NppValue value) noexcept;

	NppValue ValueStruct;
	std::string VarName;
};