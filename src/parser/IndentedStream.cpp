#include "parser/IndentedStream.hpp"
#include <iostream>
#include <streambuf>
#include <string>

IndentedStreamBuffer::IndentedStreamBuffer(std::streambuf *buf, int indentWidth)
    : buf(buf), indentWidth(indentWidth), shouldIndent(true)
{
}

auto IndentedStreamBuffer::overflow(int c) -> int
{
    if (c == '\n') {
        shouldIndent = true;
    }
    else if (shouldIndent) {
        for (int i = 0; i < indentWidth; ++i) {
            if (buf->sputc(' ') == std::char_traits<char>::eof()) {
                return std::char_traits<char>::eof();
            }
        }
        shouldIndent = false;
    }
    return buf->sputc(static_cast<char>(c));
}

IndentedStream::IndentedStream(std::ostream &os, int indentWidth)
    : std::ostream(&buffer), buffer(os.rdbuf(), indentWidth)
{
}
