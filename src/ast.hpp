#pragma once

#include <memory>
#include <string>
#include <vector>

using std::move;
using std::string;
using std::unique_ptr;
using std::vector;

class ExprAST {
public:
  virtual ~ExprAST() = default;
};

class NumberExpr : public ExprAST {
public:
  NumberExpr(double val) : value(val) {}

private:
  double value;
};

class VarExpr : public ExprAST {
public:
  VarExpr(string name) : v_name(name) {}

private:
  string v_name;
};

class BinaryExpr : public ExprAST {
public:
  BinaryExpr(char op, unique_ptr<ExprAST> lhs, unique_ptr<ExprAST> rhs) {
    Op = op;
    Lhs = move(lhs);
    Lhs = move(rhs);
  }

private:
  char Op;
  unique_ptr<ExprAST> Lhs, Rhs;
};

class FnCall : public ExprAST {
public:
  FnCall(string nme, vector<unique_ptr<ExprAST>> fargs)
      : fn(nme), args(move(fargs)) {}

private:
  string fn;
  vector<unique_ptr<ExprAST>> args;
};

// represents the prototype of a function
class FnPrototype {
public:
  FnPrototype(string name, vector<string> params) {
    fname = name;
    fparams = params;
  }

  const string get_name() const { return fname; }

  const vector<string> get_params() const { return fparams; }

private:
  string fname;
  vector<string> fparams;
};

class FnDef {
public:
  FnDef(unique_ptr<FnPrototype> fpr, unique_ptr<ExprAST> fbody)
      : proto(move(fpr)), body(move(fbody)) {}

private:
  unique_ptr<FnPrototype> proto;
  unique_ptr<ExprAST> body;
};
