// Manual top-down (MTD) parser.
// Testing default order (+-, */, **, unaries, primaries)

#include "lex.h"
#include "tokens.h"
#include <map>
#include <limits>
#include <memory>
#include <vector>
#include <utility>
#include <stdexcept>
#include <stdlib.h>
#include <math.h>

namespace {

int verbose = 0;

class ParserError : public std::runtime_error {
public:
  ParserError(const std::string& reason) : std::runtime_error(reason) {
  }
};

struct ExecutionContext {
  typedef std::map<std::string, double> scalars_t;
  typedef std::map<double, double> amap_t;
  typedef std::map<std::string, amap_t> maps_t;
  void createMap(const std::string&);
  scalars_t scalars;
  maps_t maps;
};

void ExecutionContext::createMap(const std::string& map_name) {
  maps.emplace(std::make_pair(map_name, amap_t()));
}

struct SemanticValue {
  virtual double getRValue(ExecutionContext*) const;
  virtual void assignValue(double, ExecutionContext*);
};
double SemanticValue::getRValue(ExecutionContext*) const {
  throw ParserError("NotImplemented: getRValue");
}
void SemanticValue::assignValue(double, ExecutionContext*) {
  throw ParserError("NotImplemented: assignValue");
}

struct NoneSV : SemanticValue {
  // Just to mark absense
};

struct ScalarSV : SemanticValue {
  double m_value;
  double getRValue(ExecutionContext*) const override;
  void assignValue(double, ExecutionContext*) override;
  ScalarSV(double value) : m_value(value) {
  }
};
double ScalarSV::getRValue(ExecutionContext*) const {
  return m_value;
}
void ScalarSV::assignValue(double, ExecutionContext*) {
  throw ParserError("ScalarSV: assignValue: this is rvalue");
}

struct IdentifierSV : SemanticValue {
  std::string m_name;
  double getRValue(ExecutionContext*) const override;
  void assignValue(double, ExecutionContext*) override;
  IdentifierSV(const std::string& name) : m_name(name) {
  }
};
double IdentifierSV::getRValue(ExecutionContext* ec) const {
  ExecutionContext::scalars_t::const_iterator i = ec->scalars.find(m_name);
  return (i == ec->scalars.end())
      ? (std::numeric_limits<double>::quiet_NaN())
      : i->second;
}
void IdentifierSV::assignValue(double nv, ExecutionContext* ec) {
  ExecutionContext::maps_t::const_iterator i1 = ec->maps.find(m_name);
  if (i1 != ec->maps.end()) {
    throw ParserError("IdentifierSV: assignValue: cannot assign to map");
  }
  ec->scalars[m_name] = nv;
}

struct IndexedReferenceSV : SemanticValue {
  std::unique_ptr<IdentifierSV> m_base;
  std::unique_ptr<SemanticValue> m_index;
  double getRValue(ExecutionContext*) const override;
  void assignValue(double, ExecutionContext*) override;
};
double IndexedReferenceSV::getRValue(ExecutionContext* ec) const {
  double ivalue = m_index->getRValue(ec);
  ExecutionContext::maps_t::const_iterator i1 = ec->maps.find(m_base->m_name);
  if (i1 == ec->maps.end()) {
    throw ParserError("Map not found");
  }
  const ExecutionContext::amap_t& themap = i1->second;
  ExecutionContext::amap_t::const_iterator i2 = themap.find(ivalue);
  return (i2 == themap.end())
      ? (std::numeric_limits<double>::quiet_NaN())
      : i2->second;
}
void IndexedReferenceSV::assignValue(double nv, ExecutionContext* ec) {
  double ivalue = m_index->getRValue(ec);
  ExecutionContext::maps_t::iterator i1 = ec->maps.find(m_base->m_name);
  if (i1 == ec->maps.end()) {
    throw ParserError("Map not found");
  }
  ExecutionContext::amap_t& themap = i1->second;
  themap[ivalue] = nv;
}

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
    throw ParserError("Lexer::advance: lexing error");
  }
  if (tok.tok_type == tt_Eof) {
    seen_eof = true;
  }
}

struct Parser {
  Lexer* lexer = nullptr;
  ExecutionContext* econtext = nullptr;
  typedef std::unique_ptr<SemanticValue> result_type;
  result_type statement();
  result_type processDefmap();
  result_type expression();
  result_type assignment();
  result_type addsub();
  result_type muldiv();
  result_type power();
  result_type unary();
  result_type primary();
  result_type atom();
};

