#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

typedef struct Dosar Dosar;
struct Dosar {
	int cod;
	char* nume;
	char* program;
	float medie;
};

typedef struct Nod Nod;
struct Nod {
	Nod* next;
	Dosar info;
};

typedef struct HashTable HashTable;
struct HashTable {
	Nod** vector;
	int dim;
};

HashTable initializareHashTable(int dim) {
	HashTable tab;
	tab.dim = dim;
	if (dim > 0) {
		tab.vector = (Nod**)malloc(dim*sizeof(Nod*));
		for (int i = 0;i < dim;i++) {
			tab.vector[i] = NULL;
		}
	}
	else {
		tab.vector = NULL;
	}
	return tab;
}

int functieHash(const char* nume, int dim) {
	if (dim>0) {
		return strlen(nume) % dim;
	}
	else {
		return -1;
	}
}

Nod* inserareCluster(Nod* cluster, Dosar d) {
	Nod* nod = (Nod*)malloc(sizeof(Nod));
	nod->info = d;
	nod->next = NULL;
	if (cluster) {
		Nod* aux = cluster;
		while (aux->next) {
			aux = aux->next;
		}
		aux->next = nod;
		return cluster;
	}
	return nod;
}


HashTable inserareTabela(HashTable tab, Dosar d) {
	int poz = functieHash(d.nume, tab.dim);
	tab.vector[poz] = inserareCluster(tab.vector[poz], d);
	return tab;
}

HashTable citireFisier(const char* numeF) {
	FILE* f = fopen(numeF, "r");
	HashTable tab = initializareHashTable(5);
	char buffer[100];
	char separator[] = ",\n";
	while (fgets(buffer, 100, f)) {
		Dosar d;
		char* token = strtok(buffer, separator);
		d.cod = atoi(token);
		token = strtok(NULL, separator);
		d.nume = (char*)malloc(strlen(token) + 1);
		strcpy(d.nume, token);
		token = strtok(NULL, separator);
		d.program = (char*)malloc(strlen(token) + 1);
		strcpy(d.program, token);
		token = strtok(NULL, separator);
		d.medie = atof(token);
		tab = inserareTabela(tab, d);
	}
	fclose(f);
	return tab;
}

void afiseazaDosar(Dosar d) {
	printf("\t%d. %s de la %s are media %.2f\n", d.cod, d.nume, d.program, d.medie);
}

void afiseazaCluster(Nod* cluster) {
	while (cluster) {
		afiseazaDosar(cluster->info);
		cluster = cluster->next;
	}
}

void afiseazaTabela(HashTable tab) {
	for (int i = 0;i < tab.dim;i++) {
		printf("Cluster %d\n", i);
		afiseazaCluster(tab.vector[i]);
	}
}

int numarCandidatiProgram(HashTable tab, const char* program) {
	int nr = 0;
	for (int i = 0;i < tab.dim;i++) {
		Nod* aux = tab.vector[i];
		while (aux) {
			if (strcmp(aux->info.program, program) == 0) {
				nr++;
			}
			aux = aux->next;
		}
	}
	return nr;
}

typedef struct NodPrincipal NodPrincipal;
struct NodPrincipal {
	char* program;
	NodPrincipal* next;
	Nod* candidati;
};

NodPrincipal* cautaNodPrincipal(NodPrincipal* lista, const char* program, char* gasit) {
	NodPrincipal* aux = lista;
	*gasit = 0;
	while (lista) {
		if (strcmp(lista->program, program) == 0) {
			*gasit = 1;
			return lista;
		}
		lista = lista->next;
	}
	if (*gasit==0) {
		NodPrincipal* nod = (NodPrincipal*)malloc(sizeof(NodPrincipal));
		nod->program = (char*)malloc(strlen(program) + 1);
		strcpy(nod->program, program);
		nod->next = aux;
		nod->candidati = NULL;
		return nod;
	}
}

