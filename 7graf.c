#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

typedef struct Structura Structura;
struct Structura {
	int id;
	char* nume;
};

typedef struct NodSecundar NodSecundar;
typedef struct NodPrincipal NodPrincipal;
struct NodSecundar {
	NodSecundar* next;
	NodPrincipal* info;
};

struct NodPrincipal {
	Structura info;
	NodPrincipal* next;
	NodSecundar* vecini;
};

NodPrincipal* inserareListaPrincipala(NodPrincipal* graf, Structura s) {
	NodPrincipal* nod = (NodPrincipal*)malloc(sizeof(NodPrincipal));
	nod->info = s;
	nod->vecini = NULL;
	nod->next = NULL;
	if (graf) {
		NodPrincipal* aux = graf;
		while (aux->next) {
			aux = aux->next;
		}
		aux->next = nod;
	}
	else {
		graf = nod;
	}
	return graf;
}

NodPrincipal* cautaNod(NodPrincipal* graf, int id) {
	while (graf && graf->info.id != id) {
		graf = graf->next;
	}
	return graf;
}

NodSecundar* inserareSecundara(NodSecundar* cap, NodPrincipal* info) {
	NodSecundar* nod = (NodSecundar*)malloc(sizeof(NodSecundar));
	nod->next = NULL;
	nod->info = info;
	if (cap) {
		NodSecundar* aux = cap;
		while (aux->next) {
			aux = aux->next;
		}
		aux->next = nod;
	}
	else {
		cap = nod;
	}
	return cap;
}

void inserareMuchie(NodPrincipal* graf, int start, int stop) {
	NodPrincipal* nodStart = cautaNod(graf, start);
	NodPrincipal* nodStop = cautaNod(graf, stop);
	nodStart->vecini = inserareSecundara(nodStart->vecini, nodStop);
	nodStop->vecini = inserareSecundara(nodStop->vecini, nodStart);
}

NodPrincipal* citireGrafFisier(const char* numeFisier) {
	FILE* file = fopen(numeFisier, "r");
	NodPrincipal* graf = NULL;
	char buffer[100];
	char separator[] = ",\n";

	fgets(buffer, 100, file);
	int nrNoduri = atoi(buffer);

	for (int i = 0; i < nrNoduri; i++) {
		Structura s;
		fgets(buffer, 100, file);
		char* token = strtok(buffer, separator);
		s.id = atoi(token);
		token = strtok(NULL, separator);
		s.nume = (char*)malloc(strlen(token) + 1);
		strcpy(s.nume, token);
		graf = inserareListaPrincipala(graf, s);
	}

	fgets(buffer, 100, file);
	int nrMuchii = atoi(buffer);

	for (int i = 0; i < nrMuchii; i++) {
		fgets(buffer, 100, file);
		char* token = strtok(buffer, separator);
		int start = atoi(token);
		token = strtok(NULL, separator);
		int stop = atoi(token);
		inserareMuchie(graf, start, stop);
	}

	fclose(file);
	return graf;
}

void afisareStructura(Structura s) {
	printf("%d. %s\n", s.id, s.nume);
}

typedef struct NodCoada NodCoada;
struct NodCoada {
	int id;
	NodCoada* next;
};

NodCoada* inserareCoada(NodCoada* coada, int id) {
	NodCoada* nod = (NodCoada*)malloc(sizeof(NodCoada));
	nod->id = id;
	nod->next = NULL;
	if (coada) {
		NodCoada* aux = coada;
		while (aux->next) {
			aux = aux->next;
		}
		aux->next = nod;
	}
	else {
		coada = nod;
	}
	return coada;
}

int extragereCoada(NodCoada** coada) {
	int id = -1;
	if (*coada) {
		id = (*coada)->id;
		NodCoada* aux = (*coada)->next;
		free(*coada);
		(*coada) = aux;
	}
	return id;
}

int getNrNoduri(NodPrincipal* graf) {
	int nr = 0;
	while (graf) {
		nr++;
		graf = graf->next;
	}
	return nr;
}

void parcurgereLatime(NodPrincipal* graf, int nodPlecare) {
	NodCoada* coada = NULL;
	int nrNoduri = getNrNoduri(graf);
	char* vizitate = (char*)malloc(nrNoduri);
	for (int i = 0;i < nrNoduri;i++) {
		vizitate[i] = 0;
	}
	coada = inserareCoada(coada, nodPlecare);
	vizitate[nodPlecare];
	while (coada) {
		int idNodCurent = extragereCoada(&coada);
		NodPrincipal* nodCurent = cautaNod(graf, idNodCurent);
		afisareStructura(nodCurent->info);
		vizitate[nodCurent->info.id] = 1;
		NodSecundar* vecini = nodCurent->vecini;
		while (vecini) {
			if (vizitate[vecini->info->info.id] == 0) {
				vizitate[vecini->info->info.id] = 1;
				coada = inserareCoada(coada, vecini->info->info.id);
			}
			vecini = vecini->next;
		}
	}
	if (vizitate) {
		free(vizitate);
		vizitate = NULL;
	}
}

NodCoada* inserareStiva(NodCoada* stiva, int id) {
	NodCoada* nod = (NodCoada*)malloc(sizeof(NodCoada));
	nod->id = id;
	nod->next = stiva;
	stiva = nod;
	return stiva;
}

int extragereStiva(NodCoada** stiva) {
	return extragereCoada(stiva);
}

void parcurgereAdancime(NodPrincipal* graf, int nodPlecare) {
	NodCoada* stiva = NULL;
	int nrNoduri = getNrNoduri(graf);
	char* vizitate = (char*)malloc(nrNoduri);
	for (int i = 0;i < nrNoduri;i++) {
		vizitate[i] = 0;
	}
	stiva = inserareStiva(stiva, nodPlecare);
	vizitate[nodPlecare] = 1;
	while (stiva) {
		int idNodCurent = extragereStiva(&stiva);
		NodPrincipal* nodCurent = cautaNod(graf, idNodCurent);
		afisareStructura(nodCurent->info);
		NodSecundar* veciniCurenti = nodCurent->vecini;
		while (veciniCurenti) {
			if (vizitate[veciniCurenti->info->info.id] == 0) {
				stiva = inserareStiva(stiva, veciniCurenti->info->info.id);
				vizitate[veciniCurenti->info->info.id] = 1;
			}
			veciniCurenti = veciniCurenti->next;
		}
	}
	if (vizitate) {
		free(vizitate);
		vizitate = NULL;
	}
}

void parcurgereCompleta(NodPrincipal* graf) {
	while (graf) {
		afisareStructura(graf->info);
		while (graf->vecini) {
			printf("\t");
			afisareStructura(graf->vecini->info->info);
			graf->vecini = graf->vecini->next;
		}
		graf = graf->next;
	}
}

NodSecundar* stergereListaSecundara(NodSecundar* ls) {
	while (ls) {
		NodSecundar* aux = ls->next;
		free(ls);
		ls = aux;
	}
	ls = NULL;
	return ls;
}

NodPrincipal* stergereGraf(NodPrincipal* graf) {
	while (graf) {
		NodPrincipal* aux = graf->next;
		free(graf->info.nume);
		graf->vecini = stergereListaSecundara(graf->vecini);
		free(graf);
		graf = aux;
	}
	graf = NULL;
	return graf;
}

int main() {
	NodPrincipal* graf = citireGrafFisier("7structuri.txt");
	parcurgereLatime(graf, 1);
	printf("\n\n");
	parcurgereAdancime(graf, 4);
	printf("\n\n");
	parcurgereCompleta(graf);
	stergereGraf(graf);
	return 0;
}