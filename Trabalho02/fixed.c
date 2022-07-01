/**
 * @file   fixed.c
 * @author Ana Lívia Ruegger Saldanha (N.USP 8586691)
 * @author André Kenji Hidaka Matsumoto (N. USP 12542689)
 * @brief  SCC0215 - Organização de Arquivos
 *         Trabalho 02
 *
 *         Módulo que trabalha com arquivos Tipo 1 (Arquivo de Dados para
 *         Registros de Tamanho Fixo)
 *
 * @date   2022-06-29
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define HEADER_SIZE 182
#define REGISTER_SIZE 97

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

typedef struct register_type1_index
{
    int id;
    int rrn;
}   index_t1;

/**
 * @brief Cria um arquivo binário de dados do Tipo 1 e insere os dados de
 * cabeçalho.
 *
 * @param file_name nome para o arquivo binário que será criado;
 * @return ponteiro para o arquivo criado (FILE *).
 */
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

/**
 * @brief Lê de um arquivo .csv os dados de um registro e insere em uma struct
 * register_type1.
 *
 * @param fp ponteiro para o arquivo .csv;
 * @return ponteiro para a struct do registro lido (reg_t1 *).
 */
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

/**
 * @brief Desaloca a memória utilizada por um registro (struct register_type1).
 *
 * @param reg ponteiro para a struct que deve ser desalocada.
 */
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

/**
 * @brief Escreve um registro em um arquivo binário de dados
 * (aberto previamente).
 *
 * @param reg registro para escrever no arquivo;
 * @param output arquivo .bin de saída.
 */
static void write_register(reg_t1 *reg, FILE *output)
{
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

    while (bytes_written < REGISTER_SIZE)
    {
        fwrite("$", sizeof(char), 1, output);
        bytes_written++;
    }

    // free_register(reg);
}

/**
 * @brief Lê e escreve em um arquivo de dados .bin todos os registros contidos
 * em um arquivo .csv, um registro por vez.
 *
 * @param input arquivo .csv de entrada;
 * @param output arquivo .bin de saída.
 */
void read_and_write_all_type1(FILE *input, FILE *output)
{
    char *input_header = read_until(input, '\n');
    int register_count = 0;

    char c = fgetc(input);
    while (c != EOF)
    {
        ungetc(c, input);
        reg_t1 *reg = read_register_from_csv(input);
        write_register(reg, output);
        register_count++;

        c = fgetc(input);
    }

    fseek(output, 174, SEEK_SET);
    fwrite(&register_count, sizeof(int), 1, output);

    fseek(output, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, output);

    free(input_header);
}

/**
 * @brief Lê de um arquivo binário os dados de um registro e insere em uma
 * struct register_type1.
 *
 * @param fp ponteiro para o arquivo binário de dados;
 * @return ponteiro para a struct do registro lido (reg_t1 *).
 */
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

/**
 * @brief Imprime de forma organizada, na saída padrão, os dados contidos em
 * uma struct register_type1.
 *
 * @param reg ponteiro para a struct do registro que deve ser impresso.
 */
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

/**
 * @brief Imprime os dados de todos os registros contidos em um arquivo de
 * dados binário do Tipo 1.
 *
 * @param fp ponteiro para o arquivo binário de dados.
 */
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

    for (long int offset = HEADER_SIZE; offset < file_size; offset += REGISTER_SIZE)
    {
        fseek(fp, offset, SEEK_SET);
        reg_t1 *reg = read_register_from_bin(fp);
        print_register_info(reg);
        free_register(reg);
    }
}

