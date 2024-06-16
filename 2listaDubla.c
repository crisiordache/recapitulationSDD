#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct Oja Oja;
struct Oja {
	int cod;
	char* culoare;
	float timpUscare;
};

typedef struct Nod Nod;
struct Nod {
	Oja info;
	Nod* next;
	Nod* prev;
};

typedef struct ListaDubla ListaD;
struct ListaDubla {
	Nod* prim;
	Nod* ultim;
};

ListaD inserareInceput(ListaD ld, Oja o) {
	Nod* nod = (Nod*)malloc(sizeof(Nod));
	nod->info = o;
	nod->prev = NULL;
	nod->next = ld.prim;
	if (ld.prim != NULL) {
		ld.prim->prev = nod;
	}
	else {
		ld.ultim = nod;
	}
	ld.prim = nod;
	return ld;
}

ListaD inserareSfarsit(ListaD ld, Oja o) {
	Nod* nod = (Nod*)malloc(sizeof(Nod));
	nod->info = o;
	nod->prev = ld.ultim;
	nod->next = NULL;
	if (ld.ultim != NULL) {
		ld.ultim->next = nod;
	}
	else {
		ld.prim = nod;
	}
	ld.ultim = nod;
	return ld;
}

ListaD citireListaDublaDinFisier(const char* numeFisier) {
	ListaD ld;
	ld.prim = NULL;
	ld.ultim = NULL;
	FILE* file = fopen(numeFisier, "r");
	char buffer[100];
	char separator[] = ",\n";
	while (fgets(buffer, 100, file)!=NULL) {
		Oja o;
		char* token = strtok(buffer, separator);
		o.cod = atoi(token);
		token = strtok(NULL, separator);
		o.culoare = (char*)malloc(strlen(token) + 1);
		strcpy(o.culoare, token);
		token = strtok(NULL, separator);
		o.timpUscare = atof(token);
		ld = inserareSfarsit(ld, o);
	}
	fclose(file);
	return ld;
}

void afisareOja(Oja o) {
	printf("Oja %d - %s, timp de uscare: %.2f\n", o.cod, o.culoare, o.timpUscare);
}

void afisareListaDublaDeLaInceput(ListaD ld) {
	while (ld.prim) {
		afisareOja(ld.prim->info);
		ld.prim = ld.prim->next;
	}
}

void afisareListaDublaDeLaSfarsit(ListaD ld) {
	while (ld.ultim) {
		afisareOja(ld.ultim->info);
		ld.ultim = ld.ultim->prev;
	}
}

void stergereListaDubla(ListaD* ld) {
	while (ld->prim) {
		Nod* aux = ld->prim->next;
		free(ld->prim->info.culoare);
		free(ld->prim);
		ld->prim = aux;
	}
	ld->prim = NULL;
	ld->ultim = NULL;
}

int main() {
	ListaD ld = citireListaDublaDinFisier("2oje.txt");
	afisareListaDublaDeLaInceput(ld);
	afisareListaDublaDeLaSfarsit(ld);
	stergereListaDubla(&ld);
	printf("\nDupa stergere:");
	afisareListaDublaDeLaInceput(ld);
	return 0;
}