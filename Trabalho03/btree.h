/**
 * @file   btree.h
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

void write_header(FILE *fp, int type);

long search(FILE *fp, int type, int id);
void driver(FILE *fp, int type, int id, int rrn);
