/**
 * @file   btree.c
 * @author Ana Lívia Ruegger Saldanha (N.USP 8586691)
 * @author André Kenji Hidaka Matsumoto (N. USP 12542689)
 * @brief  SCC0215 - Organização de Arquivos
 *         Trabalho 03
 *
 *         Módulo para manipulação de arquivos de índice árvore-B
 *
 * @date   2022-07-20
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#define INDEX_HEADER_SIZE_T1 45
#define INDEX_HEADER_SIZE_T2 57

#define NODE_SIZE_T1 45
#define NODE_SIZE_T2 57

typedef union reference
{
    int rrn; // Usado no tipo1
    long offset; // Usado no tipo2
}   ref_t;

typedef struct key_ref
{
    int id;
    ref_t ref;
}   key_ref_t;

typedef struct node
{
    char type;
    int n_keys;

    key_ref_t keys[3];
    int children[4];
}   node_t;

typedef struct header
{
    char status;
    int root;
    int next_rrn;
    int n_nodes;
}   header_t;

// ============================ FUNÇÕES AUXILIARES =============================

/**
 * @brief Função auxiliar que lê de um arquivo binário de índice árvore-B
 * o RRN do nó raiz.
 *
 * @param fp ponteiro para o arquivo de índice árvore-B;
 * @return RRN do nó raiz (int).
 */
static int get_root(FILE *fp)
{
    int root;
    fseek(fp, 1, SEEK_SET);
    fread(&root, sizeof(int), 1, fp);

    return root;
}

/**
 * @brief Adiciona uma nova chave em um nó de árvore-B.
 *
 * @param node ponteiro para o nó de índice árvore-B;
 * @param key chave a ser inserida;
 */
void update_node(node_t *node, key_ref_t *key)
{
    // Procura o índice em que a chave sera inserida.
    int i = 0;
    while(node->keys[i].id < key->id && i != 2)
        i++;

    // Move as chaves necessárias para a direita.
    for (int j = 2; j > i; j--)
    {
        node->keys[j].id  = node->keys[j - 1].id;
        node->keys[j].ref = node->keys[j - 1].ref;
    }

    node->keys[i].id  = key->id;
    node->keys[i].ref = key->ref;

    node->n_keys = node->n_keys + 1;
}

// ============================ FUNÇÕES PARA DEBUG =============================


void print_node(node_t *node)
{
    printf("Type: %c / Number of keys = %d\n", node->type, node->n_keys);

    printf("   ");
    for (int i = 0; i < 3; i++)
    {
        printf("|%4d", node->keys[i].id);
    }
    printf("|\n");

    for (int i = 0; i < 4; i++)
    {
        printf("%4d ", node->children[i]);
    }
    printf("\n");
}


// ============================ FUNÇÕES DE LEITURA =============================

/**
 * @brief Lê o cabeçalho de árvore-B de um arquivo binário de índice.
 *
 * @param fp ponteiro para o arquivo de índice árvore-B;
 * @return ponteiro para o cabeçalho lido (header_t *).
 */
header_t *read_header(FILE *fp)
{
    fseek(fp, 0, SEEK_SET);

    header_t *header = (header_t *)malloc(sizeof(header_t));

    fread(&header->status,   sizeof(char), 1, fp);
    fread(&header->root,     sizeof(int),  1, fp);
    fread(&header->next_rrn, sizeof(int),  1, fp);
    fread(&header->n_nodes,  sizeof(int),  1, fp);

    return header;
}


// ============================ FUNÇÕES DE ESCRITA =============================

/**
 * @brief Escreve o cabeçalho de árvore-B em um arquivo binário de índice.
 *
 * @param fp ponteiro para o arquivo de índice árvore-B;
 * @param type tipo de arquivo (1 ou 2);
 */
void write_header(FILE *fp, int type)
{
    fwrite("0", sizeof(char), 1, fp);

    int temp = -1;
    fwrite(&temp, sizeof(int), 1, fp);

    temp = 0;
    fwrite(&temp, sizeof(int), 1, fp);
    fwrite(&temp, sizeof(int), 1, fp);

    int bytes_left;
    if (type == 1) bytes_left = INDEX_HEADER_SIZE_T1 - ftell(fp);
    else bytes_left = INDEX_HEADER_SIZE_T2 - ftell(fp);

    while (bytes_left != 0)
    {
        fwrite("$", sizeof(char), 1, fp);
        bytes_left--;
    }
}

