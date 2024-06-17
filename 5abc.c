#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct Gunoi Gunoi;
struct Gunoi {
	int id;
	char* tip;
	float capacitate;
};

typedef struct Nod Nod;
struct Nod {
	Nod* stanga;
	Nod* dreapta;
	Gunoi info;
};

Nod* inserareArbore(Nod* rad, Gunoi g) {
	if (rad) {
		if (rad->info.id < g.id) {
			rad->dreapta = inserareArbore(rad->dreapta, g);
		}
		else {
			rad->stanga = inserareArbore(rad->stanga, g);
		}
	}
	else {
		Nod* nod = (Nod*)malloc(sizeof(Nod));
		nod->stanga = NULL;
		nod->dreapta = NULL;
		nod->info = g;

		rad = nod;
	}

	return rad;
}

Nod* citireArboreFisier(const char* numeFisier) {
	FILE* file = fopen(numeFisier, "r");
	Nod* rad = (Nod*)malloc(sizeof(Nod));
	rad = NULL;
	char buffer[100];
	char separator[] = ",\n";
	while (fgets(buffer, 100, file)) {
		Gunoi g;
		char* token = strtok(buffer, separator);
		g.id = atoi(token);
		token = strtok(NULL, separator);
		g.tip = (char*)malloc(strlen(token) + 1);
		strcpy(g.tip, token);
		token = strtok(NULL, separator);
		g.capacitate = atof(token);
		rad = inserareArbore(rad, g);
	}
	fclose(file);
	return rad;
}

void afisareGunoi(Gunoi g) {
	printf("%d. Gunoiul de %s are capacitate de %.2f l\n", g.id, g.tip, g.capacitate);
}

void afisareArborePreOrdine(Nod* rad) {
	if (rad) {
		afisareGunoi(rad->info);
		afisareArborePreOrdine(rad->stanga);
		afisareArborePreOrdine(rad->dreapta);
	}
}

void afisareArborePostOrdine(Nod* rad) {
	if (rad) {
		afisareArborePostOrdine(rad->stanga);
		afisareArborePostOrdine(rad->dreapta);
		afisareGunoi(rad->info);
	}
}

void afisareArboreInOrdine(Nod* rad) {
	if (rad) {
		afisareArboreInOrdine(rad->stanga);
		afisareGunoi(rad->info);
		afisareArboreInOrdine(rad->dreapta);
	}
}

Gunoi cautaDupaId(Nod* rad, int id) {
	if (rad) {
		if (rad->info.id > id) {
			cautaDupaId(rad->stanga, id);
		}
		else {
			if (rad->info.id < id) {
				cautaDupaId(rad->dreapta, id);
			}
			else {
				return rad->info;
			}
		}
	}
	else {
		Gunoi g;
		g.id = -1;
		g.tip = "";
		g.capacitate = 0;
		return g;
	}
}

float calculCapacitateTotala(Nod* rad) {
	int suma = 0;
	if (rad) {
		suma += rad->info.capacitate;
		suma += calculCapacitateTotala(rad->stanga);
		suma += calculCapacitateTotala(rad->dreapta);
	}
	return suma;
}

Nod* stergereArbore(Nod* rad) {
	if (rad) {
		stergereArbore(rad->stanga);
		stergereArbore(rad->dreapta);
		free(rad->info.tip);
		free(rad);
		rad = NULL;
	}
	return rad;
}

int main() {
	Nod* rad = citireArboreFisier("5gunoi.txt");
	afisareArborePostOrdine(rad);

	printf("\nGunoiul cu id 1:\n");
	Gunoi cautat1 = cautaDupaId(rad, 1);
	afisareGunoi(cautat1);

	printf("\nGunoiul cu id 3:\n");
	Gunoi cautat2 = cautaDupaId(rad, 3);
	afisareGunoi(cautat2);

	float t = calculCapacitateTotala(rad);
	printf("\nCapacitatea totala de gunoi: %.2f l\n", t);

	rad = stergereArbore(rad);
	printf("\nArborele dupa stergere:");
	afisareArboreInOrdine(rad);
	return 0;
}