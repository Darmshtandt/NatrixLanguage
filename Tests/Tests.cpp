#include "pch.h"
#include "CppUnitTest.h"
#include "../NatrixLanguage/Natrix.h"
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(Tests)
	{
	public:
		[[nodiscard]] static std::string ExecuteCode(std::string code) {
			std::ostringstream ss;
			Natrix n;
			n.SetOutputStream(&ss);
			n.Interpret(std::move(code));
			return ss.str();
		}

		TEST_METHOD(If_Else_ElseIf) {
			std::string a = ExecuteCode(R"(
				var Fibbo = (2 + 2) * 2;
				if (((Fibbo * 2) - 8) == 8)
					print("if1\n");
				else {
					print("else1\n");
					if (false)
						print("if2\n");
					else if (false)
						print("else if2\n");
					else
						print("else2\n");
					print("finally2\n");
				}
				print("finally1\n");
				)");
			Assert::IsTrue(a == "if1\nfinally1\n");
		}

		TEST_METHOD(Scopes) {
			std::string a = ExecuteCode(R"(
				{
					var var1 = 42;
					{
						var var1 = 69;
					}
					print(var1);
				}
				)");
			Assert::IsTrue(a == "42");
		}

		TEST_METHOD(Func) {
			std::string a = ExecuteCode(R"(
				func Fib() {
					for (var i = 0; i != 4; ++i) {
						print("4\n");
					}
				}

				Fib();
				)");
			Assert::IsTrue(a == "4\n4\n4\n4\n");
		}

		TEST_METHOD(Func_SemicolonError) {
			std::string a = ExecuteCode(R"(
				func Fib() {
					for (var i = 0; i != 4; ++i) {
						print("4\n");
					}
				}

				Fib()
				)");
		}
	};
}
