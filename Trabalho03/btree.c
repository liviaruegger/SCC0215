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

#define ROOT     '0'
#define INTERNAL '1'
#define LEAF     '2'

#define MAX_KEYS 3


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

typedef struct split_node
{
    char type;
    int n_keys;

    key_ref_t keys[4];
    int children[5];
}   split_node_t;


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
 * @brief Adiciona uma nova chave em uma struct nó de árvore-B 
 * (em memória primária).
 *
 * @param node ponteiro para o nó de índice árvore-B;
 * @param key chave a ser inserida.
 */
static void update_node(node_t *node, key_ref_t *key, int rrn)
{
    // Procura o índice em que a chave sera inserida.
    int i = 0;
    while(i != 2 && node->keys[i].id != -1 && node->keys[i].id < key->id)
        i++;

    // Move as chaves necessárias para a direita.
    for (int j = 2; j > i; j--)
    {
        node->keys[j].id  = node->keys[j - 1].id;
        node->keys[j].ref = node->keys[j - 1].ref;
        node->children[j + 1] = node->children[j];
    }

    node->keys[i].id  = key->id;
    node->keys[i].ref = key->ref;
    node->children[i + 1] = rrn;

    node->n_keys = node->n_keys + 1;
}

/**
 * @brief "Reseta" uma struct nó de árvore-B, retirando todas as informações
 * anteriormente contidas nela.
 * 
 * @param node ponteiro para o nó que deve ser resetado;
 * @param type tipo de arquivo (1 ou 2).
 */
static void reset_node(node_t *node, int type)
{
    for (int i = 0; i < 3; i++)
    {
        node->keys[i].id  = -1;
        if (type == 1) node->keys[i].ref.rrn = -1;
        else node->keys[i].ref.offset = -1;
        node->children[i] = -1;
    }

    node->children[3] = -1;
    node->n_keys = 0;
}

/**
 * @brief Copia as informações (chaves e filhos) de um nó de árvore-B para um nó
 * especial temporário, com uma chave a mais, que será dividido na função split.
 * 
 * @param node ponteiro para o nó que será copiado;
 * @param type tipo de arquivo (1 ou 2);
 * @return nó temporário que será dividido na função split (split_node_t *). 
 */
static split_node_t *copy_node_into_split_node(node_t *node, int type)
{
    split_node_t *split_node = (split_node_t *)malloc(sizeof(split_node_t));

    split_node->type = node->type;

    for (int i = 0; i < 3; i++)
    {
        split_node->keys[i].id = node->keys[i].id;

        if (type == 1)
            split_node->keys[i].ref.offset = node->keys[i].ref.offset;
        else if (type == 2)
            split_node->keys[i].ref.offset = node->keys[i].ref.offset;

        split_node->children[i] = node->children[i];
    }
    split_node->children[3] = node->children[3];

    split_node->keys[3].id = -1;
    if (type == 1)
        split_node->keys[3].ref.rrn = -1;
    else if (type == 2)
        split_node->keys[3].ref.offset = -1;
    split_node->children[4] = -1;

    return split_node;
}

/**
 * @brief Insere uma nova chave em um 'split node' (um nó especial temporário,
 * contendo uma chave a mais, que será dividido na função split).
 * 
 * @param node ponteiro para um nó temporário (split node);
 * @param key chave que deve ser inserida;
 * @param rrn referência do nó filho à direita da chave.
 */
static void insert_key_into_split_node(split_node_t *node, key_ref_t key, int rrn)
{
    // Procura o índice em que a chave sera inserida.
    int i = 0;
    while (node->keys[i].id < key.id && i < MAX_KEYS)
        i++;

    // Move as chaves necessárias para a direita.
    for (int j = MAX_KEYS; j > i; j--)
    {
        node->keys[j].id  = node->keys[j - 1].id;
        node->keys[j].ref = node->keys[j - 1].ref;
        node->children[j + 1] = node->children[j];
    }

    node->keys[i].id  = key.id;
    node->keys[i].ref = key.ref;
    node->children[i + 1] = rrn;
}


// ============================ FUNÇÕES DE LEITURA =============================

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


// ============================ FUNÇÕES DE ESCRITA =============================

/**
 * @brief Escreve o cabeçalho de árvore-B em um arquivo binário de índice.
 *
 * @param fp ponteiro para o arquivo de índice árvore-B;
 * @param type tipo de arquivo (1 ou 2).
 */
