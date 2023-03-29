#pragma once

#include <iostream>
#include <string>
using std::cerr;
using std::cin;
using std::endl;
using std::getline;
using std::string;

enum TokenType { ENDFILE, DEF, EXTERN, IDENTIFIER, NUMBER, CHAR, SEMI };

struct Token {
  string text;
  TokenType type;
};

struct Lexer {
  string src;
  string::size_type INDEX;

  Lexer() {
    src = "";
    INDEX = 0;
  }

  void update_src() {
    getline(cin, src);
    INDEX = 0;
  }

  char cur() {
    if (INDEX < src.size()) {
      return src[INDEX];
    }
    return EOF;
  }
  char advance() {
    if (INDEX < src.size())
      INDEX += 1;
    return cur();
  }
};

static Token next_token(Lexer &l) {
  auto cur = l.cur();
  if (cur == EOF) {
    return Token{"\0", ENDFILE};
  }

  while (isspace(cur)) {
    cur = l.advance();
  }

  if (isalpha(cur)) {
    string ident;
    ident += cur;

    while (isalnum(cur = l.advance())) {
      ident += cur;
    }

    if (ident == "def") {
      return Token{ident, DEF};
    } else if (ident == "ext") {
      return Token{ident, EXTERN};
    } else {
      return Token{ident, IDENTIFIER};
    }
  }

  if (isdigit(cur)) {
    string num;
    num += cur;
    cur = l.advance();
    bool seen_dot = false;
    do {
      num += cur;
      cur = l.advance();
      if (cur == '.') {
        if (seen_dot) {
          cerr << "Only 1 dot is allowed in a valid number." << endl;
          exit(2);
        } else {
          num += cur;
          cur = l.advance();
          seen_dot = true;
          continue;
        }
      }
    } while (isdigit(cur));

    if (num.back() == '.') {
      num += '0';
    }

    // for converting to double
    strtod(num.c_str(), 0);
    return Token{num, NUMBER};
  }

  if (cur == '#') {
    // single line comments
    do
      cur = l.advance();
    while (cur != EOF && cur != '\n' && cur != '\r');
    if (cur != EOF)
      return next_token(l);
  }

  if (cur == EOF) {
    return Token{"\0", ENDFILE};
  }

  string ch;
  ch += cur;
  cur = l.advance();
  if (ch == ";")
    return Token{ch, SEMI};
  return Token{ch, CHAR};
}
