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
#include "utils.h"

#define INDEX_HEADER_SIZE_T1 45
#define INDEX_HEADER_SIZE_T2 57

#define NODE_SIZE_T1 45
#define NODE_SIZE_T2 57

typedef union reference
{
    int rrn; // Usado no tipo1
    long int offset; // Usado no tipo2
}   ref_t;

typedef struct key_ref
{
    int id;
    ref_t ref;
}   key_t;

typedef struct node
{
    char type;
    int n_keys;

    key_t keys[3];
    int children[4];
}   node_t;

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
            fread(&node->keys[i].ref.offset, sizeof(long int), 1, fp);
    }

    for (int i = 0; i < 4; i++)
        fread(&node->children[i], sizeof(int), 1, fp);

    return node;
}
