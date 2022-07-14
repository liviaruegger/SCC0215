/**
 * @file   data.c
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

#include <stdio.h>
#include "utils.h"

#define DATA_HEADER_SIZE_T1 182
#define DATA_HEADER_SIZE_T2 190

#define REGISTER_SIZE_T1 97

typedef struct reg
{
    char removed;
    int  register_size; // Apenas tipo2
    int  next;

    int  id;
    int  year;
    int  qtt;
    char *state;

    int  city_namesize;
    char *codeC5; // Apenas tipo2
    char *city;

    int  brand_namesize;
    char *codeC6; // Apenas tipo2
    char *brand;

    int  model_namesize;
    char *codeC7; // Apenas tipo2
    char *model;
}   reg_t;