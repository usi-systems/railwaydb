%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {intergdb::lang}
%define parser_class_name {Parser}
 
%code requires{
   namespace intergdb {
     namespace lang {
        class Driver;
         class Scanner;
     }     
   }           
}
 
%lex-param   { Scanner  &scanner  }
%parse-param { Scanner  &scanner  }
 
%lex-param   { Driver  &driver  }
%parse-param { Driver  &driver  }
 
%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   
   /* include for all driver functions */
   #include "Driver.h"
  
   /* this is silly, but I can't figure out a way around */
   static int yylex(intergdb::lang::Parser::semantic_type *yylval,
                    intergdb::lang::Scanner  &scanner,
                    intergdb::lang::Driver   &driver);
   
}
 
/* token types */
%union {
   std::string *sval;
}
 

%token            END    0     "end of file" 
%token   <sval>   IDENTIFIER 
%token   <sval>   CONSTANT 
%token   <sval>   STRING_LITERAL 
%token            RIGHT_ARROW
 
/* destructor rule for <sval> objects */
%destructor { if ($$)  { delete ($$); ($$) = nullptr; } } <sval>
  
%%
 
program : END | decls END;
 
decls
  : decl
  | decls decl
  ;
 
decl
  : vertex
  | edge
  ;

vertex
  : CONSTANT STRING_LITERAL         { driver.printVertex(); } 
  ;
 
edge
  : CONSTANT RIGHT_ARROW CONSTANT   { driver.printEdge(); } 
  ;

%%
 
 
void 
intergdb::lang::Parser::error( const std::string &err_message )
{
   std::cerr << "Error: " << err_message << "\n"; 
}
 
 
/* include for access to scanner.yylex */
#include "Scanner.h"
static int 
yylex( intergdb::lang::Parser::semantic_type *yylval,
       intergdb::lang::Scanner  &scanner,
       intergdb::lang::Driver   &driver )
{
   return( scanner.yylex(yylval) );
}

