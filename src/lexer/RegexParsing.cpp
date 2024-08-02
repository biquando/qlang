#include "lexer/RegexParsing.hpp"

#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "lexer/Node.hpp"
#include "lexer/State.hpp"

using namespace RegexParsing;

bool RegexParsing::debug = false;
#define DBG                     \
    if (!RegexParsing::debug) { \
    } else                      \
        std::cerr

static auto escape(char c) -> char
{
    switch (c) {
    case '\\':
        return '\\';
    case 'n':
        return '\n';
    case 't':
        return '\t';
    case 'r':
        return '\r';
    case '\'':
        return '\'';
    case '\"':
        return '\"';
    case 'b':
        return '\b';
    case 'f':
        return '\f';
    case 'v':
        return '\v';
    case 'a':
        return '\a';
    case '0':
        return '\0';
    default:
        return c;
    }
}

static inline void addSpecial(std::vector<int> &tokens, char c)
{
    assert(c > 0);
    tokens.push_back(-static_cast<int>(c));
}

static inline void addLiteral(std::vector<int> &tokens, char c)
{
    tokens.push_back(c);
}

auto RegexParsing::tokenize(const std::string &text) -> std::vector<int>
{
    std::vector<int> tokens;

    bool inQuotes = false;
    bool inSquareBrackets = false;
    for (size_t i = 0; text[i] != '\0'; i++) {
        char c = text[i];

        if (inQuotes) {
            if (c == '\"') {
                inQuotes = false;
                addSpecial(tokens, ')');
            } else {
                addLiteral(tokens, c);
            }
            continue;
        }

        if (inSquareBrackets) {
            // If there is a carat (^) at the beginning, this case is handled
            // when entering the square brackets.
            switch (c) {
            case ']':
                inSquareBrackets = false;
                addSpecial(tokens, ']');
                break;
            case '\\':
                c = text[++i];
                assert(c != '\0');
                addLiteral(tokens, escape(c));
                break;
            case '-':
                addSpecial(tokens, c);
                break;
            default:
                addLiteral(tokens, c);
            }
            continue;
        }

        switch (c) {
        case '\\':
            c = text[++i];
            assert(c != '\0');
            addLiteral(tokens, escape(c));
            break;
        case '[':
            inSquareBrackets = true;
            addSpecial(tokens, c);
            if (text[i + 1] == '^') {
                c = text[++i];
                addSpecial(tokens, c);
            }
            break;
        case '\"':
            inQuotes = true;
            addSpecial(tokens, '(');
            break;
        // Unsupported operators
        // case '^':
        // case '$':
        // case '<':
        // case '>':
        // case '/':
        // case '{':
        // case '}':
        case '(':
        case ')':
        case ']':
        case '.':
        case '|':
        case '+':
        case '*':
        case '?':
            addSpecial(tokens, c);
            break;
        case ' ':
            break;
        default:
            addLiteral(tokens, c);
            break;
        }
    }

    return tokens;
}

static auto equalsSpecial(int token, char c) -> bool
{
    return token < 0 && (char)(-token) == c;
}

/* validation:
 * - nonempty
 * - matching () and []
 * - on the left side of alternation: literal or -)].*+?
 * - on the right side of alternation: literal or -([.
 * - plus/star/opt comes after a literal or -)].
 * - valid ranges for -'-' in []
 *     - for each -'-' at idx i
 *         - check for literals at i-1 and i+1
 *         - check that i+2 is not -'-'
 *     - don't need to check that -'-' are only in [] because the tokenizer
 *       takes care of that, and [] are checked to be matched earlier
 */
