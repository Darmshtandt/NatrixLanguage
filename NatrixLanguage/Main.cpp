#include <cassert>
#include <fstream>
#include <memory>
#include <sstream>

#include "Lexer.h"
#include "Parser.h"
#include "Errors.h"

/*
? - 0 or 1
* - 0+
+ - 1+

Code ::= Var | If | For | While | Func | Return
CodeBlock ::= '{' Code '}'
AnyCode ::= Code | CodeBlock

ValueStruct ::= IntVal | FloatVal | BoolVal | Symbol | Condition | Expression
Arguments ::= Var (',' Var)*
Bit ::= Bit_xor ('|' Bit_xor)*
Bit_xor ::= Bit_and ('^' Bit_and)*
Bit_and ::= Bit_neg ('&' Bit_neg)*
Bit_neg ::= ('~')? Bit_primary
Bit_primary ::= ValueStruct | '(' Bit ')'

Condition ::= Condition_and ('||' Condition_and)*
Condition_and ::= Condition_neg ('&&' Condition_neg)*
Condition_neg ::= ('!')? Condition_primary
Condition_primary ::= ValueStruct | '(' Condition ')'

Expression ::= Expression_MultDivMod (('+' | '-') Expression_MultDivMod)*
Expression_MultDivMod ::= Expression_postfix (('*' | '/' | '%') Expression_postfix)*
Expression_postfix ::= ('+' | '-' | '++' | '--') Expression_primary
Expression_primary ::= ValueStruct | '(' Expression ')' | Expression ('++' | '--')

Var ::= 'var' Symbol ('=' ValueStruct)? ';'
If ::= 'if' '(' Condition ')' AnyCode ('else' AnyCode)?
For ::= 'for' '(' Var ';' Condition ';' Expression ')' AnyCode | '(;' Condition ';' Expression ')' AnyCode
While ::= 'while' '(' Condition ')' AnyCode
Func ::= 'func' Symbol '(' Arguments ')' CodeBlock
Return ::= 'return' ValueStruct ';'
*/

int main() {
	std::unique_ptr<Lexer> pLexer;
	std::unique_ptr<Parser> pParser;

	try {
		std::ifstream file("first.npp");
		assert(file.is_open());

		std::stringstream ss;
		ss << file.rdbuf();
		file.close();

		const std::string code = ss.str();

		pLexer = std::make_unique<Lexer>(code);
		pParser = std::make_unique<Parser>(*pLexer);

		pParser->GetScopeStack().Top()->CreateFunction("print", [] (const NppFuncArgs& args) {
			if (args.size() != 1)
				throw SyntaxError(PERROR_NUMBER_ARGUMENTS);

			std::printf(args[0].ValueStruct.ToString().c_str());
			return VariableStruct("", NppValue());
			});

		std::vector<Token> tokens;
		while (true) {
			Token token = pLexer->GetNextToken();
			if (token.Type == TokenType::_EOF)
				break;

			tokens.push_back(token);
		}

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

		std::printf("%c%d: %s\nLine: %d\nCharacter: %d\n", 
			errorLitera, e.Code, e.what(), pParser->GetTextPosition().Line,
			pParser->GetTextPosition().Charaster);
	}

	printf("\n>>> END <<<\n");
	getchar();
	return 0;
}