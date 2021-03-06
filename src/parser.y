%define parse.error verbose
%locations
%{
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "swift-lite.h"
bool didPass = true;
extern int yylineno;
extern char *yytext;
extern FILE *yyin;
void yyerror(const char* fmt, ...);
void print_ast_node(ASTNode *,int);
int yylex();
%}

%union {
	int intValue;
	float floatValue;
	char charValue;
	char idValue[32];
        char cmpCommand[2];
        class ASTNode *ptr;
};

%type  <ptr> Exp STATEMENT WHOLESTATEMENT STATEMENTLIST Args PARAMETERS PARAMETER VARLIST VAR Specifier DEFINE DEFINEASSIGN  program ExternalDefineList ExternalDefine FUNCDEFINE FUNCCALL 

%token <intValue> INTEGER
%token <idValue> ID TYPE
%token <floatValue> FLOAT
%token <charValue> CHAR
%token <cmpCommand> COMPARE

%token NEW ANNOUNCE ASSIGN LBR RBR LCBR RCBR RETURN IF ELSE WHILE BREAK CONTINUE FUNC FUNC_RETURN_TYPE COMMA EOL
%token AND OR NOT PLUS MINUS MULTI DIVID INCREASE DECREASE 
%token FUNC_CALL FUNC_ANNOUNCE IF_THEN IF_THEN_ELSE Exp_STATMENT WHOLE_STATEMENT STATEMENT_LIST ARGS FUNC_PARAMETERS FUNC_PARAMETER 
%token VAR_LIST VAR_DEFINE DEFINE_ASSIGN DEFINE_LIST EXT_DEF_LIST EXT_DEF FUNCTION


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

program: 
        ExternalDefineList {if(didPass){print_ast_node($1,0);entrypoint($1);}else{printf("\n parse failed, Compile preogree exit early! \n");} }
        ;
ExternalDefineList:
        ExternalDefine {$$=make_node(EXT_DEF_LIST,yylineno,{$1});}
      | ExternalDefine ExternalDefineList {$$=make_node(EXT_DEF_LIST,yylineno,{$1,$2});}        
        ;

ExternalDefine:
        DEFINE {$$=$1;}
      | DEFINEASSIGN {$$=$1;}
      | FUNCDEFINE WHOLESTATEMENT {$$=make_node(FUNCTION,yylineno,{$1,$2});}
      | error EOL {$$=nullptr;}
      | EOL {$$=nullptr;}
        ;
STATEMENT:
      
      IF LBR Exp RBR WHOLESTATEMENT %prec LOWER_THEN_ELSE {$$=make_node(IF_THEN,yylineno,{$3,$5});}
      | IF LBR Exp RBR WHOLESTATEMENT ELSE WHOLESTATEMENT {$$=make_node(IF_THEN_ELSE,yylineno,{$3,$5,$7});}
      
      | WHILE LBR Exp RBR WHOLESTATEMENT {$$=make_node(WHILE,yylineno,{$3,$5});}

      | BREAK EOL {$$=make_node(BREAK,yylineno);}
      | CONTINUE EOL {$$=make_node(CONTINUE,yylineno);}
      | RETURN EOL   {$$=make_node(RETURN,yylineno,{nullptr});}
      | RETURN Exp EOL   {$$=make_node(RETURN,yylineno,{$2});}
      | FUNCCALL EOL {$$=$1;}
      | Exp EOL {$$=make_node(Exp_STATMENT,yylineno,{$1});}
      | DEFINE EOL{$$=$1;}
      | DEFINEASSIGN EOL{$$=$1;}
      | EOL {$$=nullptr;}
      ;
WHOLESTATEMENT:
      LCBR STATEMENTLIST RCBR {$$=make_node(WHOLE_STATEMENT,yylineno,{$2});}
      ;
STATEMENTLIST:
        STATEMENT {$$=make_node(STATEMENT_LIST,yylineno,{$1});}
      | STATEMENT STATEMENTLIST {$$=make_node(STATEMENT_LIST,yylineno,{$1,$2});}
      ;

PARAMETERS:
        PARAMETER {$$=make_node(FUNC_PARAMETERS,yylineno,{$1});}
      | PARAMETER COMMA PARAMETERS {$$=make_node(FUNC_PARAMETERS,yylineno,{$1,$3});}
        ;

PARAMETER:
        VAR ANNOUNCE Specifier {$$=make_node(FUNC_PARAMETER,yylineno,{$1,$3});}
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
        DEFINE ASSIGN Exp {$$=make_node(ASSIGN,yylineno,{$1,$3});}
        ;

FUNCDEFINE:
        FUNC VAR LBR RBR {$$=make_node(FUNC_ANNOUNCE,yylineno,{$2});}
      | FUNC VAR LBR PARAMETERS RBR {$$=make_node(FUNC_ANNOUNCE,yylineno,{$2,$4});}
      | FUNC VAR LBR RBR FUNC_RETURN_TYPE Specifier {$$=make_node(FUNC_ANNOUNCE,yylineno,{$2,$6});}
      | FUNC VAR LBR PARAMETERS RBR FUNC_RETURN_TYPE Specifier {$$=make_node(FUNC_ANNOUNCE,yylineno,{$2,$4,$7});}
        ;
