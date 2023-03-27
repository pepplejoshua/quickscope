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
