/**
 * @file   variable.c
 * @author Ana Lívia Ruegger Saldanha (N.USP 8586691)
 * @author André Kenji Hidaka Matsumoto (N. USP 12542689)
 * @brief  SCC0215 - Organização de Arquivos
 *         Trabalho 01
 *
 *         Módulo que trabalha com arquivos Tipo 2 (Arquivo de Dados para
 *         Registros de Tamanho Variável)
 *
 * @date   2022-05-26
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

typedef struct register_type2
{
    char removed;
    int register_size;
    long int next;

    int id;
    int year;
    int qtt;
    char *state;

    int city_namesize;
    char *codC5;
    char *city;

    int brand_namesize;
    char *codC6;
    char *brand;

    int model_namesize;
    char *codC7;
    char *model;
} reg_t2;

typedef struct search_field_register_type2
{
    int id;
    int year;
    int qtt;
    char *state;
    char *city;
    char *brand;
    char *model;
} s_reg_t2;

/**
 * @brief Cria um arquivo binário de dados do Tipo 2 e insere os dados de
 * cabeçalho.
 *
 * @param file_name nome para o arquivo binário que será criado;
 * @return ponteiro para o arquivo criado (FILE *).
 */
FILE *new_type2_file(char *file_name)
{
    FILE *fp = fopen(file_name, "wb");

    long int temp_value = -1; // Argumento para fwrite()

    fwrite("0",                                         sizeof(char), 1, fp); // 0
    fwrite(&temp_value,                                 sizeof(long int), 1, fp); // 1
    fwrite("LISTAGEM DA FROTA DOS VEICULOS NO BRASIL",  sizeof(char), 40, fp); // 9
    fwrite("CODIGO IDENTIFICADOR: ",                    sizeof(char), 22, fp); // 49
    fwrite("ANO DE FABRICACAO: ",                       sizeof(char), 19, fp); // 71
    fwrite("QUANTIDADE DE VEICULOS: ",                  sizeof(char), 24, fp); // 90
    fwrite("ESTADO: ",                                  sizeof(char), 8, fp); // 114
    fwrite("0",                                         sizeof(char), 1, fp); // 122
    fwrite("NOME DA CIDADE: ",                          sizeof(char), 16, fp); // 123
    fwrite("1",                                         sizeof(char), 1, fp); // 139
    fwrite("MARCA DO VEICULO: ",                        sizeof(char), 18, fp); // 140
    fwrite("2",                                         sizeof(char), 1, fp); // 158
    fwrite("MODELO DO VEICULO: ",                       sizeof(char), 19, fp); // 159
    temp_value = 0;
    fwrite(&temp_value,                                 sizeof(long int), 1, fp); // 178
    int temp_value2 = 0;
    fwrite(&temp_value2,                                sizeof(int), 1, fp);  // 186

    return fp;
}

/**
 * @brief Lê de um arquivo .csv os dados de um registro e insere em uma struct
 * register_type2.
 *
 * @param fp ponteiro para o arquivo .csv;
 * @return ponteiro para a struct do registro lido (reg_t2*).
 */
reg_t2 *read_t2_register_from_csv(FILE *fp)
{
    // Alocação do registro
    reg_t2 *reg = malloc(sizeof(reg_t2));

    // Preenchimento do registro...
    reg->removed = '0';
    reg->register_size = 0;

    reg->next = -1;
    reg->register_size += sizeof(reg->next);

    char *id = read_until(fp, ',');
    reg->id = atoi(id);
    reg->register_size += sizeof(reg->id);
    free(id);

    char *ano = read_until(fp, ',');
    if (ano[0] >= '0' && ano[0] <= '9')
        reg->year = atoi(ano);
    else
        reg->year = -1;
    reg->register_size += sizeof(reg->year);
    free(ano);

    char *cidade = read_until(fp, ',');
    if (cidade[0] != '\0')
    {
        reg->city_namesize = strlen(cidade);
        reg->register_size += sizeof(reg->city_namesize);
        reg->city = cidade;
        reg->register_size += reg->city_namesize;
        reg->codC5 = "0";
        reg->register_size += 1;
    }
    // Se não tiver cidade:
    else
    {
        reg->city_namesize = -1;
        free(cidade);
    }

    char *qtt = read_until(fp, ',');
    if (qtt[0] >= '0' && qtt[0] <= '9')
        reg->qtt = atoi(qtt);
    else
        reg->qtt = -1;
    reg->register_size += sizeof(reg->qtt);
    free(qtt);

    char *sigla = read_until(fp, ',');
    if (sigla[0] != '\0')
        reg->state = sigla;
    // Se não tiver estado:
    else
    {
        reg->state = "$$";
        free(sigla);
    }
    reg->register_size += 2;

    char *marca = read_until(fp, ',');
    if (marca[0] != '\0')
    {
        reg->brand_namesize = strlen(marca);
        reg->register_size += sizeof(reg->brand_namesize);
        reg->brand = marca;
        reg->register_size += reg->brand_namesize;
        reg->codC6 = "1";
        reg->register_size += 1;
    }
    // Se não tiver marca:
    else
    {
        reg->brand_namesize = -1;
        free(marca);
    }

    char *modelo = read_line(fp);
    if (modelo[0] != '\0')
    {
        reg->model_namesize = strlen(modelo);
        reg->register_size += sizeof(reg->model_namesize);
        reg->model = modelo;
        reg->register_size += reg->model_namesize;
        reg->codC7 = "2";
        reg->register_size += 1;
    }
    // Se não tiver modelo:
    else
    {
        reg->model_namesize = -1;
        free(modelo);
    }

    return reg;
}

