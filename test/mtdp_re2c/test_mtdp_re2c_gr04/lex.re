#include <string>
#include "lex.h"
#include "tokens.h"

/*!max:re2c*/

void lex_init(lex_state *lr, const std::string& input) {
  lr->input = input;
  for (int i = 0; i < YYMAXFILL; ++i) {
    lr->input.push_back(0);
  }
  lr->input_pos = lr->input.c_str();
}

bool lex(lex_state *lr) {
  /*!stags:re2c format = 'const char *@@;'; */
  const char *p1 = nullptr;
  const char *p2 = nullptr;
  retry:
  /*!re2c
    re2c:define:YYCTYPE = char;
    re2c:yyfill:enable = 0;
    re2c:define:YYCURSOR = "lr->input_pos";

    ident = [A-Za-z_][A-Za-z0-9_]*;
    dec = [0-9]+;
    wsp = [ \t\r\n\v\f]+;
    end = "\x00";

    * {
      return false;
    }
    @p1 dec @p2 {
      lr->tok_type = tt_Number;
      lr->str_value.assign(p1, p2-p1);
      lr->int_value = strtol(lr->str_value.c_str(), nullptr, 10);
      return true;
    }
    @p1 ident @p2 {
      lr->tok_type = tt_Identifier;
      lr->str_value.assign(p1, p2-p1);
      return true;
    }
    "@" @p1 ident @p2 {
      lr->tok_type = tt_Keyword;
      lr->str_value.assign(p1, p2-p1);
      return true;
    }
    [+] { lr->tok_type = tt_Plus; return true; }
    [-] { lr->tok_type = tt_Minus; return true; }
    [*][*] { lr->tok_type = tt_DoubleStar; return true; }
    [*] { lr->tok_type = tt_Star; return true; }
    [/] { lr->tok_type = tt_Slash; return true; }
    [(] { lr->tok_type = tt_LeftParen; return true; }
    [)] { lr->tok_type = tt_RightParen; return true; }
    [\[] { lr->tok_type = tt_LeftBracket; return true; }
    [\]] { lr->tok_type = tt_RightBracket; return true; }
    [=] { lr->tok_type = tt_Eq; return true; }
    wsp { goto retry; }
    end { lr->tok_type = tt_Eof; return true; }
   */
}
// vim: set ts=2 sts=2 sw=2 et :
