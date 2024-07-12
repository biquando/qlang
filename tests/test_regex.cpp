#include "lexer/RegexParsing.hpp"
#include <cassert>
#include <cctype>
#include <iostream>
#include <vector>

void printTokens(std::vector<int> tokens)
{
    for (int c : tokens) {
        if (isprint(c)) {
            std::cout << "  " << (char)c;
        }
        else if (c < 0) {
            std::cout << (char)(-c);
        }
        else {
            std::cout << std::hex << "  0x" << c << std::dec;
        }
        std::cout << '\n';
    }
}

int main()
{
    RegexParsing::debug = true;

    // Tokenization
    std::cout << "\nDecimal (" << R"([0-9]+)" << "):\n";
    printTokens(RegexParsing::tokenize(R"([0-9]+)"));
    std::cout << "\nHex (" << R"(0x[0-9a-fA-F]+)" << "):\n";
    printTokens(RegexParsing::tokenize(R"(0x[0-9a-fA-F]+)"));
    std::cout << "\nString (" << R"(\"([^"\\\n]|\\.)*\")" << "):\n";
    printTokens(RegexParsing::tokenize(R"(\"([^"\\\n]|\\.)*\")"));
    std::cout << "\nQuote test (" << R"("sw])i*\t([ch\")" << "):\n";
    printTokens(RegexParsing::tokenize(R"("sw])i*\t([ch\")"));

    // Validation
    std::vector<std::string> failingTests{
        R"()",
        R"(a(b(c)d)e(f)g)h)",
        R"(a(b(c)d(())e(f)g)h)",
        R"([^abc][def)",
        R"([^abc][def]b[^]asdf)",
        R"([^abc][def]b[]asdf)",
        R"((a|))",
        R"(a|)",
        R"((|b))",
        R"(|b)",
        R"(a|+)",
        R"(+)",
        R"(a+*)",
        R"(abc(+def)*)",
        R"([-b])",
        R"([^-b])",
        R"([^a-])",
        R"([^a-b-c])",
    };
    std::vector<std::string> passingTests{
        R"([0-9]+)",
        R"(0x[0-9a-fA-F]+)",
        R"(\"([^"\\\n]|\\.)*\")",
        R"(hello)",
        R"(a(b(c)d(i)e(f)g)h)",
        R"([^abc][def])",
        R"((a|b))",
        R"(a|b)",
        R"(a+|b)",
        R"(abc(z+def)*)",
        R"([^a-b])",
        R"([^a-bc-d])",
    };
    std::cout << "\nShould fail:\n";
    for (const std::string &test : failingTests) {
        std::cout << "\x1B[90m>\x1B[0m " << test << "\n";
        bool passes = RegexParsing::validate(RegexParsing::tokenize(test));
        assert(!passes);
    }
    std::cout << "\nShould pass:\n";
    for (const std::string &test : passingTests) {
        std::cout << "\x1B[90m>\x1B[0m " << test << "\n";
        bool passes = RegexParsing::validate(RegexParsing::tokenize(test));
        assert(passes);
    }
    std::cout << "\nAll validation tests passed\n";

    // Decomposition
    std::cout << "\nConstruction tests:\n";
    for (auto &test : passingTests) {
        std::cout << "\x1B[90m>\x1B[0m " << test << "\n";
        if (RegexParsing::debug) {
            printTokens(RegexParsing::tokenize(test));
        }
        RegexParsing::Pattern p(test);
    }
}
