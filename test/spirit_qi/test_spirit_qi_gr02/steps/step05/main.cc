#include <boost/spirit/include/qi.hpp>
#include <boost/format.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <stdexcept>
#include <typeinfo>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

namespace {

  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
  using SI = std::string::iterator;
  int verbose = 0;

  // The suggested method to create recursive rule set is to define them
  // as struct or class members.
  struct Parser : qi::grammar<SI, double(), ascii::space_type> {
    typedef qi::rule<SI, double(), ascii::space_type> TRule;
    TRule expression, addsub, muldiv, power, atom;
    Parser() : Parser::base_type(expression) {
      using qi::double_;
      using qi::_val;
      using qi::_1;
      // NB C++14 allows "auto" in lambdas, and this typedef isn't used in it.
      typedef boost::spirit::context<
          boost::fusion::cons<double&, boost::fusion::nil>,
          boost::fusion::vector0<>
        > f_context_d1;
      expression = addsub[
          ([](double& attribute, f_context_d1& context, bool& pass) {
            double& exposed = boost::fusion::at_c<0>(context.attributes);
            if (verbose >= 2) {
              printf("__: expr.0: _1=%g\n", attribute);
            }
            if (verbose >= 3) {
              const std::type_info& ti = typeid(context);
              printf("__: expr.0: context type: %s\n", ti.name());
            }
            exposed = attribute;
            pass = true;
          })
        ];
      //-expression = addsub[ _val = _1 ];
      addsub = muldiv [ _val = _1 ] >>
          *(
              ('+' >> muldiv [
                ([](double& attribute, f_context_d1& context, bool& pass) {
                  double& exposed = boost::fusion::at_c<0>(context.attributes);
                  if (verbose >= 2) {
                    printf("__: addsub.plus: %g+%g\n", exposed, attribute);
                  }
                  exposed += attribute;
                  pass = true;
                })
              ])
            | ('-' >> muldiv [ _val = _val - _1 ])
          );
      muldiv = power [ _val = _1 ] >>
          *(
              ('*' >> power [ _val = _val * _1 ])
            | ('/' >> power [ _val = _val / _1 ])
          );
      power = atom [ _val = _1 ];
      atom =
            double_[ _val = _1 ]
          | ('(' >> expression[ _val = _1 ] >> ')')
          | ('+' >> atom [ _val = _1 ])
          | ('-' >> atom [ _val = -_1 ])
          ;
    }
  };

  bool parseExpression(const char *expr_text, double &rResult) {
    std::string expr_str{expr_text};
    SI first = expr_str.begin();
    const auto last = expr_str.end();
    // parser is any beast than has member function "parse" with requested
    // signature and semantics.
    Parser parser;
    bool r = qi::phrase_parse(first, last,
        parser, ascii::space, rResult);
    return r;
  }

} // unnamed namespace

void testExpression(const char *expr_text, double expected) {
  if (verbose) {
    printf("testExpression: expr=<%s>, expected=%g\n", expr_text, expected);
  }
  double result = -1.0e40;
  bool r = parseExpression(expr_text, /*ref*/ result);
  if (!r) {
    throw std::runtime_error((boost::format("parsing failed: false from parseExpression; text=<%1%>") % expr_text).str());
  }
  if (fabs(result - expected) > 1.0e-6) {
    throw std::runtime_error((boost::format("parsing failed: wrong answer; text=<%s> expected=%g result=%g") % expr_text % expected % result).str());
  }
}

int main() {
  const char *vb = getenv("VERBOSE");
  if (vb) {
    verbose = strtol(vb, NULL, 0);
  }
  testExpression("0", 0);
  testExpression("(1.2)", 1.2);
  testExpression("((6.23))", 6.23);
  testExpression("1+2", 3);
  testExpression("9 - 4", 5);
  testExpression("(11-(2+3))", 6);
  testExpression("+0", 0);
  testExpression("-0", 0);
  testExpression("-1", -1);
  testExpression("--1", 1);
  testExpression("++1", 1);
  testExpression("+-1", -1);
  testExpression("1+-1", 0);
  testExpression("3+4*5", 23);
  testExpression("3+(4*5)", 23);
  testExpression("(3+4)*5", 35);
  testExpression("-(3+4)*5", -35);
  testExpression("(3 + 4) * 5", 35);
  testExpression("(2+3) * (7-4) + 11", 26);
  testExpression("(2+3) * -(7-4) + 11", -4);
}
// vim: set ts=2 sts=2 sw=2 et :