/**
 * @brief Lê da entrada padrão os parâmetros para busca de um registro.
 *
 * @return struct register_type1 contendo os paramêtros para busca (reg_t1 *);
 * os campos que não devem ser considerados são marcados com -1 ou NULL.
 */
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
    scanf(" %d", &n);
    getchar(); // Consome o '\n' ou espaço

    for (int i = 0; i < n; i++)
    {
        char *field_name = read_word(stdin);
        char *field_content = NULL;
        char c = getchar();

        if (c == '"')
        {
            field_content = read_until(stdin, '"');
            getchar(); // Consome o '\n' ou espaço

            if      (strcmp(field_name, "sigla")  == 0) reg->state = field_content;
            else if (strcmp(field_name, "cidade") == 0) reg->city  = field_content;
            else if (strcmp(field_name, "marca")  == 0) reg->brand = field_content;
            else if (strcmp(field_name, "modelo") == 0) reg->model = field_content;
        }
        else if (c == 'N') // NULO
        {
            read_word(stdin); // Descarta leitura "ULO "
            field_content = NULL;

            if (strcmp(field_name, "sigla") == 0)
            {
                reg->state = malloc(sizeof(char) * 2);
                reg->state[0] = '$';
                reg->state[1] = '$';
            }
            else if (strcmp(field_name, "cidade") == 0)
            {
                reg->city  = field_content;
                reg->city_namesize = -2;            
            }
            else if (strcmp(field_name, "marca") == 0)
            {
                reg->brand = field_content;
                reg->brand_namesize = -2;
            }
            else if (strcmp(field_name, "modelo") == 0)
            {
                reg->model = field_content;
                reg->model_namesize = -2;
            }
            else if (strcmp(field_name, "ano") == 0)
            {
                reg->year = -2; // Identifica para sobrescrever
            }
            else if (strcmp(field_name, "qtt") == 0)
            {
                reg->qtt  = -2; // Identifica para sobrescrever
            }
        }
        else
        {
            ungetc(c, stdin);
            int value;
            scanf(" %d ", &value);

            if      (strcmp(field_name, "id")  == 0) reg->id   = value;
            else if (strcmp(field_name, "ano") == 0) reg->year = value;
            else if (strcmp(field_name, "qtt") == 0) reg->qtt  = value;
        }

        free(field_name);
    }

    return reg;
}

/**
 * @brief Compara dois registros contidos em structs register_type1, sendo um
 * deles um registro previamente lido de um arquivo de dados e o outro um
 * registro contendo parâmetros para busca. Caso as condições de busca sejam
 * satisfeitas, o registro completo será impresso na saída padrão.
 *
 * @param reg ponteiro para uma struct reg_t1 contendo os dados de um registro
 * buscado, que foi lido previamente de um arquivo de dados;
 * @param search_parameters ponteiro para a struct contendo os parâmetros de
 * busca.
 */
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

/**
 * @brief Lê da entrada padrão n parâmetros para busca de registros e executa
 * essa busca linearmente em um arquivo binário de dados.
 *
 * @param fp ponteiro para o arquivo de dados no qual será feita a busca.
 */
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

    for (long int offset = HEADER_SIZE; offset < file_size; offset += REGISTER_SIZE)
    {
        fseek(fp, offset, SEEK_SET);
        reg_t1 *reg = read_register_from_bin(fp);
        compare_and_print_if_matched(reg, search_parameters);
        free_register(reg);
    }

    free_register(search_parameters);
}

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

    long int offset = HEADER_SIZE + (rrn * REGISTER_SIZE);

    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);

    if (offset >= file_size)
    {
        printf("Registro inexistente.\n");
        return;
    }

    fseek(fp, offset, SEEK_SET);
    reg_t1 *reg = read_register_from_bin(stdin);
    print_register_info(reg);

    free_register(reg);
}

/**
 * @brief Ordena o vetor de índices.
 *
 * @param index ponteiro para o vetor de índices.
 * @param size tamanho do vetor de índices.
 */
void insertionSort_t1(index_t1 *index, int size)
{
    int j;
    for (j = 1; j < size; j++)
    {
        int chave = index[j].id;
        int rrn = index[j].rrn;
        int i = j - 1;
        while (i >= 0 && index[i].id > chave)
        {
            index[i + 1].id = index[i].id;
            index[i + 1].rrn = index[i].rrn;
            i--;
        }
        index[i + 1].id = chave;
        index[i + 1].rrn = rrn;
    }
}

/**
 * @brief Faz uma busca binária de um id no vetor de índices.
 *
 * @param index ponteiro para o vetor de índices.
 * @param beg extremo esquerdo do vetor.
 * @param end extremo direito do vetor.
 * @param value id a ser buscado.
 *
 * @return posição do elemento ou -1, caso não seja encontrado.
 */
