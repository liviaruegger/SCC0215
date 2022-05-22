/**
 * @brief Módulo que trabalha com arquivos Tipo 1 (Arquivo de Dados para
 *        Registros de Tamanho Fixo)
 */

#include <stdio.h>
#include <stdlib.h>
#include "fixed.h"

typedef struct register_type1
{
    char removed[1];
    int  register_size;
    long int next;

    int  id;
    int  year;
    int  qtt;
    char state[2];

    int  city_namesize;
    char *city;
    
    char *brand;
    char *model;

    int  brand_namesize;
    int  model_namesize;
}   reg_t1;

FILE *new_file(char *file_name)
{
    FILE *fp = fopen(file_name, "wb");

    fwrite("0", sizeof(char), 1, fp); // 0
    fwrite(-1,  sizeof(int),  1, fp); // 1

    fwrite("LISTAGEM DA FROTA DOS VEICULOS NO BRASIL", sizeof(char), 40, fp); // 5
    
    fwrite("CODIGO IDENTIFICADOR: ",   sizeof(char), 22, fp); // 45
    fwrite("ANO DE FABRICACAO: ",      sizeof(char), 19, fp); // 67
    fwrite("QUANTIDADE DE VEICULOS: ", sizeof(char), 24, fp); // 86
    fwrite("ESTADO: ",                 sizeof(char),  8, fp); // 110
    fwrite("0",                        sizeof(char),  1, fp); // 118
    fwrite("NOME DA CIDADE: ",         sizeof(char), 16, fp); // 119
    fwrite("1",                        sizeof(char),  1, fp); // 135
    fwrite("MARCA DO VEICULO: ",       sizeof(char), 18, fp); // 136
    fwrite("2",                        sizeof(char),  1, fp); // 154
    fwrite("MODELO DO VEICULO: ",      sizeof(char), 19, fp); // 155
    fwrite(0,                          sizeof(int),   1, fp); // 174
    fwrite(0,                          sizeof(int),   1, fp); // 178

    return fp;    
}

reg_t1 read_register(FILE *fp)
{
    

}