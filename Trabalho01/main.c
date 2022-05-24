#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "fixed.h"

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
            
            if (file_type[4] == '1')
            {
                FILE *output_fp = new_type1_file(output_file);
                read_and_write_all_type1(input_fp, output_fp);
                close_type1_file(output_fp);
            }
            else if (file_type[4] == '2')
            {

            }

            binarioNaTela(output_file);
            free(output_file);       
            break;
        
        case 2:
            /* code */
            break;
        
        case 3:
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

                    // TODO: chamar funcao de acordo com tipo de arquivo

                    free(field_content);
                }
                else
                {
                    ungetc(c, stdin);
                    int value;
                    scanf("%d", &value);

                    // TODO: chamar funcao de acordo com tipo de arquivo
                }

                free(field_name);
            }
            
            break;
        
        case 4:
            scanf("%d", &rrn);
            print_type1_register(input_fp, rrn);
            break;
    }

    free(file_type);
    free(input_file);

    return 0;
}