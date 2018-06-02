// Manual top-down (MTD) parser.
// Testing default order (+-, */, **, unaries, primaries)

#include "lex.h"
#include "tokens.h"
#include <stdexcept>
#include <stdlib.h>
#include <math.h>

static int verbose = 0;

struct Lexer {
  lex_state tok;
  bool seen_eof = false;
  Lexer(const std::string& text) {
    lex_init(&tok, text);
  }
  void prepare() {
    if (tok.tok_type == tt_NONE) {
      advance();
    }
  }
  void advance();
  bool seenEof() const { return seen_eof; }
};

void Lexer::advance() {
  bool r = lex(&tok);
  if (!r) {
    throw std::runtime_error("Lexer::advance: lexing error");
  }
  if (tok.tok_type == tt_Eof) {
    seen_eof = true;
  }
}

struct Parser {
  Lexer *lexer = nullptr;
  double expression();
  double addsub();
  double muldiv();
  double power();
  double unary();
  double atom();
};

double Parser::expression() {
  return addsub();
}

double Parser::addsub() {
  double v = muldiv();
  for(;;) {
    if (lexer->tok.tok_type == tt_Plus) {
      lexer->advance();
      double v2 = muldiv();
      v += v2;
    }
    else if (lexer->tok.tok_type == tt_Minus) {
      lexer->advance();
      double v2 = muldiv();
      v -= v2;
    } else {
      break;
    }
  }
  return v;
}

double Parser::muldiv() {
  double v = power();
  for(;;) {
    if (lexer->tok.tok_type == tt_Star) {
      lexer->advance();
      double v2 = power();
      v *= v2;
    }
    else if (lexer->tok.tok_type == tt_Slash) {
      lexer->advance();
      double v2 = power();
      v /= v2;
    } else {
      break;
    }
  }
  return v;
}

double Parser::power() {
  double v = unary();
  if (lexer->tok.tok_type == tt_DoubleStar) {
    lexer->advance();
    double v2 = power();
    v = pow(v, v2);
  }
  return v;
}

double Parser::unary() {
  if (lexer->tok.tok_type == tt_Plus) {
    lexer->advance();
    return unary();
  } else if (lexer->tok.tok_type == tt_Minus) {
    lexer->advance();
    return -unary();
  } else {
    return atom();
  }
}

double Parser::atom() {
  if (lexer->tok.tok_type == tt_LeftParen) {
    lexer->advance();
    double v = expression();
    if (lexer->tok.tok_type != tt_RightParen) {
      throw std::runtime_error("Expecting right paren");
    }
    lexer->advance();
    return v;
  } else if (lexer->tok.tok_type == tt_Number) {
    double v = lexer->tok.int_value;
    lexer->advance();
    return v;
  } else {
    throw std::runtime_error("atom: unexpected token");
  }
}

static double calcExpression(const std::string& text) {
  Parser parser;
  Lexer lexer(text);
  parser.lexer = &lexer;
  lexer.prepare();
  double ret = parser.expression();
  if (!lexer.seenEof()) {
    throw std::runtime_error("calcExpression: seen garbage at end\n");
  }
  return ret;
}

static void testExpression(const std::string& text, double expected) {
  double r = 0.0;
  if (verbose >= 1) {
    printf("testExpression(\"%s\")\n", text.c_str());
  }
  try {
    r = calcExpression(text);
    if (r != expected) {
      printf("Fail: text=\"%s\" expected=%g r=%g\n", text.c_str(),
          expected, r);
      exit(1);
    }
  } catch (std::exception& ex) {
    printf("Fail: text=\"%s\"\n reason=%s", text.c_str(), ex.what());
    // TODO? traceback
    exit(1);
  }
}

void testExpressionFail(const std::string& text) {
  if (verbose >= 1) {
    printf("testExpressionFail(\"%s\")\n", text.c_str());
  }
  bool raised = false;
  try {
    (void) calcExpression(text);
  } catch (std::exception& ex) {
    raised = true;
  }
  if (!raised) {
    printf("Fail: not raised for: \"%s\"\n", text.c_str());
    exit(1);
  }
}

int main() {
  const char *vt = getenv("VERBOSE");
  if (vt) {
    verbose = strtol(vt, nullptr, 10);
  }
  testExpression("0", 0);
  testExpression("+0", 0);
  testExpression("-0", 0);
  testExpression("-1", -1);
  testExpression("--1", 1);
  testExpression("++1", 1);
  testExpression("+-1", -1);
  testExpression("1+1", 2);
  testExpression("1+-1", 0);
  testExpression("3+4*5", 23);
  testExpression("3+(4*5)", 23);
  testExpression("(3+4)*5", 35);
  testExpression("-(3+4)*5", -35);
  testExpression("(3 + 4) * 5", 35);
  testExpression("(2+3) * (7-4) + 11", 26);
  testExpression("(2+3) * -(7-4) + 11", -4);
  testExpression("2**2", 4);
  testExpression("-2**2", 4);
  testExpression("2**-2", 0.25);
  testExpression("3*3**3", 81);
  testExpression("2**2**2", 16);
  testExpression("2**3**2", 512);
  testExpressionFail("");
  testExpressionFail("(");
  testExpressionFail(")");
  testExpressionFail("2+");
  testExpressionFail("$");
  testExpressionFail("2$");
  testExpressionFail("-((2+3)");
  testExpressionFail("-(2+3))");
  testExpressionFail("2* *2");
}
// vim: set ts=2 sts=2 sw=2 et :
