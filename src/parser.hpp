#pragma once

#include "ast.hpp"
#include "lexer.hpp"
#include <memory>
#include <stdio.h>
#include <string>

using std::fprintf;
using std::make_unique;
using std::string;
using std::unique_ptr;

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

static unique_ptr<ExprAST> parse_id_expr() {
  string name = cur_token.text;
  advance();

  if (cur_token.text != "") {
    return make_unique<VarExpr>(name);
  }
}
