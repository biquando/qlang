#pragma once

#include <iostream>
#include <streambuf>

class IndentedStreamBuffer : public std::streambuf {
  public:
    explicit IndentedStreamBuffer(std::streambuf *buf, int indentWidth = 2);

  protected:
    auto overflow(int c) -> int override;

  private:
    std::streambuf *buf;
    int indentWidth;
    bool shouldIndent;
};

class IndentedStream : public std::ostream {
  public:
    explicit IndentedStream(std::ostream &os, int indentWidth = 2);

  private:
    IndentedStreamBuffer buffer;
};
