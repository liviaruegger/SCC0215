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

#define N_REG_REM_FIELD_OFFSET_T1 178
#define N_REG_REM_FIELD_OFFSET_T2 186

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
 * @brief Retorna o RRN (para arquivo tipo1) ou o offset (para arquivo tipo2)
 * contido no campo 'topo' do cabeçalho de um arquivo de dados.
 *
 * @param fp ponteiro para o arquivo de dados;
 * @param type tipo de arquivo (1 para registro de tamanho fixo, 2 para
 * registro de tamanho variável);
 * @return RRN ou offset armazenado no campo 'topo' (long).
 */
static long get_top(FILE *fp, int type)
{
    long top;
    fseek(fp, 1, SEEK_SET);
    if (type == 1) fread(&top, sizeof(int), 1, fp);
    else fread(&top, sizeof(long), 1, fp);

    return top;
}

/**
 * @brief Atualiza o topo escrito no cabeçalho do arquivo.
 *
 * @param fp ponteiro para o arquivo de dados;
 * @param new_top valor (RRN ou offset) que deve ser colocado no campo 'topo'
 * do cabeçalho;
 * @param type tipo de arquivo (1 para registro de tamanho fixo, 2 para
 * registro de tamanho variável).
 */
static void update_top(FILE *fp, long new_top, int type)
{
    fseek(fp, 1, SEEK_SET);
    if (type == 1)
    {
        int new_top_type1 = (int)new_top;
        fwrite(&new_top_type1, sizeof(int), 1, fp);
    }
    else
    {
        fwrite(&new_top, sizeof(long), 1, fp);
    }
}

/**
 * @brief Atualiza o campo 'nroRegRem' no cabeçalho.
 *
 * @param fp ponteiro para o arquivo binário de dados;
 * @param n quantidade de novos registros removidos;
 * @param type tipo de arquivo (1 para registro de tamanho fixo, 2 para
 * registro de tamanho variável).
 */
static void update_n_reg_rem(FILE *fp, int n, int type)
{
    int temp;

    // Lê do cabeçalho o número de registros removidos
    if (type == 1) fseek(fp, N_REG_REM_FIELD_OFFSET_T1, SEEK_SET);
    else fseek(fp, N_REG_REM_FIELD_OFFSET_T2, SEEK_SET);
    fread(&temp, sizeof(int), 1, fp);

    // Atualiza
    n = n + temp;
    if (type == 1) fseek(fp, N_REG_REM_FIELD_OFFSET_T1, SEEK_SET);
    else fseek(fp, N_REG_REM_FIELD_OFFSET_T2, SEEK_SET);
    fwrite(&n, sizeof(int), 1, fp);
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
        fread(&reg->next.offset,  sizeof(long), 1, fp);
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
 * @brief Lê da entrada padrão os dados de um registro e insere em uma
 * struct reg.
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

    if (type == 1) reg->next.rrn = -1;
    else reg->next.offset = -1;
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
        char *temp = read_until(stdin, ' ');
        free(temp);
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
        char *temp = read_until(stdin, ' ');
        free(temp);
        reg->city = NULL;
    }

    if (reg->city)
    {
        reg->city_namesize = strlen(reg->city);
        reg->register_size += reg->city_namesize + 5;
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
        char *temp = read_until(stdin, ' ');
        free(temp);
        reg->brand = NULL;
    }

    if (reg->brand)
    {
        reg->brand_namesize = strlen(reg->brand);
        reg->register_size += reg->brand_namesize + 5;
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
        char *temp = read_line(stdin);
        free(temp);
        reg->model = NULL;
    }

    if (reg->model)
    {
        reg->model_namesize = strlen(reg->model);
        reg->register_size += reg->model_namesize + 5;
    }
    else reg->model_namesize = 0;

    // Contar e guardar o tamanho do registro
    reg->register_size += 22; // Campos de tamanho fixo

    return reg;
}

// ============================ FUNÇÕES DE ESCRITA =============================

/**
 * @brief Escreve um registro em um arquivo binário de dados.
 *
 * @param fp ponteiro para o arquivo binário de dados;
 * @param reg registro que será escrito no arquivo;
 * @param type tipo de arquivo (1 para registro de tamanho fixo, 2 para
 * registro de tamanho variável).
 */
