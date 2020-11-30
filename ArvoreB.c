#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M 5
#define NULO -1
#define PROMOCAO 5 //ajustar valor
#define SEM_PROMOCAO 2
#define INSERCAO 3
#define ENCONTRADO 1 // blz
#define NAO_ENCONTRADO 0 // blz

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

void insere_na_pagina(int chave, int filhoD, pagina *pg) { // Feito
	int i;
	for(i = pg->num_chaves;i > 0 && chave < pg->chaves[i-1];i--) {
		pg->chaves[i] = pg->chaves[i-1];
		pg->filhos[i+1] = pg->filhos[i];
	}
	pg->num_chaves++;
	pg->chaves[i] = chave;
	pg->filhos[i+1] = filhoD;
}

void divide(int chave, int filho_d, pagina *pg, int *chave_pro, int *filho_d_pro, pagina *novapagina, FILE *arvb) { // Feito
	int pagaux_chaves[M], pagaux_filhos[M+1], pagaux_num_chaves, i, mediana;
	// Copiar PAG para PAGAUX
	for(i=0;i<pg->num_chaves;i++) {
		pagaux_chaves[i] = pg->chaves[i];
		pagaux_filhos[i] = pg->filhos[i];
	}
	pagaux_filhos[i] = pg->filhos[i];
	pagaux_num_chaves = pg->num_chaves;
	// Inserir CHAVE e FILHO_D  em PAGAUX
	insere_na_pagina(chave, filho_d, pg);
	inicializa_pagina(novapagina);
	inicializa_pagina(pg);
	// CHAVE_PRO recebe a mediana de PAGAUX
	mediana = pagaux_num_chaves/2;
	*chave_pro = pagaux_chaves[mediana];
	// FILHO_D_PRO  recebe o novo RRN
	*filho_d_pro = novo_rrn(arvb);
	// Copiar as chaves e ponteiros para os locais adequados
	for(i = 0;i < mediana;i++) {
		pg->chaves[i] = pagaux_chaves[i];
		pg->filhos[i] = pagaux_filhos[i];
		pg->num_chaves++;
	}
	pg->filhos[i] = pagaux_filhos[i];
	
	for(i = mediana+1;i < pagaux_num_chaves;i++) {
		novapagina->chaves[novapagina->num_chaves] = pagaux_chaves[i];
		novapagina->filhos[novapagina->num_chaves] = pagaux_filhos[i];
		novapagina->num_chaves++;
	}
	novapagina->filhos[novapagina->num_chaves] = pagaux_filhos[i];
}

int busca_na_pagina(int chave, pagina pg, int *pos) { // Feito
	int i=0;
	while(i < pg.num_chaves && chave > pg.chaves[i]) 
		i++;
	*pos = i;
	if(*pos < pg.num_chaves && chave ==  pg.chaves[*pos]) {
		return ENCONTRADO;
	} else {
		return NAO_ENCONTRADO;
	}
}

int inserir(int rrn_atual, int chave, int *filho_d_pro, int *chave_pro, FILE *arvb) { // Semi Feito
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
	
	if(busca_na_pagina(chave, pg, &pos) == ENCONTRADO) {
		fprintf(stderr, "Erro: Chave duplicada\n");
		return -1;
	}
	
	retorno = inserir(pg.filhos[pos], chave, &rrn_pro, &chv_pro, arvb);
	
	if(retorno == SEM_PROMOCAO || retorno == -1)
		return retorno;
	else {
		if(pg.num_chaves < M-1) {
			insere_na_pagina(chv_pro, rrn_pro, &pg);
			escreve_pagina(rrn_atual, &pg, arvb);
			return SEM_PROMOCAO;
		} else {
			divide(chv_pro, rrn_pro, &pg, chave_pro, filho_d_pro, &novapg, arvb); //conferir
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

/* int ler_chave(FILE *file, int *chave) {
	return fscanf(file, "%d", chave);
} */

void criar_dat(FILE *arvb) {
	int chave;
	char buffer[500];
	cabecalho cab;
	pagina raiz;
	
	cab.raiz = 0;
	fwrite(&cab, sizeof(cabecalho), 1, arvb);
	inicializa_pagina(&raiz);
	escreve_pagina(cab.raiz, &raiz, arvb);
	
	while(fscanf(arvb, "%d", &chave) != EOF) {
		if(inserir_chave(chave, &(cab.raiz), arvb) == -1) {
			fprintf(stderr, "Erro: chave \"%d\" ja existente", chave);
		}
	}
	
	fseek(arvb, 0, SEEK_SET);
	fwrite(&cab, sizeof(cabecalho), 1, arvb);
}

void imprimir_arvore(FILE *arvb) { // Feito
	cabecalho cab;
	pagina pg;
	int i, rrn =0;
	fseek(arvb, 0, SEEK_SET);
	fread(&cab, sizeof(cabecalho), 1, arvb);
	fseek(arvb, sizeof(cabecalho), SEEK_SET);
	
	while(fread(&pg, sizeof(pagina), 1, arvb)) {
		if(rrn == cab.raiz) {
			printf("- - - - Pagina Raiz - - - -\n");
		}
		printf("Pagina %d\n", rrn);
		
		printf("Chaves: ");
		for(i=0;i<pg.num_chaves-1;i++) {
			printf("%d | ", pg.chaves[i]);
		}
		printf("%d\n", pg.chaves[i]);
		
		printf("Filhos: ");
		for(i=0;i<pg.num_chaves;i++) {
			printf("%d | ", pg.filhos[i]);
		}
		printf("%d\n", pg.filhos[i]);
		
		if(rrn == cab.raiz) {
			printf("- - - - - - - - - - -");
		}
		printf("\n");
		rrn++;
	}
}

int altura(FILE *arvb) { // Feito
	int i, rrn = 0, altura = -1;
	cabecalho cab;
	pagina pg;
	
	fseek(arvb, 0, SEEK_SET);
	fread(&cab, sizeof(cabecalho), 1, arvb);
	
	rrn = cab.raiz;
	while(rrn != -1) {
		le_pagina(rrn, &pg, arvb);
		rrn = pg.filhos[0];
		altura++;
	}
	
	return altura;
}

void print_log(FILE *arvb) { // Feito
	int num_chaves=0, num_pgs=0;
	pagina pg;
	
	fseek(arvb, sizeof(cabecalho), SEEK_SET);
	while(fread(&pg, sizeof(pagina), 1, arvb)) {
		num_pgs++;
		num_chaves += pg.num_chaves;
	}
	
	printf("\n- - - - - - - - - - - - -\n");
	printf("Estatisticas da Arvore-B:\n");
	printf("> Altura: %d\n", altura(arvb));
	printf("> Numero de chaves: %d\n", num_chaves);
	printf("> Numero de paginas: %d\n", num_pgs);
	printf("> Taxa de ocupacao: %.2f\n", (float)(100*num_chaves)/(num_pgs*(M-1)));
}

int main(int argc, char **argv) {

    if (argc < 3) {
        fprintf(stderr, "Numero incorreto de argumentos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s (-c|-p) nome_arquivo\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-c") == 0) {
		FILE* arvoreB = fopen("btree.dat", "w+b");
        printf("Criando Arvore-B... nome do arquivo de chaves = %s\n", argv[2]);
		
		criar_dat(arvoreB);
		
		fclose(arvoreB);
    } else if (strcmp(argv[1], "-p") == 0) {
		FILE* arvoreB = fopen("btree.dat", "rb");
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