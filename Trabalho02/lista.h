typedef struct reg_remov{
    int size;
    long int offset;
} RegRemov_t;
typedef RegRemov_t elem;
typedef struct lista Lista_t;

Lista_t *criar();
void desalocar(Lista_t *lista);
void resetar(Lista_t *lista);
void inserir(Lista_t *lista, int size, long int offset);
void remover_ultimo(Lista_t *lista, elem *x);
void imprimir(Lista_t *lista);
void update_topo (Lista_t *lista, FILE *data_fp);
