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


// ============================ FUNÇÕES PARA DEBUG =============================


void print_node(node_t *node)
{

}


// ============================ FUNÇÕES DE ESCRITA =============================

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