static void write_register(FILE *fp, reg_t *reg, int type)
{
    // Campos de tamanho fixo
    fwrite(&reg->removed, sizeof(char), 1, fp);
    if (type == 1)
    {
        fwrite(&reg->next.rrn, sizeof(int), 1, fp);
    }
    else
    {
        fwrite(&reg->register_size, sizeof(int), 1, fp);
        fwrite(&reg->next.offset,  sizeof(long), 1, fp);
    }
    fwrite(&reg->id,   sizeof(int),  1, fp);
    fwrite(&reg->year, sizeof(int),  1, fp);
    fwrite(&reg->qtt,  sizeof(int),  1, fp);
    fwrite(reg->state, sizeof(char), 2, fp);

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

void insert_registers_from_file_type1(FILE *data_fp, FILE *index_fp)
{
    // Armazena o tamanho do arquivo de dados.
    fseek(data_fp, 0, SEEK_END);
    long file_size = ftell(data_fp);

    int id, ref;

    for (long offset = DATA_HEADER_SIZE_T1; offset < file_size; offset += REGISTER_SIZE_T1)
    {
        fseek(data_fp, offset, SEEK_SET);

        ref = (ftell(fp) - DATA_HEADER_SIZE_T1) / REGISTER_SIZE_T1;
        reg_t *reg = read_register_from_bin(fp, 1);

        if (reg->removed == '0')
        {
            id = reg->id;
            // insert()
        }
        free_register(reg);
    }
}


void insert_registers_from_file_type2(FILE *data_fp, FILE *index_fp)
{
    // Armazena o tamanho do arquivo de dados.
    fseek(data_fp, 0, SEEK_END);
    long file_size = ftell(data_fp);

    int id;
    long ref;

    fseek(data_fp, DATA_HEADER_SIZE_T2, SEEK_SET);

    while (ftell(data_fp) < file_size)
    {
        ref = ftell(data_fp);
        reg_t *reg = read_register_from_bin(fp, 2);

        if (reg->removed == '0')
        {
            id = reg->id;
            // insert()
        }
    }
}


/**
 * @brief Adiciona novo(s) registro(s) a um arquivo de dados e atualiza o
 * arquivo de índice.
 *
 * @param data_fp ponteiro para o arquivo de dados;
 * @param index_fp ponteiro para o arquivo de índice;
 * @param n_registers número de registros a serem adicionados.
 */
void insert_new_registers_type1(FILE *data_fp, FILE *index_fp, int n_registers)
{
    int rrn;

    int pop_count = 0;

    for (int i = 0; i < n_registers; i++)
    {
        reg_t *reg = read_register_from_stdin(1);

        int top = (int)get_top(data_fp, 1);
        if (top == -1)
        {
            fseek(data_fp, 174, SEEK_SET);
            fread(&rrn, sizeof(int), 1, data_fp);

            int next_rrn = rrn + 1;
            fseek(data_fp, 174, SEEK_SET);
            fwrite(&next_rrn, sizeof(int), 1, data_fp);

            fseek(data_fp, 0, SEEK_END);
        }
        else
        {
            int offset = DATA_HEADER_SIZE_T1 + (top * REGISTER_SIZE_T1) + 1;
            fseek(data_fp, offset, SEEK_SET);

            int next; // Novo topo
            fread(&next, sizeof(int), 1, data_fp);
            update_top(data_fp, next, 1);

            offset = DATA_HEADER_SIZE_T1 + (top * REGISTER_SIZE_T1);
            fseek(data_fp, offset, SEEK_SET);
            rrn = top;

            pop_count++;
        }

        int id = reg->id;

        write_register(data_fp, reg, 1);
        free_register(reg);

        // Marcar o arquivo de índice como inconsistente
        update_header_status(index_fp, '0');

        // Adicionar no índice árvore-B
        // TODO -> inserir campos: id, rrn (essas duas variáveis mesmo)
    }

    update_n_reg_rem(data_fp, (-1) * pop_count, 1);

    // Marcar o arquivo de índice como consistente
    update_header_status(index_fp, '1');
}

/**
 * @brief Adiciona novo(s) registro(s) a um arquivo de dados e atualiza o
 * arquivo de índice.
 *
 * @param data_fp ponteiro para o arquivo de dados;
 * @param index_fp ponteiro para o arquivo de índice;
 * @param n_registers número de registros a serem adicionados.
 */
void insert_new_registers_type2(FILE *data_fp, FILE *index_fp, int n_registers)
{
    long offset;
    int pop_count = 0;

    for (int i = 0; i < n_registers; i++)
    {
        reg_t *reg = read_register_from_stdin(2);

        update_header_status(data_fp, '0');

        long top = get_top(data_fp, 2);
        if (top == -1)
        {
            // Adiciona no fim do arquivo.
            fseek(data_fp, 0, SEEK_END);
            offset = ftell(data_fp);
            write_register(data_fp, reg, 2);

            // Atualiza o campo 'proxByteOffset'
            fseek(data_fp, 0, SEEK_END);
            long next_byte_offset = ftell(data_fp);
            fseek(data_fp, 178, SEEK_SET);
            fwrite(&next_byte_offset, sizeof(long), 1, data_fp);
        }
        else
        {
            int top_size, size_diff;
            fseek(data_fp, top + 1, SEEK_SET);
            fread(&top_size, sizeof(int), 1, data_fp);

            size_diff = reg->register_size - top_size;

            // Cabe
            if (size_diff <= 0)
            {
                long prox;
                fread(&prox, sizeof(long), 1, data_fp);

                reg->register_size = top_size;
                fseek(data_fp, top, SEEK_SET);
                write_register(data_fp, reg, 2);

                // Atualiza o campo 'topo'.
                update_top(data_fp, prox, 2);

                offset = top;

                pop_count++;
            }
            // Não cabe
            else
            {
                // Adiciona no fim do arquivo.
                fseek(data_fp, 0, SEEK_END);
                offset = ftell(data_fp);
                write_register(data_fp, reg, 2);

                // Atualiza o campo 'proxByteOffset'
                fseek(data_fp, 0, SEEK_END);
                long next_byte_offset = ftell(data_fp);
                fseek(data_fp, 178, SEEK_SET);
                fwrite(&next_byte_offset, sizeof(long), 1, data_fp);
            }
        }
        free_register(reg);

        // Marcar o arquivo de índice como inconsistente
        update_header_status(index_fp, '0');

        // Adicionar no índice árvore-B
        // TODO -> inserir campos: id, rrn (essas duas variáveis mesmo)
    }
    update_n_reg_rem(data_fp, (-1) * pop_count, 2);

    // Marcar os arquivos como consistentes
    update_header_status(index_fp, '1');
    update_header_status(data_fp,  '1');
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
