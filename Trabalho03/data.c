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
#include <string.h>
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

/**
 * @brief Lê um inteiro em um offset especificado.
 *
 * @param fp ponteiro para o arquivo.
 * @param offset offset do inteiro a ser lido.
 * @return inteiro lido (int).
 */
static int read_int_type(FILE *fp, long offset)
{
    int value;
    fseek(fp, offset, SEEK_SET);
    fread(&value, sizeof(int), 1, fp);
    return value;
}

/**
 * @brief Escreve um inteiro em um offset especificado.
 *
 * @param fp ponteiro para o arquivo.
 * @param offset offset do inteiro.
 * @param value valor do inteiro.
 */
static void write_int_type(FILE *fp, long offset, int value)
{
    fseek(fp, offset, SEEK_SET);
    fwrite(&value, sizeof(int), 1, fp);
}

/**
 * @brief Lê um long em um offset especificado.
 *
 * @param fp ponteiro para o arquivo.
 * @param offset offset do long a ser lido.
 * @return long lido (long).
 */
static long read_long_type(FILE *fp, long offset)
{
    long value;
    fseek(fp, offset, SEEK_SET);
    fread(&value, sizeof(long), 1, fp);
    return value;
}

/**
 * @brief Escreve um long em um offset especificado.
 *
 * @param fp ponteiro para o arquivo.
 * @param offset offset do long.
 * @param value valor do long.
 */
