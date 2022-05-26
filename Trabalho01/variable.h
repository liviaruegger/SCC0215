/**
 * @file   variable.h
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

// Funcionalidade 1
FILE *new_type2_file(char *file_name);
void read_and_write_all_type2(FILE *input, FILE *output);

// Funcionalidade 2
void print_all_from_bin_type2(FILE *fp);

// Funcionalidade 3
void search_by_parameters_type2(FILE *fp);