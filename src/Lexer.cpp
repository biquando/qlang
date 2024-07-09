#include "Lexer.hpp"
#include "State.hpp"
#include "Token.hpp"
#include <functional>
#include <iostream>
#include <vector>

int Lexer::nextChar(FILE *fd)
{
    int c = fgetc(fd);
    if (c == '\n') {
        loc.line++;
        loc.col = 0;
    }
    else {
        loc.col++;
    }
    return c;
}

std::pair<bool, int> Lexer::transitionStates(std::vector<int> &states, char c)
{
    bool stillMatching = false;
    int firstAcceptedState = -1;

    for (int i = 0; i < (int)states.size(); i++) {
        states[i] = transitionFns[i](states[i], c);
        if (firstAcceptedState < 0 && states[i] == (int)State::Accept) {
            firstAcceptedState = i;
        }
        if (states[i] != (int)State::Accept &&
            states[i] != (int)State::Reject) {
            stillMatching = true;
        }
    }

    return std::pair<bool, int>(stillMatching, firstAcceptedState);
}

std::vector<std::unique_ptr<Token>> Lexer::tokenize(FILE *fd)
{
    std::vector<std::unique_ptr<Token>> tokens;
    std::vector<char> currToken;
    std::vector<int> states(numTokenTypes(), (int)State::Enter);

    std::function<void()> reset = [&currToken, &states]() {
        currToken.clear();
        for (int &state : states) {
            state = (int)State::Enter;
        }
    };
    reset();

    int c = nextChar(fd);
    if (c == EOF) {
        return tokens;
    }
    while (true) {
        auto [stillMatching, firstAcceptedState] = transitionStates(states, c);

        if (!stillMatching) {
            if (firstAcceptedState < 0) {
                // FIXME: push this error up
                std::cerr << "Unexpected character at line " << loc.line
                          << " col " << loc.col << ": "
                          << ((c == EOF) ? "EOF" : std::string(1, c)) << "\n";
                return tokens;
            }
            else {
                std::string tokenText(currToken.begin(), currToken.end());
                std::unique_ptr<Token> token =
                    constructorFns[firstAcceptedState](tokenText);
                if (token != nullptr) {
                    tokens.push_back(std::move(token));
                }
            }
            reset();

            if (c == EOF) {
                break;
            }
            continue;
        }

        currToken.push_back(c);
        c = nextChar(fd);
    }

    return tokens;
}
