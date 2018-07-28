grammar gr03;

PLUS: '+';
MINUS: '-';
STAR: '*';
SLASH: '/';
DSTAR: '**';
LPAREN: '(';
RPAREN: ')';
NUMBER: [0-9]+;
WS: [ \t\r\n]+ -> skip;

expression: addsub;

addsub: muldiv
  | addsub PLUS muldiv
  | addsub MINUS muldiv
  ;
muldiv: unary
  | muldiv STAR unary
  | muldiv SLASH unary
  ;
unary: power
  | PLUS unary
  | MINUS unary
  ;
power: atom DSTAR unary
  | atom
  ;
atom: '(' expression ')'
  | NUMBER
  ;
