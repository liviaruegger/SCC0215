/**
 * @file   data.h
 * @author Ana Lívia Ruegger Saldanha (N.USP 8586691)
 * @author André Kenji Hidaka Matsumoto (N. USP 12542689)
 * @brief  SCC0215 - Organização de Arquivos
 *         Trabalho 03
 *
 *         Módulo para manipulação de arquivos de dados
 *
 * @date   2022-07-20
 *
 */

void build_index_from_file_type1(FILE *data_fp, FILE *index_fp);
void build_index_from_file_type2(FILE *data_fp, FILE *index_fp);

void search_by_rrn_type1(FILE *fp, int rrn);
void search_by_offset_type2(FILE *fp, long offset);

void insert_new_registers_type1(FILE *data_fp, FILE *index_fp, int n_registers);
void insert_new_registers_type2(FILE *data_fp, FILE *index_fp, int n_registers);
