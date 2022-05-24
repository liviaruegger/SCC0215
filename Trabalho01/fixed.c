/**
 * @brief Módulo que trabalha com arquivos Tipo 1 (Arquivo de Dados para
 *        Registros de Tamanho Fixo)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define HEADER_SIZE 182

typedef struct register_type1
{
    char removed;
    int  next;

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

    reg->removed = '0';

    reg->next = -1;

    char *id = read_until(fp, ',');
    reg->id = atoi(id);
    free(id);
    
    char *year = read_until(fp, ',');
    if (year[0] >= '0' && year[0] <= '9') reg->year = atoi(year);
    else reg->year = -1;
    free(year);

    char *city = read_until(fp, ',');
    if (city[0] != '\0')
    {
        reg->city = city;
    }
    else // Se for nulo, não será armazenado no arquivo de dados
    {
        reg->city = NULL;
        free(city);
    }

    if (reg->city) reg->city_namesize = strlen(reg->city);
    else reg->city_namesize = 0;

    char *qtt = read_until(fp, ',');
    if (qtt[0] >= '0' && qtt[0] <= '9') reg->qtt = atoi(qtt);
    else reg->qtt = -1;
    free(qtt);

    char *state = read_until(fp, ',');
    if (state[0] != '\0')
    {
        reg->state = state;
    }
    else // Se for nulo, não será armazenado no arquivo de dados
    {
        reg->state = NULL;
        free(state);
    }

    char *brand = read_until(fp, ',');
    if (brand[0] != '\0')
    {
        reg->brand = brand;
    }
    else // Se for nulo, não será armazenado no arquivo de dados
    {
        reg->brand = NULL;
        free(brand);
    }

    if (reg->brand) reg->brand_namesize = strlen(reg->brand);
    else reg->brand_namesize = 0;

    char *model = read_line(fp);
    if (model[0] != '\0')
    {
        reg->model = model;
    }
    else // Se for nulo, não será armazenado no arquivo de dados
    {
        reg->model = NULL;
        free(model);
    }

    if (reg->model) reg->model_namesize = strlen(reg->model);
    else reg->model_namesize = 0;

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
    fwrite(&reg->removed, sizeof(char), 1, output);
    fwrite(&reg->next,    sizeof(int),  1, output);
    fwrite(&reg->id,      sizeof(int),  1, output);
    fwrite(&reg->year,    sizeof(int),  1, output);
    fwrite(&reg->qtt,     sizeof(int),  1, output);

    if (reg->state) fwrite(reg->state, sizeof(char), 2, output);
    else fwrite ("$$", sizeof(char), 2, output);

    int bytes_written = 19;

    // Campos de tamanho variável
    if (reg->city_namesize > 0)
    {
        fwrite(&reg->city_namesize, sizeof(int),  1, output);
        fwrite("0",                 sizeof(char), 1, output);
        fwrite(reg->city,           sizeof(char), reg->city_namesize, output);
        bytes_written += 4 + 1 + reg->city_namesize;
    }

    if (reg && reg->brand_namesize && reg->brand)
    {
        fwrite(&reg->brand_namesize, sizeof(int),  1, output);
        fwrite("1",                  sizeof(char), 1, output);
        fwrite(reg->brand,           sizeof(char), reg->brand_namesize, output);
        bytes_written += 4 + 1 + reg->brand_namesize;
    }

    if (reg && reg->model_namesize && reg->model)
    {
        fwrite(&reg->model_namesize, sizeof(int),  1, output);
        fwrite("2",                  sizeof(char), 1, output);
        fwrite(reg->model,           sizeof(char), reg->model_namesize, output);
        bytes_written += 4 + 1 + reg->model_namesize;
    }

    while (bytes_written < 97)
    {
        fwrite("$", sizeof(char), 1, output);
        bytes_written++;
    }

    free_register(reg);
}

void read_and_write_all(FILE *input, FILE *output)
{   
    int register_count = 0;

    char c = fgetc(input);
    while (c != EOF)
    {
        ungetc(c, input);
        read_and_write_register(input, output);
        register_count++;

        c = fgetc(input);
    }

    fseek(output, 174, SEEK_SET);
    fwrite(&register_count, sizeof(int), 1, output);

    fclose(input);
}

void close_file(FILE *fp)
{
    fseek(fp, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, fp);

    fclose(fp);
}