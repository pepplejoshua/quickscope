#include <iostream>
#include <string>
using std::cerr;
using std::endl;
using std::string;

enum TokenType { ENDFILE, DEF, EXTERN, IDENTIFIER, NUMBER, CHAR };

struct Token {
  string text;
  TokenType type;
};

Token next_token() {
  static auto cur = ' ';
  if (cur == EOF) {
    return Token{"\0", ENDFILE};
  }

  while (isspace(cur)) {
    cur = getchar();
  }

  if (isalpha(cur)) {
    string ident;
    ident += cur;

    while (isalnum(cur = getchar())) {
      ident += cur;
    }

    if (ident == "def") {
      return Token{ident, IDENTIFIER};
    } else if (ident == "ext") {
      return Token{ident, EXTERN};
    } else {
      return Token{ident, IDENTIFIER};
    }
  } else if (isdigit(cur)) {
    string num;
    num += cur;
    bool seen_dot = false;
    do {
      if (cur == '.') {
        if (seen_dot) {
          cerr << "Only 1 dot is allowed in a valid number." << endl;
          exit(2);
        } else {
          num += cur;
          cur = getchar();
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
  } else if (cur == '#') {
    // single line comments
    do
      cur = getchar();
    while (cur != EOF && cur != '\n' && cur != '\r');
    if (cur != EOF)
      return next_token();

    if (cur == EOF) {
      return Token{"\0", ENDFILE};
    }
  }
  string ch;
  ch += cur;
  cur = getchar();
  return Token{ch, CHAR};
}
