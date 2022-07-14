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
    char *data_filename = read_word(stdin);
    char *index_filename = read_word(stdin);

    int type;
    if (file_type[4] == '1') type = 1;
    else if (file_type[4] == '2') type = 2;
    free(file_type);

    FILE *data_fp = NULL;
    FILE *index_fp = NULL;
    
    if (func == 9) // Criar índice
    {
        data_fp = fopen(data_filename, "rb");
        index_fp = fopen(index_filename, "wb");
    }
    else if (func == 10) // Busca
    {
        data_fp = fopen(data_filename, "rb");
        index_fp = fopen(index_filename, "rb");
    }
    else if (func == 11) // Inserção
    {
        data_fp = fopen(data_filename, "r+b");
        index_fp = fopen(index_filename, "r+b");
    }
    
    if (!data_fp || !index_fp)
    {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    switch (func)
    {
        case 9:

            break;

        case 10:

            break;

        case 11:

            break;
    }

    fclose(data_fp);
    fclose(index_fp);

    if (func == 11) binarioNaTela(data_filename);
    if (func == 10 || func == 11) binarioNaTela(index_filename);

    free(data_filename);
    free(index_filename);

    return 0;
}
