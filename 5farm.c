#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

typedef struct Farmacie Farmacie;
struct Farmacie {
	unsigned short int id;
	char* denumire;
	float cifraAfaceri;
	char* sef;
	unsigned char nrAngajati;
	char* localitate;
};

typedef struct Nod Nod;
struct Nod {
	Nod* stanga;
	Nod* dreapta;
	Farmacie info;
};

Nod* inserareArbore(Nod* rad, Farmacie f) {
	if (rad) {
		if (rad->info.id > f.id) {
			rad->stanga = inserareArbore(rad->stanga, f);
		}
		else {
			rad->dreapta = inserareArbore(rad->dreapta, f);
		}
	}
	else {
		Nod* nod = (Nod*)malloc(sizeof(Nod));
		nod->info = f;
		nod->stanga = NULL;
		nod->dreapta = NULL;
		rad = nod;
	}
	return rad;
}

Nod* citireFisier(const char* numeFisier) {
	FILE* file = fopen(numeFisier, "r");
	Nod* rad = NULL;
	char buffer[100];
	char separator[] = ",\n";
	while (fgets(buffer, 100, file)) {
		Farmacie f;
		char* token = strtok(buffer, separator);
		f.id = (unsigned short int)atoi(token);
		token = strtok(NULL, separator);
		f.denumire = (char*)malloc(strlen(token) + 1);
		strcpy(f.denumire, token);
		token = strtok(NULL, separator);
		f.cifraAfaceri = atof(token);
		token = strtok(NULL, separator);
		f.sef = (char*)malloc(strlen(token) + 1);
		strcpy(f.sef, token);
		token = strtok(NULL, separator);
		f.nrAngajati = (unsigned char)atoi(token);
		token = strtok(NULL, separator);
		f.localitate= (char*)malloc(strlen(token) + 1);
		strcpy(f.localitate, token);
		rad = inserareArbore(rad,f);
	}
	fclose(file);
	return rad;
}

void afisareFarmacie(Farmacie f) {
	printf("%d. Farmacia %s din %s condusa de %s are %u angajati si castiga %.2f pe an\n", f.id, f.denumire, f.localitate, f.sef, f.nrAngajati, f.cifraAfaceri);
}

void afisarePreOrdine(Nod* rad) {
	if (rad) {
		afisareFarmacie(rad->info);
		afisarePreOrdine(rad->stanga);
		afisarePreOrdine(rad->dreapta);
	}
}

int numarAngajatiLocalitate(Nod* rad, const char* localitate) {
	if (rad) {
		int total = 0;
		if (strcmp(rad->info.localitate, localitate) == 0) {
			total += rad->info.nrAngajati;
		}
		total += numarAngajatiLocalitate(rad->dreapta, localitate);
		total += numarAngajatiLocalitate(rad->stanga, localitate);
		return total;
	}
	return 0;
}

float cifraAfaceriLunaraDupaId(Nod* rad, unsigned short int id) {
	if (rad) {
		if (rad->info.id > id) {
			return cifraAfaceriLunaraDupaId(rad->stanga, id);
		}
		else {
			if (rad->info.id < id) {
				return cifraAfaceriLunaraDupaId(rad->dreapta, id);
			}
			else {
				return rad->info.cifraAfaceri / 12;
			}
		}
	}
	return 0;
}

Nod* dezalocareNod(Nod* nod) {
	free(nod->info.denumire);
	free(nod->info.localitate);
	free(nod->info.sef);
	free(nod);
	nod = NULL;
	return nod;
}

Nod* stergeFrunzeDupaNivel(Nod* rad, int nivel, int curent) {
	if (rad) {
		if (curent == nivel - 1) {
			if (rad->dreapta != NULL && rad->dreapta->dreapta == NULL && rad->dreapta->stanga == NULL) {
				rad->dreapta = dezalocareNod(rad->dreapta);
			}
			if (rad->stanga != NULL && rad->stanga->dreapta == NULL && rad->stanga->stanga == NULL) {
				rad->stanga = dezalocareNod(rad->stanga);
			}
		}
		else {
			rad->stanga = stergeFrunzeDupaNivel(rad->stanga, nivel, curent + 1);
			rad->dreapta = stergeFrunzeDupaNivel(rad->dreapta, nivel, curent + 1);
		}
	}
	return rad;
}

typedef struct NodL NodL;
struct NodL {
	unsigned short int id;
	NodL* next;
};

NodL* inserareLista(NodL* lista,unsigned short int id) {
	NodL* nod = (NodL*)malloc(sizeof(NodL));
	nod->id = id;
	nod->next = lista;
	return nod;
}

NodL* farmaciiDePeNivel(Nod* rad, int nivel, int curent, NodL* lista) {
	if (rad) {
		if (curent == nivel) {
			lista = inserareLista(lista, rad->info.id);
		}
		else {
			lista = farmaciiDePeNivel(rad->stanga, nivel, curent + 1, lista);
			lista = farmaciiDePeNivel(rad->dreapta, nivel, curent + 1, lista);
		}
	}
	return lista;
}


int main() {
	Nod* rad = citireFisier("5farmacii.txt");
	afisarePreOrdine(rad);
	int total = numarAngajatiLocalitate(rad, "Iasi");
	printf("\nTotal angajati Iasi: %d\n", total);
	float cifra = cifraAfaceriLunaraDupaId(rad, 51);
	printf("\nCifra de afaceri lunara a farmaciei cu id 51: %.2f\n", cifra);
	rad = stergeFrunzeDupaNivel(rad, 2, 0);
	printf("\nDupa stergere:\n");
	afisarePreOrdine(rad);
	NodL* lista = farmaciiDePeNivel(rad, 1, 0, NULL);
	return 0;
}