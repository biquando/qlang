#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// G -> i S
//   -> i S e S
// S -> s
//   -> { S* }

// G -> i S
//   -> i S e S
// S -> s
//   -> { L }
//   -> { }
// L -> s
//   -> s L

// G  -> i S G1
// G1 -> e S
//    ->
// S  -> s
//    -> { S1
// S1 -> L }
//    -> }
// L  -> s L1
// L1 -> L
//    ->

struct Node {
    Node(std::vector<int> children) : children(children) {}
    std::vector<int> children;
};

class Parser {
  public:
    Parser() = default;

    void parse(std::string tokens)
    {
        this->tokens = tokens;
        i = 0;
        token = nextToken();
        G();
        eat(-1);
        std::cerr << "Parsed successfully\n";
    }

  private:
    std::string tokens;
    int token;
    unsigned i;

    void G()
    {
        switch (token) {
        case 'i':
            eat('i');
            S();
            if (token == 'e') {
                eat('e');
                S();
            }
            break;
        default:
            error();
        }
    }

    void S()
    {
        switch (token) {
        case 's':
            eat('s');
            break;
        case '{':
            eat('{');
            while (token == 's' || token == '{') {
                S();
            }
            eat('}');
            break;
        default:
            error();
        }
    }

    void eat(int t)
    {
        if (token == t) {
            token = nextToken();
        }
        else {
            error();
        }
    }

    int nextToken()
    {
        if (i == tokens.size()) {
            return -1;
        }
        return tokens[i++];
    }

    void error()
    {
        std::cerr << "Parse error at token " << i << "\n";
        std::exit(1);
    }
};

int main()
{
    Parser p;
    p.parse("i{s{{}s}s}e{ss}");
}
