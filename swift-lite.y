%define parse.error verbose
%locations
%{
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "def.h"
extern int yylineno;
extern char *yytext;
extern FILE *yyin;
void yyerror(const char* fmt, ...);
void display(ASTNode *,int);
int yylex();
%}

%union {
	int type_int;
	float type_float;
	char type_char;
	char type_id[32];
        char type_cmp[2];
	TYPE type_type;
        class ASTNode *ptr;
};

%type  <ptr> program ExtDefList ExtDef  Specifier ExtDecList FuncDec CompSt VarList VarDec ParamDec Stmt StmList DefList Def DecList Dec Exp Args

%token <type_int> INTEGER
%token <type_id> ID
%token <type_float> FLOAT
%token <type_char> CHAR
%token <type_type> TYPE
%token <type_cmp> COMPARE

%token NEW ANNOUNCE ASSIGN LBR RBR LCBR RCBR RETURN IF ELSE WHILE BREAK CONTINUE FUNC FUNC_RETURN_TYPE EOL
%token AND OR NOT PLUS MINUS MULTI DIVID INCREASE DECREASE 
%token FUNC_CALL 


%left ASSIGN
%left OR
%left AND
%left COMPARE
%left PLUS MINUS
%left MULTI DIVID
%right INCREASE NOT DECREASE

%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE

%%

program: ExtDefList    { display($1,0); entrypoint($1);}
         ;
ExtDefList: {$$=NULL;}
          | ExtDef ExtDefList {$$=make_node(2,EXT_DEF_LIST,yylineno,$1,$2);}
          ;
ExtDef:   Specifier ExtDecList SEMI   {$$=make_node(2,EXT_VAR_DEF,yylineno,$1,$2);}
         |Specifier FuncDec CompSt    {$$=make_node(3,FUNC_DEF,yylineno,$1,$2,$3);}
         | error SEMI   {$$=NULL;}
         ;
ExtDecList:  VarDec      {$$=$1;}
           | VarDec COMMA ExtDecList {$$=make_node(2,EXT_DEC_LIST,yylineno,$1,$3);}
           ;  
VarDec:  ID          {$$=make_node(0,ID,yylineno);$$->type_id = $1;}
         ;
FuncDec: ID LP VarList RP   {$$=make_node(1,FUNC_DEC,yylineno,$3);$$->type_id = $1;}
		|ID LP  RP   {$$=make_node(0,FUNC_DEC,yylineno);$$->type_id = $1;$$->ptr[0]=NULL;}

        ;
VarList: ParamDec  {$$=make_node(1,PARAM_LIST,yylineno,$1);}
        | ParamDec COMMA  VarList  {$$=make_node(2,PARAM_LIST,yylineno,$1,$3);}
        ;
ParamDec: Specifier VarDec         {$$=make_node(2,PARAM_DEC,yylineno,$1,$2);}
         ;

CompSt: LC DefList StmList RC    {$$=make_node(2,COMP_STM,yylineno,$2,$3);}
       ;


Exp:    Exp ASSIGN Exp  {$$=make_node(2,ASSIGN,yylineno,$1,$3);$$->type_id = "ASSIGN";}
      | Exp AND Exp     {$$=make_node(2,AND,yylineno,$1,$3);$$->type_id = "AND";}
      | Exp OR Exp      {$$=make_node(2,OR,yylineno,$1,$3);$$->type_id = "OR";}
      | Exp COMPARE Exp {$$=make_node(2,COMPARE,yylineno,$1,$3);$$->type_id = $2;}
      | Exp PLUS Exp    {$$=make_node(2,PLUS,yylineno,$1,$3);$$->type_id = "PLUS";}
      | Exp MINUS Exp   {$$=make_node(2,MINUS,yylineno,$1,$3);$$->type_id = "MINUS";}
      | Exp MULTI Exp   {$$=make_node(2,MULTI,yylineno,$1,$3);$$->type_id = "MULTI";}
      | Exp DIVID Exp   {$$=make_node(2,DIVID,yylineno,$1,$3);$$->type_id = "DIVID";}

      | LBR Exp RBR     {$$=$2;}

      | NOT Exp         {$$=make_node(1,NOT,yylineno,$2);$$->type_id = "NOT";}
      | INCREASE Exp    {$$=make_node(1,INCREASE,yylineno,$2);$$->type_id = "INCREASE";}
      | Exp INCREASE    {$$=make_node(1,INCREASE,yylineno,$1);$$->type_id = "INCREASE";}
      | DECREASE Exp    {$$=make_node(1,INCREASE,yylineno,$2);$$->type_id = "DECREASE";}
      | Exp DECREASE    {$$=make_node(1,INCREASE,yylineno,$1);$$->type_id = "DECREASE";}

      | ID LBR Args RBR {$$=make_node(1,FUNC_CALL,yylineno,$3);$$->type_id = $1;} 
      | ID LBR RBR      {$$=make_node(0,FUNC_CALL,yylineno);$$->type_id = $1;}

      | ID              {$$=make_node(ID,yylineno);$$->type_id = $1;}
      | INT             {$$=make_node(INT,yylineno);$$->type_int=$1;$$->type=INT;}
      | FLOAT           {$$=make_node(FLOAT,yylineno);$$->type_float=$1;$$->type=FLOAT;}
      | CHAR            {$$=make_node(CHAR,yylineno);$$->type_char=$1;$$->type=CHAR;}
      ;
