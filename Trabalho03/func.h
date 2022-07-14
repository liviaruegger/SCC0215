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

void functionality_09(int file_type, FILE *data_fp, FILE *index_fp);
void functionality_10(int file_type, FILE *data_fp, FILE *index_fp, int id);
void functionality_11(int file_type, FILE *data_fp, FILE *index_fp, int n);