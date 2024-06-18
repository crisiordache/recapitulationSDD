#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct Programare Programare;
struct Programare {
	char* data;
	char* client;
	float durata;
	char* servicii;
	int cost;
};

typedef struct NodSecundar NodSecundar;
typedef struct NodPrincipal NodPrincipal;
struct NodSecundar {
	Programare info;
	NodSecundar* next;
};

struct NodPrincipal {
	char* data;
	NodPrincipal* next;
	NodSecundar* programari;
};

NodSecundar* inserareListaSecundara(NodSecundar* lista, Programare p) {
	NodSecundar* nod = (NodSecundar*)malloc(sizeof(NodSecundar));
	nod->info = p;
	nod->next = lista;
	return nod;
}

NodPrincipal* cautaData(NodPrincipal* lista, const char* data, char* gasit) {
	NodPrincipal* aux = lista;
	while (aux) {
		if (strcmp(aux->data, data) == 0) {
			*gasit = 1;
			return aux;
		}
		aux = aux->next;
	}
	*gasit = 0;
	NodPrincipal* nod = (NodPrincipal*)malloc(sizeof(NodPrincipal));
	nod->data = (char*)malloc(strlen(data) + 1);
	strcpy(nod->data, data);
	nod->next = lista;
	nod->programari = NULL;
	return nod;
}

NodPrincipal* inserareListaPrincipala(NodPrincipal* lista, Programare p) {
	char gasit;
	NodPrincipal* nodData = cautaData(lista, p.data, &gasit);
	nodData->programari = inserareListaSecundara(nodData->programari,p);
	if (gasit == 0) {
		lista = nodData;
	}
	return lista;
}

NodPrincipal* citireFisier(const char* numeFisier) {
	FILE* file = fopen(numeFisier, "r");
	NodPrincipal* lista = NULL;
	char buffer[100];
	char separator[] = ",\n";
	while (fgets(buffer, 100, file)) {
		Programare p;
		char* token = strtok(buffer, separator);
		p.data = (char*)malloc(strlen(token) + 1);
		strcpy(p.data, token);
		token = strtok(NULL, separator);
		p.client = (char*)malloc(strlen(token) + 1);
		strcpy(p.client, token);
		token = strtok(NULL, separator);
		p.durata = atof(token);
		token = strtok(NULL, separator);
		p.servicii = (char*)malloc(strlen(token) + 1);
		strcpy(p.servicii, token);
		token = strtok(NULL, separator);
		p.cost = atoi(token);
		lista = inserareListaPrincipala(lista, p);
	}
	fclose(file);
	return lista;
}

void afisareProgramare(Programare p) {
	printf("\t%s are programare la %s de %.2f ore - %dRON\n", p.client, p.servicii, p.durata, p.cost);
}

void afisareLista(NodPrincipal* lista) {
	while (lista) {
		printf("%s\n", lista->data);
		//trebuie neaparat aux ca sa nu se modifice lista secundara
		NodSecundar* aux = lista->programari;
		while (aux) {
			afisareProgramare(aux->info);
			aux = aux->next;
		}
		lista = lista->next;
	}
}

int valoareProgramariDupaData(NodPrincipal* lista, const char* data) {
	while (lista) {
		if (strcmp(lista->data, data) == 0) {
			int val = 0;
			NodSecundar* aux = lista->programari;
			while (aux) {
				val += aux->info.cost;
				aux = aux->next;
			}
			return val;
		}
		lista = lista->next;
	}
	return 0;
}

void modificaCostDupaClient(NodPrincipal* lista, const char* client, int costNou) {
	while (lista) {
		NodSecundar* aux = lista->programari;
		while (aux) {
			if (strcmp(aux->info.client, client) == 0) {
				aux->info.cost = costNou;
			}
			aux = aux->next;
		}
		lista = lista->next;
	}
}

NodPrincipal* stergereProgramariScurte(NodPrincipal* lista, float durataMin) {
	NodPrincipal* p = lista;
	NodPrincipal* anteriorP = NULL;

	while (p) {
		NodSecundar* curent = p->programari;
		NodSecundar* anterior = NULL;

		while (curent) {
			if (curent->info.durata < durataMin) {
				if (anterior == NULL) {
					p->programari = curent->next;
				}
				else {
					anterior->next = curent->next;
				}
				NodSecundar* deSters = curent;
				curent = curent->next;

				free(deSters->info.data);
				free(deSters->info.client);
				free(deSters->info.servicii);
				free(deSters);
			}
			else {
				anterior = curent;
				curent = curent->next;
			}
		}
		if (p->programari == NULL) {
			if (anteriorP == NULL) {
				NodPrincipal* deSters = p;
				lista = p->next;
				p = p->next;

				free(deSters->data);
				free(deSters);
			}
			else {
				NodPrincipal* deSters = p;
				anteriorP->next = p->next;
				p = p->next;

				free(deSters->data);
				free(deSters);
			}
		}
		else {
			anteriorP = p;
			p = p->next;
		}
	}
	return lista;
}

int main() {
	NodPrincipal* lista = citireFisier("7programari.txt");
	afisareLista(lista);
	int val = valoareProgramariDupaData(lista,"01-01-2024");
	printf("\nIn data de 01-01-2024 s-au incasat %dRON\n", val);
	modificaCostDupaClient(lista, "Maria Ionescu", 1000);
	afisareLista(lista);
	lista = stergereProgramariScurte(lista, 2);
	printf("\nDupa stergere:\n");
	afisareLista(lista);
	return 0;
}
