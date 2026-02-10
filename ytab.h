/* k0 Keywords */
#define BREAK 258
#define CONTINUE 259
#define DO 260
#define ELSE 261
#define K_FALSE 262
#define FOR 263
#define FUN 264
#define IF 265
#define IN 266
#define K_NULL 267
#define RETURN 268
#define K_TRUE 269
#define VAL 270
#define VAR 271
#define WHEN 272
#define WHILE 273
#define IMPORT 274

/* Errors */
#define OP_ERROR 296         // illegal operator, such as bitwise operations
#define ILL_IDENT 297        // illegal identifier, such as 12e
#define UNCLOSED_COMMENT 298 // good for catching something like /* world
#define COULD_NOT_MATCH 299  // general could not match error - good for catching something like "hello
#define KEYWORD_ERROR 300    // Kotlin reserved keyword not present in k0

/* Assignment operators */
#define ASSIGN 301       // =
#define PLUS_ASSIGN 302  // +=
#define MINUS_ASSIGN 303 // -=

/* Binary arithmetic */
#define PLUS 304  // +
#define MINUS 305 // -
#define MUL 306   // *
#define DIV 307   // /
#define MOD 308   // %

/* increment/decrement */
#define INC 309 // ++
#define DEC 310 // --

/* Unary negation */
#define NEG 311 // -

/* Binary comparison */
#define EQ 312  // ==
#define NEQ 313 // !=
#define GT 314  // >
#define LT 315  // <
#define GTE 316 // >=
#define LTE 317 // <=

/* Referential equality */
#define REQ 318  // ===
#define RNEQ 319 // !==

/* Logical operators */
#define AND 320 // &&
#define OR 321  // ||
#define NOT 322 // !

/* Non-null assertion */
#define NN_ASSERT 323 // !!

/* Subscript and dot */
#define LSQUARE 324 // [
#define DOT 325     // .

/* Safe call */
#define SAFE_CALL 326 // ?.

/* Elvis */
#define ELVIS 327 // ?:

/* Nullable type suffix */
#define NULLABLE 328 // ?

/* Ranges */
#define RANGE_INCL 329 // ..
#define RANGE_EXCL 330 // ..<

/* Type cast */
#define TYPE_CAST 331 // (type)

/* Punctuation */
#define LPAREN 332    // (
#define RPAREN 333    // )
#define COMMA 334     // ,
#define SEMICOLON 335 // ;
#define LBRACE 336    // {
#define RBRACE 337    // }
#define COLON 338     // :
#define RSQUARE 339   // ]

/* Literals */
#define INT 340
#define REAL 341
#define STRING 342

/* Identifiers */
#define IDENT 343
