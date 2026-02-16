#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ytab.h"

extern FILE *yyin;
extern char *yytext;
extern int yyleng;
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

struct token *create_token(int category, int lineno, char *filename);
struct tokenlist *insert_at_head(struct tokenlist *head, struct token *t);
struct tokenlist *insert_at_tail(struct tokenlist *head, struct token *t);
void print_tokens_rev(struct tokenlist *head);
void print_tokens(struct tokenlist *head);
void free_list(struct tokenlist *head);
char *consume_sval(int str_type);

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
                        case ILL_IDENT:
                                printf("%s:%d: scanner error: \'%s\' is an illegal identifier\n", filename, lineno, yytext);
                                exit(1);
                        case OP_ERROR:
                                printf("%s:%d: scanner error: \'%s\' is an illegal operator\n", filename, lineno, yytext);
                                exit(1);
                        default:
                                struct token *newTok = create_token(rv, lineno, filename);
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

struct token *create_token(int category, int lineno, char *filename)
{
        struct token *newTok = malloc(sizeof(*newTok));

        // set general fields
        newTok->category = category;
        newTok->text = strdup(yytext);
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
                newTok->ival = atoi(yytext);
                break;
        case REAL:
                newTok->dval = atof(yytext);
                break;
        case STRING:
                newTok->sval = consume_sval(STRING);
                if (newTok->sval == NULL)
                {
                        printf("%s:%d: scanner error: unsupported escape sequence in %s\n", filename, lineno, yytext);
                        exit(1);
                }
                break;
        case MULTI_STRING:
                newTok->sval = consume_sval(MULTI_STRING);
                if (newTok->sval == NULL)
                {
                        printf("%s:%d: scanner error: unsupported escape sequence in %s\n", filename, lineno, yytext);
                        exit(1);
                }
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
        struct tokenlist *curr = head;

        printf("%-10s %-20s %-12s %-15s %-10s %-12s %-20s\n",
               "Category", "Text", "Line", "Filename", "Ival", "Dval", "Sval");
        printf("-----------------------------------------------------------------------------------------------\n");

        while (curr != NULL)
        {
                printf("%-10d %-20.20s %-12d %-15.15s %-10d %-12.5f %-20.20s\n",
                       curr->t->category,
                       curr->t->text,
                       curr->t->lineno,
                       curr->t->filename,
                       curr->t->ival,
                       curr->t->dval,
                       curr->t->sval);

                curr = curr->next;
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

char *consume_sval(int str_type)
{
        // allocate worst-case size (raw string length including escapes and quotes)
        char *tmp = (char *)malloc(yyleng);
        char *start = tmp;

        // pointer to iterate over the raw lexer text
        char *raw_text = yytext + 1;     // skip initial quote
        char *end = yytext + yyleng - 1; // stop before final quote

        if (str_type == MULTI_STRING)
        {
                raw_text = yytext + 3;
                end = yytext + yyleng - 3;
        }

        // iterate through raw text, consuming escapes and copying characters
        while (raw_text < end)
        {
                // encountered a backslash, meaning an escape sequence
                if (*raw_text == '\\')
                {
                        raw_text++; // move to escaped character

                        // check what the escaped character is
                        switch (*raw_text)
                        {
                        case 'n': // consume newline
                                *tmp = '\n';
                                break;
                        case 't': // consume tab
                                *tmp = '\t';
                                break;
                        case '\\': // consume literal backslash
                                *tmp = '\\';
                                break;
                        case '\'': // consume apostrophe
                                *tmp = '\'';
                                break;
                        case '\"': // consume quote
                                *tmp = '\"';
                                break;
                        case 'r': // consume carriage return
                                *tmp = '\r';
                                break;
                        case 'b': // consume backspace
                                *tmp = '\b';
                                break;
                        default:
                                // unsupported escape sequence
                                free(start);
                                return NULL;
                        }
                }
                else
                {
                        // regular character, copy as-is
                        *tmp = *raw_text;
                }

                tmp++;
                raw_text++;
        }

        // null-terminate the final string value
        *tmp = '\0';

        return start;
}
