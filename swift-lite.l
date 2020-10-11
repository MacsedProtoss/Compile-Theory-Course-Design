%{
#include "swift-lite.tab.h"
#include "stdio.h"
#include "string.h"
int yycolumn=1;
#define YY_USER_ACTION    	yylloc.first_line=yylloc.last_line=yylineno; \
	yylloc.first_column=yycolumn;	yylloc.last_column=yycolumn+yyleng-1; yycolumn+=yyleng;

typedef enum{
	Char = 1,
	Int = 2,
	Float = 3
} TYPE;

typedef union {
	int type_int;
	float type_float;
	char type_char;
	char type_id[32];
	char type_cmp[2];
	TYPE type_type;
} YYLVAL;
#define YYSTYPE YYLVAL
%}

%%
"var"										{strcpy(yylval.type_id,yytext);  return NEW;}
":"											{return ANNOUNCE;}
"Char"										{yylval.type_type=Char;  return TYPE;}
"Int"										{yylval.type_type=Int;  return TYPE;}
"Float"										{yylval.type_type=Float;  return TYPE;}
"'"[A-Za-z0-9_]"'"							{yylval.type_char=yytext[1];  return CHAR;}
[+-]?0.[0-9]*|[+-]?[1-9][0-9]*.[0-9]*       {yylval.type_float=atof(yytext); return FLOAT;}
0|[+-]?[1-9][0-9]*                          {yylval.type_int=atoi(yytext);	return INTEGER;}
[_a-zA-Z][_a-zA-Z0-9]*                      {strcpy(yylval.type_id,yytext);	return ID;}

">"|"<"|">="|"<="|"=="|"!="					{strcpy(yylval.type_cmp,yytext);	return COMPARE;}

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
"break"      								{return BREAK;}
"continue"      							{return CONTINUE;}

"func"										{return FUNC;}
"->"										{return FUNC_RETURN_TYPE;}

[,]											{return COMMA}

[\n;]	                                	{return EOL;}
[ \r\t]     								{}

.                                           {printf("Error type A :Mysterious character \"%s\"\n\t at Line %d\n", yytext, yylineno);}
%%

int yywrap()
{
return 1;
}

