grammar gr04;

statement: stmt_defmap #stmt1
  | expression #stmt0
  ;
 
stmt_defmap: KW_DEFMAP IDENTIFIER
  ;

expression: assignment;

assignment: primary '=' assignment #asg1
  | addsub #asg0
  ;

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
unary: primary #un0
  | '+' unary #unplus
  | '-' unary #unminus
  ;
primary: '(' expression ')' # primexpr
  | IDENTIFIER '[' expression ']' # primindex
  | atom #primatom
  ;

atom: NUMBER #atomnum
  | IDENTIFIER #atomid
  ;

KW_DEFMAP: '@defmap';
IDENTIFIER: [A-Za-z_][A-Za-z0-9_]*;
NUMBER: [0-9]+;
WS: [ \t\r\n]+ -> skip;