Parser::result_type Parser::statement() {
  if (lexer->tok.tok_type == tt_Keyword &&
      lexer->tok.str_value == "defmap") {
    return processDefmap();
  }
  return expression();
}

Parser::result_type Parser::processDefmap() {
  lexer->advance();
  if (lexer->tok.tok_type != tt_Identifier) {
    throw ParserError("processDefmap: identifier expected");
  }
  econtext->createMap(lexer->tok.str_value);
  lexer->advance();
  Parser::result_type r{new NoneSV()};
  return r;
}

Parser::result_type Parser::expression() {
  return assignment();
}

Parser::result_type Parser::assignment() {
  Parser::result_type v = addsub();
  if (lexer->tok.tok_type == tt_Eq) {
    lexer->advance();
    Parser::result_type rpart = assignment();
    double v2 = rpart->getRValue(econtext);
    v->assignValue(v2, econtext);
    v.reset(new ScalarSV(v2));
  }
  return v;
}

Parser::result_type Parser::addsub() {
  Parser::result_type v = muldiv();
  for(;;) {
    if (lexer->tok.tok_type == tt_Plus) {
      double v1 = v->getRValue(econtext);
      lexer->advance();
      double v2 = muldiv()->getRValue(econtext);
      v.reset(new ScalarSV(v1 + v2));
    }
    else if (lexer->tok.tok_type == tt_Minus) {
      double v1 = v->getRValue(econtext);
      lexer->advance();
      double v2 = muldiv()->getRValue(econtext);
      v.reset(new ScalarSV(v1 - v2));
    } else {
      break;
    }
  }
  return v;
}

Parser::result_type Parser::muldiv() {
  Parser::result_type v = power();
  for(;;) {
    if (lexer->tok.tok_type == tt_Star) {
      double v1 = v->getRValue(econtext);
      lexer->advance();
      double v2 = muldiv()->getRValue(econtext);
      v.reset(new ScalarSV(v1 * v2));
    }
    else if (lexer->tok.tok_type == tt_Slash) {
      double v1 = v->getRValue(econtext);
      lexer->advance();
      double v2 = muldiv()->getRValue(econtext);
      v.reset(new ScalarSV(v1 / v2));
    } else {
      break;
    }
  }
  return v;
}

Parser::result_type Parser::power() {
  Parser::result_type v = unary();
  if (lexer->tok.tok_type == tt_DoubleStar) {
    lexer->advance();
    double v1 = v->getRValue(econtext);
    double v2 = power()->getRValue(econtext);
    v.reset(new ScalarSV(pow(v1, v2)));
  }
  return v;
}

Parser::result_type Parser::unary() {
  if (lexer->tok.tok_type == tt_Plus) {
    lexer->advance();
    Parser::result_type v;
    v.reset(new ScalarSV(unary()->getRValue(econtext)));
    return v;
  } else if (lexer->tok.tok_type == tt_Minus) {
    lexer->advance();
    Parser::result_type v;
    v.reset(new ScalarSV(-unary()->getRValue(econtext)));
    return v;
  } else {
    return primary();
  }
}

Parser::result_type Parser::primary() {
  Parser::result_type part1 = atom();
  if (lexer->tok.tok_type == tt_LeftBracket) {
    IdentifierSV* id_on_left = dynamic_cast<IdentifierSV*> (part1.get());
    if (!id_on_left) {
      throw ParserError("Parser::primary: only allow identifier before []");
    }
    lexer->advance();
    Parser::result_type part2 = expression();
    if (lexer->tok.tok_type != tt_RightBracket) {
      throw ParserError("Expecting right bracket");
    }
    lexer->advance();
    Parser::result_type index;
    index.reset(new ScalarSV(part2->getRValue(econtext)));
    std::unique_ptr<IndexedReferenceSV> irsv {new IndexedReferenceSV()};
    // NB there is gap when nobody keeps pointer to id_on_left :)
    // I don't know solution for this
    part1.release();
    irsv->m_base.reset(id_on_left);
    irsv->m_index = std::move(index);
    part1 = std::move(irsv);
  }
  return part1;
}

Parser::result_type Parser::atom() {
  if (lexer->tok.tok_type == tt_LeftParen) {
    lexer->advance();
    Parser::result_type v = expression();
    if (lexer->tok.tok_type != tt_RightParen) {
      throw ParserError("Expecting right paren");
    }
    lexer->advance();
    return v;
  } else if (lexer->tok.tok_type == tt_Number) {
    Parser::result_type v;
    v.reset(new ScalarSV(lexer->tok.int_value));
    lexer->advance();
    return v;
  } else if (lexer->tok.tok_type == tt_Identifier) {
    Parser::result_type v;
    v.reset(new IdentifierSV(lexer->tok.str_value));
    lexer->advance();
    return v;
  } else {
    if (verbose >= 2) {
      printf("atom: fail: tok_type=%d\n", lexer->tok.tok_type);
    }
    throw ParserError("atom: unexpected token");
  }
}

