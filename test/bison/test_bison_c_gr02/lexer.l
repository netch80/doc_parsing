%option reentrant
%option prefix="el_"
%option outfile="lexer.c"
%option header-file="lexer.h"
%option noyywrap
%option bison-bridge
%{
#include "pli.h"
#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

#define YY_INPUT(buf, result, max_size) \
	{ result = my_fread(yyscanner, buf, max_size); \
	} while(0)
static unsigned my_fread(yyscan_t yyscanner, char *buf, size_t max_size);

//-static double lval_static = 0;

// Damn bison
extern double ep_lval;
%}

%%

\*\*		{
			return DSTAR;
		}
[*]		{
			return STAR;
		}
[/]		{
			return SLASH;
		}
\+		{
			return PLUS;
		}
[-]		{
			return MINUS;
		}
[0-9]+		{
			double value = strtod(yytext, NULL);
			//-lval_static = value;
			//-el_set_lval(&lval_static, yyscanner);
			*yylval_param = value;
			//ep_lval = strtod(yytext, NULL);
			//printf("__: number %lg\n", value);
			return NUMBER;
		}
[ \t]+
[(]		{
			return LPAREN;
		}
[)]		{
			return RPAREN;
		}
		/* NB we shan't detect more than 1 character as long */
.		{
			if (g_verbose) {
				fprintf(stderr, "Wrong token: %*.*s\n",
					(int) yyleng, (int) yyleng, yytext);
			}
			return WRONG_TOKEN;
		}

%%

static unsigned my_fread(yyscan_t yyscanner, char *buf, size_t max_size) {
  struct lexer_source *lsrc = (struct lexer_source*) el_get_extra(yyscanner);
  unsigned put = 0;
  while (max_size > 0 && lsrc->currpos < lsrc->limit) {
    char c = *lsrc->currpos++;
    //printf("__: my_fread: put %d to buffer\n", c);
    *buf++ = c;
    --max_size;
    ++put;
  }
  return put;
}