int binarySearch_t1(index_t1 *index, int beg, int end, int value)
{
    int mid;
    if(end >= beg)
    {
        mid = (beg + end) / 2;
        if(index[mid].id == value)
        {
            return mid;
        }
        else if(index[mid].id < value)
        {
            return binarySearch_t1(index, mid + 1, end, value);
        }
        else
        {
            return binarySearch_t1(index, beg, mid - 1, value);
        }
    }
    return -1;
}

/**
 * @brief Escreve o vetor de índices em um arquivo de índices binário.
 *
 * @param index ponteiro para o vetor de índices.
 * @param size tamanho do vetor de índices.
 * @param fp ponteiro para o arquivo binário.
 */
void type1_index_ram_to_disk (index_t1 *index, int size, FILE *fp)
{
    insertionSort_t1(index, size);

    fseek(fp, 0, SEEK_SET);
    fwrite("0", sizeof(char), 1, fp);

    for (int i = 0; i < size; i++)
    {
        fwrite(&index[i].id,  sizeof(int), 1, fp);
        fwrite(&index[i].rrn, sizeof(int), 1, fp);
    }
}

/**
 * @brief Lê um arquivo de índices binário e armazena as informaçõe em um vetor
 * de índices.
 *
 * @param size tamanho do vetor de índices.
 * @param fp ponteiro para o arquivo de dados binário.
 */
static index_t1 *type1_index_disk_to_ram (int *size, FILE *fp)
{
    index_t1 *index = NULL;
    fseek(fp, 0, SEEK_SET);
    char c = fgetc(fp);

    if (c == '1')
    {
        *size = 0;
        int id;
        while (fread(&id, sizeof(int), 1, fp) == 1)
        {
            *size = *size + 1;

            index = realloc(index, *size * sizeof(index_t1));
            index[*size - 1].id = id;
            fread(&index[*size - 1].rrn, sizeof(int), 1, fp);
        }
    }
    return index;
}

/**
 * @brief Lê um arquivo de dados binário e, com base nele, cria um arquivo
 * de índices.
 *
 * @param data_fp ponteiro para o arquivo de dados.
 * @param file_name nome do arquivo de índices que será criado.
 */
FILE *new_type1_index(FILE *data_fp, char *file_name)
{
    FILE *index_fp = fopen(file_name, "wb");
    fwrite("0", sizeof(char), 1, index_fp);

    index_t1 *index = NULL;
    int size = 0;
    long int offset;

    fseek(data_fp, 182, SEEK_SET);
    offset = ftell(data_fp);
    char c = fgetc(data_fp);
    while (c != EOF) {
        if (c == '0')
        {
            size++;
            index = realloc(index, size * sizeof(index_t1));

            fseek(data_fp, 4, SEEK_CUR);
            fread(&index[size - 1].id, sizeof(int), 1, data_fp);
            index[size - 1].rrn = (offset - HEADER_SIZE) / REGISTER_SIZE;
            fseek(data_fp, 88, SEEK_CUR);
        }
        else
        {
            fseek(data_fp, 96, SEEK_CUR);
        }
        offset = ftell(data_fp);
        c = fgetc(data_fp);
    }
    type1_index_ram_to_disk(index, size, index_fp);

    free(index);

    fseek(index_fp, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, index_fp);

    return index_fp;
}

/**
 * @brief Atualiza o campo 'nroRegRem' no cabeçalho.
 *
 * @param fp ponteiro para o arquivo de dados binário.
 * @param qnt quantidade de novos registros removidos.
 */
void type1_update_nroRegRem(FILE *fp, int qnt)
{
    int temp;

    // Lê o nroRegRem do cabeçalho
    fseek(fp, 178, SEEK_SET);
    fread(&temp, sizeof(int), 1, fp);

    // Atualiza nroRegRem
    qnt = qnt + temp;
    fseek(fp, 178, SEEK_SET);
    fwrite(&qnt, sizeof(int), 1, fp);
}

/**
 * @brief Atualiza o cabeçalho do registro de dados binário.
 *
 * @param fp ponteiro para o arquivo de dados binário.
 * @param qnt quantidade de novos registros removidos.
 * @param topo RRN que será armazenado no campo 'topo'.
 */
void type1_update_header(FILE *fp, int qnt, int *top)
{
    type1_update_nroRegRem(fp, qnt);
    fseek(fp, 1, SEEK_SET);
    fwrite(top, sizeof(int), 1, fp);
}

