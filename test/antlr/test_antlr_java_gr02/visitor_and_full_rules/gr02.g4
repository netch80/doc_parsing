grammar gr02;

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
muldiv: power
  | muldiv STAR power
  | muldiv SLASH power
  ;
power: unary DSTAR power
  | unary
  ;
unary: atom
  | PLUS unary
  | MINUS unary
  ;
atom: '(' expression ')'
  | NUMBER
  ;
