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

    char status;
    fread(&status, sizeof(char), 1, data_fp);

    if (!data_fp || !index_fp || status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }
    /*
    // Armazena o tamanho do arquivo de dados.
    fseek(data_fp, 0, SEEK_END);
    long file_size = ftell(data_fp);

    int id, verifier;
    if (file_type == 1)
    {
        int ref;

        for (long offset = 182; offset < file_size; offset += 97)
        {
            fseek(data_fp, offset, SEEK_SET);

            verifier = get_key_type1(data_fp, &id, &ref);

            // Verifica se o registro não está removido.
            if (verifier == 1)
                printf(" ");
                // insert()
        }
    }
    else
    {
        long ref;

        fseek(data_fp, 190, SEEK_SET); //DATA_HEADER_SIZE_T2

        while (ftell(data_fp) < file_size)
        {
            verifier = get_key_type2(data_fp, &id, &ref);

            // Verifica se o registro não está removido.
            if (verifier == 1)
                printf(" ");
                //insert()
        }
    }
    */
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
    else insert_new_registers_type2(data_fp, index_fp, n);

    fclose(data_fp);
    fclose(index_fp);
}