NodPrincipal* inserareListaPrincipala(NodPrincipal* lista, Dosar d) {
	char gasit;
	NodPrincipal* nodGasit = cautaNodPrincipal(lista, d.program, &gasit);
	if (gasit == 0) {
		lista = nodGasit;
	}
	nodGasit->candidati = inserareCluster(nodGasit->candidati, d);
	return lista;
}

NodPrincipal* conversieListaDeListe(HashTable tab) {
	NodPrincipal* lista = NULL;
	for (int i = 0;i < tab.dim;i++) {
		Nod* aux = tab.vector[i];
		while (aux) {
			if (aux->info.medie < 9) {
				Dosar d;
				d.cod = aux->info.cod;
				d.medie = aux->info.medie;
				d.nume = (char*)malloc(strlen(aux->info.nume) + 1);
				strcpy(d.nume, aux->info.nume);
				d.program = (char*)malloc(strlen(aux->info.program) + 1);
				strcpy(d.program, aux->info.program);
				lista = inserareListaPrincipala(lista, d);
			}
			aux = aux->next;
		}
	}
	return lista;
}

void afisareListaListe(NodPrincipal* lista) {
	int i = 0;
	while (lista) {
		printf("Nodul %d\n", i);
		i++;
		afiseazaCluster(lista->candidati);
		lista = lista->next;
	}
}

float ceaMaiMicaMedie(NodPrincipal* lista, char** program) {
    float min = 11;
    while (lista) {
        Nod* aux = lista->candidati;
        while (aux) {
            if (aux->info.medie < min) {
                min = aux->info.medie;
                *program = lista->program;
            }
            aux = aux->next;
        }
        lista = lista->next;
    }
    return min;
}

Nod* stergereNod(Nod* lista, float medie) {
	Nod* curent = lista;
	Nod* prev = NULL;
	while (curent) {
		if (curent->info.medie == medie) {
			if (prev) {
				prev->next = curent->next;
			}
			else {
				lista = curent->next;
			}
			free(curent->info.nume);
			free(curent->info.program);
			free(curent);
			break;
		}
		prev = curent;
		curent = curent->next;
	}
	return lista;
}

NodPrincipal* stergereMin(NodPrincipal* lista) {
	char* program;
	float min = ceaMaiMicaMedie(lista, &program);
	char gasit;
	NodPrincipal* nodP = cautaNodPrincipal(lista, program, &gasit);
	nodP->candidati = stergereNod(nodP->candidati, min);
	return lista;
}

Dosar stergereDosa(Dosar d) {
	free(d.nume);
	free(d.program);
	return d;
}

Nod* stergereListaSec(Nod* lista) {
	while (lista) {
		lista->info = stergereDosa(lista->info);
		Nod* aux = lista->next;
		free(lista);
		lista = aux;
	}
	lista = NULL;
	return lista;
}

HashTable stergereTabela(HashTable tab) {
	for (int i = 0;i < tab.dim;i++) {
		tab.vector[i] = stergereListaSec(tab.vector[i]);
	}
	free(tab.vector);
	tab.vector = NULL;
	tab.dim = 0;
	return tab;
}

NodPrincipal* stergereListaL(NodPrincipal* lista) {
	while (lista) {
		lista->candidati = stergereListaSec(lista->candidati);
		free(lista->program);
		Nod* aux = lista->next;
		free(lista);
		lista = aux;
	}
	lista = NULL;
	return lista;
}

int main() {
	HashTable tab = citireFisier("3dosare.txt");
	afiseazaTabela(tab);
	int nr = numarCandidatiProgram(tab, "Informatica");
	printf("\nCandidati la Informatica: %d\n\n", nr);
	NodPrincipal* lista = conversieListaDeListe(tab);
	afisareListaListe(lista);
	printf("\n\n");
	lista = stergereMin(lista);
	afisareListaListe(lista);
	tab = stergereTabela(tab);
	lista = stergereListaL(lista);
	afisareListaListe(lista);
	afiseazaTabela(tab);
	return 0;
}