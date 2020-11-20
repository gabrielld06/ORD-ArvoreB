#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M 5
#define NULO -1
#define PROMOCAO 5 //ajustar valor
#define SEM_PROMOCAO 2
#define INSERCAO 3

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

int novo_rrn(FILE *arvb) {
	int offset;
	fseek(arvb, 0, SEEK_END);
	offset = ftell(arvb);
	return (offset-sizeof(cabecalho))/sizeof(pagina);
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

void insere_na_pagina(int chave, int filhoD, pagina *pg) {
	int i;
	for(i = pg->num_chaves;i > 0 && chave < pg->chaves[i-1];i--) {
		pg->chaves[i] = pg->chaves[i-1];
		pg->filhos[i+1] = pg->filhos[i];
	}
	pg->num_chaves++;
	pg->chaves[i] = chave;
	pg->filhos[i+1] = filhoD;
}

void divide(int chave, int rrn, pagina *pg, int *chave_pro, int *filho_d_pro, pagina *novapagina, FILE *arvb) {
	int pagaux_chaves[M], pagaux_filhos[M+1], pagaux_num_chaves;
	int i, med;
	
	for(i = 0;i < pg->num_chaves;i++) {
		pagaux_chaves[i] = pg->chaves[i];
		pagaux_filhos[i] = pg->filhos[i];
	}
	pagaux_filhos[i] = pg->filhos[i];
	pagaux_num_chaves = pg->num_chaves;
	
	insere_na_pagina(chave, rrn, pg);
	
	med = pagaux_num_chaves/2;
	*chave_pro = pagaux_chaves[med];
	
	inicializa_pagina(pg);
	for(i = 0;i < med;i++) {
		pg->chaves[i] = pagaux_chaves[i];
		pg->filhos[i] = pagaux_filhos[i];
		pg->num_chaves++;
	}
	pg->filhos[i] = pagaux_filhos[i];
	
	inicializa_pagina(novapagina);
	for(i = med+1;i < pagaux_num_chaves;i++) {
		novapagina->chaves[novapagina->num_chaves] = pagaux_chaves[i];
		novapagina->filhos[novapagina->num_chaves] = pagaux_filhos[i];
		novapagina->num_chaves++;
	}
	novapagina->filhos[novapagina->num_chaves] = pagaux_filhos[i];
	
	*filho_d_pro = novo_rrn(arvb);
}

int inserir(int rrn_atual, int chave, int *chave_pro, int *filho_d_pro, FILE *arvb) {
	pagina pg, novapg;
	int chv_pro, rrn_pro;
	int pos;
	int retorno;
	
	if(rrn_atual == NULO) {
		*chave_pro = chave;
		*filho_d_pro = NULO;
		return PROMOCAO;
	}
	
	le_pagina(rrn_atual, &pg, arvb);
	
	//pos = pos_chave(chave, pg->chave, pg->num_chaves); TODO
	if(pos < pg.num_chaves && pg.chaves[pos] == chave) return -1;
	
	retorno = inserir(pg.filhos[pos], chave, &chv_pro, &rrn_pro, arvb);
	
	if(retorno == SEM_PROMOCAO || retorno == -1)
		return retorno;
	else {
		if(pg.num_chaves < M-1) {
			insere_na_pagina(chv_pro, rrn_pro, &pg);
			escreve_pagina(rrn_atual, &pg, arvb);
			return SEM_PROMOCAO;
		} else {
			divide(chv_pro, rrn_pro, &pg, chave_pro, filho_d_pro, &novapg, arvb);
			escreve_pagina(rrn_atual, &pg, arvb);
			escreve_pagina(*filho_d_pro, &novapg, arvb);
			return PROMOCAO;
		}
	}
}

int inserir_chave(int chave, int *rrn_raiz, FILE *arvb) {
	pagina raiz;
	int chave_pro, filho_d_pro;
	int retorno;
	
	retorno = inserir(*rrn_raiz, chave, &chave_pro, &filho_d_pro, arvb);
	
	if(retorno == -1)
		return -1;
	else if(retorno == PROMOCAO) {
		pagina novaraiz;
		
		inicializa_pagina(&novaraiz);
		novaraiz.chaves[0] = chave_pro;
		novaraiz.filhos[0] = *rrn_raiz;
		novaraiz.filhos[1] = filho_d_pro;
		novaraiz.num_chaves = 1;
		
		*rrn_raiz = novo_rrn(arvb);
		escreve_pagina(*rrn_raiz, &novaraiz, arvb);
	}
	return INSERCAO;
}

int ler_chave(FILE *file, int *chave) {
	return fscanf(file, "%d", chave);
}

/* void criar(char *nome_arq) {
	FILE *chaves, *arvb;
	int reg_cont, i;
	int chave;
	char buffer[500];
	cabecalho cab;
	pagina raiz;
	
	
} */

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