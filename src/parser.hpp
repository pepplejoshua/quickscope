#pragma once

#include "ast.hpp"
#include "lexer.hpp"
#include <ctype.h>
#include <map>
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

using std::fprintf;
using std::make_unique;
using std::map;
using std::string;
using std::unique_ptr;
using std::vector;

static unique_ptr<ExprAST> parse_expr();

unique_ptr<ExprAST> Err(string msg) {
  fprintf(stderr, "Error: %s\n", msg.c_str());
  return nullptr;
}

unique_ptr<FnPrototype> ErrP(string msg) {
  Err(msg);
  return nullptr;
}

static Token cur_token;

static void advance() { cur_token = next_token(); }

static unique_ptr<NumberExpr> parse_num() {
  auto num = strtod(cur_token.text.c_str(), 0);
  auto res = make_unique<NumberExpr>(num);
  advance();
  return res;
}

// '(' ExprAST ')'
static unique_ptr<ExprAST> parse_group() {
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

static unique_ptr<ExprAST> parse_id_expr() {
  string name = cur_token.text;
  advance();

  if (cur_token.text != "(") {
    return make_unique<VarExpr>(name);
  }

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

static unique_ptr<ExprAST> parse_primary() {
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

struct Precedences {
  map<char, int> binopPrecendences;

  Precedences() {
    binopPrecendences['<'] = 10;
    binopPrecendences['+'] = 20;
    binopPrecendences['-'] = 20;
    binopPrecendences['*'] = 40;
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
};

static Precedences BinOps;

static unique_ptr<ExprAST> parse_bin_expr(int, unique_ptr<ExprAST>);

static unique_ptr<ExprAST> parse_expr() {
  auto lhs = parse_primary();
  if (!lhs)
    return lhs;
  return parse_bin_expr(0, std::move(lhs));
}

static unique_ptr<ExprAST> parse_bin_expr(int expr_prec,
                                          unique_ptr<ExprAST> lhs) {
  while (true) {
    int prec = BinOps.get_token_prec();
    if (prec < expr_prec)
      return lhs;

    char bin_op = cur_token.text[0];
    advance();
    auto rhs = parse_primary();
    if (!rhs)
      return rhs;

    int next_prec = BinOps.get_token_prec();
    if (prec < next_prec) {
      rhs = parse_bin_expr(prec + 1, std::move(rhs));
      if (!rhs)
        return rhs;
    }

    lhs = make_unique<BinaryExpr>(bin_op, std::move(lhs), std::move(rhs));
  }
}
