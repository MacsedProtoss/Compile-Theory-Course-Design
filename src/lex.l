%{
#include "parser.tab.h"
#include "stdio.h"
#include "string.h"
#include "swift-lite.h"
int yycolumn=1;
#define YY_USER_ACTION    	yylloc.first_line=yylloc.last_line=yylineno; \
	yylloc.first_column=yycolumn;	yylloc.last_column=yycolumn+yyleng-1; yycolumn+=yyleng;

typedef union {
	int intValue;
	float floatValue;
	char charValue;
	char idValue[32];
    char cmpCommand[2];
} YYLVAL;
#define YYSTYPE YYLVAL
%}

%%
"var"										{if(PRINT_DUALITY){printf("(token,var)\n");};strcpy(yylval.idValue,yytext);  return NEW;}
":"											{if(PRINT_DUALITY){printf("(token,:)\n");};return ANNOUNCE;}
"Char"										{if(PRINT_DUALITY){printf("(token,Char(type))\n");};strcpy(yylval.idValue,yytext);  return TYPE;}
"Int"										{if(PRINT_DUALITY){printf("(token,Int(type))\n");};strcpy(yylval.idValue,yytext);  return TYPE;}
"Float"										{if(PRINT_DUALITY){printf("(token,Float(type))\n");};strcpy(yylval.idValue,yytext);  return TYPE;}
"return"     								{if(PRINT_DUALITY){printf("(token,return)\n");};return RETURN;}
"if"         								{if(PRINT_DUALITY){printf("(token,if)\n");};return IF;}
"else"       								{if(PRINT_DUALITY){printf("(token,else)\n");};return ELSE;}
"while"      								{if(PRINT_DUALITY){printf("(token,while)\n");};return WHILE;}
"break"      								{if(PRINT_DUALITY){printf("(token,break)\n");};return BREAK;}
"continue"      							{if(PRINT_DUALITY){printf("(token,continue)\n");};return CONTINUE;}

"func"										{if(PRINT_DUALITY){printf("(token,func)\n");};return FUNC;}
"->"										{if(PRINT_DUALITY){printf("(token,->)\n");};return FUNC_RETURN_TYPE;}

">"|"<"|">="|"<="|"=="|"!="					{if(PRINT_DUALITY){printf("(token,compare(%s))\n",yytext);};strcpy(yylval.cmpCommand,yytext);	return COMPARE;}

"&&"										{if(PRINT_DUALITY){printf("(token,&&)\n");};return AND;}
"||"										{if(PRINT_DUALITY){printf("(token,||)\n");};return OR;}
"!"											{if(PRINT_DUALITY){printf("(token,!)\n");};return NOT;}

[+]											{if(PRINT_DUALITY){printf("(token,+)\n");};return PLUS;}
[-]											{if(PRINT_DUALITY){printf("(token,-)\n");};return MINUS;}
[*]											{if(PRINT_DUALITY){printf("(token,*)\n");};return MULTI;}
[/]											{if(PRINT_DUALITY){printf("(token,/)\n");};return DIVID;}

"++"										{if(PRINT_DUALITY){printf("(token,++)\n");};return INCREASE;}
"--"										{if(PRINT_DUALITY){printf("(token,--)\n");};return DECREASE;}

[(]											{if(PRINT_DUALITY){printf("(token,'(')\n");};return LBR;}
[)]											{if(PRINT_DUALITY){printf("(token,')')\n");};return RBR;}
[{]											{if(PRINT_DUALITY){printf("(token,'{')\n");};return LCBR;}
[}]											{if(PRINT_DUALITY){printf("(token,'}')\n");};return RCBR;}

[=]											{if(PRINT_DUALITY){printf("(token,=)\n");};return ASSIGN;}

[,]											{if(PRINT_DUALITY){printf("(token,',')\n");};return COMMA;}

['][A-Za-z0-9_][']							{if(PRINT_DUALITY){printf("(token,charValue(%c))\n",yytext[1]);};yylval.charValue=yytext[1];  return CHAR;}
[+-]?0[.][0-9]*|[+-]?[1-9][0-9]*[.][0-9]*       {if(PRINT_DUALITY){printf("(token,floatValue(%s))\n",yytext);};yylval.floatValue=atof(yytext); return FLOAT;}
0|[+-]?[1-9][0-9]*                          {if(PRINT_DUALITY){printf("(token,intValue(%s))\n",yytext);};yylval.intValue=atoi(yytext);	return INTEGER;}
[_a-zA-Z][_a-zA-Z0-9]*                      {if(PRINT_DUALITY){printf("(token,ID(%s))\n",yytext);};strcpy(yylval.idValue,yytext);	return ID;}

[\n;]	                                	{if(PRINT_DUALITY){printf("(token,EOL)\n");};if(yytext[0]=='\n'){yylineno++;yycolumn=1;};return EOL;}
[ \r\t]     								{}

.                                           {printf("Error type A :Mysterious character \"%s\"\n\t at Line %d\n", yytext, yylineno);}
%%

int yywrap()
{
return 1;
}

