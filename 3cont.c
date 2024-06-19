#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

typedef struct ContBancar Cont;
struct ContBancar {
	int id;
	char* client;
	float sold;
	char* valuta;
};

typedef struct Nod Nod;
struct Nod {
	Cont info;
	Nod* next;
};

typedef struct HashTable HashTable;
struct HashTable {
	Nod** vector;
	int dim;
};

int functieHash(const char* client, int dim) {
	return strlen(client) % dim;
}

HashTable initializareTabela(int dim) {
	HashTable tab;
	tab.dim = dim;
	if (dim != 0) {
		tab.vector = (Nod**)malloc(dim * sizeof(Nod*));
		for (int i = 0;i < dim;i++) {
			tab.vector[i] = NULL;
		}
	}
	else {
		tab.vector = NULL;
	}
	return tab;
}

Nod* inserareCluster(Nod* cluster, Cont c) {
	Nod* nod = (Nod*)malloc(sizeof(Nod));
	nod->info = c;
	nod->next = NULL;
	if (cluster) {
		Nod* aux = cluster;
		while (aux->next) {
			aux = aux->next;
		}
		aux->next = nod;
	}
	else {
		cluster = nod;
	}
	return cluster;
}

HashTable inserareTabela(HashTable tab, Cont c) {
	if (tab.vector) {
		int hash = functieHash(c.client, tab.dim);
		tab.vector[hash] = inserareCluster(tab.vector[hash], c);
	}
	return tab;
}

HashTable citireFisier(const char* numeFisier) {
	FILE* file = fopen(numeFisier, "r");
	HashTable tab = initializareTabela(5);
	char buffer[100];
	char separator[] = ",\n";
	while (fgets(buffer, 100, file)) {
		Cont c;
		char* token = strtok(buffer, separator);
		c.id = atoi(token);
		token = strtok(NULL, separator);
		c.client = (char*)malloc(strlen(token) + 1);
		strcpy(c.client, token);
		token = strtok(NULL, separator);
		c.sold = atof(token);
		token = strtok(NULL, separator);
		c.valuta = (char*)malloc(strlen(token) + 1);
		strcpy(c.valuta, token);
		tab = inserareTabela(tab, c);
	}
	fclose(file);
	return tab;
}

void afisareCont(Cont c) {
	printf("\t%d. %s are in cont %.2f %s\n", c.id, c.client, c.sold, c.valuta);
}

void afisareCluster(Nod* cluster) {
	while (cluster) {
		afisareCont(cluster->info);
		cluster = cluster->next;
	}
}

void afisareTabela(HashTable tab) {
	for (int i = 0;i < tab.dim;i++) {
		printf("Cluster %d\n", i);
		afisareCluster(tab.vector[i]);
	}
}

Cont initializareCont(int id, const char* client, float sold, const char* valuta) {
	Cont c;
	c.id = id;
	c.client = (char*)malloc(strlen(client) + 1);
	strcpy(c.client, client);
	c.sold = sold;
	c.valuta = (char*)malloc(strlen(valuta) + 1);
	strcpy(c.valuta, valuta);
	return c;
}

Cont* conturiDupaValuta(HashTable tab, const char* valuta, int* dim) {
	*dim = 0;
	for (int i = 0;i < tab.dim;i++) {
		Nod* aux = tab.vector[i];
		while (aux) {
			if (strcmp(aux->info.valuta, valuta) == 0) {
				(*dim)++;
			}
			aux = aux->next;
		}
	}
	Cont* vector = (Cont*)malloc((*dim) * sizeof(Cont));
	int k = 0;
	for (int i = 0;i < tab.dim;i++) {
		Nod* aux = tab.vector[i];
		while (aux) {
			if (strcmp(aux->info.valuta, valuta) == 0) {
				vector[k++] = initializareCont(aux->info.id, aux->info.client, aux->info.sold, aux->info.valuta);
			}
			aux = aux->next;
		}
	}
	return vector;
}

typedef struct Cluster Cluster;
struct Cluster {
	int dimensiune;
	int index;
};

Cluster* dimensiuniClustere(HashTable tab) {
	Cluster* dimensiuni = (Cluster*)malloc(tab.dim * sizeof(Cluster));
	for (int i = 0;i < tab.dim;i++) {
		Cluster c;
		c.dimensiune = 0;
		c.index = i;
		Nod* aux = tab.vector[i];
		while (aux) {
			c.dimensiune++;
			aux = aux->next;
		}
		
		dimensiuni[i] = c;
	}
	return dimensiuni;
}

Nod* stergereLista(Nod* lista) {
	while (lista) {
		Nod* aux = lista->next;
		free(lista->info.client);
		free(lista->info.valuta);
		free(lista);
		lista = aux;
	}
	lista = NULL;
	return lista;
}

HashTable stergereTabela(HashTable tab) {
	for (int i = 0;i < tab.dim;i++) {
		stergereLista(tab.vector[i]);
	}
	free(tab.vector);
	tab.vector = NULL;
	tab.dim = 0;
	return tab;
}

Cont* stergerevectorConturi(Cont* vector, int* dim) {
	for (int i = 0;i < *dim;i++) {
		free(vector[i].client);
		free(vector[i].valuta);
	}
	free(vector);
	vector = NULL;
	*dim = 0;
	return vector;
}

int main() {
	HashTable tab = citireFisier("3conturi.txt");
	afisareTabela(tab);
	int dim;
	Cont* vector = conturiDupaValuta(tab, "EURO", &dim);
	printf("\nConturi cu EURO:\n");
	for (int i = 0;i < dim;i++) {
		afisareCont(vector[i]);
	}
	Cluster* dimensiuni = dimensiuniClustere(tab);
	printf("\nClusterele si dimensiunile lor:\n");
	for (int i = 0;i < tab.dim;i++) {
		printf("Clusterul %d are %d conturi\n", dimensiuni[i].index, dimensiuni[i].dimensiune);
	}
	free(dimensiuni);
	dimensiuni = NULL;
	vector = stergerevectorConturi(vector, &dim);
	tab = stergereTabela(tab);
	return 0;
}