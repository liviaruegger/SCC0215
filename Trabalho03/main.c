/**
 * @file   main.c
 * @author Ana Lívia Ruegger Saldanha (N.USP 8586691)
 * @author André Kenji Hidaka Matsumoto (N. USP 12542689)
 * @brief  SCC0215 - Organização de Arquivos
 *         Trabalho 03
 *
 * @date   2022-07-20
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int main()
{
    int func, rrn, n;
    scanf("%d", &func);
    getchar(); // Consome o '\n'

    char *file_type = read_word(stdin);
    char *input_file = read_word(stdin);
    char *output_file = NULL;
    char *index_file = NULL;
    FILE *output_fp = NULL;
    FILE *index_fp = NULL;

    FILE *input_fp = NULL;
    if (func == 1) input_fp = fopen(input_file, "r");
    //else if (func == 6 || func == 7 || func == 8) input_fp = fopen(input_file, "r+b");
    else input_fp = fopen(input_file, "rb");

    if (!input_fp)
    {
        printf("Falha no processamento do arquivo.\n");
        free(file_type);
        free(input_file);
        return 0;
    }

    int type;
    if (file_type[4] == '1') type = 1;
    else if (file_type[4] == '2') type = 2;

    switch (func)
    {
        case 9:

            break;

        case 10:

            break;

        case 11:

            break;
    }

    fclose(input_fp);
    // if (func == 6 || func == 7 || func == 8)
    // {
    //     binarioNaTela(input_file);
    //     binarioNaTela(index_file);
    //     free(index_file);
    // }

    free(file_type);
    free(input_file);

    return 0;
}
