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
#include "utils.h"

#define INDEX_HEADER_SIZE_T1 45
#define INDEX_HEADER_SIZE_T2 57