FUNCCALL:
        ID LBR Args RBR {$$=make_node(FUNC_CALL,yylineno,{$3});$$->data = $1;} 
      | ID LBR RBR      {$$=make_node(FUNC_CALL,yylineno);$$->data = $1;}
        ;
Exp:
        Exp ASSIGN Exp  {$$=make_node(ASSIGN,yylineno,{$1,$3});}
      | Exp AND Exp     {$$=make_node(AND,yylineno,{$1,$3});}
      | Exp OR Exp      {$$=make_node(OR,yylineno,{$1,$3});}
      | Exp COMPARE Exp {$$=make_node(COMPARE,yylineno,{$1,$3});$$->data = $2;}
      | Exp PLUS Exp    {$$=make_node(PLUS,yylineno,{$1,$3});}
      | Exp MINUS Exp   {$$=make_node(MINUS,yylineno,{$1,$3});}
      | Exp MULTI Exp   {$$=make_node(MULTI,yylineno,{$1,$3});}
      | Exp DIVID Exp   {$$=make_node(DIVID,yylineno,{$1,$3});}

      
      | FUNCCALL ASSIGN Exp  {$$=make_node(ASSIGN,yylineno,{$1,$3});}
      | FUNCCALL AND Exp     {$$=make_node(AND,yylineno,{$1,$3});}
      | FUNCCALL OR Exp      {$$=make_node(OR,yylineno,{$1,$3});}
      | FUNCCALL COMPARE Exp {$$=make_node(COMPARE,yylineno,{$1,$3});$$->data = $2;}
      | FUNCCALL PLUS Exp    {$$=make_node(PLUS,yylineno,{$1,$3});}
      | FUNCCALL MINUS Exp   {$$=make_node(MINUS,yylineno,{$1,$3});}
      | FUNCCALL MULTI Exp   {$$=make_node(MULTI,yylineno,{$1,$3});}
      | FUNCCALL DIVID Exp   {$$=make_node(DIVID,yylineno,{$1,$3});}
      
      | Exp ASSIGN FUNCCALL  {$$=make_node(ASSIGN,yylineno,{$1,$3});}
      | Exp AND FUNCCALL     {$$=make_node(AND,yylineno,{$1,$3});}
      | Exp OR FUNCCALL      {$$=make_node(OR,yylineno,{$1,$3});}
      | Exp COMPARE FUNCCALL {$$=make_node(COMPARE,yylineno,{$1,$3});$$->data = $2;}
      | Exp PLUS FUNCCALL    {$$=make_node(PLUS,yylineno,{$1,$3});}
      | Exp MINUS FUNCCALL   {$$=make_node(MINUS,yylineno,{$1,$3});}
      | Exp MULTI FUNCCALL   {$$=make_node(MULTI,yylineno,{$1,$3});}
      | Exp DIVID FUNCCALL   {$$=make_node(DIVID,yylineno,{$1,$3});}

      | FUNCCALL ASSIGN FUNCCALL  {$$=make_node(ASSIGN,yylineno,{$1,$3});}
      | FUNCCALL AND FUNCCALL     {$$=make_node(AND,yylineno,{$1,$3});}
      | FUNCCALL OR FUNCCALL      {$$=make_node(OR,yylineno,{$1,$3});}
      | FUNCCALL COMPARE FUNCCALL {$$=make_node(COMPARE,yylineno,{$1,$3});$$->data = $2;}
      | FUNCCALL PLUS FUNCCALL    {$$=make_node(PLUS,yylineno,{$1,$3});}
      | FUNCCALL MINUS FUNCCALL   {$$=make_node(MINUS,yylineno,{$1,$3});}
      | FUNCCALL MULTI FUNCCALL   {$$=make_node(MULTI,yylineno,{$1,$3});}
      | FUNCCALL DIVID FUNCCALL   {$$=make_node(DIVID,yylineno,{$1,$3});}
      | NOT FUNCCALL              {$$=make_node(NOT,yylineno,{$2});}
      | INCREASE FUNCCALL    {$$=make_node(INCREASE,yylineno,{$2});}
      | FUNCCALL INCREASE    {$$=make_node(INCREASE,yylineno,{$1});}
      | DECREASE FUNCCALL    {$$=make_node(DECREASE,yylineno,{$2});}
      | FUNCCALL DECREASE    {$$=make_node(DECREASE,yylineno,{$1});}

      | LBR Exp RBR     {$$=$2;}

      | NOT Exp         {$$=make_node(NOT,yylineno,{$2});}
      | INCREASE Exp    {$$=make_node(INCREASE,yylineno,{$2});}
      | Exp INCREASE    {$$=make_node(INCREASE,yylineno,{$1});}
      | DECREASE Exp    {$$=make_node(DECREASE,yylineno,{$2});}
      | Exp DECREASE    {$$=make_node(DECREASE,yylineno,{$1});}

      | ID              {$$=make_node(ID,yylineno);$$->data = $1;}
      | INTEGER         {$$=make_node(INTEGER,yylineno);$$->data=$1;$$->type="Int";}
      | FLOAT           {$$=make_node(FLOAT,yylineno);$$->data=$1;$$->type="Float";}
      | CHAR            {$$=make_node(CHAR,yylineno);$$->data=$1;$$->type="Char";}
      ;       
Args:
        Exp COMMA Args    {$$=make_node(ARGS,yylineno,{$1,$3});}
      | Exp               {$$=make_node(ARGS,yylineno,{$1});}
        ;
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
    didPass = false;
}
