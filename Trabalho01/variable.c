/**
 * @brief Módulo que trabalha com arquivos Tipo 2 (Arquivo de Dados para
 *        Registros de Tamanho Variável)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

FILE *new_type2_file(char *file_name)
{
  FILE *fp = fopen(file_name, "wb");

  long int temp_value = -1; // integer argument for fwrite

  // 0 - status
  fwrite("0",                                         sizeof(char), 1, fp);
  // 1 - pile top
  fwrite(&temp_value,                                 sizeof(long int), 1, fp);
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
  temp_value = 0;
  fwrite(&temp_value,                                 sizeof(long int), 1, fp);
  // 186 - numRemovedRegisters
  int temp_value2 = 0;
  fwrite(&temp_value2,                                 sizeof(int), 1, fp);

  fseek(fp, 190, SEEK_SET);

  return fp;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

reg_t2 *read_t2_register_from_csv(FILE *fp)
{
  reg_t2 *reg = malloc(sizeof(reg_t2));

  reg->removed = '0';
  reg->register_size = 0;

  reg->next = -1;
  reg->register_size += sizeof(reg->next);

  char *id = read_until(fp, ',');
  reg->id = atoi(id);
  reg->register_size += sizeof(reg->id);
  free(id);

  char *ano = read_until(fp, ',');
  if(ano[0] >= '0' && ano[0] <= '9')
    reg->year = atoi(ano);
  else
    reg->year = -1;
  reg->register_size += sizeof(reg->year);
  free(ano);

  char *cidade = read_until(fp, ',');
  if(cidade[0] != '\0')
  {
    reg->city_namesize = strlen(cidade);
    reg->register_size += sizeof(reg->city_namesize);
    reg->city = cidade;
    reg->register_size += reg->city_namesize;
    reg->codC5 = "0";
    reg->register_size += 1;
  }
  else{
    reg->city_namesize = -1;
    free(cidade);
  }


  char *qtt = read_until(fp, ',');
  if(qtt[0] >= '0' && qtt[0] <= '9')
    reg->qtt = atoi(qtt);
  else
    reg->qtt = -1;
  reg->register_size += sizeof(reg->qtt);
  free(qtt);

  char *sigla = read_until(fp, ',');
  if(sigla[0] != '\0')
    reg->state = sigla;
  else{
    reg->state = "$$";
    free(sigla);
  }
  reg->register_size += 2;

  char *marca = read_until(fp, ',');
  if(marca[0] != '\0')
  {
    reg->brand_namesize = strlen(marca);
    reg->register_size += sizeof(reg->brand_namesize);
    reg->brand = marca;
    reg->register_size += reg->brand_namesize;
    reg->codC6 = "1";
    reg->register_size += 1;
  }
  else{
    reg->brand_namesize = -1;
    free(marca);
  }

  char *modelo = read_line(fp);
  if(modelo[0] != '\0')
  {
    reg->model_namesize = strlen(modelo);
    reg->register_size += sizeof(reg->model_namesize);
    reg->model = modelo;
    reg->register_size += reg->model_namesize;
    reg->codC7 = "2";
    reg->register_size += 1;
  }
  else{
    reg->model_namesize = -1;
    free(modelo);
  }

  return reg;
}



void read_and_write_register_t2(FILE *input, FILE *output)
{
  fseek(input, 61, SEEK_SET);
  char c;
  do{
    reg_t2 *reg = read_t2_register_from_csv(input);

    fwrite(&reg->removed, sizeof(char), 1, output);
    fwrite(&reg->register_size, sizeof(int), 1, output);
    fwrite(&reg->next, sizeof(long int), 1, output);
    fwrite(&reg->id, sizeof(int), 1, output);
    fwrite(&reg->year, sizeof(int), 1, output);
    fwrite(&reg->qtt, sizeof(int), 1, output);
    fwrite(reg->state, sizeof(char), 2, output);
    if(reg->state != "$$")
      free(reg->state);

    if(reg->city_namesize != -1){
      fwrite(&reg->city_namesize, sizeof(int), 1, output);
      fwrite(reg->codC5, sizeof(char), 1, output);
      fwrite(reg->city, sizeof(char), reg->city_namesize, output);
      free(reg->city);
    }

    if(reg->brand_namesize != -1){
      fwrite(&reg->brand_namesize, sizeof(int), 1, output);
      fwrite(reg->codC6, sizeof(char), 1, output);
      fwrite(reg->brand, sizeof(char), reg->brand_namesize, output);
      free(reg->brand);
    }

    if(reg->model_namesize != -1){
      fwrite(&reg->model_namesize, sizeof(int), 1, output);
      fwrite(reg->codC7, sizeof(char), 1, output);
      fwrite(reg->model, sizeof(char), reg->model_namesize, output);
      free(reg->model);
    }

    free(reg);

    c = fgetc(input);
    if(c != EOF){
      fputc(c, input);
      fseek(input, -1, SEEK_CUR);
    }
  } while(c != EOF);

  long int temp = (long int)ftell(output);
  fseek(output, 178, SEEK_SET);
  fwrite(&temp, sizeof(long int), 1, output);

  fseek(output, 0, SEEK_SET);
  fwrite("1",                                         sizeof(char), 1, output);
}
