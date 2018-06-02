#ifndef tokens_h_included_
#define tokens_h_included_

enum tokens {
  tt_NONE,
  tt_Eof,
  tt_Number,
  tt_Plus,
  tt_Minus,
  tt_Star,
  tt_Slash,
  tt_DoubleStar,
  tt_LeftParen,
  tt_RightParen,
  tt_LeftBracket,
  tt_RightBracket,
  tt_Eq,
  tt_Identifier,
  tt_Keyword
};

#endif
// vim: set ts=2 sts=2 sw=2 et :