/**
 * @brief Empilha um novo RRN na pilha do registro de dados.
 *
 * @param fp ponteiro para o arquivo de dados binário.
 * @param topo RRN que está no topo.
 * @param rrn novo rrn a ser empilhado.
 */
void type1_update_stack(FILE *fp, int *topo, int rrn)
{
    fseek(fp, HEADER_SIZE + (rrn * REGISTER_SIZE) + 1, SEEK_SET);
    fwrite(topo, sizeof(int), 1, fp);
    *topo = rrn;
}

/**
 * @brief Verifica se um registro cumpre os requisitos de busca de outro.
 *
 * @param reg registro que será verificado.
 * @param search_parameters registro de parâmetros.
 *
 * @return retorna 0 caso o registro não cumpra os requisitos e 1, caso sim.
 */
static int verify_reg_t1(reg_t1 *reg, reg_t1 *search_parameters)
{
    if (search_parameters->id != -1 &&
        search_parameters->id != reg->id)
        return 0;

    if (search_parameters->year != -1 &&
        search_parameters->year != reg->year)
        return 0;

    if (search_parameters->qtt != -1 &&
        search_parameters->qtt != reg->qtt)
        return 0;

    if ((search_parameters->state != NULL) &&
        (reg->state == NULL || strcmp(search_parameters->state, reg->state) != 0))
        return 0;

    if ((search_parameters->city != NULL) &&
        (reg->city == NULL || strcmp(search_parameters->city, reg->city) != 0))
        return 0;

    if ((search_parameters->brand != NULL) &&
        (reg->brand == NULL || strcmp(search_parameters->brand, reg->brand) != 0))
        return 0;

    if ((search_parameters->model != NULL) &&
        (reg->model == NULL || strcmp(search_parameters->model, reg->model) != 0))
        return 0;

    return 1;
}

/**
 * @brief Utilizando um registro de parâmetros e um vetor de índices, procura o
 * registro.
 *
 * @param data_fp ponteiro para o arquivo de dados.
 * @param index ponteiro para o vetor de índices
 * @param index_size tamanho do vetor de índices.
 * @param search_parameters registro de parâmetros.
 * @param size tamanho do vetor retornado.
 *
 * @return retorna um vetor unitário contendo o RRN do registro, caso ele seja
 * encontrado e NULL, caso não seja.
 */
int *type1_search_id (FILE *data_fp, index_t1 *index, int index_size, reg_t1 *search_param, int *size)
{
    int *rrns = NULL;
    *size = 0;
    int position = binarySearch_t1(index, 0, index_size, search_param->id);

    if (position != -1)
    {
        int rrn = index[position].rrn;
        fseek(data_fp, HEADER_SIZE + (rrn * REGISTER_SIZE), SEEK_SET);

        reg_t1 *reg = read_register_from_bin(data_fp);
        if (reg->removed == '0')
        {
            if(verify_reg_t1(reg, search_param) == 1)
            {
                *size = 1;
                rrns = malloc(sizeof(int));
                rrns[0] = rrn;
            }
        }
        free_register(reg);
    }

    return rrns;
}

/**
 * @brief Utilizando um registro de parâmetros, procura o(s) registro(s) que sa-
 * tisfazam os critérios de busca.
 *
 * @param fp ponteiro para o arquivo de dados.
 * @param search_parameters registro de parâmetros.
 * @param rrns_size tamanho do vetor retornado.
 *
 * @return retorna um vetor contendo o(s) RRN(s) do(s) registro(s) que cumprir
 * os parâmetros e NULL, caso não seja encontrado nenhum registro.
 */
int *type1_search_parameters_rrn(FILE *fp, reg_t1 *search_parameters, int *rrns_size)
{
    int *rrns = NULL;
    *rrns_size = 0;
    char status;
    fseek(fp, 0, SEEK_SET);
    fread(&status, sizeof(char), 1, fp);

    if (status == '0')
    {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);

    for (long int offset = HEADER_SIZE; offset < file_size; offset += REGISTER_SIZE)
    {
        fseek(fp, offset, SEEK_SET);
        reg_t1 *reg = read_register_from_bin(fp);
        if (verify_reg_t1(reg, search_parameters) == 1)
        {
            *rrns_size = *rrns_size + 1;
            rrns = realloc(rrns, *rrns_size * sizeof(int));
            rrns[*rrns_size - 1] = (offset - HEADER_SIZE) / REGISTER_SIZE;
        }
        free_register(reg);
    }
    return rrns;
}

