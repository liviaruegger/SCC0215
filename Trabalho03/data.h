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

int get_key_type1(FILE *fp, int *id, int *ref);
int get_key_type2(FILE *fp, int *id, long *ref);

void insert_new_register_type1(FILE *fp, int *id,  int *ref);
void insert_new_register_type2(FILE *fp, int *id, long *ref);

void search_by_rrn_type1(FILE *fp, int rrn);
void search_by_offset_type2(FILE *fp, long offset);

void insert_new_registers_type1(FILE *data_fp, FILE *index_fp, int n_registers);