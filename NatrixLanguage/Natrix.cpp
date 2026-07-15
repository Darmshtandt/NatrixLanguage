#include "Natrix.h"
#include "Parser.h"

#include <format>
#include <iostream>

Natrix::Natrix() :
	m_pOutputStream(&std::cout)
{
}

void Natrix::Interpret(std::string code) {
	std::unique_ptr<Lexer> pLexer;
	std::unique_ptr<Parser> pParser;

	try {
		pLexer = std::make_unique<Lexer>(std::move(code));
		pParser = std::make_unique<Parser>(*pLexer);

		pParser->GetScopeStack().Top()->CreateFunction("print", 
			[this](const NppFuncArgs& args) {
			if (args.size() != 1)
				throw SyntaxError(PERROR_NUMBER_ARGUMENTS);

			(*m_pOutputStream) 
				<< args[0].ValueStruct.ToString().c_str();
			return VariableStruct("", NppValue());
			});

		pParser->ParseAll();
	}
	catch (const SyntaxError& e) {
		char errorLitera = '\0';
		switch (e.Code / 1000) {
		case 1:
			errorLitera = 'L';
			break;

		case 2:
			errorLitera = 'P';
			break;
		}

		(*m_pOutputStream)
			<< std::format("%c%d: %s\nLine: %d\nCharacter: %d\n",
				errorLitera, e.Code, e.what(), pParser->GetTextPosition().Line,
				pParser->GetTextPosition().Charaster);
	}
}

void Natrix::SetOutputStream(std::ostream* pStream) noexcept {
	m_pOutputStream = pStream;
}