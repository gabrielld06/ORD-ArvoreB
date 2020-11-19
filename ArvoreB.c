#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M 5
#define NULO -1;

typedef struct cabecalho_raiz {
	int raiz;
}cabecalho;

typedef struct paginas {
	int num_chaves;
	int chaves[M-1];
	int filhos[M];
}pagina;

void le_pagina(int rrn, pagina *pg, FILE *arvb) {
	int offset = sizeof(cabecalho) * rrn * sizeof(pagina);
	fseek(arvb, offset, SEEK_SET);
	fread(pg, sizeof(pagina), 1, arvb);
}

void escreve_pagina(int rrn, pagina *pg, FILE *arvb) {
	int offset = sizeof(cabecalho) * rrn * sizeof(pagina);
	fseek(arvb, offset, SEEK_SET);
	fwrite(pg, sizeof(pagina), 1, arvb);
}

int inicializa_pagina(pagina *pg) {
	int i;
	for(i = 0;i < M;i++) {
		pg -> chaves[i] = NULO;
		pg -> filhos[i] = NULO;
	}
	pg -> filhos[i] = NULO;
	pg -> num_chaves = 0;
}

void insere_na_pagina(int chave, int filhos[], pagina *pg) {
	
}

int le_pagina(pagina *pg) {
	int i;
	for(i = 0;i < M;i++) {
		pg -> chaves[i] = NULO;
		pg -> filhos[i] = NULO;
	}
	pg -> filhos[i] = NULO;
	pg -> num_chaves = 0;
}

int main(int argc, char **argv) {

    if (argc < 3) {
        fprintf(stderr, "Numero incorreto de argumentos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s (-c|-p) nome_arquivo\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-c") == 0) {
		FILE* arvoreB = fopen("btree.dat", "w");
        printf("Criando Arvore-B... nome do arquivo de chaves = %s\n", argv[2]);
		
    } else if (strcmp(argv[1], "-p") == 0) {
		FILE* arvoreB = fopen("btree.dat", "r");
		if(arvoreB == NULL) {
			fprintf(stderr, "Erro ao abrir arquivo: Arquivo nao existe\n");
			exit(EXIT_FAILURE);
		}
        printf("Modo de impressão da Arvore-B ativado\n");
    } else {
        fprintf(stderr, "Opcao \"%s\" nao suportada!\n", argv[1]);
    }

    return 0;
}