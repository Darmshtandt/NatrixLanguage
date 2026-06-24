#include "ValueStruct.h"

#include <cassert>

NppValue::NppValue(Value&& value) noexcept :
	m_Value(value)
{
}

bool NppValue::IsTrue() const noexcept {
	return std::visit([](auto&& val) -> bool {
		using T = std::decay_t<decltype(val)>;
		if constexpr (std::is_same_v<T, std::string>)
			return true;
		if constexpr (std::is_same_v<T, std::monostate>)
			return false;
		if constexpr (std::is_same_v<T, bool>)
			return val;
		if constexpr (std::is_arithmetic_v<T>)
			return val == 0;
		
		return false;
	}, m_Value);
}

std::string NppValue::ToString() const {
	return std::visit([] (auto&& val) -> std::string {
		using T = std::decay_t<decltype(val)>;
		if constexpr (std::is_same_v<T, std::monostate>)
			return "";
		else if constexpr (std::is_same_v<T, std::string>)
			return val;
		else if constexpr (std::is_same_v<T, bool>)
			return val ? "true" : "false";
		else if constexpr (std::is_arithmetic_v<T>)
			return std::to_string(val);
		else
			throw SyntaxError(PERROR_UNKOWN_TYPE);
		}, m_Value);
}

NppValue& NppValue::operator += (const NppValue& other) {
	m_Value = BinaryOperation(m_Value, other.m_Value, std::plus<>{});
	return *this;
}

NppValue& NppValue::operator -= (const NppValue& other) {
	m_Value = BinaryOperation(m_Value, other.m_Value, std::minus<>{});
	return *this;
}

NppValue& NppValue::operator *= (const NppValue& other) {
	m_Value = BinaryOperation(m_Value, other.m_Value, std::multiplies<>{});
	return *this;
}

NppValue& NppValue::operator /= (const NppValue& other) {
	m_Value = BinaryOperation(m_Value, other.m_Value, [] (auto x, auto y) { return x / y; });
	return *this;
}

NppValue& NppValue::operator %= (const NppValue& other) {
	m_Value = std::visit(
		[&] (auto&& x, auto&& y) -> Value {
			using A = std::decay_t<decltype(x)>;
			using B = std::decay_t<decltype(y)>;

			if constexpr (!std::is_arithmetic_v<A> || !std::is_arithmetic_v<B>)
				throw SyntaxError("operator not applicable");
			else if constexpr (std::is_integral_v<A> && std::is_integral_v<B>)
				return x % y;
			else if constexpr (std::is_integral_v<A>)
				return x % static_cast<int>(y);
			else if constexpr (std::is_integral_v<B>)
				return std::fmodf(static_cast<float>(x), y);
			else
				return std::fmodf(x, y);
		},
		m_Value, other.m_Value);
	return *this;
}

NppValue NppValue::operator + (const NppValue& other) const {
	NppValue val(*this);
	return val += other;
}
NppValue NppValue::operator - (const NppValue& other) const {
	NppValue val(*this);
	return val -= other;
}
NppValue NppValue::operator * (const NppValue& other) const {
	NppValue val(*this);
	return val *= other;
}
NppValue NppValue::operator / (const NppValue& other) const {
	NppValue val(*this);
	return val /= other;
}
NppValue NppValue::operator % (const NppValue& other) const {
	NppValue val(*this);
	return val %= other;
}


int NppValue::operator <=> (const NppValue& other) const {
	return CompareOperation(m_Value, other.m_Value, [] (auto x, auto y) {
		if (x < y)
			return -1;
		if (x > y)
			return 1;
		return 0;
	});
}


NppValue NppValue::operator - () const {
	return UnaryOperation(m_Value, [] (auto x) { return -x; });
}
NppValue NppValue::operator ~ () const {
	return std::visit([&] (auto&& x) -> Value {
		using T = std::decay_t<decltype(x)>;

		if constexpr (!std::is_integral_v<T>)
			throw SyntaxError("operator not applicable");
		else
			return ~x;
		}, m_Value);
}
NppValue NppValue::operator ! () const {
	return UnaryOperation(m_Value, [] (auto x) { return !x; });
}


NppValue& NppValue::operator ++ () {
	m_Value = UnaryOperation(m_Value, [] (auto x) { return x + 1; });
	return *this;
}
NppValue& NppValue::operator -- () {
	m_Value = UnaryOperation(m_Value, [] (auto x) { return x - 1; });
	return *this;
}

const NppValue::Value& NppValue::Get() const noexcept {
	return m_Value;
}