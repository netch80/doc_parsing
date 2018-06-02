%define api.pure full
%define api.prefix {ep_}
%define api.value.type {double}
%param {struct parser_state *pstate}
%{
#include "pli.h"
#include <math.h>
#include "lexer.h"

int ep_lex(YYSTYPE*, struct parser_state*);
void ep_error(struct parser_state *_pstate, const char *estr);
%}

%token NUMBER PLUS MINUS STAR SLASH DSTAR LPAREN RPAREN WRONG_TOKEN

%%

exp: addsub {
		//printf("__: exp\n");
		$$ = $1;
		pstate->exp_result = $1;
	};
addsub: muldiv { $$ = $1; } |
        addsub PLUS muldiv { $$ = $1 + $3; } |
	addsub MINUS muldiv { $$ = $1 - $3; };
muldiv: power { $$ = $1; }|
        muldiv STAR power { $$ = $1 * $3; } |
	muldiv SLASH power { $$ = $1 / $3; };
power:  unary DSTAR power { $$ = pow($1, $3); } |
        unary { $$ = $1; };
unary: atom { $$ = $1; } |
        PLUS unary { $$ = $2; } |
	MINUS unary { $$ = -$2; };
atom: NUMBER { $$ = $1; } |
        LPAREN exp RPAREN { $$ = $2; };
%%

int ep_lex(YYSTYPE *lvalp, struct parser_state *pstate) {
  yyscan_t lexer_cookie = pstate->lexer_cookie;
  int ret = el_lex(lvalp, lexer_cookie);
  //printf("__: ep_lex: ret=%d value=%lg\n", ret, *lvalp);
  return ret;
}

void ep_error(struct parser_state *_pstate, const char *estr) {
  if (g_verbose) {
    fprintf (stderr, "ep_error: %s\n", estr);
  }
}
