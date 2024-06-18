#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rezervare Rezervare;
struct Rezervare {
	unsigned int id;
	char* hotel;
	unsigned char nrCamereRez;
	char* client;
	float suma;
};

typedef struct Nod Nod;
struct Nod {
	Rezervare info;
	Nod* dreapta;
	Nod* stanga;
};

Nod* inserareArbore(Nod* rad, Rezervare r) {
	if (rad) {
		if (r.id > rad->info.id) {
			rad->dreapta = inserareArbore(rad->dreapta, r);
		}
		else {
			rad->stanga = inserareArbore(rad->stanga, r);
		}
	}
	else {
		Nod* nod = (Nod*)malloc(sizeof(Nod));
		nod->info = r;
		nod->stanga = NULL;
		nod->dreapta = NULL;
		rad = nod;
	}
	return rad;
}

Nod* citireArboreFisier(const char* numeFisier) {
	FILE* file = fopen(numeFisier, "r");
	Nod* rad = NULL;
	char buffer[100];
	char separator[] = ",\n";
	while (fgets(buffer, 100, file)) {
		Rezervare r;
		char* token = strtok(buffer, separator);
		r.id = (unsigned int)atoi(token);
		token = strtok(NULL, separator);
		r.hotel = (char*)malloc(strlen(token) + 1);
		strcpy(r.hotel, token);
		token = strtok(NULL, separator);
		r.nrCamereRez = (unsigned char)atoi(token);
		token = strtok(NULL, separator);
		r.client = (char*)malloc(strlen(token) + 1);
		strcpy(r.client, token);
		token = strtok(NULL, separator);
		r.suma = atof(token);
		rad = inserareArbore(rad, r);
	}
	fclose(file);
	return rad;
}

void afisareRezervare(Rezervare r) {
	printf("%d. %s a rezervat %d camere la %s - %.2f lei\n", r.id, r.client, r.nrCamereRez, r.hotel, r.suma);
}

void parcurgerePreOrdine(Nod* rad) {
	if (rad) {
		afisareRezervare(rad->info);
		parcurgerePreOrdine(rad->stanga);
		parcurgerePreOrdine(rad->dreapta);
	}
}

float valoareRezDupaClient(Nod* rad, const char* client) {
	if (rad) {
		float val = 0;
		if (strcmp(rad->info.client, client) == 0) {
			val += rad->info.suma;
		}
		val += valoareRezDupaClient(rad->stanga, client);
		val += valoareRezDupaClient(rad->dreapta, client);
		return val;
	}
	return 0;
}

Nod* stergereIdMinim(Nod* rad) {
	if (rad) {
		Nod* aux = rad;
		//daca nu are nimic in stanga inseamna ca minimul e in radacina
		if (!rad->stanga) {
			free(rad->info.client);
			free(rad->info.hotel);
			Nod* aux = rad->dreapta;
			free(rad);
			rad = aux;
		}
		else {
			//minimul va fi in cel mai de jos nod de stanga
			//ma duc pana la penultimul ca sa rup legatura intre el si ultimul
			while (rad->stanga->stanga) {
				rad = rad->stanga;
			}
			free(rad->stanga->info.client);
			free(rad->stanga->info.hotel);
			free(rad->stanga);
			rad->stanga = NULL;
		}
		return aux;
	}
	return NULL;
}

int inaltimeArbore(Nod* rad) {
	if (rad) {
		int hd = inaltimeArbore(rad->dreapta);
		int hs = inaltimeArbore(rad->stanga);
		return 1+(hd > hs ? hd : hs);
	}
	return 0;
}

int getNivel(Nod* rad, int id) {
	if (rad) {
		int nivel = 0;
		if (id > rad->info.id) {
			nivel += 1 + getNivel(rad->dreapta, id);
		}
		else {
			if (id < rad->info.id) {
				nivel += 1 + getNivel(rad->stanga, id);
			}
			else {
				return nivel;
			}
		}
	}
}

void numaraNoduriPeNivel(Nod* rad, int nivel, int* vector) {
	if (rad) {
		vector[nivel]++;
		numaraNoduriPeNivel(rad->stanga, nivel + 1, vector);
		numaraNoduriPeNivel(rad->dreapta, nivel + 1, vector);
	}
}

int* nrNoduriPeFiecareNivel(Nod* rad, int* dim) {
	if (rad) {
		*dim = inaltimeArbore(rad);
		int* vector = (int*)malloc((*dim) * sizeof(int));
	
		for (int i = 0; i < *dim; i++) {
			vector[i] = 0;
		}
		numaraNoduriPeNivel(rad, 0, vector);
		return vector;
	}
	*dim = 0;
	return NULL;
}

Nod* stergereArbore(Nod* rad) {
	if (rad) {
		rad->stanga = stergereArbore(rad->stanga);
		rad->dreapta = stergereArbore(rad->dreapta);
		free(rad->info.client);
		free(rad->info.hotel);
		free(rad);
		rad = NULL;
	}
	return rad;
}

int main() {
	Nod* rad = citireArboreFisier("5rezervari.txt");
	parcurgerePreOrdine(rad);
	float val = valoareRezDupaClient(rad, "Marian");
	printf("\nValoarea totala a rezervarilor lui Marian: %.2f lei\n", val);
	rad = stergereIdMinim(rad);
	printf("\nDupa stergerea nodului minim:\n");
	parcurgerePreOrdine(rad);
	int dim;
	int* vector = nrNoduriPeFiecareNivel(rad, &dim);
	for (int i = 0;i < dim;i++) {
		printf("\nPe nivelul %d sunt %d noduri\n", i, vector[i]);
	}
	rad = stergereArbore(rad);
	if (vector) {
		free(vector);
		vector = NULL;
		dim = 0;
	}
	return 0;
}