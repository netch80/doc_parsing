grammar gr02;

expression: addsub;

addsub: muldiv #as0
  | addsub '+' muldiv #asplus
  | addsub '-' muldiv #asminus
  ;
muldiv: power #md0
  | muldiv '*' power #mdmul
  | muldiv '/' power #mddiv
  ;
power: unary '**' power #pwrpwr
  | unary #pwr0
  ;
unary: atom #un0
  | '+' unary #unplus
  | '-' unary #unminus
  ;
atom: '(' expression ')' # atomexpr
  | NUMBER #atomnum
  ;

NUMBER: [0-9]+;
WS: [ \t\r\n]+ -> skip;
