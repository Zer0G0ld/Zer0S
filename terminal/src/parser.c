#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELIM " "

char **parse_input(char *input) {
    int bufsize = 64, i = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "Erro de alocação de memória\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(input, DELIM);
    while (token != NULL) {
        tokens[i] = strdup(token);  // Copia o token para garantir uma string independente
        if (!tokens[i]) {
            fprintf(stderr, "Erro de alocação de memória para token\n");
            exit(EXIT_FAILURE);
        }
        i++;

        if (i >= bufsize) {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "Erro de alocação de memória\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, DELIM);
    }
    tokens[i] = NULL;

    return tokens;
}
