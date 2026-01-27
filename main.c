#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "int_codes.h"

extern FILE *yyin;
extern char *yytext;
extern int rows;
int yylex(void);

typedef struct
{
        int category;   /* the integer code returned by yylex */
        char *text;     /* the actual string (lexeme) matched */
        int lineno;     /* the line number on which the token occurs */
        char *filename; /* the source file in which the token occurs */
        int ival;       /* for integer constants, store binary value here */
        double dval;    /* for real constants, store binary value here */
        char *sval;     /* for string constants, malloc space, de-escape, store the string (less quotes and after escapes) here */
} token;

typedef struct
{
        token t;
        struct tokenlist *next;
} tokenlist;

tokenlist *insert_at_head(tokenlist *head, token t, size_t tok_size);

int main(int argc, char *argv[])
{
        if (argc != 2)
        {
                printf("2 arguments required\n");
                exit(1);
        }
        char *filename = argv[1];
        yyin = fopen(filename, "r");
        if (!yyin)
        {
                printf("Error reading file\n");
                exit(1);
        }
        else
        {
                struct tokenlist *head = NULL;

                printf("Category                Text            Line Number             Filename                Ival/Sval\n");
                printf("---------------------------------------------------------------------------------------------------\n");
                int rv = yylex();
                while (rv != 0) // yylex returning 0 indicates EOF
                {

                        if (rv == KEYWORD_ERROR) // if yylex returned a non k0 keyword
                        {
                                printf("ERROR: %s:%d Kotlin reserved keyword: \"%s\" is not a part of k0, exiting...\n", filename, rows, yytext);
                                exit(1);
                        }
                        else if (rv == COULD_NOT_MATCH)
                        {
                                printf("ERROR: scanner could not match %s\n", yytext);
                        }
                        else
                        {
                                token *newTok = (token *)malloc(sizeof(token));
                                switch (rv)
                                {
                                case INT:
                                        newTok->ival = atoi(yytext);
                                        newTok->sval = NULL;
                                        break;
                                case FLOAT:
                                        newTok->dval = (float)atof(yytext);
                                        newTok->sval = NULL;
                                        break;
                                case STRING:
                                        char *new_string = (char *)malloc(sizeof(yytext));
                                        newTok->sval = new_string;
                                }
                                size_t data_size = sizeof(newTok);
                                tokenlist *newToken = insert_at_head(head, newTok, data_size);
                                printf("%d              %s              %d              %s              Ival/Sval\n", rv, yytext, rows, filename);
                        }
                        rv = yylex();
                }
                fclose(yyin);
        }

        return 0;
}

tokenlist *insert_at_head(tokenlist *head, token t, size_t tok_size)
{
        tokenlist *newToken = (tokenlist *)malloc(sizeof(tokenlist));
        if (newToken == NULL)
        {
                perror("malloc failed\n");
                exit(EXIT_FAILURE);
        }
        newToken->t = malloc(tok_size);
        newToken->next = head;
        memcpy(newToken->t, tok, data_size);
        return newToken;
}