#pragma once

#include <ostream>
#include <string>

class Natrix {
public:
	Natrix();

	void Interpret(std::string code);

	void SetOutputStream(std::ostream* pStream) noexcept;

private:
	std::ostream* m_pOutputStream;
};