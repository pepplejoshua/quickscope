#pragma once

#include "ast.hpp"
#include "lexer.hpp"
#include <ctype.h>
#include <iostream>
#include <map>
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::fprintf;
using std::make_unique;
using std::map;
using std::string;
using std::unique_ptr;
using std::vector;

static unique_ptr<ExprAST> parse_expr();

struct Parser {
  map<char, int> binopPrecendences;
  Lexer l;
  Token cur_token;

  Parser() {
    binopPrecendences['<'] = 10;
    binopPrecendences['+'] = 20;
    binopPrecendences['-'] = 20;
    binopPrecendences['*'] = 40;
    l.update_src();
    cur_token = next_token(l);
  }

  unique_ptr<ExprAST> Err(string msg) {
    cout << "Error: " << msg << endl;
    return nullptr;
  }

  unique_ptr<FnPrototype> ErrP(string msg) {
    Err(msg);
    return nullptr;
  }

  Token advance() {
    cur_token = next_token(l);
    return cur_token;
  }

  Token cur() { return cur_token; }

  unique_ptr<NumberExpr> parse_num() {
    auto num = strtod(cur_token.text.c_str(), 0);
    auto res = make_unique<NumberExpr>(num);
    advance();
    return res;
  }

  unique_ptr<ExprAST> parse_group() {
    advance();
    auto expr = parse_expr();
    if (auto expr = parse_expr()) {
      if (cur_token.text != ")")
        return Err("Expected a ')'");
      advance();
      return expr;
    } else {
      return nullptr;
    }
  }

  unique_ptr<ExprAST> parse_id_expr() {
    string name = cur_token.text;
    advance();

    if (cur_token.text != "(") {
      return make_unique<VarExpr>(name);
    }

    advance();
    vector<unique_ptr<ExprAST>> args;
    while (true) {
      if (auto Arg = parse_id_expr())
        args.push_back(std::move(Arg));
      else
        return nullptr;

      if (cur_token.text == ")")
        break;

      if (cur_token.text != ",")
        return Err("Expected a ',' or ')' in arg-list.");
      advance();
    }
    advance();
    return make_unique<FnCall>(name, std::move(args));
  }

  unique_ptr<ExprAST> parse_primary() {
    switch (cur_token.type) {
    case IDENTIFIER:
      return parse_id_expr();
    case NUMBER:
      return parse_num();
    case CHAR: {
      if (cur_token.text == "(") {
        return parse_group();
      }
    }
    default:
      return Err("Unknown token when expecting an expression :> " +
                 cur_token.text);
    }
  }

  int get_token_prec() {
    if (cur_token.type != CHAR) {
      return -1;
    }

    char op = cur_token.text[0];
    if (!(isascii(op)))
      return -1;

    int precedence = binopPrecendences[op];
    if (precedence <= 0)
      return -1;

    return precedence;
  }

  unique_ptr<ExprAST> parse_expr() {
    auto lhs = parse_primary();
    if (!lhs)
      return lhs;
    return parse_bin_expr(0, std::move(lhs));
  }

  unique_ptr<ExprAST> parse_bin_expr(int expr_prec, unique_ptr<ExprAST> lhs) {
    while (true) {
      int prec = get_token_prec();
      if (prec < expr_prec)
        return lhs;

      char bin_op = cur_token.text[0];
      advance();
      auto rhs = parse_primary();
      if (!rhs)
        return rhs;

      int next_prec = get_token_prec();
      if (prec < next_prec) {
        rhs = parse_bin_expr(prec + 1, std::move(rhs));
        if (!rhs)
          return rhs;
      }

      lhs = make_unique<BinaryExpr>(bin_op, std::move(lhs), std::move(rhs));
    }
  }

  unique_ptr<FnPrototype> parse_fn_prototype() {
    if (cur_token.type != IDENTIFIER)
      return ErrP("Expected a function name in the function prototype.");

    string fn_name = cur_token.text;
    advance();

    if (cur_token.text != "(")
      return ErrP("Expected a '(' in the function prototype.");

    // read list of parameter name
    vector<string> params;
    while (advance().type == IDENTIFIER)
      params.push_back(cur_token.text);

    if (cur_token.text != ")")
      return ErrP("Expected a ')' in the function prototype.");

    advance();
    return make_unique<FnPrototype>(fn_name, params);
  }

  unique_ptr<FnDef> parse_fn_def() {
    advance();
    auto Proto = parse_fn_prototype();
    if (!Proto)
      return nullptr;

    if (auto BodyExpr = parse_expr()) {
      return make_unique<FnDef>(std::move(Proto), std::move(BodyExpr));
    } else {
      return nullptr;
    }
  }

  unique_ptr<FnPrototype> parse_extern_decl() {
    advance();
    return parse_fn_prototype();
  }

  unique_ptr<FnDef> parse_top_lvl_expr() {
    if (auto Expr = parse_expr()) {
      // making an anonymous fn, with the expr as its body
      auto Proto = make_unique<FnPrototype>("", vector<string>());
      return make_unique<FnDef>(std::move(Proto), std::move(Expr));
    }
    return nullptr;
  }

  void HandleFnDef() {
    if (parse_fn_def()) {
      cout << "Parsed a function definition.\n" << std::flush;
      cin.clear();
    } else {
      // Skip token for error recovery.
      advance();
    }
  }

  void HandleExtern() {
    if (parse_extern_decl()) {
      cout << "Parsed an extern\n" << std::flush;
      cin.clear();
    } else {
      // Skip token for error recovery.
      advance();
    }
  }

  void HandleTopLevelExpr() {
    // Evaluate a top-level expression into an anonymous function.
    if (parse_top_lvl_expr()) {
      cout << "Parsed a top-level expr\n" << std::flush;
      cin.clear();
    } else {
      // Skip token for error recovery.
      advance();
    }
  }
};
