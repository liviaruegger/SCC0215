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

FILE *new_type1_file(char *file_name)
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

static reg_t1 *read_register_from_csv(FILE *fp)
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
    else
    {
        free(state);
        reg->state = malloc(sizeof(char) * 2);
        reg->state[0] = '$';
        reg->state[1] = '$';
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

static void free_register(reg_t1 *reg)
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

static void read_and_write_register(FILE *input, FILE *output)
{
    reg_t1 *reg = read_register_from_csv(input);
    
    // Campos de tamanho fixo
    fwrite(&reg->removed, sizeof(char), 1, output);
    fwrite(&reg->next,    sizeof(int),  1, output);
    fwrite(&reg->id,      sizeof(int),  1, output);
    fwrite(&reg->year,    sizeof(int),  1, output);
    fwrite(&reg->qtt,     sizeof(int),  1, output);
    fwrite(reg->state,    sizeof(char), 2, output);

    int bytes_written = 19;

    // Campos de tamanho variável
    if (reg->city_namesize > 0)
    {
        fwrite(&reg->city_namesize, sizeof(int),  1, output);
        fwrite("0",                 sizeof(char), 1, output);
        fwrite(reg->city,           sizeof(char), reg->city_namesize, output);
        bytes_written += 4 + 1 + reg->city_namesize;
    }

    if (reg->brand_namesize > 0)
    {
        fwrite(&reg->brand_namesize, sizeof(int),  1, output);
        fwrite("1",                  sizeof(char), 1, output);
        fwrite(reg->brand,           sizeof(char), reg->brand_namesize, output);
        bytes_written += 4 + 1 + reg->brand_namesize;
    }

    if (reg->model_namesize > 0)
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

void read_and_write_all_type1(FILE *input, FILE *output)
{   
    char *input_header = read_until(input, '\n');
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

    fseek(output, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, output);

    free(input_header);
}

static reg_t1 *read_register_from_bin(FILE *fp)
{
    reg_t1 *reg = malloc(sizeof(reg_t1));

    // Campos de tamanho fixo
    fread(&reg->removed, sizeof(char), 1, fp);
    fread(&reg->next,    sizeof(int),  1, fp);
    fread(&reg->id,      sizeof(int),  1, fp);
    fread(&reg->year,    sizeof(int),  1, fp);
    fread(&reg->qtt,     sizeof(int),  1, fp);

    reg->state = malloc(sizeof(char) * 3);
    fread(reg->state, sizeof(char), 2, fp);
    reg->state[2] = '\0';

    // Campos de tamanho variável
    reg->city_namesize  = 0;
    reg->brand_namesize = 0;
    reg->model_namesize = 0;
    reg->city  = NULL;
    reg->brand = NULL;
    reg->model = NULL;

    char c;

    // Cidade
    fseek(fp, 4, SEEK_CUR);
    fread(&c, sizeof(char), 1, fp);
    fseek(fp, -5, SEEK_CUR);
    
    if (c == '0')
    {
        fread(&reg->city_namesize, sizeof(int),  1, fp);
        fseek(fp, 1, SEEK_CUR);
        reg->city = malloc(sizeof(char) * (reg->city_namesize + 1));
        fread(reg->city, sizeof(char), reg->city_namesize, fp);
        reg->city[reg->city_namesize] = '\0';
    }

    // Marca
    fseek(fp, 4, SEEK_CUR);
    fread(&c, sizeof(char), 1, fp);
    fseek(fp, -5, SEEK_CUR);

    if (c == '1')
    {
        fread(&reg->brand_namesize, sizeof(int),  1, fp);
        fseek(fp, 1, SEEK_CUR);
        reg->brand = malloc(sizeof(char) * (reg->brand_namesize + 1));
        fread(reg->brand, sizeof(char), reg->brand_namesize, fp);
        reg->brand[reg->brand_namesize] = '\0';
    }

    // Modelo
    fseek(fp, 4, SEEK_CUR);
    fread(&c, sizeof(char), 1, fp);
    fseek(fp, -5, SEEK_CUR);

    if (c == '2')
    {
        fread(&reg->model_namesize, sizeof(int),  1, fp);
        fseek(fp, 1, SEEK_CUR);
        reg->model = malloc(sizeof(char) * (reg->model_namesize + 1));
        fread(reg->model, sizeof(char), reg->model_namesize, fp);
        reg->model[reg->model_namesize] = '\0';
    }    

    return reg;
}

static void print_register_info(reg_t1 *reg)
{
    if (reg->removed == '1')
    {
        printf("Registro inexistente.\n");
        return;
    }

    if (reg->brand) printf("MARCA DO VEICULO: %s\n", reg->brand);
    else printf("MARCA DO VEICULO: NAO PREENCHIDO\n");
    
    if (reg->model) printf("MODELO DO VEICULO: %s\n", reg->model);
    else printf("MODELO DO VEICULO: NAO PREENCHIDO\n");

    if (reg->year != -1) printf("ANO DE FABRICACAO: %d\n", reg->year);
    else printf("ANO DE FABRICACAO: NAO PREENCHIDO\n");

    if (reg->city) printf("NOME DA CIDADE: %s\n", reg->city);
    else printf("NOME DA CIDADE: NAO PREENCHIDO\n");

    if (reg->qtt != -1) printf("QUANTIDADE DE VEICULOS: %d\n\n", reg->qtt);
    else printf("QUANTIDADE DE VEICULOS: NAO PREENCHIDO\n\n");
}

void print_all_from_bin_type1(FILE *fp)
{
    char status;
    fread(&status, sizeof(char), 1, fp);

    if (status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);

    for (long int offset = HEADER_SIZE; offset < file_size; offset += 97)
    {
        fseek(fp, offset, SEEK_SET);
        reg_t1 *reg = read_register_from_bin(fp);
        print_register_info(reg);
        free_register(reg);
    }
}

static reg_t1 *get_search_parameters()
{
    reg_t1 *reg = malloc(sizeof(reg_t1));

    reg->id    = -1;
    reg->year  = -1;
    reg->qtt   = -1;
    reg->state = NULL;
    reg->city  = NULL;
    reg->brand = NULL;
    reg->model = NULL;

    int n;
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

            if      (strcmp(field_name, "sigla")  == 0) reg->state = field_content;
            else if (strcmp(field_name, "cidade") == 0) reg->city  = field_content;
            else if (strcmp(field_name, "marca")  == 0) reg->brand = field_content;
            else if (strcmp(field_name, "modelo") == 0) reg->model = field_content;
        }
        else
        {
            ungetc(c, stdin);
            int value;
            scanf("%d", &value);

            if      (strcmp(field_name, "id")  == 0) reg->id   = value;
            else if (strcmp(field_name, "ano") == 0) reg->year = value;
            else if (strcmp(field_name, "qtt") == 0) reg->qtt  = value;
        }

        free(field_name);
    }
    
    return reg;
}

