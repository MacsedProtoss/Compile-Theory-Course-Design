%error-verbose
%locations
%{
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "parser.tab.h"
#include "swift-lite.h"
extern int yylineno;
extern char *yytext;
extern FILE *yyin;
void yyerror(const char* fmt, ...);
void display(ASTNode *,int);
int yylex();
%}

%union {
	int intValue;
	float floatValue;
	char charValue;
	char idValue[32];
        char cmpCommand[2];
	TYPE type;
        class ASTNode *ptr;
};

%type  <ptr> Exp STATEMENT WHOLESTATEMENT STATEMENTLIST Args PARAMETERS PARAMETER VARLIST VAR Specifier DEFINE DEFINEASSIGN DEFINELIST

%token <intValue> INTEGER
%token <idValue> ID
%token <floatValue> FLOAT
%token <charValue> CHAR
%token <type> TYPE
%token <cmpCommand> COMPARE

%token NEW ANNOUNCE ASSIGN LBR RBR LCBR RCBR RETURN IF ELSE WHILE BREAK CONTINUE FUNC FUNC_RETURN_TYPE COMMA EOL
%token AND OR NOT PLUS MINUS MULTI DIVID INCREASE DECREASE 
%token FUNC_CALL FUNC_ANNOUNCE IF_THEN IF_THEN_ELSE Exp_STATMENT WHOLE_STATEMENT STATEMENT_LIST ARGS FUNC_PARAMETERS FUNC_PARAMETER 
%token VAR_LIST VAR_DEFINE DEFINE_ASSIGN DEFINE_LIST


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

Exp:
        Exp ASSIGN Exp  {$$=make_node(ASSIGN,yylineno,{$1,$3});}
      | Exp AND Exp     {$$=make_node(AND,yylineno,{$1,$3});}
      | Exp OR Exp      {$$=make_node(OR,yylineno,{$1,$3});}
      | Exp COMPARE Exp {$$=make_node(COMPARE,yylineno,{$1,$3});$$->data = $2;}
      | Exp PLUS Exp    {$$=make_node(PLUS,yylineno,{$1,$3});}
      | Exp MINUS Exp   {$$=make_node(MINUS,yylineno,{$1,$3});}
      | Exp MULTI Exp   {$$=make_node(MULTI,yylineno,{$1,$3});}
      | Exp DIVID Exp   {$$=make_node(DIVID,yylineno,{$1,$3});}

      | LBR Exp RBR     {$$=$2;}

      | NOT Exp         {$$=make_node(NOT,yylineno,{$2});}
      | INCREASE Exp    {$$=make_node(INCREASE,yylineno,{$2});}
      | Exp INCREASE    {$$=make_node(INCREASE,yylineno,{$1});}
      | DECREASE Exp    {$$=make_node(DECREASE,yylineno,{$2});}
      | Exp DECREASE    {$$=make_node(DECREASE,yylineno,{$1});}

      | ID LBR Args RBR {$$=make_node(FUNC_CALL,yylineno,{$3});$$->data = $1;} 
      | ID LBR RBR      {$$=make_node(FUNC_CALL,yylineno);$$->data = $1;}

      | ID              {$$=make_node(ID,yylineno);$$->data = $1;}
      | INTEGER             {$$=make_node(INT,yylineno);$$->data=$1;$$->type=Int;}
      | FLOAT           {$$=make_node(FLOAT,yylineno);$$->data=$1;$$->type=Float;}
      | CHAR            {$$=make_node(CHAR,yylineno);$$->data=$1;$$->type=Char;}
      ;
STATEMENT:
        WHOLESTATEMENT {$$=$1;}
      | FUNC VAR LBR RBR STATEMENT {$$=make_node(FUNC_ANNOUNCE,yylineno,{$2,$5});}
      | FUNC VAR LBR PARAMETERS RBR STATEMENT {$$=make_node(FUNC_ANNOUNCE,yylineno,{$2,$4,$6});}
      | FUNC VAR LBR RBR FUNC_RETURN_TYPE Specifier STATEMENT {$$=make_node(FUNC_ANNOUNCE,yylineno,{$2,$6,$7});}
      | FUNC VAR LBR PARAMETERS RBR FUNC_RETURN_TYPE Specifier STATEMENT {$$=make_node(FUNC_ANNOUNCE,yylineno,{$2,$4,$7,$8});}
      
      | IF LBR Exp RBR STATEMENT %prec LOWER_THEN_ELSE {$$=make_node(IF_THEN,yylineno,{$3,$5});}
      | IF LBR Exp RBR STATEMENT ELSE STATEMENT {$$=make_node(IF_THEN_ELSE,yylineno,{$3,$5,$7});}
      
      | WHILE LBR Exp RBR STATEMENT {$$=make_node(WHILE,yylineno,{$3,$5});}

      | BREAK EOL {$$=make_node(BREAK,yylineno);}
      | CONTINUE EOL {$$=make_node(CONTINUE,yylineno);}
      | RETURN Exp EOL   {$$=make_node(RETURN,yylineno,{$2});}
      | Exp EOL {$$=make_node(Exp_STATMENT,yylineno,{$1});}
      | EOL {$$=NULL;}
      ;
    
WHOLESTATEMENT:
        {$$=NULL;}
      | LCBR STATEMENTLIST RCBR {$$=make_node(WHOLE_STATEMENT,yylineno,{$2});}
      ;
STATEMENTLIST:
        {$$=NULL;}
      | STATEMENT STATEMENTLIST {$$=make_node(STATEMENT_LIST,yylineno,{$1,$2});}
      ;

Args:
        Exp COMMA Args    {$$=make_node(ARGS,yylineno,{$1,$3});}
      | Exp               {$$=make_node(ARGS,yylineno,{$1});}
        ;
PARAMETERS:
        PARAMETER {$$=make_node(FUNC_PARAMETERS,yylineno,{$1});}
      | PARAMETER COMMA PARAMETERS {$$=make_node(FUNC_PARAMETERS,yylineno,{$1,$3});}
        ;

PARAMETER:
        VAR ANNOUNCE Specifier {$$=make_node(FUNC_PARAMETER,yylineno);}
        ;

VARLIST:
        VAR {$$=make_node(VAR_LIST,yylineno,{$1});}
      | VAR COMMA VARLIST {$$=make_node(VAR_LIST,yylineno,{$1,$3});}
      ;
VAR:
        ID {$$=make_node(ID,yylineno);$$->data = $1;}
        ;
Specifier:
        TYPE {$$=make_node(TYPE,yylineno);$$->type = $1;}
        ;
DEFINE:
        NEW VARLIST ANNOUNCE Specifier {$$=make_node(VAR_DEFINE,yylineno,{$2,$4});}
        ;
DEFINEASSIGN:
        DEFINE ASSIGN Exp {$$=make_node(DEFINE_ASSIGN,yylineno,{$1,$3});}
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
