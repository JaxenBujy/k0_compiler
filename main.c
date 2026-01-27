#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ytab.h"

extern FILE *yyin;
extern char *yytext;
extern int rows;
int yylex(void);

struct token
{
        int category;   /* the integer code returned by yylex */
        char *text;     /* the actual string (lexeme) matched */
        int lineno;     /* the line number on which the token occurs */
        char *filename; /* the source file in which the token occurs */
        int ival;       /* for integer constants, store binary value here */
        double dval;    /* for real constants, store binary value here */
        char *sval;     /* for string constants, malloc space, de-escape, store the string (less quotes and after escapes) here */
};

struct tokenlist
{
        struct token *t;
        struct tokenlist *next;
};

struct token *create_token(int category, char *text, int lineno, char *filename);
struct tokenlist *insert_at_head(struct tokenlist *head, struct token *t);
void print_tokens(struct tokenlist *head);
void free_list(struct tokenlist *head);

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
                                exit(1);
                        }
                        else
                        {
                                // create token and add it to linked list
                                struct token *newTok = create_token(rv, yytext, rows, filename);
                                head = insert_at_head(head, newTok);
                        }
                        rv = yylex();
                }
                print_tokens(head);
                free_list(head);
                fclose(yyin);
        }

        return 0;
}

struct token *create_token(int category, char *text, int lineno, char *filename)
{
        struct token *newTok = malloc(sizeof(*newTok));

        // set general fields
        newTok->category = category;
        newTok->text = strdup(text);
        newTok->lineno = lineno;
        newTok->filename = strdup(filename);

        // set defaults for literal vals
        newTok->ival = 0;
        newTok->dval = 0.0;
        newTok->sval = NULL;

        // set specific fields depending on data
        switch (category)
        {
        case INT:
                newTok->ival = atoi(text);
                break;
        case FLOAT:
                newTok->dval = atof(text);
                break;
        case STRING:
                newTok->sval = (char *)malloc(strlen(text) + 1);
                strcpy(newTok->sval, text);
                break;
        }
        return newTok;
}

struct tokenlist *insert_at_head(struct tokenlist *head, struct token *t)
{
        struct tokenlist *newTokEntry = malloc(sizeof(*newTokEntry));
        newTokEntry->t = t;
        newTokEntry->next = head;
        head = newTokEntry;
        return head;
}

void print_tokens(struct tokenlist *head)
{
        struct tokenlist *temp = head;
        printf("Category                Text            Line Number             Filename                Ival            Dval            Sval\n");
        printf("---------------------------------------------------------------------------------------------------------------------------------\n");

        while (temp != NULL)
        {
                printf("%d              %s              %d              %s              %d              %f              %s\n", temp->t->category, temp->t->text, temp->t->lineno, temp->t->filename, temp->t->ival, temp->t->dval, temp->t->sval);
                temp = temp->next;
        }
}

void free_list(struct tokenlist *head)
{
        struct tokenlist *curr = head;
        while (curr != NULL)
        {
                struct tokenlist *temp = curr->next;
                free(curr->t->text);
                free(curr->t->filename);
                free(curr->t->sval);
                free(curr->t);
                free(curr);
                curr = temp;
        }
}