static void compare_and_print_if_matched(reg_t1 *reg, reg_t1 *search_parameters)
{
    if (search_parameters->id != -1 &&
        search_parameters->id != reg->id)
        return;

    if (search_parameters->year != -1 &&
        search_parameters->year != reg->year)
        return;

    if (search_parameters->qtt != -1 &&
        search_parameters->qtt != reg->qtt)
        return;
    
    if ((search_parameters->state != NULL) && 
        (reg->state == NULL || strcmp(search_parameters->state, reg->state) != 0))
        return;
    
    if ((search_parameters->city != NULL) &&
        (reg->city == NULL || strcmp(search_parameters->city, reg->city) != 0))
        return;

    if ((search_parameters->brand != NULL) &&
        (reg->brand == NULL || strcmp(search_parameters->brand, reg->brand) != 0))
        return;
    
    if ((search_parameters->model != NULL) &&
        (reg->model == NULL || strcmp(search_parameters->model, reg->model) != 0))
        return;

    print_register_info(reg);
}

void search_by_parameters_type1(FILE *fp)
{
    char status;
    fread(&status, sizeof(char), 1, fp);
    fseek(fp, 0, SEEK_SET);

    if (status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    reg_t1 *search_parameters = get_search_parameters();

    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);

    for (long int offset = HEADER_SIZE; offset < file_size; offset += 97)
    {
        fseek(fp, offset, SEEK_SET);
        reg_t1 *reg = read_register_from_bin(fp);
        compare_and_print_if_matched(reg, search_parameters);
        free_register(reg);
    }

    free_register(search_parameters);
}

void search_by_rrn_type1(FILE *fp, int rrn)
{
    char status;
    fread(&status, sizeof(char), 1, fp);

    if (status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    long int offset = HEADER_SIZE + (rrn * 97);

    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);

    if (offset >= file_size)
    {
        printf("Registro inexistente.\n");
        return;
    }

    fseek(fp, offset, SEEK_SET);
    reg_t1 *reg = read_register_from_bin(fp);
    print_register_info(reg);

    free_register(reg);
}