/**
 * @brief Remove os registros com base em um vetor que armazena os RRNs deles.
 *
 * @param fp ponteiro para o arquivo de dados.
 * @param ind ponteiro para o vetor de índices.
 * @param size_ind tamanho do vetor de índices.
 * @param rrns vetor de RRNs dos registros a serem removidos.
 * @param size_rrn tamanho do vetor de RRNs.
 * @param topo RRN do topo da pilha.
 * @param qnt quantidade de novos registros removidos.
 */
void type1_delete(FILE *fp, index_t1 *ind, int *size_ind, int *rrns, int size_rrn, int *topo, int *qnt)
{
    for (int i = 0; i < size_rrn; i++)
    {
        fseek(fp, HEADER_SIZE + (rrns[i] * REGISTER_SIZE), SEEK_SET);
        fwrite("1", sizeof(char), 1, fp);

        int id;
        fseek(fp, 4, SEEK_CUR);
        fread(&id, sizeof(int), 1, fp);
        if (id != -1)
        {
            int pos = binarySearch_t1(ind, 0, *size_ind, id);
            if (pos != -1)
            {
                for (int j = pos; j < *size_ind - 1; j++)
                {
                    ind[j] = ind[j + 1];
                }
                *size_ind = *size_ind - 1;
            }
        }
        *qnt = *qnt + 1;
        type1_update_stack(fp, topo, rrns[i]);
    }
}

/**
 * @brief Retorna o RRN no campo 'topo'.
 *
 * @param fp ponteiro para o arquivo de dados.
 *
 * @return RRN armazenado no campo 'topo'.
 */
static int type1_get_top(FILE *fp)
{
    int top;
    fseek(fp, 1, SEEK_SET);
    fread(&top, sizeof(int), 1, fp);
    return top;
}

/**
 * @brief Atualiza o topo escrito no cabeçalho do arquivo.
 * 
 * @param fp ponteiro para o arquivo;
 * @param new_top RRN que deve ser colocado no campo 'topo' do cabeçalho.
 */
static void type1_update_top(FILE *fp, int new_top)
{
    fseek(fp, 1, SEEK_SET);
    fwrite(&new_top, sizeof(int), 1, fp);
}

/**
 * @brief Função que lê os parâmetros dos registros a serem removidos da entrada
 * padrão(stdin) e remove eles, caso possível. Em seguida, atualiza o arquivo
 * de dados e de índices.
 *
 * @param data_fp ponteiro para o arquivo de dados.
 * @param index_name nome do arquivo binário.
 */
void type1_delete_from (FILE *data_fp, char *index_name)
{
    int n, topo, size, id, rrns_size, qnt = 0;
    int *rrns;
    FILE *index_fp = fopen(index_name, "rb");
    index_t1 *index = type1_index_disk_to_ram (&size, index_fp);
    topo = type1_get_top(data_fp);

    scanf(" %d", &n);

    for (int i = 0; i < n; i++) {
        rrns = NULL;
        rrns_size = 0;

        reg_t1 *search_parameters = get_search_parameters();

        if(search_parameters->id != -1)
        {
            rrns = type1_search_id (data_fp, index, size, search_parameters, &rrns_size);
        }
        else
        {
            rrns = type1_search_parameters_rrn(data_fp, search_parameters, &rrns_size);
        }
        type1_delete(data_fp, index, &size, rrns, rrns_size, &topo, &qnt);

        if (rrns != NULL) free(rrns);

        free_register(search_parameters);
    }
    fclose(index_fp);

    index_fp = fopen(index_name, "wb");
    type1_index_ram_to_disk(index, size, index_fp);
    update_header_status(index_fp);
    fclose(index_fp);
    free(index);

    type1_update_header(data_fp, qnt, &topo);
    update_header_status(data_fp);
}

/**
 * @brief Lê da entrada padrão os dados de um registro e insere em uma struct
 * register_type1.
 *
 * @return ponteiro para a struct do registro lido (reg_t1 *).
 */