/**
 * @brief Lê e escreve em um arquivo binário (.bin) todos os registros de um
 * arquivo .csv, um registro por vez.
 *
 * @param input arquivo .cvs de entrada;
 * @param output arquivo .bin de saida.
 */
void read_and_write_all_type2(FILE *input, FILE *output)
{
    // Move o pointeiro do arquivo para o primeiro registro.
    fseek(input, 61, SEEK_SET);
    char c; // verificador EOF.

    do
    {
        // Lê dados do .csv e armazena na struct register_type2.
        reg_t2 *reg = read_t2_register_from_csv(input);

        // Escreve os elementos da struct no arquivo binário.
        fwrite(&reg->removed, sizeof(char), 1, output);
        fwrite(&reg->register_size, sizeof(int), 1, output);
        fwrite(&reg->next, sizeof(long int), 1, output);
        fwrite(&reg->id, sizeof(int), 1, output);
        fwrite(&reg->year, sizeof(int), 1, output);
        fwrite(&reg->qtt, sizeof(int), 1, output);
        fwrite(reg->state, sizeof(char), 2, output);
        if (reg->state != "$$")
            free(reg->state);

        // Se tiver cidade:
        if (reg->city_namesize != -1)
        {
            fwrite(&reg->city_namesize, sizeof(int), 1, output);
            fwrite(reg->codC5, sizeof(char), 1, output);
            fwrite(reg->city, sizeof(char), reg->city_namesize, output);
            free(reg->city);
        }

        // Se tiver marca:
        if (reg->brand_namesize != -1)
        {
            fwrite(&reg->brand_namesize, sizeof(int), 1, output);
            fwrite(reg->codC6, sizeof(char), 1, output);
            fwrite(reg->brand, sizeof(char), reg->brand_namesize, output);
            free(reg->brand);
        }

        // Se tiver modelo:
        if (reg->model_namesize != -1)
        {
            fwrite(&reg->model_namesize, sizeof(int), 1, output);
            fwrite(reg->codC7, sizeof(char), 1, output);
            fwrite(reg->model, sizeof(char), reg->model_namesize, output);
            free(reg->model);
        }

        free(reg);

        // Verifica EOF
        c = fgetc(input);
        if (c != EOF)
        {
            fputc(c, input);
            fseek(input, -1, SEEK_CUR);
        }
    } while (c != EOF);

    // Armazena nextByteOffset e escreve ele no arquivo binário.
    long int temp = ftell(output);
    fseek(output, 178, SEEK_SET); // nextByteOffset começa no byte 178.
    fwrite(&temp, sizeof(long int), 1, output);

    // Termina de escrever no arquivo binário.
    fseek(output, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, output);
}

/**
 * @brief Desaloca um struct register_type2 e também, caso tenha, as strings contidas
 * nele.
 *
 * @param reg ponteiro para a struct que deve ser desalocada.
 */
void free_reg_t2(reg_t2 *reg)
{
    if (reg->brand_namesize != -1)
    {
        free(reg->codC6);
        free(reg->brand);
    }

    if (reg->model_namesize != -1)
    {
        free(reg->codC7);
        free(reg->model);
    }

    if (reg->city_namesize != -1)
    {
        free(reg->codC5);
        free(reg->city);
    }

    free(reg->state);
    free(reg);
}

/**
 * @brief Imprime na saída padrão os dados contidos em uma struct register_type2.
 *
 * @param reg ponteiro para a struct que deve ser impressa.
 */
