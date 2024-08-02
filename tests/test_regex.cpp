#include <cassert>
#include <cctype>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

#include "lexer/RegexParsing.hpp"

void printTokens(const std::vector<int> &tokens)
{
    for (int c : tokens) {
        if (isprint(c)) {
            std::cout << "  " << (char)c;
        } else if (c < 0) {
            std::cout << (char)(-c);
        } else {
            std::cout << std::hex << "  0x" << c << std::dec;
        }
        std::cout << '\n';
    }
}

class TestRegex : public testing::Test {
  protected:
    TestRegex() : failingTests{
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
    }, passingTests{
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
    }
    {
        RegexParsing::debug = true;
    }

    std::vector<std::string> failingTests;
    std::vector<std::string> passingTests;
};

TEST_F(TestRegex, Tokenization)
{
    std::cout << "\nDecimal (" << R"([0-9]+)" << "):\n";
    printTokens(RegexParsing::tokenize(R"([0-9]+)"));
    std::cout << "\nHex (" << R"(0x[0-9a-fA-F]+)" << "):\n";
    printTokens(RegexParsing::tokenize(R"(0x[0-9a-fA-F]+)"));
    std::cout << "\nString (" << R"(\"([^"\\\n]|\\.)*\")" << "):\n";
    printTokens(RegexParsing::tokenize(R"(\"([^"\\\n]|\\.)*\")"));
    std::cout << "\nQuote test (" << R"("sw])i*\t([ch\")" << "):\n";
    printTokens(RegexParsing::tokenize(R"("sw])i*\t([ch\")"));
}

TEST_F(TestRegex, ValidationFail)
{
    std::cout << "\nShould fail:\n";
    for (const std::string &test : failingTests) {
        std::cout << "\x1B[90m>\x1B[0m " << test << "\n";
        bool passes = RegexParsing::validate(RegexParsing::tokenize(test));
        EXPECT_FALSE(passes);
    }
}

TEST_F(TestRegex, ValidationPass)
{
    std::cout << "\nShould pass:\n";
    for (const std::string &test : passingTests) {
        std::cout << "\x1B[90m>\x1B[0m " << test << "\n";
        bool passes = RegexParsing::validate(RegexParsing::tokenize(test));
        EXPECT_TRUE(passes);
    }
    std::cout << "\nAll validation tests passed\n";
}

TEST_F(TestRegex, Construction)
{
    std::cout << "\nConstruction tests:\n";
    for (auto &test : passingTests) {
        std::cout << "\x1B[90m>\x1B[0m " << test << "\n";
        if (RegexParsing::debug) {
            printTokens(RegexParsing::tokenize(test));
        }
        RegexParsing::Pattern p(test);
    }
}
