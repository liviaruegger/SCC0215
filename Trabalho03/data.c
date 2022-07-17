/**
 * @file   data.c
 * @author Ana Lívia Ruegger Saldanha (N.USP 8586691)
 * @author André Kenji Hidaka Matsumoto (N. USP 12542689)
 * @brief  SCC0215 - Organização de Arquivos
 *         Trabalho 03
 *
 *         Módulo para manipulação de arquivos de dados
 *
 * @date   2022-07-20
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#define DATA_HEADER_SIZE_T1 182
#define DATA_HEADER_SIZE_T2 190

#define REGISTER_SIZE_T1 97

typedef union next
{
    int rrn; // Usado no tipo1
    long offset; // Usado no tipo2
}   next_t;

typedef struct reg
{
    char removed;
    int  register_size; // Apenas tipo2

    next_t next;

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
}   reg_t;


// ============================ FUNÇÕES AUXILIARES =============================

/**
 * @brief Desaloca a memória utilizada por um registro (struct reg).
 *
 * @param reg ponteiro para a struct que deve ser desalocada.
 */
static void free_register(reg_t *reg)
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

/**
 * @brief Imprime de forma organizada, na saída padrão, os dados contidos em
 * uma struct de registro.
 *
 * @param reg ponteiro para a struct do registro que deve ser impresso.
 */
static void print_register_info(reg_t *reg)
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


// ============================ FUNÇÕES DE LEITURA =============================

/**
 * @brief Lê de um arquivo binário os dados de um registro e insere em uma
 * struct reg.
 *
 * @param fp ponteiro para o arquivo binário de dados;
 * @param type tipo de arquivo (1 para registro de tamanho fixo, 2 para
 * registro de tamanho variável);
 * @return ponteiro para a struct do registro lido (reg_t *).
 */
static reg_t *read_register_from_bin(FILE *fp, int type)
{
    reg_t *reg = malloc(sizeof(reg_t));

    // Campos de tamanho fixo
    fread(&reg->removed, sizeof(char), 1, fp);
    if (type == 1)
    {
        fread(&reg->next.rrn, sizeof(int), 1, fp);
    }
    else
    {
        fread(&reg->register_size, sizeof(int), 1, fp);
        fread(&reg->next.offset, sizeof(long), 1, fp);
    }
    fread(&reg->id,   sizeof(int), 1, fp);
    fread(&reg->year, sizeof(int), 1, fp);
    fread(&reg->qtt,  sizeof(int), 1, fp);

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
    if (fread(&c, sizeof(char), 1, fp) != 1)
        return reg;
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
    if (fread(&c, sizeof(char), 1, fp) != 1)
        return reg;
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
    if (fread(&c, sizeof(char), 1, fp) != 1)
        return reg;
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


// ============================ FUNÇÕES DE ESCRITA =============================

/**
 * @brief Armazena o id e o valor referente a ele e retorna um verificador de
 * status removido do registro.
 *
 * @param fp ponteiro para o arquivo binário de dados;
 * @param id ponteiro para o inteiro que irá armazenar o id do registro.
 * @param ref ponteiro para o inteiro que irá armazenar o valor referente ao id
 * do registro.
 * @return caso o registro esteja removido, retorna 0. Caso contrário, retorna 1.
 */
int get_key_type1(FILE *fp, int *id, int *ref)
{
    *ref = (ftell(fp) - DATA_HEADER_SIZE_T1) / REGISTER_SIZE_T1;
    reg_t *reg = read_register_from_bin(fp, 1);

    if (reg->removed == '1')
    {
        free_register(reg);
        return 0;
    }

    *id = reg->id;
    free_register(reg);
    return 1;
}

/**
 * @brief Armazena o id e o valor referente a ele e retorna um verificador de
 * status removido do registro.
 *
 * @param fp ponteiro para o arquivo binário de dados;
 * @param id ponteiro para o inteiro que irá armazenar o id do registro.
 * @param ref ponteiro para o long que irá armazenar o valor referente ao id do
 * registro.
 * @return caso o registro esteja removido, retorna 0. Caso contrário, retorna 1.
 */
int get_key_type2(FILE *fp, int *id, long *ref)
{
    *ref = ftell(fp);
    reg_t *reg = read_register_from_bin(fp, 2);

    if (reg->removed == '1')
    {
        free_register(reg);
        return 0;
    }

    *id = reg->id;
    free_register(reg);
    return 1;
}

// ============================ FUNÇÕES PARA BUSCA =============================

/**
 * @brief A partir de um RNN, busca um registro epecífico em um arquivo binário
 * e imprime seus dados na saída padrão (ou informa que ele não existe).
 *
 * @param fp ponteiro para o arquivo de dados no qual será feita a busca;
 * @param rrn número relativo do registro.
 */
void search_by_rrn_type1(FILE *fp, int rrn)
{
    char status;
    fread(&status, sizeof(char), 1, fp);

    if (status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    long int offset = DATA_HEADER_SIZE_T1 + (rrn * REGISTER_SIZE_T1);

    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);

    if (offset >= file_size)
    {
        printf("Registro inexistente.\n");
        return;
    }

    fseek(fp, offset, SEEK_SET);
    reg_t *reg = read_register_from_bin(fp, 1);
    print_register_info(reg);

    free_register(reg);
}

/**
 * @brief A partir de um byte offset, busca um registro epecífico em um arquivo
 * binário e imprime seus dados na saída padrão (ou informa que ele não existe).
 *
 * @param fp ponteiro para o arquivo de dados no qual será feita a busca;
 * @param offset byte offset do registro buscado.
 */
void search_by_offset_type2(FILE *fp, long offset)
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

    if (offset >= file_size)
    {
        printf("Registro inexistente.\n");
        return;
    }

    fseek(fp, offset, SEEK_SET);
    reg_t *reg = read_register_from_bin(fp, 2);
    print_register_info(reg);

    free_register(reg);
}
