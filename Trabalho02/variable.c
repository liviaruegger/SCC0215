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

typedef struct register_type2_index
{
    int id;
    long int offset;
} t2_index;

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
    char c = fgetc(input);
    while (c != EOF)
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
    }

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

    scanf(" %d", &n);
    //printf("n = %d\n", n);
    getchar(); // Consome o '\n'

    for (int i = 0; i < n; i++)
    {
        char *field_name = read_word(stdin);
        //printf("field_name = %s\n", field_name);
        char *field_content = NULL;

        char c = getchar();
        //printf("\nc = %c\n", c);
        // Se for uma palavra
        if (c == '"')
        {
            field_content = read_until(stdin, '"');
            //printf("field_content = %s\n", field_content);
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
            scanf(" %d ", &value);
            //printf("value = %d\n", value);

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

/**
 * @brief Retorna um vetor de offsets dos registros que satisfazem os critérios
 * de busca.
 *
 * @param fp ponteiro para o arquivo de dados.
 */
long int *modded_search_by_parameters_type2(FILE *fp, s_reg_t2 *reg_s, int *size)
{
    char c;
    long int *array = NULL;
    long int offset;
    int verifier, size_skip;
    reg_t2 *reg;
    *size = 0;
    fseek(fp, 190, SEEK_SET); // Move o pointeiro do arquivo para o primeiro registro.

    offset = ftell(fp); // Armazena o offset do registro que será lido
    c = fgetc(fp);
    while (c != EOF)
    {
        if (c == '0')
        {
            // Le o registro do arquivo binario e armazena na struct
            reg = t2_file_to_struct(fp);
            // Verifica os filtros lidos da entrada com os valores da struct
            verifier = verify_reg_t2(reg, reg_s);
            free_reg_t2(reg);

            if (verifier != 0)
            {
                *size = *size + 1;
                array = realloc(array, *size * sizeof(long int));
                array[*size - 1] = offset;
            }
            // Como o id é único, não precisamos percorrer novamente
            if (verifier == 2) break;
        }
        else
        {
            fread(&size_skip, sizeof(int), 1, fp);
            fseek(fp, size_skip, SEEK_CUR);
        }
        offset = ftell(fp);
        c = fgetc(fp);
    }

    return array;
}
/**
 * @brief Retorna o indice do registro de indice que satisfaz o id de busca.
 *
 * @param t2_index **index vetor de structs do tipo t2_index
 * @param int beg indice de inicio do vetor
 * @param int end indice de fim do vetor
 * @param int value valor a ser buscado
 *
 * @return indice do registro com o id buscado, retorna -1 se não for encontrado
 */
int binarySearch (t2_index **index, int beg, int end, int value)
{
    int mid;
    if(end >= beg)
    {
        mid = (beg + end) / 2;
        if(index[mid]->id == value)
        {
            return mid;
        }
        else if(index[mid]->id < value)
        {
            return binarySearch(index, mid + 1, end, value);
        }
        else
        {
            return binarySearch(index, beg, mid - 1, value);
        }
    }
    return -1;
}

/**
 * @brief Escreve o cabeçalho do arquivo de indices.
 *
 * @param char *file_name nome do arquivo de indices que será criado
 *
 * @return ponteiro para o arquivo de indices criado (FILE *)
 */
FILE *new_t2_index_header (char *file_name)
{
    FILE *fp = fopen(file_name, "wb");
    fwrite("0", sizeof(char), 1, fp);

    return fp;
}

/**
 * @brief Retorna o id de um registro de tipo 2.
 *
 * @param FILE *fp ponteiro do registro do arquivo
 *
 * @return id do registro
 */
int read_type2_id (FILE *fp)
{
    int id;
    fseek(fp, 8, SEEK_CUR);
    fread(&id, sizeof(int), 1, fp);
    return id;
}

/**
 * @brief Desaloca o registro de indices na RAM
 *
 * @param t2_index **p ponteiro para o vetor de indices
 * @param int size tamanho do vetor de indices
 */
void free_type2_index (t2_index **p, int size)
{
    for(int i = 0; i < size; i++)
    {
        free(p[i]);
    }
    free(p);
}

/**
 * @brief Adiciona um registro no registro de indices em RAM
 *
 * @param t2_index **index vetor de indices em RAM
 * @param int *size ponteiro para um int que armazena o tamanho do vetor
 * @param int id id a ser inserido
 * @param int off offset a ser inserido
 *
 * @return ponteiro para o vetor de indices atualizado
 */

t2_index **add_t2_index_entry (t2_index **index, int *size, int id, long int off)
{
    *size = *size + 1;

    t2_index *entry = malloc(sizeof(t2_index));
    entry->id = id;
    entry->offset = off;

    index = realloc(index, *size * sizeof(t2_index*));
    index[*size - 1] = entry;

    return index;
}

/**
 * @brief Lê um arquivo de dados e cria o arquivo de indices em RAM
 *
 * @param FILE *fp ponteiro para o arquivo a ser lido
 * @param int *size ponteiro para um inteiro que armazena o tamanho do vetor
 *
 * @return ponteiro para o vetor de indices criado
 */
t2_index **create_index_ram (FILE *fp, int *size)
{
    t2_index **index = NULL;
    int id, tamanho_registro;
    long int offset;
    *size = 0;

    // Verificação de status
    char c = fgetc(fp);

    while (c != EOF)
    {
        fread(&tamanho_registro, sizeof(int), 1, fp);
        tamanho_registro += 5;  // Adiciona tamanho do campo 'removido' e
                                // 'tamanho do registro

        // Verifica se o registro foi removido
        if (c == '0')
        {
            offset = ftell(fp) - 5; // -5, por causa das leituras
            id = read_type2_id(fp);
            index = add_t2_index_entry(index, size, id, offset);
            fseek(fp, (tamanho_registro - 17), SEEK_CUR);
        }
        else
        {
            fseek(fp, tamanho_registro - 5, SEEK_CUR);
        }
        c = fgetc(fp);
    }

    return index;
}

void insertionSort (t2_index **rp, int size)
{
	int j;
	for(j = 1; j < size; j++)
    {
		int chave = rp[j]->id;
		int i = j - 1;
		while (i >= 0 && rp[i]->id > chave){
            rp[i + 1] = rp[i];
			i--;
		}
		rp[i + 1] = rp[j];
	}
}

/**
 * @brief Lê o vetor de registros de indices na RAM e armazena em disco
 *
 * @param t2_index **rp ponteiro para o vetor de indices
 * @param int index_size tamanho do vetor de indices
 * @param FILE *fp arquivo em disco que sera escrito
 *
 * @return ponteiro para o arquivo de indices criado (FILE *)
 */
void t2_index_ram_to_disk (t2_index **rp, int index_size, FILE *fp)
{
    fseek(fp, 0, SEEK_SET);
    fwrite("0", sizeof(char), 1, fp);

    for (int i = 0; i < index_size; i++)
    {
        fwrite(&rp[i]->id, sizeof(int), 1, fp);
        fwrite(&rp[i]->offset, sizeof(long int), 1, fp);
    }
}

/**
 * @brief Lê o arquivo de indices e armazena em RAM
 *
 * @param int *index_size ponteiro para o inteiro que armazena o tamanho do vetor
 * @param FILE *fp ponteiro para o arquivo que será lido
 *
 * @return ponteiro para o vetor criado
 */

t2_index **t2_index_disk_to_ram (int *index_size, FILE *fp)
{
    t2_index **index = NULL;
    fseek(fp, 0, SEEK_SET);
    char c = fgetc(fp);

    if (c == '1')
    {
        *index_size = 0;
        int id;
        long int offset;

        while (fread(&id, sizeof(int), 1, fp) == 1)
        {
            fread(&offset, sizeof(long int), 1, fp);
            index = add_t2_index_entry(index, index_size, id, offset);
        }
    }
    return index;
}

/**
 * @brief Cria um arquivo de indices a partir do arquivo de dados
 *
 * @param FILE *data_fp ponteiro para o arquivo de dados
 * @param char *file_name nome do arquivo de indices
 *
 * @return ponteiro para o arquivo de indices criado (FILE *)
 */
FILE *new_t2_index_file (FILE *data_fp, char *file_name)
{
    FILE *index_fp = new_t2_index_header(file_name);
    t2_index **index = NULL;
    int index_size = 0;

    fseek(data_fp, 190, SEEK_SET); // Move o pointeiro do arquivo para o primeiro registro.

    index = create_index_ram(data_fp, &index_size);
    insertionSort(index, index_size);

    t2_index_ram_to_disk(index, index_size, index_fp);

    free_type2_index(index, index_size);

    // Termina de escrever no arquivo binário.
    fseek(index_fp, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, index_fp);

    return index_fp;
}

/**
 * @brief Retorna o campo 'topo' do cabeçalho do arquivo de dados
 *
 * @param FILE *data_fp ponteiro para o arquivo de dados
 *
 * @return offset armazenado no campo 'topo'
 */
long int get_list_start(FILE *data_fp)
{
    long int offset;
    fseek(data_fp, 1, SEEK_SET);
    fread(&offset, sizeof(long int), 1, data_fp);
    return offset;
}

/**
 * @brief Insere de forma decrescente um novo offset no arquivo de dados
 *
 * @param FILE *data_fp ponteiro para o arquivo de dados
 * @param long int new_offset novo offset a ser inserido
 * @param int new_size tamanho do registro que foi removido
 */
void update_list_start(FILE *data_fp, long int new_offset, int new_size)
{
    long int aux, ant = -1, prox, topo;
    int aux_size;

    fseek(data_fp, 1, SEEK_SET); // vai para o campo 'topo'
    fread(&aux, sizeof(long int), 1, data_fp);
    topo = aux;

    if(aux != -1)
    {
        fseek(data_fp, aux + 1, SEEK_SET);
        fread(&aux_size, sizeof(int), 1, data_fp);
        fread(&prox, sizeof(long int), 1, data_fp);
    }

    while(aux != -1 && new_size < aux_size)
    {
        ant = aux;
        aux = prox;

        if(aux != -1)
        {
            fseek(data_fp, aux + 1, SEEK_SET);
            fread(&aux_size, sizeof(int), 1, data_fp);
            fread(&prox, sizeof(long int), 1, data_fp);
        }
    }

    // Lista vazia / Inserir no começo
    if (ant == -1)
    {
        fseek(data_fp, new_offset + 5, SEEK_SET);
        fwrite(&topo, sizeof(long int), 1, data_fp);
        fseek(data_fp, 1, SEEK_SET);
        fwrite(&new_offset, sizeof(long int), 1, data_fp);
    }
    else
    {
        fseek(data_fp, new_offset + 5, SEEK_SET);
        fwrite(&aux, sizeof(long int), 1, data_fp);
        fseek(data_fp, ant + 5, SEEK_SET);
        fwrite(&new_offset, sizeof(long int), 1, data_fp);
    }
}


/**
 * @brief Marca um registro como removido
 *
 * @param FILE *data_fp ponteiro para o arquivo de dados
 * @param long int offset offset do arquivo a ser removido
 */
void type2_mark_removed(FILE *data_fp, long int offset)
{
    int size;

    if (ftell(data_fp) != offset)
    {
        fseek(data_fp, offset, SEEK_SET);
    }
    fwrite("1", sizeof(char), 1, data_fp);
    fread(&size, sizeof(int), 1, data_fp);

    update_list_start(data_fp, offset, size);

}


/**
 * @brief Lê os parâmetros de busca da entrada padrão(stdin), busca o registro
 * e remove ele, caso possível.
 *
 * @param FILE *data_fp ponteiro para o arquivo de dados
 * @param FILE *index_fp ponteiro para o arquivo de indices
 * @param int *index_size ponteiro para o inteiro que armazena o tamanho do
 * registro de indices
 * @param t2_index **index ponteiro para o vetor de indices em RAM
 * @param int *qnt quantidade de registros removidos
 */
void type2_deletion(FILE *data_fp, FILE *index_fp, int *index_size, t2_index **index, int *qnt)
{
    s_reg_t2 *reg_search = get_reg_t2_search_parameters();
    int position;

    // Se tem id na busca
    if(reg_search->id != -1)
    {
        position = binarySearch(index, 0, *index_size - 1, reg_search->id);
        if (position != -1)
        {
            fseek(data_fp, index[position]->offset, SEEK_SET);

            // Verificação de status
            char c = fgetc(data_fp);
            if (c == '0')
            {
                reg_t2 *reg = t2_file_to_struct(data_fp);

                if(verify_reg_t2(reg, reg_search) != 0)
                {
                    //printf("passou\n\n");
                    type2_mark_removed(data_fp, index[position]->offset);
                    free(index[position]);
                    for(int j = position; j < *index_size - 1; j++)
                    {
                        index[j] = index[j + 1];
                    }
                    *index_size = *index_size - 1;
                    *qnt = *qnt + 1;
                }
                free_reg_t2(reg);
            }
        }
    }
    else
    {
        int f3_array_size;
        long int *array = modded_search_by_parameters_type2(data_fp, reg_search, &f3_array_size);

        for(int i = 0; i < f3_array_size; i++)
        {
            int id;
            type2_mark_removed(data_fp, array[i]);
            fseek(data_fp, array[i] + 13, SEEK_SET);
            fread(&id, sizeof(int), 1, data_fp);
            if(id != -1)
            {
                position = binarySearch(index, 0, *index_size - 1, id);
                if (position != -1)
                {
                    free(index[position]);
                    for(int j = position; j < *index_size - 1; j++)
                    {
                        index[j] = index[j + 1];
                    }
                    *index_size = *index_size - 1;
                    *qnt = *qnt + 1;
                }
            }
        }
        free(array);
    }
    free_s_reg_t2(reg_search);

}

/**
 * @brief Reescreve o arquivo de indices
 *
 * @param t2_index **index ponteiro para o vetor de indices
 * @param int index_size tamanho do registro de indices
 * @param FILE *index_fp ponteiro para o arquivo de indices
 */
void rewrite_t2_index(t2_index **index, int index_size, FILE *index_fp)
{
    insertionSort(index, index_size - 1);
    t2_index_ram_to_disk(index, index_size, index_fp);
}

/**
 * @brief Printa a fila de offsets do arquivo de dados
 * Obs: só para debugar
 */
void fila(FILE *fp)
{
    long int aux, ant = -1, prox, topo;
    int aux_size;

    fseek(fp, 1, SEEK_SET); // vai para o campo 'topo'
    fread(&aux, sizeof(long int), 1, fp);
    if(aux != -1)
    {
        fseek(fp, aux + 1, SEEK_SET);
        fread(&aux_size, sizeof(int), 1, fp);
        fread(&prox, sizeof(long int), 1, fp);
    }

    topo = aux;
    printf("topo = %ld; aux_size = %d\n", topo, aux_size);

    while(aux != -1)
    {
        ant = aux;
        aux = prox;

        if(aux != -1)
        {
            fseek(fp, aux + 1, SEEK_SET);
            fread(&aux_size, sizeof(int), 1, fp);
            fread(&prox, sizeof(long int), 1, fp);
            printf("aux = %ld; aux_size = %d\n", aux, aux_size);
        }
    }
}

/**
 * @brief Atualiza o campo 'status' no cabeçalho para indicar término de escrita
 *
 * @param FILE *fp ponteiro para o arquivo
 */
void update_header_status(FILE *fp)
{
    fseek(fp, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, fp);
}

/**
 * @brief Atualiza o campo 'nroRegRem' no cabeçalho do arquivo de dados
 *
 * @param FILE *input_fp ponteiro para o arquivo de dados
 * @param int qnt quantidade nova de registros removidos
 */
void t2_update_nroRegRem(FILE *data_fp, int qnt)
{
    int temp;
    // Lê o nroRegRem do cabeçalho
    fseek(data_fp, 186, SEEK_SET);
    fread(&temp, sizeof(int), 1, data_fp);
    // Atualiza nroRegRem
    qnt = qnt + temp;
    fseek(data_fp, 186, SEEK_SET);
    fwrite(&qnt, sizeof(int), 1, data_fp);
}

/**
 * @brief Lê da entrada padrão(stdin) a quantidade de registros a serem removi-
 * dos e em seguida remove eles, se possível. Depois disso, reescreve o arquivo
 * de índice
 *
 * @param FILE *input_fp ponteiro para o arquivo de dados
 * @param FILE *index_fp ponteiro para o arquivo de indices
 * @param char *index_file nome do arquivo de indices
 */
void funct6(FILE *input_fp, FILE *index_fp, char *index_file)
{
    int n, index_size, qnt = 0;
    scanf(" %d", &n);

    t2_index **index = t2_index_disk_to_ram (&index_size, index_fp);

    for(int i = 0; i < n; i++)
    {
        type2_deletion(input_fp, index_fp, &index_size, index, &qnt);
    }
    fclose(index_fp);

    index_fp = fopen(index_file, "wb");
    rewrite_t2_index(index, index_size, index_fp);
    update_header_status(index_fp);
    fclose(index_fp);

    t2_update_nroRegRem(input_fp, qnt);
    update_header_status(input_fp);

    free_type2_index(index, index_size);
}

/**
 * @brief Remove as aspas de uma string
 *
 * @param char *s string que sera alterada
 */
void remove_quotes(char *s)
{
    int i = 0;
    if(s[0] == '"')
    {
        do
        {
            s[i] = s[i + 1];
            i++;
        } while(s[i] != '"');
        s[i - 1] = '\0';
    }
}

/**
 * @brief Escreve o cabeçalho do arquivo de indices.
 *
 * @param char *file_name nome do arquivo de indices que será criado
 *
 * @return ponteiro para o arquivo de indices criado (FILE *)
 */
 reg_t2 *read_t2_register_from_stdin()
 {
     // Alocação do registro
     reg_t2 *reg = malloc(sizeof(reg_t2));

     // Preenchimento do registro...
     reg->removed = '0';
     reg->register_size = 0;

     reg->next = -1;
     reg->register_size += sizeof(reg->next);

     char *id = read_until(stdin, ' ');
     reg->id = atoi(id);
     reg->register_size += sizeof(reg->id);
     free(id);

     char *ano = read_until(stdin, ' ');
     if (ano[0] >= '0' && ano[0] <= '9')
         reg->year = atoi(ano);
     else
         reg->year = -1;
     reg->register_size += sizeof(reg->year);
     free(ano);

     char *qtt = read_until(stdin, ' ');
     if (qtt[0] >= '0' && qtt[0] <= '9')
         reg->qtt = atoi(qtt);
     else
         reg->qtt = -1;
     reg->register_size += sizeof(reg->qtt);
     free(qtt);

     char *sigla = read_until(stdin, ' ');
     if (strcmp(sigla, "NULO") != 0)
     {
         remove_quotes(sigla);
         reg->state = sigla;
     }

     // Se não tiver estado:
     else
     {
         reg->state = "$$";
         free(sigla);
     }
     reg->register_size += 2;

     char *cidade = read_until(stdin, ' ');
     if (strcmp(cidade, "NULO") != 0)
     {
         remove_quotes(cidade);
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

     char *marca = read_until(stdin, ' ');
     if (strcmp(marca, "NULO") != 0)
     {
         remove_quotes(marca);
         printf("marca = %s\n", marca);
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

     char *modelo = read_line(stdin);
     if (strcmp(modelo, "NULO") != 0)
     {
         remove_quotes(modelo);
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



void funct7(FILE *input_fp, FILE *index_fp, char *index_file)
{
    reg_t2 *reg = read_t2_register_from_stdin();
    print_t2_register(reg);
}
