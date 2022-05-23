/**
 * @brief Módulo que trabalha com arquivos Tipo 1 (Arquivo de Dados para
 *        Registros de Tamanho Fixo)
 */

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#define HEADER_SIZE 182

typedef struct register_type1
{
    char removed;
    int  register_size;
    long int next;

    int  id;
    int  year;
    int  qtt;
    char *state;

    int  city_namesize;
    char *city;
    
    int  brand_namesize;
    char *brand;

    int  model_namesize;
    char *model;
}   reg_t1;

FILE *new_file(char *file_name)
{
    FILE *fp = fopen(file_name, "wb");

    int default_value = -1;

    fwrite("0",            sizeof(char), 1, fp); // 0
    fwrite(&default_value, sizeof(int),  1, fp); // 1

    fwrite("LISTAGEM DA FROTA DOS VEICULOS NO BRASIL", sizeof(char), 40, fp); // 5
    
    default_value = 0;

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
    fwrite(&default_value,             sizeof(int),   1, fp); // 174
    fwrite(&default_value,             sizeof(int),   1, fp); // 178

    return fp;
}

reg_t1 *read_register_from_csv(FILE *fp)
{
    reg_t1 *reg = malloc(sizeof(reg_t1));
    reg->register_size = 0;

    reg->removed = '0';
    reg->register_size += sizeof(reg->removed);

    reg->next = -1;
    reg->register_size += sizeof(reg->next);

    char *id = read_until(fp, ',');
    reg->id = atoi(id);
    reg->register_size += sizeof(reg->id);
    free(id);
    
    char *year = read_until(fp, ',');
    if (year[0] >= '0' && year[0] <= '9') reg->year = atoi(year);
    else reg->year = -1;
    reg->register_size += sizeof(reg->year);
    free(year);

    char *city = read_until(fp, ',');
    if (city[0] != '\0') reg->city = city;
    else reg->city = NULL; // Se for nulo, não será armazenado no arquivo de dados
    if (reg->city != NULL) reg->register_size += sizeof(reg->city - 1);

    if (reg->city != NULL) reg->city_namesize = sizeof(reg->city - 1);
    else reg->city_namesize = 0;

    char *qtt = read_until(fp, ',');
    if (qtt[0] >= '0' && qtt[0] <= '9') reg->qtt = atoi(qtt);
    else reg->qtt = -1;
    reg->register_size += sizeof(reg->qtt);
    free(qtt);

    char *state = read_until(fp, ',');
    if (state[0] != '\0') reg->state = state;
    else reg->state = NULL; // Se for nulo, não será armazenado no arquivo de dados
    if (reg->state != NULL) reg->register_size += sizeof(reg->state - 1);

    char *brand = read_until(fp, ',');
    if (brand[0] != '\0') reg->brand = brand;
    else reg->brand = NULL; // Se for nulo, não será armazenado no arquivo de dados
    if (reg->brand != NULL) reg->register_size += sizeof(reg->brand - 1);

    if (reg->brand != NULL) reg->brand_namesize = sizeof(reg->brand - 1);
    else reg->brand_namesize = 0;

    char *model = read_until(fp, '\n');
    if (model[0] != '\0') reg->model = model;
    else reg->model = NULL; // Se for nulo, não será armazenado no arquivo de dados
    if (reg->model != NULL) reg->register_size += sizeof(reg->model - 1);

    if (reg->model != NULL) reg->model_namesize = sizeof(reg->model - 1);
    else reg->city_namesize = 0;

    return reg;
}

void free_register(reg_t1 *reg)
{   
    if (reg)
    {
        if (reg->state) free(reg->state);
        if (reg->city)  free(reg->city);
        if (reg->brand) free(reg->brand);
        if (reg->model) free(reg->model);
        free(reg);
    }
}

void read_and_write_register(FILE *input, FILE *output)
{
    reg_t1 *reg = read_register_from_csv(input);

    // Campos de tamanho fixo
    fwrite(reg->removed,        sizeof(char),     1, output); // 1
    fwrite(&reg->register_size, sizeof(int),      1, output); // 4    5
    fwrite(&reg->next,          sizeof(long int), 1, output); // 8   13
    fwrite(&reg->id,            sizeof(int),      1, output); // 4   17
    fwrite(&reg->year,          sizeof(int),      1, output); // 4   21
    fwrite(&reg->qtt,           sizeof(int),      1, output); // 4   25
    fwrite(&reg->qtt,           sizeof(int),      1, output); // 4   29
    fwrite(reg->state,          sizeof(char),     2, output); // 2   31
    
    int bytes_written = 31;

    // Campos de tamanho variável
    fwrite(&reg->city_namesize, sizeof(int),  1, output);
    fwrite("0",                 sizeof(char), 1, output);
    fwrite(reg->city,           sizeof(char), reg->city_namesize, output);
    bytes_written += 4 + 1 + reg->city_namesize;

    fwrite(&reg->brand_namesize, sizeof(int),  1, output);
    fwrite("1",                  sizeof(char), 1, output);
    fwrite(reg->brand,           sizeof(char), reg->brand_namesize, output);
    bytes_written += 4 + 1 + reg->brand_namesize;

    fwrite(&reg->model_namesize, sizeof(int),  1, output);
    fwrite("2",                  sizeof(char), 1, output);
    fwrite(reg->model,           sizeof(char), reg->model_namesize, output);
    bytes_written += 4 + 1 + reg->model_namesize;

    while (bytes_written < 97)
    {
        fwrite("$", sizeof(char), 1, output);
        bytes_written++;
    }
    
    // DEBUG
    if (bytes_written > 97) printf("DEU PROBLEMA AQUI! ESTOUROU TAMANHO DO REGISTRO\n");

    free_register(reg);
}

void read_and_write_all(FILE *input, FILE *output)
{   
    char c = fgetc(input);
    while (c != EOF)
    {
        ungetc(c, input);
        read_and_write_register(input, output);
    }
}

void close_file(FILE *fp)
{
    fseek(fp, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, fp);

    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);
    int next_rrn = ((file_size - HEADER_SIZE) / 97);

    fseek(fp, 174, SEEK_SET);
    fwrite(&next_rrn, sizeof(int), 1, fp);

    fclose(fp);
}