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
            scanf(" %d ", &value);

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

void insertionSort_t2 (t2_index *index, int size)
{
	int j;
	for(j = 1; j < size; j++)
    {
		int chave = index[j].id;
		int i = j - 1;
		while (i >= 0 && index[i].id > chave){
            index[i + 1] = index[i];
			i--;
		}
		index[i + 1] = index[j];
	}
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
int binarySearch_t2 (t2_index *index, int beg, int end, int value)
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
            return binarySearch_t2(index, mid + 1, end, value);
        }
        else
        {
            return binarySearch_t2(index, beg, mid - 1, value);
        }
    }
    return -1;
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
void type2_index_ram_to_disk (t2_index *index, int size, FILE *fp)
{
    insertionSort_t2(index, size);

    fseek(fp, 0, SEEK_SET);
    fwrite("0", sizeof(char), 1, fp);

    for (int i = 0; i < size; i++)
    {
        fwrite(&index[i].id,     sizeof(int), 1, fp);
        fwrite(&index[i].offset, sizeof(long int), 1, fp);
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

t2_index *type2_index_disk_to_ram (FILE *fp, int *size)
{
    t2_index *index = NULL;
    fseek(fp, 0, SEEK_SET);
    char c = fgetc(fp);

    if (c == '1')
    {
        *size = 0;
        int id;
        long int offset;

        while (fread(&id, sizeof(int), 1, fp) == 1)
        {
            *size = *size + 1;

            index = realloc(index, *size * sizeof(t2_index));
            index[*size - 1].id = id;
            fread(&index[*size - 1].offset, sizeof(long int), 1, fp);
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
FILE *new_type2_index (FILE *data_fp, char *file_name)
{
    FILE *index_fp = fopen(file_name, "wb");
    fwrite("0", sizeof(char), 1, index_fp);

    t2_index *index = NULL;
    int size = 0, tamanho_registro;
    long int offset;

    fseek(data_fp, 190, SEEK_SET); // Move o pointeiro do arquivo para o primeiro registro.
    offset = ftell(data_fp);

    // Verificação de status
    char c = fgetc(data_fp);

    while (c != EOF)
    {
        fread(&tamanho_registro, sizeof(int), 1, data_fp);
        tamanho_registro += 5;  // Adiciona tamanho do campo 'removido' e
                                // 'tamanho do registro

        // Verifica se o registro foi removido
        if (c == '0')
        {
            size++;
            index = realloc(index, size * sizeof(t2_index));

            fseek(data_fp, 8, SEEK_CUR);
            fread(&index[size - 1].id, sizeof(int), 1, data_fp);
            index[size - 1].offset = offset;
            fseek(data_fp, (tamanho_registro - 17), SEEK_CUR);
        }
        else
        {
            fseek(data_fp, tamanho_registro - 5, SEEK_CUR);
        }
        offset = ftell(data_fp);
        c = fgetc(data_fp);
    }

    type2_index_ram_to_disk(index, size, index_fp);

    free(index);

    // Termina de escrever no arquivo binário.
    fseek(index_fp, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, index_fp);

    return index_fp;
}

/**
 * @brief Atualiza o campo 'nroRegRem' no cabeçalho do arquivo de dados
 *
 * @param FILE *input_fp ponteiro para o arquivo de dados
 * @param int qnt quantidade nova de registros removidos
 */
void type2_update_nroRegRem(FILE *data_fp, int qnt)
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
 * @brief Atualiza o cabeçalho do registro de dados binário.
 *
 * @param fp ponteiro para o arquivo de dados binário.
 * @param qnt quantidade de novos registros removidos.
 * @param head offset que será armazenado no campo 'topo'.
 */
void type2_update_header(FILE *fp, int qnt, long int *head)
{
    type2_update_nroRegRem(fp, qnt);
    fseek(fp, 1, SEEK_SET);
    fwrite(head, sizeof(long int), 1, fp);
}

/**
 * @brief Retorna o campo 'topo' do cabeçalho do arquivo de dados
 *
 * @param FILE *data_fp ponteiro para o arquivo de dados
 *
 * @return offset armazenado no campo 'topo'
 */
long int get_list_head(FILE *data_fp)
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
void type2_update_list(FILE *data_fp, long int *head, long int new_offset, int new_size)
{
    long int aux, ant = -1, prox;
    int aux_size;

    aux = *head;

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
        fwrite(head, sizeof(long int), 1, data_fp);
        *head = new_offset;
    }
    else
    {
        fseek(data_fp, new_offset + 5, SEEK_SET);
        fwrite(&aux, sizeof(long int), 1, data_fp);
        fseek(data_fp, ant + 5, SEEK_SET);
        fwrite(&new_offset, sizeof(long int), 1, data_fp);
    }
}

long int *type2_search_id (FILE *data_fp, t2_index *index, int index_size, s_reg_t2 *search_param, int *size)
{
    long int *offsets = NULL;
    *size = 0;
    int position = binarySearch_t2(index, 0, index_size, search_param->id);

    if (position != -1)
    {
        long int offset = index[position].offset;
        fseek(data_fp, offset, SEEK_SET);
        char c = fgetc(data_fp);

        if (c == '0')
        {
            reg_t2 *reg = t2_file_to_struct(data_fp);
            if(verify_reg_t2(reg, search_param) != 0)
            {
                *size = 1;
                offsets = malloc(sizeof(long int));
                offsets[0] = offset;
            }
            free_reg_t2(reg);
        }
    }

    return offsets;
}


/**
 * @brief Retorna um vetor de offsets dos registros que satisfazem os critérios
 * de busca.
 *
 * @param fp ponteiro para o arquivo de dados.
 */
long int *type2_search_parameters_offset(FILE *fp, s_reg_t2 *reg_s, int *size)
{
    long int *offsets = NULL;
    *size = 0;

    char c;
    long int offset;
    int verifier, size_skip;
    reg_t2 *reg;

    fseek(fp, 0, SEEK_SET);
    fread(&c, sizeof(char), 1, fp);

    if (c == '0')
    {
        return NULL;
    }

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
                offsets = realloc(offsets, *size * sizeof(long int));
                offsets[*size - 1] = offset;
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

    return offsets;
}



void type2_delete (FILE *fp, t2_index *ind, int *size_ind, long int *offsets, int size_off, long int *head, int *qnt)
{
    for (int i = 0; i < size_off; i++)
    {
        int size, id;
        fseek(fp, offsets[i], SEEK_SET);
        fwrite("1", sizeof(char), 1, fp);

        fread(&size, sizeof(int), 1, fp);
        fseek(fp, 8, SEEK_CUR);
        fread(&id, sizeof(int), 1, fp);

        if (id != -1)
        {
            int pos = binarySearch_t2(ind, 0, *size_ind, id);
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
        type2_update_list(fp, head, offsets[i], size);
    }
}

/**
 * @brief Retorna o RRN no campo 'topo'.
 *
 * @param fp ponteiro para o arquivo de dados.
 *
 * @return RRN armazenado no campo 'topo'.
 */
long int type2_get_head (FILE *fp)
{
    long int head;
    fseek(fp, 1, SEEK_SET);
    fread(&head, sizeof(long int), 1, fp);
    return head;
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
void type2_delete_from (FILE *data_fp, char *index_name)
{
    int n, index_size, off_size = 0, qnt = 0;
    long int head;
    long int *offsets;

    FILE *index_fp = fopen(index_name, "rb");
    t2_index *index = type2_index_disk_to_ram (index_fp, &index_size);
    head = type2_get_head(data_fp);

    scanf(" %d", &n);
    for (int i = 0; i < n; i++) {
        offsets = NULL;
        off_size = 0;

        s_reg_t2 *reg_search = get_reg_t2_search_parameters();

        if(reg_search->id != -1)
        {
            offsets = type2_search_id (data_fp, index, index_size, reg_search, &off_size);
        }
        else
        {
            offsets = type2_search_parameters_offset(data_fp, reg_search, &off_size);
        }
        type2_delete (data_fp, index, &index_size, offsets, off_size, &head, &qnt);

        if (offsets != NULL) free(offsets);

        free_s_reg_t2(reg_search);
    }
    fclose(index_fp);

    index_fp = fopen(index_name, "wb");
    type2_index_ram_to_disk(index, index_size, index_fp);
    update_header_status(index_fp);
    fclose(index_fp);
    free(index);

    type2_update_header(data_fp, qnt, &head);
    update_header_status(data_fp);
}
