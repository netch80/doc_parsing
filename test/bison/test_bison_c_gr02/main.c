#include "pli.h"
#include "parser.h"
#include "lexer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int g_verbose = 0;

int testExpressionCommon(const char *text, double expected, int exp_fail) {
  struct parser_state pstate;
  struct lexer_source lsrc;
  memset(&pstate, 0, sizeof pstate);
  memset(&lsrc, 0, sizeof lsrc);
  if (g_verbose) {
    printf("testExpression: %s\n", text);
  }
  lsrc.currpos = text;
  lsrc.limit = text + strlen(text);
  yyscan_t flex_state;
  el_lex_init(&flex_state);
  el_set_extra(&lsrc, flex_state);
  pstate.lexer_cookie = flex_state;

  int rc = ep_parse(&pstate);
  if (!exp_fail && rc != 0) {
    fprintf(stderr, "testExpressionCommon: yyparse->%d: text=\"%s\"\n",
        rc, text);
    el_lex_destroy(flex_state);
    return rc;
  }
  if (exp_fail && rc == 0) {
    fprintf(stderr, "testExpressionCommon: yyparse->%d: text=\"%s\"\n",
        rc, text);
    el_lex_destroy(flex_state);
    return 1;
  }
  el_lex_destroy(flex_state);
  if (exp_fail) {
    return 0;
  }
  if (pstate.exp_result != expected) {
    fprintf(stderr, "testExpression fail: got=%lg expected=%lg text=\"%s\"\n",
        pstate.exp_result, expected, text);
    return 1;
  }
  return 0;
}

void testExpression(const char *text, double expected) {
  int rc = testExpressionCommon(text, expected, 0);
  if (rc != 0) {
    fprintf(stderr, "testExpression failed\n");
    exit(1);
  }
}

void testExpressionFail(const char *text) {
  int rc = testExpressionCommon(text, 0, 1);
  if (rc != 0) {
    fprintf(stderr, "testExpressionFail failed\n");
    exit(1);
  }
}

int main() {
  const char *opt = getenv("VERBOSE");
  if (opt) {
    g_verbose = strtol(opt, NULL, 10);
  }
  testExpression("0", 0.0);
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
