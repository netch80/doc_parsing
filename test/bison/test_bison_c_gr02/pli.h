#ifndef pli_h_included_
#define pli_h_included_

struct parser_state {
  double exp_result;
  void *lexer_cookie;
};

struct lexer_source {
  const char *currpos;
  const char *limit;
};

#ifndef EP_STYPE
#define EP_STYPE double
#undef YYSTYPE
#define YYSTYPE double
#endif

extern int g_verbose;

#endif

// vim: set ts=2 sts=2 sw=2 et :
