#ifndef lex_h_included_
#define lex_h_included_

#include <string>
#include "tokens.h"

struct lex_state {
  std::string input;
  const char *input_pos = nullptr;
  int tok_type = tt_NONE;
  long int_value = 0;
  std::string str_value;
};

void lex_init(lex_state *lr, const std::string& input);
bool lex(lex_state *lr);

#endif
// vim: set ts=2 sts=2 sw=2 et :