/**
 * @brief Escreve um nó de árvore-B em um arquivo binário de índice.
 *
 * @param fp ponteiro para o arquivo de índice árvore-B;
 * @param node ponteiro para o nó a ser escrito;
 * @param type tipo de arquivo (1 ou 2);
 */
void write_node(FILE *fp, node_t *node, int type)
{
    fwrite(&node->type,   sizeof(char), 1, fp);
    fwrite(&node->n_keys, sizeof(int), 1, fp);

    for (int i = 0; i < 3; i++)
    {
        fwrite(&node->keys[i].id, sizeof(int), 1, fp);

        if (type == 1)
            fwrite(&node->keys[i].ref.rrn, sizeof(int), 1, fp);
        else if (type == 2)
            fwrite(&node->keys[i].ref.offset, sizeof(long), 1, fp);
    }

    for (int i = 0; i < 4; i++)
        fwrite(&node->children[i], sizeof(int), 1, fp);
}


void write_index(FILE *fp, int type)
{
    write_header(fp, type);

    /* code */

    update_header_status(fp, '1');
}


// ============================ FUNÇÕES PARA BUSCA =============================

/**
 * @brief Lê um nó de árvore-B de um arquivo binário de índice.
 *
 * @param fp ponteiro para o arquivo de índice árvore-B;
 * @param type tipo de arquivo (1 ou 2);
 * @return ponteiro para o nó lido (node_t *).
 */
static node_t *read_node(FILE *fp, int type)
{
    node_t *node = (node_t *)malloc(sizeof(node_t));

    fread(&node->type,   sizeof(char), 1, fp);
    fread(&node->n_keys, sizeof(int),  1, fp);

    for (int i = 0; i < 3; i++)
    {
        fread(&node->keys[i].id, sizeof(int), 1, fp);

        if (type == 1)
            fread(&node->keys[i].ref.rrn, sizeof(int), 1, fp);
        else if (type == 2)
            fread(&node->keys[i].ref.offset, sizeof(long), 1, fp);
    }

    for (int i = 0; i < 4; i++)
        fread(&node->children[i], sizeof(int), 1, fp);

    return node;
}

/**
 * @brief Função auxiliar recursiva que realiza a busca no arquivo de índice
 * árvore-B.
 *
 * @param fp ponteiro para o arquivo de índice árvore-B;
 * @param type tipo de arquivo (1 ou 2);
 * @param rrn RRN da página sendo buscada na árvore-B;
 * @param key chave para busca;
 * @return valor encontrado (referência do registro no arquivo de dados),
 * ou -1 caso a chave não esteja presente na árvore-B (long).
 */
static long _search(FILE *fp, int type, int rrn, int key)
{
    if (rrn == -1) return -1;

    int node_size = (type == 1) ? NODE_SIZE_T1 : NODE_SIZE_T2;
    fseek(fp, (rrn + 1) * node_size, SEEK_SET);
    node_t *page = read_node(fp, type);

    long found_ref = -1, pos = 0;
    for (int i = 0; i < page->n_keys; i++)
    {
        if (key < page->keys[i].id)
            break;
        else if (key == page->keys[i].id)
            found_ref = (type == 1) ? (long)page->keys[i].ref.rrn : page->keys[i].ref.offset;
        else
            pos = i + 1;
    }

    if (found_ref == -1) found_ref = _search(fp, type, page->children[pos], key);

    free(page);

    return found_ref;
}

/**
 * @brief Busca um id no arquivo de índice árvore-B.
 *
 * @param fp ponteiro para o arquivo de índice árvore-B;
 * @param type tipo de arquivo (1 ou 2);
 * @param id id (chave) para busca;
 * @return valor encontrado (referência do registro no arquivo de dados),
 * ou -1 caso a chave não esteja presente na árvore-B (long).
 */
long search(FILE *fp, int type, int id)
{
    int root = get_root(fp);

    return _search(fp, type, root, id);
}

// ============================ FUNÇÕES DE INSERÇÃO ============================

char insert(FILE *fp, int type, int cur_rrn,
    key_ref_t *key, key_ref_t *promo_key, int promo_r_child)
{
    char return_value = '0';

    // PROMOTION = 'p'
    // NO PROMOTION = 'n'
    // ERROR = 'e'

    // Gui usou alguma coisa com o tipo de nó
    if (cur_rrn == -1)
    {
        promo_key = key;

    }

    /* Busca */
    node_t *page = NULL;

    if (return_value == 'n' || return_value == 'e')
        return return_value;
    else if (page->n_keys != 3)
    {
        update_node(page, key);
        //fseek(X, X, X);
        write_node(fp, page, type);
        return 'n';
    }
}
