/**
 * @file   utils.h
 * @author Ana Lívia Ruegger Saldanha (N.USP 8586691)
 * @author André Kenji Hidaka Matsumoto (N. USP 12542689)
 * @brief  SCC0215 - Organização de Arquivos
 *         Trabalho 03
 *
 *         Módulo contendo funções utilitárias
 *
 * @date   2022-07-20
 *
 */

#include <stdio.h>

char *read_word(FILE *stream);
char *read_line(FILE *stream);
char *read_until(FILE *stream, char c);

void binarioNaTela(char *nomeArquivoBinario);

void update_header_status(FILE *fp, char status);