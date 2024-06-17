#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>

typedef struct Ceai Ceai;
struct Ceai {
	int cod;
	char* denumire;
	float grameCofeina;
};

Ceai initializareCeai(int cod, const char* denumire, float grameC) {
	Ceai c;
	c.cod = cod;
	c.grameCofeina = grameC;
	c.denumire = (char*)malloc(strlen(denumire) + 1);
	strcpy(c.denumire, denumire);
	return c;
}

typedef struct Nod Nod;
struct Nod {
	Ceai info;
	Nod* next;
};

typedef struct HashTable HashTable;
struct HashTable {
	int dim;
	Nod** vector;
};

HashTable initializareTabela(int dim) {
	HashTable tabela;
	tabela.dim = dim;
	tabela.vector = (Nod**)malloc(dim * sizeof(Nod*));
	for (int i = 0;i < dim;i++) {
		tabela.vector[i] = NULL;
	}
	return tabela;
}

int functieHash(int cod, float grame, int dim) {
	if (dim > 0) {
		return (cod + (int)grame) % dim;
	}
	else {
		return -1;
	}
}

Nod* inserareFinal(Nod* cluster, Ceai c) {
	Nod* nod = (Nod*)malloc(sizeof(Nod));
	nod->info = c;
	nod->next = NULL;
	if (cluster == NULL) {
		cluster = nod;
	}
	else {
		Nod* aux = cluster;
		while (aux->next) {
			aux = aux->next;
		}
		aux->next = nod;
	}
	return cluster;
}

HashTable inserareChaining(HashTable tabela, Ceai c) {
	if (tabela.vector != NULL) {
		int poz = functieHash(c.cod, c.grameCofeina, tabela.dim);
		tabela.vector[poz] = inserareFinal(tabela.vector[poz], c);
	}
	return tabela;
}

void afisareCeai(Ceai c) {
	printf("%d. Ceaiul %s are %.2f grame de cofeina\n", c.cod, c.denumire, c.grameCofeina);
}

void afisareCluster(Nod* cluster) {
	while (cluster) {
		afisareCeai(cluster->info);
		cluster = cluster->next;
	}
}

void afisareHashTable(HashTable tabela) {
	for (int i = 0;i < tabela.dim;i++) {
		printf("Hash code=%d\n", i);
		afisareCluster(tabela.vector[i]);
	}
}

Ceai cautaDupaHash(HashTable tabela, int cod, float grameC) {
	if (tabela.dim != 0) {
		int poz = functieHash(cod, grameC, tabela.dim);
		while (tabela.vector[poz]) {
			if (tabela.vector[poz]->info.cod == cod && tabela.vector[poz]->info.grameCofeina == grameC) {
				return tabela.vector[poz]->info;
			}
			tabela.vector[poz] = tabela.vector[poz]->next;
		}
		return initializareCeai(-1, "", 0);
	}
	else {
		return initializareCeai(-1, "", 0);
	}
}

Ceai cautaDupaDenumire(HashTable tabela, const char* denumire) {
	for (int i = 0;i < tabela.dim;i++) {
		Nod* aux = tabela.vector[i];
		while (aux) {
			if (strcmp(aux->info.denumire,denumire)==0) {
				return aux->info;
			}
			aux = aux->next;
		}
	}
	return initializareCeai(-1, "", 0);
}

HashTable stergereHashTable(HashTable tabela) {
	for (int i = 0;i < tabela.dim;i++) {
		Nod* p = tabela.vector[i];
		while (p) {
			Nod* aux = p->next;
			free(p->info.denumire);
			free(p);
			p = aux;
		}
	}
	free(tabela.vector);
	tabela.vector = NULL;
	tabela.dim = 0;
	return tabela;
}

int main() {
	Ceai c1 = initializareCeai(1, "Frosty Afternoon", 2); //3
	Ceai c2 = initializareCeai(2, "Musetel", 0.4); //2
	Ceai c3 = initializareCeai(3, "English Breakfast", 5); //3
	Ceai c4 = initializareCeai(4, "Ashwagandha", 1.2); //0
	Ceai c5 = initializareCeai(5, "Pepermint", 4.5); //4

	HashTable tabela = initializareTabela(5);

	tabela = inserareChaining(tabela, c1);
	tabela = inserareChaining(tabela, c2);
	tabela = inserareChaining(tabela, c3);
	tabela = inserareChaining(tabela, c4);
	tabela = inserareChaining(tabela, c5);
	afisareHashTable(tabela);

	Ceai cautat1 = cautaDupaHash(tabela, 3, 5);
	printf("\nCeaiul cautat este: ");
	afisareCeai(cautat1);

	Ceai cautat2 = cautaDupaHash(tabela, 2, 5);
	printf("\nCeaiul cautat este: ");
	afisareCeai(cautat2);

	Ceai cautat3 = cautaDupaDenumire(tabela, "Ashwagandha");
	printf("\nCeaiul cautat este: ");
	afisareCeai(cautat3);

	Ceai cautat4 = cautaDupaDenumire(tabela, "Menta");
	printf("\nCeaiul cautat este: ");
	afisareCeai(cautat4);

	tabela = stergereHashTable(tabela);
	printf("\nTabela dupa stergere:\n");
	afisareHashTable(tabela);

	return 0;
}