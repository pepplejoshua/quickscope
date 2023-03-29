#include "ast.hpp"
#include "parser.hpp"

int main() {
  while (true) {
    cout << "?> ";
    Parser p;
    if (p.l.src == ".q") {
      return 0;
    }
    auto cur_token = p.cur();
    switch (cur_token.type) {
    case ENDFILE:
      continue;
    case SEMI: {
      p.advance();
      break;
    }
    case DEF: {
      p.HandleFnDef();
      break;
    }
    case EXTERN: {
      p.HandleExtern();
      break;
    }
    default: {
      p.HandleTopLevelExpr();
    }
    }
  }

  return 0;
}