void print_t2_register(reg_t2 *reg)
{
    printf("MARCA DO VEICULO: ");
    if (reg->brand_namesize != -1)
        printf("%s\n", reg->brand);
    else
        printf("NAO PREENCHIDO\n");

    printf("MODELO DO VEICULO: ");
    if (reg->model_namesize != -1)
        printf("%s\n", reg->model);
    else
        printf("NAO PREENCHIDO\n");

    printf("ANO DE FABRICACAO: ");
    if (reg->year != -1)
        printf("%d\n", reg->year);
    else
        printf("NAO PREENCHIDO\n");

    printf("NOME DA CIDADE: ");
    if (reg->city_namesize != -1)
        printf("%s\n", reg->city);
    else
        printf("NAO PREENCHIDO\n");

    printf("QUANTIDADE DE VEICULOS: ");
    if (reg->qtt != -1)
        printf("%d\n", reg->qtt);
    else
        printf("NAO PREENCHIDO\n");

    printf("\n");
}

/**
 * @brief Lê um registro de um arquivo binário e armazena ele em uma struct
 * register_type2.
 *
 * @param fp ponteiro para o arquivo binário;
 * @return reg_t2* ponteiro para a struct produzida.
 */
reg_t2 *t2_file_to_struct(FILE *fp)
{
    // Alocação do registro
    reg_t2 *reg = malloc(sizeof(reg_t2));

    reg->state = calloc(3, sizeof(char));

    // Read data from binary file and store it in struct.
    reg->removed = '0';
    fread(&reg->register_size,  sizeof(int), 1, fp);
    fread(&reg->next,           sizeof(long int), 1, fp);
    fread(&reg->id,             sizeof(int), 1, fp);
    fread(&reg->year,           sizeof(int), 1, fp);
    fread(&reg->qtt,            sizeof(int), 1, fp);
    fread(reg->state,           sizeof(char), 2, fp);

    reg->city_namesize = -1;
    reg->brand_namesize = -1;
    reg->model_namesize = -1;

    // sizeof(next + id + year + qtt + state)
    int current_size = 22;
    while (reg->register_size - current_size != 0)
    {
        int size;
        char *cod = calloc(2, sizeof(char));

        fread(&size, sizeof(int), 1, fp);
        fread(cod, sizeof(char), 1, fp);

        // codC5 - Cidade
        if (strcmp(cod, "0") == 0)
        {
            reg->city_namesize = size;
            reg->codC5 = cod;
            reg->city = calloc((reg->city_namesize + 1), sizeof(char));
            fread(reg->city, sizeof(char), reg->city_namesize, fp);
        }
        // cod6 - Marca
        else if (strcmp(cod, "1") == 0)
        {
            reg->brand_namesize = size;
            reg->codC6 = cod;
            reg->brand = calloc((reg->brand_namesize + 1), sizeof(char));
            fread(reg->brand, sizeof(char), reg->brand_namesize, fp);
        }
        // cod7 - Model
        else
        {
            reg->model_namesize = size;
            reg->codC7 = cod;
            reg->model = calloc((reg->model_namesize + 1), sizeof(char));
            fread(reg->model, sizeof(char), reg->model_namesize, fp);
        }

        // += sizeof(int + byte + variable)
        current_size += 4 + 1 + size;
    }
    return reg;
}

/**
 * @brief Imprime todos os registros presentes em um arquivo binário.
 *
 * @param fp ponteiro para o arquivo binário.
 */
void print_all_from_bin_type2(FILE *fp)
{
    char c;
    reg_t2 *reg = NULL;
    fseek(fp, 190, SEEK_SET); // Move o pointeiro do arquivo para o primeiro registro.

    do
    {
        // Verificação de status
        c = fgetc(fp);
        if (c == '0')
        {
            reg = t2_file_to_struct(fp);
            print_t2_register(reg);
            free_reg_t2(reg);
        }
    } while (c != EOF);

    if(reg == NULL)
    {
        printf("Registro inexistente.\n");
    }
}

/**
 * @brief Lê da entrada padrão os parâmetros de busca de registros e armazena
 * em uma struct search_field_register_type2.
 *
 * @return s_reg_t2* pointeiro para a struct search_field_register_type2.
 */
s_reg_t2 *get_reg_t2_search_parameters()
{
    // Alocação dos registros e preenchimento inicial
    s_reg_t2 *reg_t2_search = malloc(sizeof(s_reg_t2));
    reg_t2_search->id = -1;
    reg_t2_search->year = -1;
    reg_t2_search->qtt = -1;
    reg_t2_search->state = NULL;
    reg_t2_search->city = NULL;
    reg_t2_search->brand = NULL;
    reg_t2_search->model = NULL;

    // Leitura dos parâmetros na stdin
    int n;

    scanf("%d", &n);
    getchar(); // Consome o '\n'

    for (int i = 0; i < n; i++)
    {
        char *field_name = read_word(stdin);
        char *field_content = NULL;

        char c = getchar();
        // Se for uma palavra
        if (c == '"')
        {
            field_content = read_until(stdin, '"');
            getchar(); // Consome o '\n'

            if (strcmp(field_name, "cidade") == 0)
                reg_t2_search->city = field_content;

            else if (strcmp(field_name, "marca") == 0)
                reg_t2_search->brand = field_content;
            else
                reg_t2_search->model = field_content;
        }
        // Se for um número
        else
        {
            ungetc(c, stdin);
            int value;
            scanf(" %d", &value);

            if (strcmp(field_name, "id") == 0)
                reg_t2_search->id = value;

            else if (strcmp(field_name, "ano") == 0)
                reg_t2_search->year = value;

            else
                reg_t2_search->qtt = value;
        }

        free(field_name);
    }

    return reg_t2_search;
}