static void write_long_type(FILE *fp, long offset, long value)
{
    fseek(fp, offset, SEEK_SET);
    fwrite(&value, sizeof(long), 1, fp);
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

/**
 * @brief Lê da entrada padrão os dados de um registro e insere em uma struct
 * reg_t.
 *
 * @param type tipo de arquivo (1 para registro de tamanho fixo, 2 para
 * registro de tamanho variável);
 * @return ponteiro para a struct do registro lido (reg_t *).
 */
static reg_t *read_register_from_stdin(int type)
{
    // Formato da entrada:
    // id ano qtt "sigla" "cidade" "marca" "modelo"

    reg_t *reg = malloc(sizeof(reg_t));

    reg->removed = '0';
    if (type == 1)
        reg->next.rrn = -1;
    else
        reg->next.offset = -1;
    reg->register_size = 0;

    char *id = read_until(stdin, ' ');
    reg->id = atoi(id);
    free(id);

    char *year = read_until(stdin, ' ');
    if (year[0] >= '0' && year[0] <= '9') reg->year = atoi(year);
    else reg->year = -1;
    free(year);

    char *qtt = read_until(stdin, ' ');
    if (qtt[0] >= '0' && qtt[0] <= '9') reg->qtt = atoi(qtt);
    else reg->qtt = -1;
    free(qtt);

    char c = getchar();
    if (c == '"')
    {
        char *state = read_until(stdin, '"');
        getchar(); // Consome o espaço que sobra

        reg->state = state;
    }
    else // NULO
    {
        char *s = read_until(stdin, ' ');
        free(s);
        reg->state = malloc(sizeof(char) * 2);
        reg->state[0] = '$';
        reg->state[1] = '$';
    }

    c = getchar();
    if (c == '"')
    {
        char *city = read_until(stdin, '"');
        getchar(); // Consome o espaço que sobra

        reg->city = city;
    }
    else // NULO
    {
        char *s = read_until(stdin, ' ');
        free(s);
        reg->city = NULL;
    }

    if (reg->city)
    {
        reg->city_namesize = strlen(reg->city);
        reg->register_size += sizeof(reg->city_namesize);
        reg->register_size += reg->city_namesize;
        reg->register_size += 1;
    }
    else reg->city_namesize = 0;

    c = getchar();
    if (c == '"')
    {
        char *brand = read_until(stdin, '"');
        getchar(); // Consome o espaço que sobra

        reg->brand = brand;
    }
    else // NULO
    {
        char *s = read_until(stdin, ' ');
        free(s);
        reg->brand = NULL;
    }

    if (reg->brand)
    {
        reg->brand_namesize = strlen(reg->brand);
        reg->register_size += sizeof(reg->brand_namesize);
        reg->register_size += reg->brand_namesize;
        reg->register_size += 1;
    }
    else reg->brand_namesize = 0;

    c = getchar();
    if (c == '"')
    {
        char *model = read_until(stdin, '"');
        getchar(); // Consome o espaço que sobra

        reg->model = model;
    }
    else // NULO
    {
        char *s = read_line(stdin);
        free(s);
        reg->model = NULL;
    }

    if (reg->model)
    {
        reg->model_namesize = strlen(reg->model);
        reg->register_size += sizeof(reg->model_namesize);
        reg->register_size += reg->model_namesize;
        reg->register_size += 1;
    }
    else reg->model_namesize = 0;

    if (type == 2)
    {
        reg->register_size += sizeof(reg->next.offset);
        reg->register_size += sizeof(reg->id);
        reg->register_size += sizeof(reg->year);
        reg->register_size += sizeof(reg->qtt);
        reg->register_size += 2;
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

/**
 * @brief Escreve um registro em um arquivo binário de dados
 * (aberto previamente).
 *
 * @param fp arquivo .bin de saída.
 * @param reg registro para escrever no arquivo;
 * @param type tipo de arquivo (1 para registro de tamanho fixo, 2 para
 * registro de tamanho variável);
 */
static void write_register(FILE *fp, reg_t *reg, int type)
{
    // Campos de tamanho fixo
    fwrite(&reg->removed, sizeof(char), 1, fp);

    if (type == 1)
        fwrite(&reg->next,    sizeof(int),  1, fp);
    else
    {
        fwrite(&reg->register_size, sizeof(int),   1, fp);
        fwrite(&reg->next,          sizeof(long),  1, fp);
    }

    fwrite(&reg->id,      sizeof(int),  1, fp);
    fwrite(&reg->year,    sizeof(int),  1, fp);
    fwrite(&reg->qtt,     sizeof(int),  1, fp);
    fwrite(reg->state,    sizeof(char), 2, fp);


    int bytes_written;
    if (type == 1)
        bytes_written = 19;
    else
        bytes_written = 27;

    // Campos de tamanho variável
    if (reg->city_namesize > 0)
    {
        fwrite(&reg->city_namesize, sizeof(int),  1, fp);
        fwrite("0",                 sizeof(char), 1, fp);
        fwrite(reg->city,           sizeof(char), reg->city_namesize, fp);
        bytes_written += 4 + 1 + reg->city_namesize;
    }

    if (reg->brand_namesize > 0)
    {
        fwrite(&reg->brand_namesize, sizeof(int),  1, fp);
        fwrite("1",                  sizeof(char), 1, fp);
        fwrite(reg->brand,           sizeof(char), reg->brand_namesize, fp);
        bytes_written += 4 + 1 + reg->brand_namesize;
    }

    if (reg->model_namesize > 0)
    {
        fwrite(&reg->model_namesize, sizeof(int),  1, fp);
        fwrite("2",                  sizeof(char), 1, fp);
        fwrite(reg->model,           sizeof(char), reg->model_namesize, fp);
        bytes_written += 4 + 1 + reg->model_namesize;
    }

    int total_bytes;
    if (type == 1)
        total_bytes = REGISTER_SIZE_T1;
    else
        total_bytes = reg->register_size + 5;

    while (bytes_written < total_bytes)
    {
        fwrite("$", sizeof(char), 1, fp);
        bytes_written++;
    }
}

/**
 * @brief Adiciona um novo registro do tipo 1 a um arquivo de dados.
 *
 * @param fp ponteiro para o arquivo de dados.
 * @param id ponteiro para a variável em que o id será armazenado.
 * @param ref ponteiro para a variável em que o valor referente ao id será
 * armazenado.
 */
void insert_new_register_type1(FILE *fp, int *id, int *ref)
{
    reg_t *reg = read_register_from_stdin(1);
    *id = reg->id;

    update_header_status(fp, '0');

    int top = read_int_type(fp, 1);
    if (top == -1)
    {
        // Atualiza o campo 'proxRRN'.
        int next_rrn = read_int_type(fp, 174);
        *ref = next_rrn + 1; // RRN do reg inserido.
        write_int_type(fp, 174, *ref);

        fseek(fp, 0, SEEK_END);
    }
    else
    {
        long offset = DATA_HEADER_SIZE_T1 + (top * REGISTER_SIZE_T1) + 1;

        // Atualiza o campo 'topo'.
        int new_top = read_int_type(fp, offset);
        write_int_type(fp, 1, new_top);

        // Atualiza o campo 'nroRegRem'.
        int reg_rem = read_int_type(fp, 178);
        write_int_type(fp, 178, reg_rem - 1);

        *ref = top;
        fseek(fp, offset - 1, SEEK_SET);
    }

    write_register(fp, reg, 1);
    free_register(reg);
}

/**
 * @brief Adiciona um novo registro do tipo 2 a um arquivo de dados.
 *
 * @param fp ponteiro para o arquivo de dados.
 * @param id ponteiro para a variável em que o id será armazenado.
 * @param ref ponteiro para a variável em que o valor referente ao id será
 * armazenado.
 */
void insert_new_register_type2(FILE *fp, int *id, long *ref)
{
    reg_t *reg = read_register_from_stdin(2);
    *id = reg->id;

    update_header_status(fp, '0');

    long top = read_long_type(fp, 1);
    if (top == -1)
    {
        // Adiciona no fim do arquivo.
        fseek(fp, 0, SEEK_END);
        *ref = ftell(fp);
        write_register(fp, reg, 2);

        // Atualiza o campo 'proxByteOffset'
        fseek(fp, 0, SEEK_END);
        long next_byte_offset = ftell(fp);
        write_long_type(fp, 178, next_byte_offset);
    }
    else
    {
        int top_size, size_diff;
        top_size = read_int_type(fp, top + 1);

        size_diff = reg->register_size - top_size;

        // Cabe
        if (size_diff <= 0)
        {
            long prox;
            fread(&prox, sizeof(long), 1, fp);

            reg->register_size = top_size;
            fseek(fp, top, SEEK_SET);
            write_register(fp, reg, 2);

            // Atualiza o campo 'topo'.
            write_long_type(fp, 1, prox);

            // Atualiza o campo 'nroRegRem'.
            int reg_rem = read_int_type(fp, 186);
            write_int_type(fp, 186, reg_rem - 1);
        }
        // Não cabe
        else
        {
            // Adiciona no fim do arquivo.
            fseek(fp, 0, SEEK_END);
            *ref = ftell(fp);
            write_register(fp, reg, 2);

            // Atualiza o campo 'proxByteOffset'
            fseek(fp, 0, SEEK_END);
            long next_byte_offset = ftell(fp);
            write_long_type(fp, 178, next_byte_offset);
        }
    }
    free_register(reg);
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