// TODO: check for non-determinism
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto RegexParsing::validate(const std::vector<int> &tokens) -> bool
{
    // empty expression
    if (tokens.size() == 0) {
        DBG << "Validation failed: empty\n";
        return false;
    }

    // check matching () and []
    int parenDepth = 0;
    int bracketDepth = 0;
    for (int token : tokens) {
        if (equalsSpecial(token, '(')) {
            parenDepth++;
        } else if (equalsSpecial(token, ')')) {
            parenDepth--;
        } else if (equalsSpecial(token, '[')) {
            bracketDepth++;
        } else if (equalsSpecial(token, ']')) {
            bracketDepth--;
        }
        if (parenDepth < 0 || bracketDepth < 0) {
            DBG << "Validation failed: unmatched () or []\n";
            return false;
        }
    }
    if (parenDepth > 0 || bracketDepth > 0) {
        DBG << "Validation failed: unmatched () or []\n";
        return false;
    }

    // Check empty () and []
    for (unsigned i = 1; i < tokens.size(); i++) {
        if (equalsSpecial(tokens[i], ')') && equalsSpecial(tokens[i - 1], '('))
        {
            DBG << "Validation failed: Empty ()\n";
            return false;
        }
        if (equalsSpecial(tokens[i], ']')
            && (equalsSpecial(tokens[i - 1], '[')
                || equalsSpecial(tokens[i - 1], '^')))
        {
            DBG << "Validation failed: Empty []\n";
            return false;
        }
    }

    // on the left side of alternation: literal or -)].*+
    // on the right side of alternation: literal or -([.
    if (equalsSpecial(tokens[0], '|')
        || equalsSpecial(tokens[tokens.size() - 1], '|'))
    {
        DBG << "Validation failed: | is the first or last character\n";
        return false;
    }
    for (unsigned i = 1; i < tokens.size() - 1; i++) {
        if (!equalsSpecial(tokens[i], '|')) {
            continue;
        }
        if (!(tokens[i - 1] > 0 || equalsSpecial(tokens[i - 1], ')')
              || equalsSpecial(tokens[i - 1], ']')
              || equalsSpecial(tokens[i - 1], '.')
              || equalsSpecial(tokens[i - 1], '+')
              || equalsSpecial(tokens[i - 1], '*')
              || equalsSpecial(tokens[i - 1], '?')))
        {
            DBG << "Validation failed: Invalid left side of |\n";
            return false;
        }
        if (!(tokens[i + 1] > 0 || equalsSpecial(tokens[i + 1], '(')
              || equalsSpecial(tokens[i + 1], '[')
              || equalsSpecial(tokens[i + 1], '.')))
        {
            DBG << "Validation failed: Invalid right side of |\n";
            return false;
        }
    }

    // plus/star/opt comes after a literal or -)].
    if (equalsSpecial(tokens[0], '+') || equalsSpecial(tokens[0], '*')
        || equalsSpecial(tokens[0], '?'))
    {
        DBG << "Validation failed: + or * or ? at beginning\n";
        return false;
    }
    for (unsigned i = 1; i < tokens.size(); i++) {
        if (!equalsSpecial(tokens[i], '+') && !equalsSpecial(tokens[i], '*')
            && !equalsSpecial(tokens[i], '*'))
        {
            continue;
        }
        if (!(tokens[i - 1] > 0 || equalsSpecial(tokens[i - 1], ')')
              || equalsSpecial(tokens[i - 1], ']')
              || equalsSpecial(tokens[i - 1], '.')))
        {
            DBG << "Validation failed: Invalid left side of + or * or ?\n";
            return false;
        }
    }

    // - valid ranges for -'-' in []
    //     - for each -'-' at idx i
    //         - check for literals at i-1 and i+1
    //         - check that i+2 is not -'-'
    //     - don't need to check that -'-' are only in [] because the tokenizer
    //       take care of that, and [] are checked to be matched earlier
    for (unsigned i = 1; i < tokens.size() - 1; i++) {
        if (!equalsSpecial(tokens[i], '-')) {
            continue;
        }
        if (tokens[i - 1] <= 0 || tokens[i + 1] <= 0) {
            DBG << "Validation failed: Non-literals on side of - in []\n";
            return false;
        }
        assert(i < tokens.size() - 2);
        if (equalsSpecial(tokens[i + 2], '-')) {
            DBG << "Validation failed: Chained - in []\n";
            return false;
        }
    }

    return true;
}

auto RegexParsing::tokensToString(const std::vector<int> &tokens) -> std::string
{
    std::stringstream ss;
    for (int c : tokens) {
        if (isprint(c)) {
            ss << (char)c;
        } else if (c < 0) {
            ss << (char)(-c);
        } else {
            ss << std::hex << "0x" << c << std::dec;
        }
    }
    return ss.str();
}

