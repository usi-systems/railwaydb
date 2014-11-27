D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%{

#include <string>
#pragma clang diagnostic ignored "-Wdeprecated-register"
#include "Scanner.h"
 
#define STOKEN( x ) ( new std::string( x ) ) 
#define yyterminate() return( token::END ) 
#define YY_NO_UNISTD_H

typedef intergdb::lang::Parser::token token; 

%}
 
%option debug 
%option nodefault 
%option yyclass="Scanner" 
%option noyywrap 
%option c++
 
%%

"/*"			        { comment(); }

{L}({L}|{D})*	 	    { yylval->sval = STOKEN( yytext ); return( token::IDENTIFIER); }

0[xX]{H}+{IS}?		    { yylval->sval = STOKEN( yytext ); return( token::CONSTANT); }
0{D}+{IS}?	   	        { yylval->sval = STOKEN( yytext ); return( token::CONSTANT); }
{D}+{IS}?		        { yylval->sval = STOKEN( yytext ); return( token::CONSTANT); }
L?'(\\.|[^\\'])+' 	    { yylval->sval = STOKEN( yytext ); return( token::CONSTANT); }

{D}+{E}{FS}?		    { yylval->sval = STOKEN( yytext ); return( token::CONSTANT); }
{D}*"."{D}+({E})?{FS}?	{ yylval->sval = STOKEN( yytext ); return( token::CONSTANT); }
{D}+"."{D}*({E})?{FS}?	{ yylval->sval = STOKEN( yytext ); return( token::CONSTANT); }

L?\"(\\.|[^\\"])*\"	    { yylval->sval = STOKEN( yytext ); return(token::STRING_LITERAL); }

"->"			        { return(token::RIGHT_ARROW); }

[ \t\v\n\f]	   	        { /* ignore whitespace */ }
.		  	            { /* ignore bad characters */ }


%%


void 
intergdb::lang::Scanner::comment()
{
  char c, c1;
       
   loop:
       while ((c = yyinput()) != '*' && c != 0)
           putchar(c);
       
       if ((c1 = yyinput()) != '/' && c != 0)
       {
           unput(c1);
           goto loop;
       }
       
       if (c != 0)
           putchar(c1);
}
