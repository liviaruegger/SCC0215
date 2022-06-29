/**
 * @file   main.c
 * @author Ana Lívia Ruegger Saldanha (N.USP 8586691)
 * @author André Kenji Hidaka Matsumoto (N. USP 12542689)
 * @brief  SCC0215 - Organização de Arquivos
 *         Trabalho 01
 *
 * @date   2022-05-26
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "fixed.h"
#include "variable.h"

int main()
{
    int func, rrn;
    scanf("%d", &func);
    getchar(); // Consome o '\n'

    char *file_type = read_word(stdin);
    char *input_file = read_word(stdin);
    char *output_file = NULL;
    char *index_file = NULL;
    FILE *output_fp = NULL;

    FILE *input_fp = NULL;
    if (func == 1) input_fp = fopen(input_file, "r");
    else if (func == 6) input_fp = fopen(input_file, "r+b");
    else input_fp = fopen(input_file, "rb");

    if (!input_fp)
    {
        printf("Falha no processamento do arquivo.\n");
        free(file_type);
        free(input_file);
        return 0;
    }
    switch (func)
    {
        case 1:
            output_file = read_word(stdin);

            if (file_type[4] == '1')
            {
                output_fp = new_type1_file(output_file);
                read_and_write_all_type1(input_fp, output_fp);
            }
            else if (file_type[4] == '2')
            {
                output_fp = new_type2_file(output_file);
                read_and_write_all_type2(input_fp, output_fp);
            }

            fclose(output_fp);
            binarioNaTela(output_file);
            free(output_file);
            break;

        case 2:
            if (file_type[4] == '1')
            {
                print_all_from_bin_type1(input_fp);
            }
            else if (file_type[4] == '2')
            {
                print_all_from_bin_type2(input_fp);
            }
            break;

        case 3:
            if (file_type[4] == '1')
            {
                search_by_parameters_type1(input_fp);
            }
            else if (file_type[4] == '2')
            {
                search_by_parameters_type2(input_fp);
            }
            break;

        case 4:
            scanf("%d", &rrn);
            search_by_rrn_type1(input_fp, rrn);
            break;

        case 5:
            output_file = read_word(stdin);

            if (file_type[4] == '1')
            {
                output_fp = new_type1_index(input_fp, output_file);
                fclose(output_fp);

                binarioNaTela(output_file);
                free(output_file);
            }
            else if (file_type[4] == '2')
            {
                output_fp = new_type2_index (input_fp, output_file);
                fclose(output_fp);

                binarioNaTela(output_file);
                free(output_file);
            }
        break;

        case 6:
            index_file = read_word(stdin);
            if (file_type[4] == '1')
            {
                type1_delete_from (input_fp, index_file);
            }
            else if (file_type[4] == '2')
            {
                type2_delete_from(input_fp, index_file);
            }
        break;

        case 7:
            if (file_type[4] == '1')
            {
                index_file = read_word(stdin);
                type1_delete_from(input_fp, index_file);
            }
            else if (file_type[4] == '2')
            {
                index_file = read_word(stdin);
                type2_delete_from(input_fp, index_file);
            }
        break;

        case 8:
            if (file_type[4] == '1')
            {

            }
            else if (file_type[4] == '2')
            {

            }
        break;
    }

    fclose(input_fp);
    if (func == 6)
    {
        binarioNaTela(input_file);

        binarioNaTela(index_file);
        free(index_file);
    }

    free(file_type);
    free(input_file);

    return 0;
}