/* decomposition:
 * - if everything is wrapped in ()
 *     - unwrap recursively
 *     - to check for this, check that the depth is only zero at the
 * beginning/end
 * - check for alternation
 *     - if there is a -'|' outside of parentheses
 * - check for concatenation
 *     - depth := 0
 *     - nPieces := 0
 *     - for each char c
 *         - if c == -'(' or -'['
 *             - depth++
 *         - if c == -')' or -']'
 *             - depth--
 *         - if depth == 0
 *             - nPieces++
 *         - if nPieces > 1 and c != -'+' or -'*'
 *             - return true // this also gives us the boundary between pieces
 * - check for plus/star/opt
 *     - if the last char is a -plus/star/opt
 * - check for charchoice
 *     - first char is -'['
 * - check for char
 *     - assert the only char is a literal or -'.'
 */
RegexParsing::Pattern::Pattern(const std::string &text)
    : Pattern(tokenize(text))
{}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
RegexParsing::Pattern::Pattern(const std::vector<int> &tokens)
{
    DBG << "Constructing pattern with text: " << tokensToString(tokens) << "\n";
    assert(validate(tokens));

    // if everything is wrapped in ()
    bool wrapped = true;
    int depth = 0;
    for (unsigned i = 0; i < tokens.size() - 1; i++) {
        if (equalsSpecial(tokens[i], '(')) {
            depth++;
        } else if (equalsSpecial(tokens[i], ')')) {
            depth--;
        }
        if (depth == 0) {
            wrapped = false;
            break;
        }
    }
    if (!equalsSpecial(tokens[tokens.size() - 1], ')')) {
        wrapped = false;
    }
    if (wrapped) {
        auto inner = std::vector<int>(tokens.begin() + 1, tokens.end() - 1);
        DBG << "Wrapped: inner=" << tokensToString(inner) << "\n";
        Pattern p(inner);
        type = p.type;
        switch (type) {
        case Char:
            literalChar = p.literalChar;
            break;
        case CharChoice:
            charChoicePred = p.charChoicePred;
            break;
        case Concat:
        case Alternate:
            opr1 = std::move(p.opr1);
            opr2 = std::move(p.opr2);
            break;
        case Plus:
        case Star:
        case Optional:
            opr1 = std::move(p.opr1);
            break;
        }
        return;
    }

    // check for alternation
    depth = 0;
    for (unsigned i = 0; i < tokens.size(); i++) {
        if (equalsSpecial(tokens[i], '(')) {
            depth++;
        } else if (equalsSpecial(tokens[i], ')')) {
            depth--;
        } else if (depth == 0 && equalsSpecial(tokens[i], '|')) {
            auto left = std::vector<int>(tokens.begin(), tokens.begin() + i);
            auto right = std::vector<int>(tokens.begin() + i + 1, tokens.end());
            DBG << "Alternate: left=" << tokensToString(left)
                << " right=" << tokensToString(right) << "\n";
            type = Alternate;
            opr1 = std::make_unique<Pattern>(left);
            opr2 = std::make_unique<Pattern>(right);
            return;
        }
    }

    // check for concatenation
    depth = 0;
    int nPieces = 0;
    for (unsigned i = 0; i < tokens.size(); i++) {
        if (depth == 0 && !equalsSpecial(tokens[i], '+')
            && !equalsSpecial(tokens[i], '+') && !equalsSpecial(tokens[i], '*')
            && !equalsSpecial(tokens[i], '?'))
        {
            nPieces++;
        }
        if (equalsSpecial(tokens[i], '(') || equalsSpecial(tokens[i], '[')) {
            depth++;
        } else if (equalsSpecial(tokens[i], ')')
                   || equalsSpecial(tokens[i], ']'))
        {
            depth--;
        }
        if (nPieces > 1) {
            auto left = std::vector<int>(tokens.begin(), tokens.begin() + i);
            auto right = std::vector<int>(tokens.begin() + i, tokens.end());
            DBG << "Concat: left=" << tokensToString(left)
                << " right=" << tokensToString(right) << "\n";
            type = Concat;
            opr1 = std::make_unique<Pattern>(left);
            opr2 = std::make_unique<Pattern>(right);
            return;
        }
    }

    // check for plus/star/opt
    if (equalsSpecial(tokens[tokens.size() - 1], '+')) {
        auto inner = std::vector<int>(tokens.begin(), tokens.end() - 1);
        DBG << "Plus: inner=" << tokensToString(inner) << "\n";
        type = Plus;
        opr1 = std::make_unique<Pattern>(inner);
        return;
    }
    if (equalsSpecial(tokens[tokens.size() - 1], '*')) {
        auto inner = std::vector<int>(tokens.begin(), tokens.end() - 1);
        DBG << "Star: inner=" << tokensToString(inner) << "\n";
        type = Star;
        opr1 = std::make_unique<Pattern>(inner);
        return;
    }
    if (equalsSpecial(tokens[tokens.size() - 1], '?')) {
        auto inner = std::vector<int>(tokens.begin(), tokens.end() - 1);
        DBG << "Optional: inner=" << tokensToString(inner) << "\n";
        type = Optional;
        opr1 = std::make_unique<Pattern>(inner);
        return;
    }

    // check for charchoice
    if (equalsSpecial(tokens[0], '[')) {
        auto inner = std::vector<int>(tokens.begin() + 1, tokens.end() - 1);
        DBG << "CharChoice: inner=" << tokensToString(inner) << "\n";
        type = CharChoice;
        if (equalsSpecial(inner[0], '^')) {
            DBG << "Inverted choice\n";
            charChoicePred = [inner](char c) {
                if (c == EOF) {
                    return false;
                }
                for (unsigned i = 1; i < inner.size(); i++) {
                    if (i + 1 < inner.size()
                        && equalsSpecial(inner[i + 1], '-'))
                    {
                        if (c >= inner[i] && c <= inner[i + 2]) {
                            return false;
                        }
                        i += 2;
                    } else if (c == inner[i]) {
                        return false;
                    }
                }
                return true;
            };
        } else {
            DBG << "Non-inverted choice\n";
            charChoicePred = [inner](char c) {
                DBG << "calling non-inverted choice with c=" << c << "\n";
                if (c == EOF) {
                    return false;
                }
                DBG << "size of inner: " << inner.size() << "\n";
                for (unsigned i = 0; i < inner.size(); i++) {
                    DBG << "checking choice char " << i << "\n";
                    if (i + 1 < inner.size()
                        && equalsSpecial(inner[i + 1], '-'))
                    {
                        DBG << "range from " << inner[i] << " to "
                            << inner[i + 2] << "\n";
                        if (c >= inner[i] && c <= inner[i + 2]) {
                            return true;
                        }
                        i += 2;
                    } else if (c == inner[i]) {
                        return true;
                    }
                }
                DBG << "did not match :(\n";
                return false;
            };
        }
        return;
    }

    assert(tokens.size() == 1);

    // check for -'.' (which is also a char choice)
    if (equalsSpecial(tokens[0], '.')) {
        DBG << "Dot: literal=.\n";
        type = CharChoice;
        charChoicePred = [](char c) { return c != EOF && c != '\n'; };
        return;
    }

    // check for char
    DBG << "Char: literal=" << tokensToString(tokens) << "\n";
    assert(tokens[0] > 0);
    type = Char;
    literalChar = static_cast<char>(tokens[0]);
}

