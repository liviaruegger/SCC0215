/**
 * @file   fixed.h
 * @author Ana Lívia Ruegger Saldanha (N.USP 8586691)
 * @author André Kenji Hidaka Matsumoto (N. USP 12542689)
 * @brief  SCC0215 - Organização de Arquivos
 *         Trabalho 01
 *
 *         Módulo que trabalha com arquivos Tipo 1 (Arquivo de Dados para
 *         Registros de Tamanho Fixo)
 *
 * @date   2022-05-26
 *
 */

#include <stdio.h>

// Funcionalidade 1
FILE *new_type1_file(char *file_name);
void read_and_write_all_type1(FILE *input, FILE *output);

// Funcionalidade 2
void print_all_from_bin_type1(FILE *fp);

// Funcionalidade 3
void search_by_parameters_type1(FILE *fp);

// Funcionalidade 4
void search_by_rrn_type1(FILE *fp, int rrn);

// Funcionalidade 5
FILE *new_type1_index(FILE *data_fp, char *file_name);

// Funcionalidade 6
void type1_delete_from (FILE *data_fp, char *index_name);

// Funcionalidade 7
void insert_new_registers_type1(FILE *data_fp, FILE *index_fp, int n_registers);
