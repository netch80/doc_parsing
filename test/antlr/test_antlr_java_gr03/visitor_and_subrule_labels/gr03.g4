grammar gr03;

expression: addsub;

addsub: muldiv #as0
  | addsub '+' muldiv #asplus
  | addsub '-' muldiv #asminus
  ;
muldiv: unary #md0
  | muldiv '*' unary #mdmul
  | muldiv '/' unary #mddiv
  ;
unary: power #un0
  | '+' unary #unplus
  | '-' unary #unminus
  ;
power: atom '**' unary #pwrpwr
  | atom #pwr0
  ;
atom: '(' expression ')' # atomexpr
  | NUMBER #atomnum
  ;

NUMBER: [0-9]+;
WS: [ \t\r\n]+ -> skip;
