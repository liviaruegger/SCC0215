/**
 * @file   main.c
 * @author Ana Lívia Ruegger Saldanha (N.USP 8586691)
 * @author André ____________________________________
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

// --- ENTRADAS:
// FUNC 1 -> 1 tipoArquivo arquivoEntrada.csv arquivoSaida.bin
// FUNC 2 -> 2 tipoArquivo arquivoEntrada.bin
// FUNC 3 -> 3 tipoArquivo arquivoEntrada.bin n
// FUNC 4 -> 4 tipoArquivo arquivoEntrada.bin RRN

int main()
{
    int func, rrn;
    scanf("%d", &func);
    getchar(); // Consome o '\n'

    char *file_type = read_word(stdin);
    char *input_file = read_word(stdin);
    char *output_file = NULL;

    FILE *input_fp = NULL;
    if (func == 1) input_fp = fopen(input_file, "r");
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
            FILE *output_fp = NULL;

            if (file_type[4] == '1')
            {
                output_fp = new_type1_file(output_file);
                read_and_write_all_type1(input_fp, output_fp);
            }
            else if (file_type[4] == '2')
            {
                output_fp = new_type2_file(output_file);
                read_and_write_register_t2(input_fp, output_fp);
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
                print_t2_register_from_file(input_fp);
            }
            break;
        
        case 3:
            if (file_type[4] == '1')
            {
                search_by_parameters_type1(input_fp);
            }
            else if (file_type[4] == '2')
            {
                search_t2_parameter(input_fp);
            }
            break;

        case 4:
            scanf("%d", &rrn);
            search_by_rrn_type1(input_fp, rrn);
            break;
    }

    fclose(input_fp);
    free(file_type);
    free(input_file);

    return 0;
}