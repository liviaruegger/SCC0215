/**
 * @file   main.c
 * @author Ana Lívia Ruegger Saldanha (N.USP 8586691) - 100%
 * @author André Kenji Hidaka Matsumoto (N. USP 12542689) - 100%
 * @brief  SCC0215 - Organização de Arquivos
 *         Trabalho 03
 *
 * @date   2022-07-20
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "func.h"

int main()
{
    int func;
    scanf("%d", &func);
    getchar(); // Consome o '\n'

    char *file_type = read_word(stdin);
    char *data_filename = read_word(stdin);
    char *index_filename = read_word(stdin);

    int type;
    if (file_type[4] == '1') type = 1;
    else if (file_type[4] == '2') type = 2;
    free(file_type);

    switch (func)
    {
        case 9:
            functionality_09(type, data_filename, index_filename);
            break;

        case 10:
            functionality_10(type, data_filename, index_filename);
            break;

        case 11:
            functionality_11(type, data_filename, index_filename);
            break;
    }

    if (func == 11) binarioNaTela(data_filename);
    if (func == 9 || func == 11) binarioNaTela(index_filename);

    free(data_filename);
    free(index_filename);

    return 0;
}
