#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Alocando memória dinamicamente, lê e armazena uma palavra do arquivo
 * de entrada até encontrar um espaço, uma quebra de linha ou EOF.
 * 
 * @param stream entrada (arquivo ou entrada padrão);
 * @return ponteiro para a string lida (char *).
 */
char *read_word(FILE *stream)
{
    char *string = (char *) malloc(sizeof(char));
    int size = 0;

    do
    {
        string[size] = fgetc(stream);
        size++;

        if (string[size - 1] != ' '  &&
            string[size - 1] != '\r' &&
            string[size - 1] != '\n' &&
            string[size - 1] != EOF)
            string = (char *) realloc(string, size + 1);

    } while (string[size - 1] != ' '  &&
             string[size - 1] != '\r' &&
             string[size - 1] != '\n' &&
             string[size - 1] != EOF);

    if (string[size - 1] == '\r') fgetc(stream);

    string[size - 1] = '\0';

    return string;
}

/**
 * @brief Alocando memória dinamicamente, lê e armazena uma linha do arquivo de 
 * entrada até encontrar o caractere que foi passado como parâmetro.
 * 
 * @param stream entrada (arquivo ou entrada padrão);
 * @param c caractere de parada;
 * @return ponteiro para a string lida (char *).
 */
char *read_until(FILE *stream, char c)
{
    char *string = (char *) malloc(sizeof(char));
    int size = 0;

    do
    {
        string[size] = fgetc(stream);
        size++;

        if (string[size - 1] != c)
            string = (char *) realloc(string, size + 1);

    } while (string[size - 1] != c);

    string[size - 1] = '\0';

    return string;
}



// ================================= FUNÇÃO FORNECIDA ====================================

void binarioNaTela(char *nomeArquivoBinario) { /* Você não precisa entender o código dessa função. */

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for(i = 0; i < fl; i++) {
		cs += (unsigned long) mb[i];
	}
	printf("%lf\n", (cs / (double) 100));
	free(mb);
	fclose(fs);
}