static reg_t1 *read_register_from_stdin()
{
    // Formato da entrada:
    // id1 ano1 qtt1 "sigla1" "cidade1" "marca1" "modelo"

    reg_t1 *reg = malloc(sizeof(reg_t1));

    reg->removed = '0';

    reg->next = -1;

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
        read_until(stdin, ' ');
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
        read_until(stdin, ' ');
        reg->city = NULL;
    }

    if (reg->city) reg->city_namesize = strlen(reg->city);
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
        read_until(stdin, ' ');
        reg->brand = NULL;
    }

    if (reg->brand) reg->brand_namesize = strlen(reg->brand);
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
        read_line(stdin);
        reg->model = NULL;
    }

    if (reg->model) reg->model_namesize = strlen(reg->model);
    else reg->model_namesize = 0;

    return reg;
}

/**
 * @brief Insere ID e RRN de um novo registro no índice em memória primária.
 *
 * @param id ID do registro adicionado;
 * @param rrn RRN do registro adicionado;
 * @param index ponteiro para o índice (RAM);
 * @param index_size quantidade de elementos no índice;
 * @return ponteiro para o índice na RAM (index_t1 *).
 */
static index_t1 *insert_into_index(int id, int rrn, index_t1 *index, int index_size)
{
    index = realloc(index, index_size * sizeof(index_t1));
    index[index_size - 1].id = id;
    index[index_size - 1].rrn = rrn;

    return index;
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
    int index_size, rrn;
    index_t1 *index = type1_index_disk_to_ram(&index_size, index_fp);

    int pop_count = 0;

    for (int i = 0; i < n_registers; i++)
    {
        reg_t1 *reg = read_register_from_stdin();

        int top = type1_get_top(data_fp);
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
            int offset = HEADER_SIZE + (top * REGISTER_SIZE) + 1;
            fseek(data_fp, offset, SEEK_SET);

            int next; // Novo topo
            fread(&next, sizeof(int), 1, data_fp);
            type1_update_top(data_fp, next);

            offset = HEADER_SIZE + (top * REGISTER_SIZE);
            fseek(data_fp, offset, SEEK_SET);
            rrn = top;

            pop_count++;
        }

        int id = reg->id;

        write_register(reg, data_fp); // Escreve e já desaloca reg

        // Marcar o arquivo de índice como inconsistente
        fseek(index_fp, 0, SEEK_SET);
        fwrite("0", sizeof(char), 1, index_fp);

        // Adicionar no índice
        index = insert_into_index(id, rrn, index, ++index_size);
    }

    type1_index_ram_to_disk(index, index_size, index_fp);

    type1_update_nroRegRem(data_fp, (-1) * pop_count);

    // Marcar o arquivo de índice como consistente
    fseek(index_fp, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, index_fp);

    fclose(index_fp);
}

/**
 * @brief Verifica se um registro contendo parâmetros de busca tem o ID como
 * único parâmetro.
 * 
 * @param search_parameters registro contendo parâmetros de busca;
 * @return 1 quando o único parâmetro de busca é o ID e 0 quando não (int). 
 */
static int only_id(reg_t1 *search_parameters)
{
    if (search_parameters->id == -1) return 0;

    if (search_parameters->year != -1) return 0;
    if (search_parameters->qtt  != -1) return 0;
    
    if (search_parameters->state != NULL) return 0;
    if (search_parameters->city  != NULL) return 0;
    if (search_parameters->brand != NULL) return 0;
    if (search_parameters->model != NULL) return 0;

    return 1;   
}

/**
 * @brief Atualiza um registro em um arquivo de dados, modificando os campos
 * especificados.
 * 
 * @param fp ponteiro para o arquivo de dados;
 * @param offset byte offset do registro que deve ser modificado;
 * @param fields_to_update campos que devem ser atualizados.
 * @param index ponteiro para o índice (RAM);
 * @param index_size tamanho do índice.
 */
