#include <boost/spirit/include/qi.hpp>
#include <boost/format.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <stdexcept>

namespace {

  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
  using SI = std::string::iterator;

  // The suggested method to create recursive rule set is to define them
  // as struct or class members.
  struct Parser : qi::grammar<SI, double(), ascii::space_type> {
    qi::rule<SI, double(), ascii::space_type> expression, addsub, atom;
    Parser() : Parser::base_type(expression) {
      using qi::double_;
      using qi::_val;
      using qi::_1;
      using boost::phoenix::ref;
      expression = addsub[ _val = _1 ];
      addsub = atom [ _val = _1 ] >>
          *(
              ('+' >> atom [ _val = _val + _1 ])
            | ('-' >> atom [ _val = _val - _1 ])
          );
      atom =
            double_[ _val = _1 ]
          | '(' >> expression[ _val = _1 ] >> ')'
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
  double result = -1.0e40;
  bool r = parseExpression(expr_text, /*ref*/ result);
  if (!r) {
    throw std::runtime_error((boost::format("parsing failed: false from parseExpression; text=<%1%>") % expr_text).str());
  }
  if (abs(result - expected) > 1.0e-6) {
    throw std::runtime_error((boost::format("parsing failed: wrong answer; text=<%s> expected=%g result=%g") % expr_text % expected % result).str());
  }
}

int main() {
  testExpression("0", 0);
  testExpression("(1.2)", 1.2);
  testExpression("((6.23))", 6.23);
  testExpression("1+2", 3);
  testExpression("9 - 4", 5);
}
// vim: set ts=2 sts=2 sw=2 et :
