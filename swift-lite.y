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


"&&"										{return AND;}
"||"										{return OR};
"!"											{return NOT;}

"+"											{return PLUS;}
"-"											{return MINUS;}
"*"											{return MULTI;}
"/"											{return DIVID;}

"++"										{return INCREASE;}
"--"										{return DECREASE;}

"("											{return LBR}
")"											{return RBR}
"{"											{return LCBR}
"}"											{return RCBR}

"="											{return ASSIGN;}

"return"     								{return RETURN;}
"if"         								{return IF;}
"else"       								{return ELSE;}
"while"      								{return WHILE;}
"\n"|";"	                                {return EOL;}

%token NEW ANNOUNCE ASSIGN LBR RBR LCBR RCBR RETURN IF ELSE WHILE EOL
%token AND OR NOT PLUS MINUS MULTI DIVID INCREASE DECREASE


%left ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%left MOD
%right UMINUS NOT DPLUS

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
Specifier:  TYPE    {$$=make_node(0,TYPE,yylineno);$$->type_id = $1;$$->type=($1 == "float")?FLOAT:INT;}   
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

StmList: {$$=NULL; }  
        | Stmt StmList  {$$=make_node(2,STM_LIST,yylineno,$1,$2);}
        ;
Stmt:   Exp SEMI    {$$=make_node(1,EXP_STMT,yylineno,$1);}
      | CompSt      {$$=$1;}
      | RETURN Exp SEMI   {$$=make_node(1,RETURN,yylineno,$2);}
      | IF LP Exp RP Stmt %prec LOWER_THEN_ELSE   {$$=make_node(2,IF_THEN,yylineno,$3,$5);}
      | IF LP Exp RP Stmt ELSE Stmt   {$$=make_node(3,IF_THEN_ELSE,yylineno,$3,$5,$7);}
      | WHILE LP Exp RP Stmt {$$=make_node(2,WHILE,yylineno,$3,$5);}
      ;
DefList: {$$=NULL; }
        | Def DefList {$$=make_node(2,DEF_LIST,yylineno,$1,$2);}
        | error SEMI   {$$=NULL;}
        ;
Def:    Specifier DecList SEMI {$$=make_node(2,VAR_DEF,yylineno,$1,$2);}
        ;
DecList: Dec  {$$=make_node(1,DEC_LIST,yylineno,$1);}
       | Dec COMMA DecList  {$$=make_node(2,DEC_LIST,yylineno,$1,$3);}
	   ;
Dec:     VarDec  {$$=$1;}
       | VarDec ASSIGNOP Exp  {$$=make_node(2,ASSIGNOP,yylineno,$1,$3);$$->type_id = "ASSIGNOP";}
       ;
Exp:    Exp ASSIGNOP Exp {$$=make_node(2,ASSIGNOP,yylineno,$1,$3);$$->type_id = "ASSIGNOP";}
      | Exp AND Exp   {$$=make_node(2,AND,yylineno,$1,$3);$$->type_id = "AND";}
      | Exp OR Exp    {$$=make_node(2,OR,yylineno,$1,$3);$$->type_id = "OR";}
      | Exp RELOP Exp {$$=make_node(2,RELOP,yylineno,$1,$3);$$->type_id = $2;}
      | Exp PLUS Exp  {$$=make_node(2,PLUS,yylineno,$1,$3);$$->type_id = "PLUS";}
      | Exp MINUS Exp {$$=make_node(2,MINUS,yylineno,$1,$3);$$->type_id = "MINUS";}
      | Exp STAR Exp  {$$=make_node(2,STAR,yylineno,$1,$3);$$->type_id = "STAR";}
      | Exp MOD Exp  {$$=make_node(2,MOD,yylineno,$1,$3);$$->type_id = "MOD";}
      | Exp DIV Exp   {$$=make_node(2,DIV,yylineno,$1,$3);$$->type_id = "DIV";}
      | LP Exp RP     {$$=$2;}
      | MINUS Exp %prec UMINUS   {$$=make_node(1,UMINUS,yylineno,$2);$$->type_id = "UMINUS";}
      | NOT Exp       {$$=make_node(1,NOT,yylineno,$2);$$->type_id = "NOT";}
      | DPLUS  Exp      {$$=make_node(1,DPLUS,yylineno,$2);$$->type_id = "DPLUS";}
      |   Exp DPLUS      {$$=make_node(1,DPLUS,yylineno,$1);$$->type_id = "DPLUS";}
      | ID LP Args RP {$$=make_node(1,FUNC_CALL,yylineno,$3);$$->type_id = $1;}
      | ID LP RP      {$$=make_node(0,FUNC_CALL,yylineno);$$->type_id = $1;}
      | ID            {$$=make_node(0,ID,yylineno);$$->type_id = $1;}
      | INT           {$$=make_node(0,INT,yylineno);$$->type_int=$1;$$->type=INT;}
      | FLOAT         {$$=make_node(0,FLOAT,yylineno);$$->type_float=$1;$$->type=FLOAT;}
      ;
Args:    Exp COMMA Args    {$$=make_node(2,ARGS,yylineno,$1,$3);}
       | Exp               {$$=make_node(1,ARGS,yylineno,$1);}
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
}
