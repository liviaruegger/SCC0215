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
    read_word(stdin); // Descarta
    scanf("%d", &id);
    
    FILE *data_fp  = fopen(data_filename,  "rb");
    FILE *index_fp = fopen(index_filename, "rb");

    if (!data_fp || !index_fp)
    {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    /* fazer coisas */

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

    for (int i = 0; i < n; i++)
    {
        /**
         * -> Fazer leitura de infos da stdin
         * -> Buscar local para inserção
         * -> Inserir registro
         */
    }

    fclose(data_fp);
    fclose(index_fp);
}