/**
 * @file   func.c
 * @author Ana Lívia Ruegger Saldanha (N.USP 8586691)
 * @author André Kenji Hidaka Matsumoto (N. USP 12542689)
 * @brief  SCC0215 - Organização de Arquivos
 *         Trabalho 03
 *
 *         Módulo para funcionalidades, articulando a manipulação
 *         dos arquivos de dados e de índice árvore-B
 *
 * @date   2022-07-20
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "btree.h"
#include "data.h"

void functionality_09(int file_type, char *data_filename, char *index_filename)
{
    FILE *data_fp =  fopen(data_filename,  "rb");
    FILE *index_fp = fopen(index_filename, "wb");

    if (!data_fp || !index_fp)
    {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    /* fazer coisas */

    fclose(data_fp);
    fclose(index_fp);
}

void functionality_10(int file_type, char *data_filename, char *index_filename)
{
    int id;
    for (int i = 0; i < 3; i++) getchar();
    scanf("%d", &id);
    
    FILE *data_fp  = fopen(data_filename,  "rb");
    FILE *index_fp = fopen(index_filename, "rb");

    if (!data_fp || !index_fp)
    {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    long found_ref = search(index_fp, file_type, id);

    if (found_ref == -1)
    {
        printf("Registro inexistente.\n");
    }
    else
    {
        if (file_type == 1) search_by_rrn_type1(data_fp, found_ref);
        else search_by_offset_type2(data_fp, found_ref);
    }

    fclose(data_fp);
    fclose(index_fp);
}

void functionality_11(int file_type, char *data_filename, char *index_filename)
{
    int n;
    scanf("%d", &n);

    FILE *data_fp  = fopen(data_filename,  "r+b");
    FILE *index_fp = fopen(index_filename, "r+b");

    if (!data_fp || !index_fp)
    {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    if (file_type == 1) insert_new_registers_type1(data_fp, index_fp, n);
    else ; // TODO: type 2

    fclose(data_fp);
    fclose(index_fp);
}