void write_header(FILE *fp, int type)
{
    fseek(fp, 0, SEEK_SET);

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
 * @param type tipo de arquivo (1 ou 2).
 */
static void write_node(FILE *fp, node_t *node, int type)
{
    fwrite(&node->type,   sizeof(char), 1, fp);
    fwrite(&node->n_keys, sizeof(int),  1, fp);

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


// ============================ FUNÇÕES PARA BUSCA =============================

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

/**
 * @brief Função Split: este algoritmo faz o tratamento do overflow causado
 * pela inserção de uma chave na árvore-B, retornando a chave promovida.
 * 
 * @param key nova chave a ser inserida;
 * @param i_rrn RRN do filho à direita da nova chave a ser inserida;
 * @param page página de disco corrente;
 * @param promo_right_child filho à direita da chave promovida (ponteiro);
 * @param new_page nova página de disco;
 * @param fp ponteiro para o arquivo binário de índice árvore-B;
 * @param type tipo de arquivo (1 ou 2);
 * @return chave promovida (key_ref_t). 
 */
key_ref_t _split(key_ref_t key, int i_rrn, node_t *page, int *promo_right_child,
                 node_t *new_page, FILE *fp, int type)
{
    // Copia as chaves e filhos para a struct.
    split_node_t *split_node = copy_node_into_split_node(page, type);

    // Insere e ordena a nova chave e o novo rrn.
    insert_key_into_split_node(split_node, key, i_rrn);

    // promo_right_child = RRN of NEW_PAGE(prox_RRN)
    fseek(fp, 5, SEEK_SET);
    fread(promo_right_child, sizeof(int), 1, fp);

    // Atualiza o cabeçalho
    int n_nodes;
    int next_rrn = *promo_right_child + 1;
    fread(&n_nodes, sizeof(int), 1, fp);
    n_nodes++;

    fseek(fp, 5, SEEK_SET);

    fwrite(&next_rrn, sizeof(int), 1, fp);
    fwrite(&n_nodes,  sizeof(int), 1, fp);

    if (split_node->children[0] == -1)
    {
        page->type = LEAF;
        new_page->type = LEAF;
    }
    else
    {
        page->type = INTERNAL;
        new_page->type = INTERNAL;
    }

    // Copia as chaves e filhos à esquerda da chave promovida para PAGE
    reset_node(page, type);
    for (int i = 0; i < MAX_KEYS - 1; i++)
        page->keys[i] = split_node->keys[i];
    for (int i = 0; i < MAX_KEYS; i++)
        page->children[i] = split_node->children[i];
    page->n_keys = 2;

    // Copia chave e filhos à direita da chave promovida para NEW_PAGE
    reset_node(new_page, type);
    new_page->keys[0] = split_node->keys[MAX_KEYS]; // Copia chave
    for (int i = 0; i < MAX_KEYS - 1; i++) // Copia filhos
        new_page->children[i] = split_node->children[i + MAX_KEYS];
    new_page->n_keys = 1;

    // Armazena para poder desalocar split_node
    key_ref_t middle = split_node->keys[2];

    free(split_node);

    return middle;
}

/**
 * @brief Função Split: faz o tratamento do overflow causado pela inserção de
 * uma nova chave e escreve os nós modificados do árquivo de índice árvore-B.
 * 
 * @param key nova chave a ser inserida;
 * @param i_rrn RRN do filho à direita da nova chave a ser inserida;
 * @param page página de disco corrente;
 * @param page_rrn RRN da página no arquivo;
 * @param promo_right_child filho à direita da chave promovida (ponteiro);
 * @param fp ponteiro para o arquivo binário de índice árvore-B;
 * @param type tipo de arquivo (1 ou 2);
 * @return chave promovida (key_ref_t).
 */
key_ref_t split(key_ref_t key, int i_rrn, node_t *page, int page_rrn,
                int *promo_right_child, FILE *fp, int type)
{
    node_t *new_page = (node_t *)malloc(sizeof(node_t));

    key_ref_t return_value = _split(key, i_rrn, page, promo_right_child, new_page, fp, type);

    // Tamanho do nó
    int node_size = (type == 1) ? NODE_SIZE_T1 : NODE_SIZE_T2;

    // Escreve PAGE
    fseek(fp, (page_rrn + 1) * node_size, SEEK_SET);
    if (page) write_node(fp, page, type);

    // Escreve NEW_PAGE
    fseek(fp, (*promo_right_child + 1) * node_size, SEEK_SET);
    if (new_page) write_node(fp, new_page, type);

    if (new_page) free(new_page);

    return return_value;
}

/**
 * @brief Função recursiva que realiza a inserções na árvore-B, tratando os
 * casos de overflow com procedimentos de split e promoção.
 * 
 * @param fp ponteiro para o arquivo binário de índice árvore-B;
 * @param type tipo de arquivo (1 ou 2);
 * @param rrn RRN da página a ser pesquisada;
 * @param key chave a ser inserida;
 * @param promo_right_child filho à direita da chave promovida (ponteiro);
 * @return struct key_ref que pode ser a chave promovida ou apenas carregar
 * o valor id = -1 nos casos em que não há promoção (key_ref_t).
 */
static key_ref_t _insert(FILE *fp, int type, int rrn, key_ref_t key, int *promo_right_child)
{
    if (rrn == -1)
    {
        *promo_right_child = -1;
        return key; // promo_key = key
    }

    int node_size = (type == 1) ? NODE_SIZE_T1 : NODE_SIZE_T2;
    fseek(fp, (rrn + 1) * node_size, SEEK_SET);
    node_t *page = read_node(fp, type);
    char c;

    // Buscar na página
    long found_ref = -1, pos = 0;
    for (int i = 0; i < page->n_keys; i++)
    {
        if (key.id < page->keys[i].id)
            break;
        else if (key.id == page->keys[i].id)
            found_ref = (type == 1) ? (long)page->keys[i].ref.rrn : page->keys[i].ref.offset;
        else
            pos = i + 1;
    }

    key_ref_t return_value;

    if (found_ref != -1) return_value.id = -1; // Chave repetida, não precisa inserir
    else return_value = _insert(fp, type, page->children[pos], key, promo_right_child);

    if (return_value.id == -1)
    {
        free(page);
        return return_value;
    }
    else if (page->n_keys < 3)
    {
        update_node(page, &return_value, *promo_right_child);
        fseek(fp, (rrn + 1) * node_size, SEEK_SET);
        write_node(fp, page, type);

        return_value.id = -1;

        free(page);
        return return_value;
    }
    else
    {
        return_value = split(return_value, *promo_right_child, page, rrn, promo_right_child, fp, type);

        free(page);
        return return_value;
    }
}

/**
 * @brief Função que inicializa o processo de inserção na árvore-B e realiza
 * o tratamento da raiz.
 * 
 * @param fp ponteiro para o arquivo binário de índice árvore-B;
 * @param type tipo de arquivo (1 ou 2);
 * @param id chave;
 * @param ref valor de referência ligado à chave (RRN ou offset, a depender
 * do tipo de arquivo).
 */
void insert_into_index(FILE *fp, int type, int id, long ref)
{
    int rrn_root, next_rrn, n_nodes;

    // Pega nó da raiz
    fseek(fp, 1, SEEK_SET);
    fread(&rrn_root, sizeof(int), 1, fp);

    // Cria a chave a ser inserida
    key_ref_t key;
    key.id = id;
    if (type == 1) key.ref.rrn = (int)ref;
    else key.ref.offset = ref;

    int promo_right_child;
    key_ref_t return_value = _insert(fp, type, rrn_root, key, &promo_right_child);

    // Chave retornada da inserção (precisa criar nó raiz)
    if (return_value.id != -1)
    {
        fseek(fp, 5, SEEK_SET);
        fread(&next_rrn, sizeof(int), 1, fp);
        fread(&n_nodes, sizeof(int), 1, fp);

        // Cria o nó que será a raiz
        node_t *root = (node_t *)malloc(sizeof(node_t));
        reset_node(root, type);

        // Atribuições do moodle
        root->type = ROOT;
        root->n_keys = 1;
        root->keys[0].id = return_value.id;
        if (type == 1) root->keys[0].ref.rrn = return_value.ref.rrn;
        else root->keys[0].ref.offset = return_value.ref.offset;
        root->children[0] = rrn_root;
        root->children[1] = promo_right_child;

        //fseek(fp, (next_rrn + 1) * node_size, SEEK_SET); Alternativa
        fseek(fp, 0, SEEK_END);
        write_node(fp, root, type);
        rrn_root = next_rrn;
        next_rrn++;
        n_nodes++;

        // Atualiza o cabeçalho
        fseek(fp, 1, SEEK_SET);

        fwrite(&rrn_root, sizeof(int), 1, fp);
        fwrite(&next_rrn, sizeof(int), 1, fp);
        fwrite(&n_nodes,  sizeof(int), 1, fp);

        free(root);
    }
}