static double calcExpression(ExecutionContext* econtext,
    const std::string& text) {
  Parser parser;
  Lexer lexer(text);
  parser.lexer = &lexer;
  parser.econtext = econtext;
  lexer.prepare();
  Parser::result_type pret = parser.expression();
  if (!lexer.seenEof()) {
    throw ParserError("calcExpression: seen garbage at end\n");
  }
  double ret = pret->getRValue(econtext);
  return ret;
}

static void processStatement(ExecutionContext* econtext,
    const std::string& text, double &result) {
  Parser parser;
  Lexer lexer(text);
  parser.lexer = &lexer;
  parser.econtext = econtext;
  lexer.prepare();
  // NB statement here for common code with testBlock().
  // Shall we separate it into different functions?
  Parser::result_type pret = parser.statement();
  if (!lexer.seenEof()) {
    throw ParserError("calcExpression: seen garbage at end\n");
  }
  NoneSV* nonesv = dynamic_cast<NoneSV*> (pret.get());
  if (!nonesv) {
    result = pret->getRValue(econtext);
  }
}

static void testExpression(const std::string& text, double expected) {
  double r = -1.0e40;
  if (verbose >= 1) {
    printf("testExpression(\"%s\")\n", text.c_str());
  }
  try {
    ExecutionContext econtext;
    r = calcExpression(&econtext, text);
    if (r != expected) {
      printf("Fail: text=\"%s\" expected=%g r=%g\n", text.c_str(),
          expected, r);
      exit(1);
    }
  } catch (ParserError& ex) {
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
    ExecutionContext econtext;
    (void) calcExpression(&econtext, text);
  } catch (ParserError& ex) {
    raised = true;
  }
  if (!raised) {
    printf("Fail: not raised for: \"%s\"\n", text.c_str());
    exit(1);
  }
}

void testBlock(const std::string& test_name,
    const std::vector<std::string>& block, double expected) {
  if (verbose >= 1) {
    printf("testBlock(%s)\n", test_name.c_str());
  }
  double r = -1.0e40;
  ExecutionContext ec;
  for (const std::string& expr : block) {
    if (verbose >= 2) {
      printf("testBlock: expr=\"%s\"\n", expr.c_str());
    }
    processStatement(&ec, expr.c_str(), r);
  }
  if (r != expected) {
    printf("Fail: testBlock(%s): expected=%g result=%g\n",
        test_name.c_str(), expected, r);
    exit(1);
  }
}

void testBlockFail(const std::string& test_name,
    const std::vector<std::string>& block) {
  if (verbose >= 1) {
    printf("testBlockFail(%s)\n", test_name.c_str());
  }
  double r = -1.0e40;
  ExecutionContext ec;
  bool raised = false;
  for (const std::string& expr : block) {
    if (verbose >= 2) {
      printf("testBlockFail: expr=\"%s\"\n", expr.c_str());
    }
    try {
      processStatement(&ec, expr.c_str(), r);
    } catch (ParserError& ex) {
      raised = true;
    }
  }
  if (!raised) {
    printf("Fail: testBlockFail(%s): not raised\n", test_name.c_str());
    exit(1);
  }
}

} // anonymous namespace

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

  // Test set - scalar assignments and their usage
  testBlock("a=1", {"a=1", "a"}, 1);
  testBlock("a+b", {"a=3", "b=5", "a+b"}, 8);
  testBlock("a*b", {"a=b=5", "a*b"}, 25);
  testBlock("a=44", {"a=44"}, 44);
  testBlock("(a)", {"(a)=177"}, 177);
  testBlock("map1", {"@defmap zz", "zz[1]=50"}, 50);
  testBlock("map2", {"@defmap zxcv", "(zxcv[1])=255", "(zxcv[(3-2)]-2)"}, 253);
  testBlock("map3", {"@defmap yx", "yx[1]=50", "yx[2]=yx[3]=4",
      "mm=8", "yx[1]*mm + yx[2]/yx[3]"}, 401);
  testBlockFail("fail1", {"@hello"});
  testBlockFail("fail2", {"1[2]"});
  testBlockFail("fail3", {"(c=d)=3"});
}
// vim: set ts=2 sts=2 sw=2 et :
