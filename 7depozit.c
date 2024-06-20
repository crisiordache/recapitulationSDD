#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct Depozit Depozit;
struct Depozit {
	char* id;
	int capacitate; 
	float coordonate[2];
	float procentvd;
};

typedef struct NodPrincipal NodPrincipal;
typedef struct NodSecundar NodSecundar;
struct NodPrincipal {
	NodPrincipal* next;
	Depozit info;
	NodSecundar* vecini;
};

struct NodSecundar {
	NodSecundar* next;
	NodPrincipal* info;
};

NodPrincipal* inserareListaPrincipala(NodPrincipal* graf, Depozit d) {
	NodPrincipal* nod = (NodPrincipal*)malloc(sizeof(NodPrincipal));
	nod->info = d;
	nod->next = NULL;
	nod->vecini = NULL;
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

NodPrincipal* cautaNodPrincipal(NodPrincipal* graf, const char* id) {
	while (graf && strcmp(graf->info.id,id)!=0) {
		graf = graf->next;
	}
	return graf;
}

NodSecundar* inserareListaSecundara(NodSecundar* lista, NodPrincipal* info) {
	NodSecundar* nod = (NodSecundar*)malloc(sizeof(NodSecundar));
	nod->info = info;
	nod->next = NULL;
	if (lista) {
		NodSecundar* aux = lista;
		while (aux->next) {
			aux = aux->next;
		}
		aux->next = nod;
	}
	else {
		lista = nod;
	}
	return lista;
}

void inserareMuchie(NodPrincipal* graf, const char* start, const char* stop) {
	NodPrincipal* nodStart = cautaNodPrincipal(graf, start);
	NodPrincipal* nodStop = cautaNodPrincipal(graf, stop);
	nodStart->vecini = inserareListaSecundara(nodStart->vecini, nodStop);
	nodStop->vecini = inserareListaSecundara(nodStop->vecini, nodStart);
}

NodPrincipal* citireFisier(const char* nume) {
	FILE* file = fopen(nume, "r");
	NodPrincipal* graf = NULL;
	char buffer[100];
	char sep[] = ",\n";
	fgets(buffer, 100, file);
	int nrNoduri = atoi(buffer);
	for (int i = 0;i < nrNoduri;i++) {
		Depozit d;
		fgets(buffer, 100, file);
		char* token = strtok(buffer, sep);
		d.id = (char*)malloc(strlen(token) + 1);
		strcpy(d.id, token);
		token = strtok(NULL, sep);
		d.capacitate = atoi(token);
		token = strtok(NULL, sep);
		d.coordonate[0] = atof(token);
		token = strtok(NULL, sep);
		d.coordonate[1] = atof(token);
		token = strtok(NULL, sep);
		d.procentvd = atof(token);

		graf = inserareListaPrincipala(graf, d);
	}
	fgets(buffer, 100, file);
	int nrMuchii = atoi(buffer);
	for (int i = 0;i < nrMuchii;i++) {
		fgets(buffer, 100, file);
		char* token = strtok(buffer, sep);
		char* start = (char*)malloc(strlen(token) + 1);
		strcpy(start, token);
		token = strtok(NULL, sep);
		char* stop= (char*)malloc(strlen(token) + 1);
		strcpy(stop, token);
		inserareMuchie(graf, start, stop);
	}
	fclose(file);
	return graf;
}

void afisareDepozit(Depozit d) {
	printf("Depozitul %s are capacitate de %d m^3, %.2f %, locatie: %.6f, %.6f\n",d.id,d.capacitate,d.procentvd,d.coordonate[0],d.coordonate[1]);
}

void afisareGraf(NodPrincipal* graf) {
	while (graf) {
		afisareDepozit(graf->info);
		NodSecundar* aux=graf->vecini;
		while (aux) {
			printf("\t");
			afisareDepozit(aux->info->info);
			aux = aux->next;
		}
		graf = graf->next;
	}
}

typedef struct NodStiva NodStiva;
struct NodStiva {
	NodStiva* next;
	char* id;
};

NodStiva* inserareStiva(NodStiva* stiva, const char* id) {
	NodStiva* nod = (NodStiva*)malloc(sizeof(NodStiva));
	nod->id = (char*)malloc(strlen(id) + 1);
	strcpy(nod->id, id);
	nod->next = stiva;
	return nod;
}

char* extragereStiva(NodStiva** stiva) {
	if (*stiva) {
		char* extras = (*stiva)->id;
		NodStiva* aux = (*stiva)->next;
		free(*stiva);
		*stiva = aux;
		return extras;
	}
	else {
		return "-";
	}
}

int getNumarNoduri(NodPrincipal* graf) {
	int nr = 0;
	while (graf) {
		nr++;
		graf = graf->next;
	}
	return nr;
}

void parcurgereAdancime(NodPrincipal* graf, const char* nodPlecare) {
	int nrNoduri = getNumarNoduri(graf);
	char* vizitate = (char*)malloc(nrNoduri);

	for (int i = 0; i < nrNoduri; i++) {
		vizitate[i] = 0;
	}

	NodStiva* stiva = NULL;
	stiva = inserareStiva(stiva, nodPlecare);
	vizitate[atoi(nodPlecare + 1) - 1] = 1;

	while (stiva) {
		char* idNodCurent = extragereStiva(&stiva);
		NodPrincipal* nodCurent = cautaNodPrincipal(graf, idNodCurent);
		printf("%s\n", idNodCurent);
		vizitate[atoi(idNodCurent + 1) - 1] = 1; 

		NodSecundar* vecini = nodCurent->vecini;
		while (vecini) {
			NodPrincipal* vecinInfo = vecini->info;
			if (vizitate[atoi(vecinInfo->info.id + 1) - 1] == 0) {
				vizitate[atoi(vecinInfo->info.id + 1) - 1] = 1;
				stiva = inserareStiva(stiva, vecinInfo->info.id);
			}
			vecini = vecini->next;
		}
	}

	free(vizitate);
}

char** depoziteOcupate(NodPrincipal* graf, int* nr) {
	NodPrincipal* aux = graf;
	*nr = 0;
	while (aux) {
		if (1 - aux->info.procentvd >= 0.6) {
			(*nr)++;
		}
		aux = aux->next;
	}
	char** vector = (char**)malloc((*nr) * sizeof(char*));
	int k = 0;
	aux = graf;
	while (aux) {
		if (1 - aux->info.procentvd >= 0.6) {
			vector[k] = (char*)malloc(strlen(aux->info.id) + 1);
			strcpy(vector[k], aux->info.id);
			k++;
		}
		aux = aux->next;
	}
	return vector;
}


int main() {
	NodPrincipal* graf = citireFisier("7depozite.txt");
	afisareGraf(graf);
	printf("\n\n");
	parcurgereAdancime(graf, "D5");
	int nr;
	char** vector = depoziteOcupate(graf,&nr);
	for (int i = 0;i < nr;i++) {
		printf("%s  ", vector[i]);
	}
	return 0;
}
