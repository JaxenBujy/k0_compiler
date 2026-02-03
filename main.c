#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ytab.h"

extern FILE *yyin;
extern char *yytext;
extern int lineno;
extern int multi_line_start;
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
struct tokenlist *insert_at_tail(struct tokenlist *head, struct token *t);
void print_tokens_rev(struct tokenlist *head);
void print_tokens(struct tokenlist *head);
void free_list(struct tokenlist *head);
char *consume_sval(char *raw_text);

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
                int first_token = 1;

                int rv = yylex();
                while (rv != 0) // yylex returning 0 indicates EOF
                {
                        switch (rv)
                        {
                        case KEYWORD_ERROR:
                                printf("%s:%d: scanner error: Kotlin reserved keyword: \"%s\" is not a part of k0\n", filename, lineno, yytext);
                                exit(1);
                        case UNCLOSED_COMMENT:
                                printf("%s:%d: scanner error: unclosed multi-line comment\n", filename, multi_line_start);
                                exit(1);
                        case COULD_NOT_MATCH:
                                printf("%s:%d: scanner error: \'%s\' could not be matched\n", filename, lineno, yytext);
                                exit(1);
                        default:
                                struct token *newTok = create_token(rv, yytext, lineno, filename);
                                if (first_token)
                                {

                                        head = insert_at_head(head, newTok);
                                        first_token = 0;
                                }
                                else
                                {
                                        head = insert_at_tail(head, newTok);
                                }
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

        if (category == STRING)
        {
                printf("text: %s", text);
        }
        // set general fields
        newTok->category = category;
        newTok->text = strdup(text);
        newTok->lineno = lineno;
        newTok->filename = strdup(filename);

        // set defaults for literal vals
        newTok->ival = 0;
        newTok->dval = 0.0;
        newTok->sval = NULL;

        // set specific literal fields depending on data
        switch (category)
        {
        case INT:
                newTok->ival = atoi(text);
                break;
        case REAL:
                newTok->dval = atof(text);
                break;
        case STRING:
                newTok->sval = consume_sval(text);

                break;
        }
        return newTok;
}

// insert at head of the linked list
struct tokenlist *insert_at_head(struct tokenlist *head, struct token *t)
{
        struct tokenlist *newTokEntry = malloc(sizeof(*newTokEntry));
        newTokEntry->t = t;
        newTokEntry->next = head;
        head = newTokEntry;
        return head;
}

// insert at tail of the linked list. Less efficient, but seems to be more useful for the future to have it in order of tokens discovered?
struct tokenlist *insert_at_tail(struct tokenlist *head, struct token *t)
{
        struct tokenlist *temp = head;
        while (temp->next != NULL)
        {
                temp = temp->next;
        }
        struct tokenlist *newTokEntry = malloc(sizeof(*newTokEntry));
        temp->next = newTokEntry;
        newTokEntry->t = t;
        newTokEntry->next = NULL;
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

void print_tokens_rev(struct tokenlist *temp)
{
        if (temp == NULL)
        {
                return;
        }
        else
        {
                print_tokens_rev(temp->next);
                printf("%d              %s              %d              %s              %d              %f              %s\n", temp->t->category, temp->t->text, temp->t->lineno, temp->t->filename, temp->t->ival, temp->t->dval, temp->t->sval);
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

char *consume_sval(char *raw_text)
{
        char *sval = (char *)malloc(strlen(raw_text) + 1);
        strcpy(sval, raw_text);

        int escaped = 0; // flag for if an escape has been found
        while (*raw_text)
        {
                if (escaped)
                        switch (*(raw_text + 1))
                        {
                        case 'n':
                                *raw_text = '\n';
                        }
                printf("%c\n", *raw_text);
                raw_text++;
        }

        return sval;
}