auto RegexParsing::toNode(const std::shared_ptr<Pattern> &p)
    -> std::unique_ptr<lexer::Node>
{
    using namespace lexer;
    DBG << "Creating node from pattern...\n";
    switch (p->type) {
    case Pattern::Char:
        DBG << "toNode: Char\n";
        return std::make_unique<LiteralNode>(
            std::make_shared<CharState>(p->literalChar));
    case Pattern::CharChoice:
        DBG << "toNode: CharChoice\n";
        return std::make_unique<LiteralNode>(
            std::make_shared<PredState>(p->charChoicePred));
    case Pattern::Concat:
        DBG << "toNode: Concat\n";
        return std::make_unique<ConcatNode>(toNode(p->opr1), toNode(p->opr2));
    case Pattern::Alternate:
        DBG << "toNode: Alternate\n";
        return std::make_unique<AlternateNode>(toNode(p->opr1),
                                               toNode(p->opr2));
    case Pattern::Plus:
        DBG << "toNode: Plus\n";
        return std::make_unique<PlusNode>(toNode(p->opr1));
    case Pattern::Star:
        DBG << "toNode: Star\n";
        return std::make_unique<StarNode>(toNode(p->opr1));
    case Pattern::Optional:
        DBG << "toNode: Optional\n";
        return std::make_unique<OptionalNode>(toNode(p->opr1));
    }
    return nullptr;
}

auto RegexParsing::toNode(const std::string &text)
    -> std::unique_ptr<lexer::Node>
{
    DBG << "Converting " << text << " to node\n";
    std::shared_ptr<Pattern> p = std::make_shared<Pattern>(text);
    DBG << "Successfully created pattern from " << text << "\n";
    return toNode(p);
}
