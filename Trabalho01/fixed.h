#include <stdio.h>

FILE *new_file(char *file_name);
void read_and_write_all(FILE *input, FILE *output);
void close_file(FILE *fp);