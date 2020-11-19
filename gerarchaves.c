#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>


int main(int argc, char *argv[]) {
    bool randomizar = false, inverter = false;
    int n, i, j, aux;
    int *chaves;
    char *nomearq;
    FILE *fd;

    if (argc < 3) {
        fprintf(stderr, "Uso: %s N nome_arq_chaves [-i | -r]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    n = atoi(argv[1]);
    if (n < 1) {
        fprintf(stderr, "Erro: o numero de chaves (N) deve ser maior que zero.\n");
        exit(EXIT_FAILURE);
    }

    nomearq = argv[2];
    if ((fd=fopen(nomearq, "wb")) == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo %s para escrita.\n", nomearq);
        exit(EXIT_FAILURE);
    }

    if (argc > 3) {
        if (strcmp(argv[3], "-i") == 0) {
            inverter = true;
        } else if (strcmp(argv[3], "-r") == 0) {
            randomizar = true;
        }
    }

    chaves = malloc(sizeof(int)*n);

    printf("> Produzindo %d %s em ", n, n > 1 ? "chaves" : "chave");

    if (inverter) {
        printf("ordem invertida.\n");
        for (i=0; i<n; i++) {
            chaves[i] = n-i;
        }
    } else {
        for (i=0; i<n; i++) {
            chaves[i] = i+1;
        }
        if (randomizar) {
            printf("ordem randomica.\n");
            srand(time(NULL));
            for (i = n-1; i > 0; i--) {
                j = (int)((double)rand() / ((double)RAND_MAX + 1) * i);
                aux = chaves[i];
                chaves[i] = chaves[j];
                chaves[j] = aux;
            }
        } else {
            printf("ordem crescente.\n");
        }
    }


    for (i=0; i<n; i++) {
        fprintf(fd, "%d\n", chaves[i]);
    }

    fclose(fd);

    printf("> Arquivo \"%s\" escrito com sucesso!\n", nomearq);

    return 0;
}