/* -*-bison-*- */
/* Simple wc implementation in Flex */
%option noyywrap
%option noinput
%option nounput
%{
#include "ytab.h"
int lineno = 1; // keep track of line number 
int multi_line_start; // keep track of when a multi line comment begins to give error if it is unclosed
%}

whitespace  [ \t\r]+
inline_comment  \/\/[^\n]*|\/\*[^\n]\*\/
single_delim \/\*[^\n]*
hash_line  \#[^\n]*
identifier  [a-zA-Z_][a-zA-Z0-9_]*
illegal_identifier ^[^a-zA-Z_][a-zA-Z0-9_]*
basic_type "byte"|"short"|"int"|"long"|"float"|"double"|"boolean"|"string"
integer_literal  [0-9]+[1-9]*|0[xX][0-9A-Fa-f]+
real_literal  ([0-9]+\.[0-9]*|\.[0-9]+)
supported_escapes  \\[nt'"r0b$\\]
string_literal \"([^\\\"\n]|\\.)*\"
multi_string_literal \"\"\"([^\\\"]|\\.)*\"\"\"
%x COMMENT

/* RULES */
%%
 /* COMMENT state for multi-line comments */ 
"/*"    { BEGIN(COMMENT); multi_line_start = lineno; } /* switch to comment state where nothing will be counted */
<COMMENT>"*/"   {BEGIN(INITIAL); } /* switch back to initial flex state */
<COMMENT>(.) {}
<COMMENT>(\n) {lineno++;}
<COMMENT><<EOF>> {return UNCLOSED_COMMENT;}

 /* Initial state */
\n		{ lineno++; }
{inline_comment}    {}
{hash_line}  {}
{whitespace}  {}   


 /* K0 Reserved Keywords */
"break"    { return BREAK; }
"continue" { return CONTINUE; }
"do"       { return DO; }
"else"    { return ELSE; }
"false"    { return K_FALSE; }
"for"      { return FOR; }
"fun"      { return FUN; }
"if"       { return IF; }
"in"       { return IN; }
"null"     { return K_NULL; }
"return"   { return RETURN; }
"true"     { return K_TRUE; }
"val"      { return VAL; }
"var"      { return VAR; }
"when"     { return WHEN; }
"while"   { return WHILE; }
"import"  { return IMPORT; }

 /* Kotline Reserved Keywords that we are not dealing with, return KEYWORD_ERROR */
"!in"        { return KEYWORD_ERROR; }
"!is"        { return KEYWORD_ERROR; }
"as"        { return KEYWORD_ERROR; }
"as?"        { return KEYWORD_ERROR; }
"class"      { return KEYWORD_ERROR; }
"is"         { return KEYWORD_ERROR; }
"object"     { return KEYWORD_ERROR; }
"package"    { return KEYWORD_ERROR; }
"super"      { return KEYWORD_ERROR; }
"this"       { return KEYWORD_ERROR; }
"throw"      { return KEYWORD_ERROR; }
"try"        { return KEYWORD_ERROR; }
"typealias"  { return KEYWORD_ERROR; }
"typeof"     { return KEYWORD_ERROR; }
 /* Soft keywords that we are not dealing with */
"by"           { return KEYWORD_ERROR; }
"catch"        { return KEYWORD_ERROR; }
"constructor"  { return KEYWORD_ERROR; }
"delegate"     { return KEYWORD_ERROR; }
"dynamic"      { return KEYWORD_ERROR; }
"field"        { return KEYWORD_ERROR; }
"file"         { return KEYWORD_ERROR; }
"finally"      { return KEYWORD_ERROR; }
"get"          { return KEYWORD_ERROR; }
"init"         { return KEYWORD_ERROR; }
"param"        { return KEYWORD_ERROR; }
"property"     { return KEYWORD_ERROR; }
"receiver"     { return KEYWORD_ERROR; }
"set"          { return KEYWORD_ERROR; }
"setparam"     { return KEYWORD_ERROR; }
"value"        { return KEYWORD_ERROR; }
"where"        { return KEYWORD_ERROR; }
 /* Kotlin modifier keywords not in k0 */
"abstract"    { return KEYWORD_ERROR; }
"actual"      { return KEYWORD_ERROR; }
"annotation"  { return KEYWORD_ERROR; }
"companion"   { return KEYWORD_ERROR; }
"crossinline" { return KEYWORD_ERROR; }
"data"        { return KEYWORD_ERROR; }
"enum"        { return KEYWORD_ERROR; }
"expect"      { return KEYWORD_ERROR; }
"external"    { return KEYWORD_ERROR; }
"final"       { return KEYWORD_ERROR; }
"infix"       { return KEYWORD_ERROR; }
"inline"      { return KEYWORD_ERROR; }
"inner"       { return KEYWORD_ERROR; }
"internal"    { return KEYWORD_ERROR; }
"lateinit"    { return KEYWORD_ERROR; }
"noinline"    { return KEYWORD_ERROR; }
"open"        { return KEYWORD_ERROR; }
"operator"    { return KEYWORD_ERROR; }
"out"         { return KEYWORD_ERROR; }
"private"     { return KEYWORD_ERROR; }
"protected"   { return KEYWORD_ERROR; }
"public"      { return KEYWORD_ERROR; }
"reified"     { return KEYWORD_ERROR; }
"sealed"      { return KEYWORD_ERROR; }
"suspend"     { return KEYWORD_ERROR; }
"tailrec"     { return KEYWORD_ERROR; }
"vararg"      { return KEYWORD_ERROR; }
"it"          { return KEYWORD_ERROR; }


 /* Operators */
"++"        { return INC; }
"--"        { return DEC; }
"==="       { return REQ; }
"!=="       { return RNEQ; }
">="        { return GTE; }
"<="        { return LTE; }
"=="        { return EQ; }
"!="        { return NEQ; }
"&&"        { return AND; }
"||"        { return OR; }
"!!"        { return NN_ASSERT; }
"?."        { return SAFE_CALL; }
"?:"        { return ELVIS; }
"?"         { return NULLABLE; }
"..<"       { return RANGE_EXCL; }
".."        { return RANGE_INCL; }
"+="        { return PLUS_ASSIGN; }
"-="        { return MINUS_ASSIGN; }
"+"         { return PLUS; }
"-"         { return MINUS; }
"*"         { return MUL; }
"/"         { return DIV; }
"%"         { return MOD; }
"="         { return ASSIGN; }
">"         { return GT; }
"<"         { return LT; }
"!"         { return NOT; }
"."         { return DOT; }
"("{basic_type}")"   { return TYPE_CAST; }
 /* Operators that produce OP_ERROR */
"~"    { return OP_ERROR; }
"<<"   { return OP_ERROR; }
">>"   { return OP_ERROR; }
"&="   { return OP_ERROR; }
"|="   { return OP_ERROR; }
"^="   { return OP_ERROR; }
">>="  { return OP_ERROR; }
"<<="  { return OP_ERROR; }
"&"    { return OP_ERROR; }
"|"    { return OP_ERROR; }
"^"    { return OP_ERROR; }


 /* Punctuation */
"("     { return LPAREN; }
")"     { return RPAREN; }
","     { return COMMA; }
";"     { return SEMICOLON; }
"{"     { return LBRACE; }
"}"     { return RBRACE; }
":"     { return COLON; }
"["     { return LSQUARE; }
"]"     { return RSQUARE; }

 /* Literals */
{integer_literal}   { return INT; }
{real_literal}     { return REAL; }
{string_literal}    { return STRING; }
{multi_string_literal}  { return MULTI_STRING; }

 /* identifier catchall */
{identifier}    { return IDENT; }
{illegal_identifier}  { return ILL_IDENT;}

 /*Could not match */
 .      { return COULD_NOT_MATCH; }

<<EOF>> {
    return 0;
}
%%