/**
 * @brief Desaloca uma struct search_field_register_type2 e, caso possua, as
 * strings alocadas dentro dela.
 *
 * @param s_reg_t2 ponteiro para a struct search_field_register_type2 que será
 * desalocada.
 */
void free_s_reg_t2(s_reg_t2 *s_reg_t2)
{
    if (s_reg_t2->state != NULL)
        free(s_reg_t2->state);

    if (s_reg_t2->city != NULL)
        free(s_reg_t2->city);

    if (s_reg_t2->brand != NULL)
        free(s_reg_t2->brand);

    if (s_reg_t2->model != NULL)
        free(s_reg_t2->model);

    free(s_reg_t2);
}

/**
 * @brief Verifica se uma struct register_type2 possui todos os parâmetros
 *  especificados em uma struct search_by_parameters_type2.
 *
 * @param reg ponteiro para a struct register_type2, que será verificada
 * @param s_reg_t2 ponteiro para a struct search_field_register_type2, que possui
 *  os parâmetros de busca
 * @return int inteiro que informa o resultado da busca.
 *  0 - registro não cumpre os parâmetros;
 *  1 - registro cumpre os parâmetros;
 *  2 -registro cumpre os parâmetros, entre eles o id.
 */
int verify_reg_t2(reg_t2 *reg, s_reg_t2 *s_reg_t2)
{
    if (s_reg_t2->year != -1)
        if (s_reg_t2->year != reg->year)
            return 0;

    if (s_reg_t2->qtt != -1)
        if (s_reg_t2->qtt != reg->qtt)
            return 0;

    if (s_reg_t2->state != NULL)
        if (strcmp(s_reg_t2->state, reg->state) != 0)
            return 0;

    if (s_reg_t2->city != NULL)
    {
        if (reg->city_namesize == -1)
            return 0;
        else if (strcmp(s_reg_t2->city, reg->city) != 0)
            return 0;
    }

    if (s_reg_t2->brand != NULL)
    {
        if (reg->brand_namesize == -1)
            return 0;
        else if (strcmp(s_reg_t2->brand, reg->brand) != 0)
            return 0;
    }

    if (s_reg_t2->model != NULL)
    {
        if (reg->model_namesize == -1)
            return 0;
        else if (strcmp(s_reg_t2->model, reg->model) != 0)
            return 0;
    }

    if (s_reg_t2->id != -1)
    {
        if (s_reg_t2->id == reg->id)
            return 2;
        else
            return 0;
    }

    return 1;
}

/**
 * @brief Imprime todas as structs register_type2 de um arquivo binario que cumprem
 *  os parâmetros especificados na entrada padrão.
 *
 * @param fp ponteiro para o arquivo binário.
 */
void search_by_parameters_type2(FILE *fp)
{
    reg_t2 *reg;
    s_reg_t2 *reg_s;
    char c, found = 'n';
    int verifier;
    reg_s = get_reg_t2_search_parameters();
    fseek(fp, 190, SEEK_SET); // Move o pointeiro do arquivo para o primeiro registro.

    do
    {
        // Verificação de status
        c = fgetc(fp);
        if (c == '0')
        {
            // Le o registro do arquivo binario e armazena na struct
            reg = t2_file_to_struct(fp);
            // Verifica os filtros lidos da entrada com os valores da struct
            verifier = verify_reg_t2(reg, reg_s);
            // Se a struct não tiver todos os argumentos lidos
            if (verifier == 0)
                free_reg_t2(reg);
            // Se ela tiver
            else if (verifier == 1)
            {
                found = 'y';
                print_t2_register(reg);
                free_reg_t2(reg);
            }

            // Se for o id
            else if (verifier == 2)
            {
                found = 'y';
                print_t2_register(reg);
                free_reg_t2(reg);
                // Como o id é único, não precisamos percorrer novamente
                break;
            }
        }
    } while (c != EOF);

    if (found == 'n')
    {
        printf("Registro inexistente.\n");
    }

    free_s_reg_t2(reg_s);
}
