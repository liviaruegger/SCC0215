#include <stdio.h>

FILE *new_type1_file(char *file_name);
void read_and_write_all_type1(FILE *input, FILE *output);
void close_type1_file(FILE *fp);
void print_type1_register(FILE *fp, int rrn);