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
    int func, n, rrn;
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
        printf("Falha no processamento do arquivo.");
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
                close_type1_file(output_fp);
            }
            else if (file_type[4] == '2')
            {
                output_fp = new_type2_file(output_file);
                read_and_write_register_t2(input_fp, output_fp);
                fclose(output_fp);
            }

            binarioNaTela(output_file);
            free(output_file);
            break;

        case 2:
            if (file_type[4] == '1')
            {

            }
            else if (file_type[4] == '2')
            {
                print_t2_register_from_file(input_fp);
            }
            break;
        
        case 3:
            if (file_type[4] == '1')
            {
                scanf("%d", &n);
                getchar(); // Consome o '\n'

                for (int i = 0; i < n; i++)
                {
                    char *field_name = read_word(stdin);
                    char *field_content = NULL;

                    char c = getchar();
                    if (c == '"')
                    {
                        field_content = read_until(stdin, '"');
                        getchar(); // Consome o '\n'

                        // TODO: chamar funcao

                        printf("%s %s\n", field_name, field_content);

                        free(field_content);
                    }
                    else
                    {
                        ungetc(c, stdin);
                        int value;
                        scanf("%d", &value);
                        // TODO: chamar funcao

                        printf("%s %d\n", field_name, value);
                    }

                    free(field_name);
                }
            }
            else if (file_type[4] == '2')
            {
                search_t2_parameter(input_fp);
            }
            break;

        case 4:
            scanf("%d", &rrn);
            print_type1_register(input_fp, rrn);
            break;
    }

    fclose(input_fp);
    free(file_type);
    free(input_file);

    return 0;
}