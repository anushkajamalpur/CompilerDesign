```c
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN 100

/* C Keywords */
const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue",
    "default", "do", "double", "else", "enum", "extern",
    "float", "for", "goto", "if", "int", "long",
    "register", "return", "short", "signed", "sizeof",
    "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while",
    "class", "public", "private", "protected",
    "new", "delete", "this", "true", "false"
};

int keywordCount = sizeof(keywords) / sizeof(keywords[0]);

/* Check whether a word is a keyword */
int isKeyword(char *word)
{
    int i;

    for (i = 0; i < keywordCount; i++)
    {
        if (strcmp(word, keywords[i]) == 0)
            return 1;
    }

    return 0;
}

/* Check for operators */
int isOperator(char ch)
{
    return (ch == '+' || ch == '-' || ch == '*' ||
            ch == '/' || ch == '%' || ch == '=' ||
            ch == '<' || ch == '>' || ch == '!' ||
            ch == '&' || ch == '|' || ch == '^');
}

/* Check for separators */
int isSeparator(char ch)
{
    return (ch == '(' || ch == ')' ||
            ch == '{' || ch == '}' ||
            ch == '[' || ch == ']' ||
            ch == ';' || ch == ',' ||
            ch == ':');
}

int main()
{
    FILE *fp;
    char filename[100];
    char ch;
    char token[MAX_TOKEN];

    int i;

    /* Token counters */
    int keywords_count = 0;
    int identifiers_count = 0;
    int operators_count = 0;
    int constants_count = 0;
    int string_literals_count = 0;
    int separators_count = 0;
    int special_symbols_count = 0;
    int comments_count = 0;

    printf("Enter source file name: ");
    scanf("%99s", filename);

    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        return 1;
    }

    printf("\nTOKEN TYPE\n");
    printf("--------------------------------------------------\n");

    while ((ch = fgetc(fp)) != EOF)
    {
        /* Ignore white spaces */
        if (isspace((unsigned char)ch))
        {
            continue;
        }

        /* Comments */
        if (ch == '/')
        {
            char next = fgetc(fp);

            /* Single-line comment */
            if (next == '/')
            {
                i = 0;
                token[i++] = '/';
                token[i++] = '/';

                while ((ch = fgetc(fp)) != EOF && ch != '\n')
                {
                    if (i < MAX_TOKEN - 1)
                        token[i++] = ch;
                }

                token[i] = '\0';

                printf("%-30s Comment\n", token);
                comments_count++;
                continue;
            }

            /* Multi-line comment */
            else if (next == '*')
            {
                i = 0;
                token[i++] = '/';
                token[i++] = '*';

                while ((ch = fgetc(fp)) != EOF)
                {
                    if (i < MAX_TOKEN - 1)
                        token[i++] = ch;

                    if (ch == '*')
                    {
                        next = fgetc(fp);

                        if (next == '/')
                        {
                            if (i < MAX_TOKEN - 1)
                                token[i++] = next;

                            break;
                        }

                        ungetc(next, fp);
                    }
                }

                token[i] = '\0';

                printf("%-30s Comment\n", token);
                comments_count++;
                continue;
            }

            /* Division operator */
            else
            {
                if (next != EOF)
                    ungetc(next, fp);

                printf("%-30c Operator\n", ch);
                operators_count++;
                continue;
            }
        }

        /* String literal */
        if (ch == '"')
        {
            i = 0;
            token[i++] = ch;

            while ((ch = fgetc(fp)) != EOF)
            {
                if (i < MAX_TOKEN - 1)
                    token[i++] = ch;

                if (ch == '"')
                    break;
            }

            token[i] = '\0';

            printf("%-30s String Literal\n", token);
            string_literals_count++;
            continue;
        }

        /* Character constant */
        if (ch == '\'')
        {
            i = 0;
            token[i++] = ch;

            while ((ch = fgetc(fp)) != EOF)
            {
                if (i < MAX_TOKEN - 1)
                    token[i++] = ch;

                if (ch == '\'')
                    break;
            }

            token[i] = '\0';

            printf("%-30s Constant\n", token);
            constants_count++;
            continue;
        }

        /* Identifier or Keyword */
        if (isalpha((unsigned char)ch) || ch == '_')
        {
            i = 0;
            token[i++] = ch;

            while ((ch = fgetc(fp)) != EOF &&
                   (isalnum((unsigned char)ch) || ch == '_'))
            {
                if (i < MAX_TOKEN - 1)
                    token[i++] = ch;
            }

            token[i] = '\0';

            if (ch != EOF)
                ungetc(ch, fp);

            if (isKeyword(token))
            {
                printf("%-30s Keyword\n", token);
                keywords_count++;
            }
            else
            {
                printf("%-30s Identifier\n", token);
                identifiers_count++;
            }

            continue;
        }

        /* Numeric constants */
        if (isdigit((unsigned char)ch))
        {
            i = 0;
            token[i++] = ch;

            while ((ch = fgetc(fp)) != EOF &&
                   (isdigit((unsigned char)ch) || ch == '.'))
            {
                if (i < MAX_TOKEN - 1)
                    token[i++] = ch;
            }

            token[i] = '\0';

            if (ch != EOF)
                ungetc(ch, fp);

            printf("%-30s Constant\n", token);
            constants_count++;

            continue;
        }

        /* Separators */
        if (isSeparator(ch))
        {
            printf("%-30c Separator\n", ch);
            separators_count++;
            continue;
        }

        /* Operators */
        if (isOperator(ch))
        {
            char next;
            char op[3];

            op[0] = ch;
            op[1] = '\0';

            next = fgetc(fp);

            /*
             * Check two-character operators:
             * ==, !=, <=, >=, ++, --,
             * +=, -=, *=, /=, &&, ||
             */
            if ((ch == '=' && next == '=') ||
                (ch == '!' && next == '=') ||
                (ch == '<' && next == '=') ||
                (ch == '>' && next == '=') ||
                (ch == '+' && next == '+') ||
                (ch == '-' && next == '-') ||
                (ch == '+' && next == '=') ||
                (ch == '-' && next == '=') ||
                (ch == '*' && next == '=') ||
                (ch == '/' && next == '=') ||
                (ch == '&' && next == '&') ||
                (ch == '|' && next == '|'))
            {
                op[1] = next;
                op[2] = '\0';
            }
            else
            {
                if (next != EOF)
                    ungetc(next, fp);
            }

            printf("%-30s Operator\n", op);
            operators_count++;

            continue;
        }

        /* Special symbols */
        printf("%-30c Special Symbol\n", ch);
        special_symbols_count++;
    }

    fclose(fp);

    /* Display token counts */
    printf("\n--------------------------------------------------\n");
    printf("Token Count\n");
    printf("--------------------------------------------------\n");

    printf("Keywords        : %d\n", keywords_count);
    printf("Identifiers     : %d\n", identifiers_count);
    printf("Operators       : %d\n", operators_count);
    printf("Constants       : %d\n", constants_count);
    printf("String Literals : %d\n", string_literals_count);
    printf("Separators      : %d\n", separators_count);
    printf("Special Symbols : %d\n", special_symbols_count);
    printf("Comments        : %d\n", comments_count);

    return 0;
}
```