STATEMENT:
        WHOLESTATEMENT {$$=$1;}
      | FUNC ID LBR RBR STATEMENT {$$=make_node(FUNC_ANNOUNCE,yylineno,{$2,$5});$$->type_id = "FUNC_ANNOUNCE";}
      | FUNC ID LBR FUNCANNOUNCEARGS RBR STATEMENT {$$=make_node(FUNC_ANNOUNCE,yylineno,{$2,$4,$6});$$->type_id = "FUNC_ANNOUNCE";}
      | FUNC ID LBR RBR FUNC_RETURN_TYPE TYPE STATEMENT {$$=make_node(FUNC_ANNOUNCE,yylineno,{$2,$6,$7});$$->type_id = "FUNC_ANNOUNCE";}
      | FUNC ID LBR FUNCANNOUNCEARGS RBR FUNC_RETURN_TYPE TYPE STATEMENT {$$=make_node(FUNC_ANNOUNCE,yylineno,{$2,$4,$7,$8});$$->type_id = "FUNC_ANNOUNCE";}
      
      | IF LBR Exp RBR STATEMENT %prec LOWER_THEN_ELSE {$$=make_node(IF_THEN,yylineno,{$3,$5});$$->type_id = "IF_THEN";}
      | IF LBR Exp RP STATEMENT ELSE STATEMENT {$$=make_node(IF_THEN_ELSE,yylineno,{$3,$5,$7});$$->type_id = "IF_THEN_ELSE";}
      
      | WHILE LBR Exp RBR STATEMENT {$$=make_node(WHILE,yylineno,{$3,$5});$$->type_id = "WHILE";}

      | BREAK EOL {$$=make_node(BREAK,yylineno);}
      | CONTINUE EOL {$$=make_node(CONTINUE,yylineno);}
      | RETURN Exp EOL   {$$=make_node(RETURN,yylineno,{$2});}
      | Exp EOL {$$=make_node(1,Exp_STATMENT,yylineno,$1);}
      | EOL {$$=NULL;}
      ;
    
WHOLESTATEMENT:
        LCBR STATEMENTLIST RCBR {$$=make_node(WHOLE_STATEMENT,yylineno,{$1});}
      ;
STATEMENTLIST:
        {$$=NULL;}
      | STATEMENT STATEMENTLIST {$$=make_node(STATEMENT_LIST,yylineno,{$1,$2});}
      ;

Args:
        Exp COMMA Args    {$$=make_node(ARGS,yylineno,{$1,$3});}
      | Exp               {$$=make_node(ARGS,yylineno,{$1});}
        ;
FUNCANNOUNCEARGS:
        ANNOUNCEARG {$$=make_node(FUNC_ARG_ANNOUNCE,yylineno,{$1});}
      | ANNOUNCEARG COMMA FUNCANNOUNCEARGS {$$=make_node(FUNC_ARG_ANNOUNCE,yylineno,{$1,$3});}
        ;

ANNOUNCEARG:
        VAR ANNOUNCE Specifier {$$=make_node(ARG_ANNOUNCE,yylineno);}
        ;

VARLIST:
        VAR {$$=make_node(VAR_LIST,yylineno,{$1});}
      | VAR COMMA VARLIST {$$=make_node(VAR_LIST,yylineno,{$1,$3});}
      ;
VAR:
        ID {$$=make_node(ID,yylineno);$$->type_id = $1;}
        ;
Specifier:
        TYPE {$$=make_node(TYPE,yylineno);$$->type_type = $1;}
        ;
DEFINE:
        NEW VARLIST ANNOUNCE Specifier {$$=make_node(VAR_DEFINE,yylineno,{$2,$4});$$->type_id = "VAR_DEFINE";}
        ;
DEFINEASSIGN:
        DEFINE ASSIGN Exp {$$=make_node(ASSIGN,yylineno,{$1,$3});}
        ;
DEFINELIST:
        {$$=NULL;}
      | DEFINE {$$=make_node(DEFINE_LIST,yylineno,{$1});}
      | DEFINEASSIGN {$$=make_node(DEFINE_LIST,yylineno,{$1});}
      | error EOL {$$=NULL;}


       
%%

int main(int argc, char *argv[]) {
    yyin = fopen(argv[1], "r");
    if (!yyin) exit(0);
    yylineno = 1;
    yyparse();
    return 0;
}

#include<stdarg.h>
void yyerror(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Grammar Error at Line %d Column %d: ", yylloc.first_line,yylloc.first_column);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}
