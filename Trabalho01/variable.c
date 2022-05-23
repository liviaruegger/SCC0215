/**
 * @brief Módulo que trabalha com arquivos Tipo 2 (Arquivo de Dados para
 *        Registros de Tamanho Variável)
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct register_type2
{
  char removed;
  int register_size;
  long int next;
  int id;
  int year;
  int qtt;
  char *state;

  int  city_namesize;
  char *codC5;
  char *city;

  int  brand_namesize;
  char *codC6;
  char *brand;

  int  model_namesize;
  char *codC7;
  char *model;
} reg_t2;

FILE *new_type2_file(char *file_name)
{
  FILE *fp = fopen(file_name, "wb");

  int temp_value = -1; // integer argument for fwrite

  // 0 - status
  fwrite("0",                                         sizeof(char), 1, fp);
  // 1 - pile top
  fwrite(&temp_value,                                 sizeof(long int), 1, fp);
  temp_value = 0;
  // 9 - description
  fwrite("LISTAGEM DA FROTA DOS VEICULOS NO BRASIL",  sizeof(char), 40, fp);
  // 49 - desC1
  fwrite("CODIGO IDENTIFICADOR: ",                    sizeof(char), 22, fp);
  // 71 - desC2
  fwrite("ANO DE FABRICACAO: ",                       sizeof(char), 19, fp);
  // 90 - desC3
  fwrite("QUANTIDADE DE VEICULOS: ",                  sizeof(char), 24, fp);
  // 114 - desC4
  fwrite("ESTADO: ",                                  sizeof(char), 8, fp);
  // 122 - codC5
  fwrite("0",                                         sizeof(char), 1, fp);
  // 123 - desC5
  fwrite("NOME DA CIDADE: ",                          sizeof(char), 16, fp);
  // 139 - codC6
  fwrite("1",                                         sizeof(char), 1, fp);
  // 140 - desC6
  fwrite("MARCA DO VEICULO: ",                        sizeof(char), 18, fp);
  // 158 - codC7
  fwrite("2",                                         sizeof(char), 1, fp);
  // 159 - desC7
  fwrite("MODELO DO VEICULO: ",                       sizeof(char), 19, fp);
  // 178 - nextByteOffset
  fwrite(&temp_value,                                 sizeof(long int), 1, fp);
  // 186 - numRemovedRegisters
  fwrite(&temp_value,                                 sizeof(int), 1, fp);

  fseek(fp, 0, SEEK_SET);
  fwrite("1",                                         sizeof(char), 1, fp);

  fclose(fp);
}

// Soh de testezin
int main(){




  return 0;
}