static void update_register(FILE *fp, long int offset, reg_t1 *fields_to_update, index_t1 *index, int index_size)
{
    fseek(fp, offset, SEEK_SET);
    reg_t1 *reg = read_register_from_bin(fp);

    if (fields_to_update->id != -1)
    {
        int pos = binarySearch_t1(index, 0, index_size - 1, reg->id);
        
        reg->id = fields_to_update->id;
        index[pos].id = reg->id;
    }

    if (fields_to_update->year == -2)
        reg->year = -1;
    else if (fields_to_update->year != -1)
        reg->year = fields_to_update->year;

    if (fields_to_update->qtt == -2)
        reg->qtt = -1;
    if (fields_to_update->qtt != -1)
        reg->qtt = fields_to_update->qtt;

    if (fields_to_update->state != NULL)
        reg->state = fields_to_update->state;

    if (fields_to_update->city != NULL)
    {
        reg->city = fields_to_update->city;
        reg->city_namesize = strlen(reg->city);
    }
    else if (fields_to_update->city_namesize == -2)
    {
        reg->city = NULL;
        reg->city_namesize = 0;
    }

    if (fields_to_update->brand != NULL)
    {
        reg->brand = fields_to_update->brand;
        reg->brand_namesize = strlen(reg->brand);
    }
    else if (fields_to_update->brand_namesize == -2)
    {
        reg->brand = NULL;
        reg->brand_namesize = 0;
    }

    if (fields_to_update->model != NULL)
    {
        reg->model = fields_to_update->model;
        reg->model_namesize = strlen(reg->model);
    }
    else if (fields_to_update->model_namesize == -2)
    {
        reg->model = NULL;
        reg->model_namesize = 0;
    }

    fseek(fp, offset, SEEK_SET);
    write_register(reg, fp);
}

/**
 * @brief Encontra registros que satisfazem os critérios de busca em 'search_parameters'
 * e os atualiza com os campos/valores identificados em 'fields_to_update'.
 * 
 * @param fp ponteiro para o arquivo de dados que deve ser atualizado;
 * @param search_parameters struct com parâmetros de busca;
 * @param fields_to_update campos/valores para atualizar;
 * @param index ponteiro para o índice (RAM);
 * @param index_size tamanho do índice.
 */
static void update_by_search_parameters(FILE *fp, reg_t1 *search_parameters, reg_t1 *fields_to_update, index_t1 *index, int index_size)
{
    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);

    for (long int offset = HEADER_SIZE; offset < file_size; offset += REGISTER_SIZE)
    {
        fseek(fp, offset, SEEK_SET);
        reg_t1 *reg = read_register_from_bin(fp);
        if (verify_reg_t1(reg, search_parameters))
            update_register(fp, offset, fields_to_update, index, index_size);
    }
}

/**
 * @brief Atualiza registros no arquivo de dados segundo especificações lidas
 * da entrada padrão, que identificam quais os registros a serem modificados e
 * quais os campos/valores a serem atualizados.
 * 
 * @param data_fp ponteiro para o arquivo de dados;
 * @param index_fp ponteiro para o arquivo de índice;
 * @param n_registers número de buscas a serem feitas.
 */
void update_registers_type1(FILE *data_fp, FILE *index_fp, int n)
{
    // Marcar arquivo de dados como inconsistente
    fseek(data_fp, 0, SEEK_SET);
    fwrite("0", sizeof(char), 1, data_fp);

    int index_size;
    index_t1 *index = type1_index_disk_to_ram(&index_size, index_fp);

    for (int i = 0; i < n; i++)
    {
        reg_t1 *search_parameters = get_search_parameters();
        reg_t1 *fields_to_update = get_search_parameters();

        if (only_id(search_parameters))
        {
            int pos = binarySearch_t1(index, 0, index_size - 1, search_parameters->id);
            int rrn = index[pos].rrn;
            
            if (rrn != -1)
            {
                long int offset = HEADER_SIZE + (rrn * REGISTER_SIZE);
                update_register(data_fp, offset, fields_to_update, index, index_size);
            }
        }
        else
        {
            update_by_search_parameters(data_fp, search_parameters, fields_to_update, index, index_size);
        }
    }

    // Atualizar índice
    type1_index_ram_to_disk(index, index_size, index_fp);

    // Marcar o arquivo de índice como consistente
    fseek(index_fp, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, index_fp);
    fclose(index_fp);

    // Marcar arquivo de dados como consistente
    fseek(data_fp, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, data_fp);
}
