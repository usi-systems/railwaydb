#ifndef __SCANNER_HPP__
#define __SCANNER_HPP__ 1
 
#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif
 
#undef  YY_DECL
#define YY_DECL int  intergdb::lang::Scanner::yylex()
 
#include "Parser.tab.hh"
 
namespace intergdb {
namespace lang {
 
class Scanner : public yyFlexLexer{
public:
   
   Scanner(std::istream *in) : yyFlexLexer(in),
                                  yylval( nullptr ){};
   
   int yylex(intergdb::lang::Parser::semantic_type *lval)
   {
      yylval = lval;
      return( yylex() ); 
   }
   
 
private:
   /* hide this one from public view */
   int yylex();
   /* yyval ptr */
   intergdb::lang::Parser::semantic_type *yylval;
   void comment();


};
 
} /* end namespace lang */
} /* end namespace intergdb */
 
#endif /* END __MCSCANNER_HPP__ */


