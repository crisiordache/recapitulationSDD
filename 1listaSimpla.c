#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

typedef struct Melodie Melodie;
struct Melodie {
	int id;
	char* artist;
	char* nume;
	float durata;
};

typedef struct Nod Nod;
struct Nod {
	Melodie info;
	Nod* next;
};

Nod* inserareaInceput(Nod* lista, Melodie m) {
	Nod* nod = (Nod*)malloc(sizeof(Nod));
	nod->info = m;
	nod->next = lista;
	return nod;
}

Nod* inserareSfarsit(Nod* lista, Melodie m) {
	Nod* nod = (Nod*)malloc(sizeof(Nod));
	nod->info = m;
	nod->next = NULL;
	if (lista == NULL) {
		lista = nod;
	}
	else {
		Nod* aux = lista;
		while (aux->next) {
			aux = aux->next;
		}
		aux->next = nod;
	}
	return lista;
}

Nod* citireListaDinFisier(const char* fisier) {
	Nod* lista = NULL;
	FILE* f = fopen(fisier, "r");
	char buffer[100];
	char separator[] = ",\n";
	while (fgets(buffer, 100, f) != NULL) {
		char* token = strtok(buffer, separator);
		Melodie m;
		m.id = atoi(token);
		token = strtok(NULL, separator);
		m.artist = (char*)malloc(strlen(token) + 1);
		strcpy(m.artist, token);
		token = strtok(NULL, separator);
		m.nume = (char*)malloc(strlen(token) + 1);
		strcpy(m.nume, token);
		token = strtok(NULL, separator);
		m.durata = atof(token);
		lista = inserareSfarsit(lista, m);
	}
	fclose(f);
	return lista;
}

void afisareMelodie(Melodie m) {
	printf("%d. %s, de %s are %.2f secunde\n", m.id, m.nume, m.artist, m.durata);
}

void afisareLista(Nod* lista) {
	while (lista) {
		afisareMelodie(lista->info);
		lista = lista->next;
	}
}

float durataTotala(Nod* lista) {
	float total = 0;
	while (lista) {
		total += lista->info.durata;
		lista = lista->next;
	}
	return total;
}

Melodie initializareMelodie(int id, const char* nume, const char* artist, float durata) {
	Melodie m;
	m.id = id;
	m.nume = (char*)malloc(strlen(nume) + 1);
	strcpy(m.nume, nume);
	m.artist = (char*)malloc(strlen(artist) + 1);
	strcpy(m.artist, artist);
	m.durata = durata;
	return m;
}

Melodie cautaDupaId(Nod* lista, int id) {
	while (lista) {
		if (lista->info.id == id) {
			//deep copy
			return initializareMelodie(lista->info.id, lista->info.nume, lista->info.artist, lista->info.durata);
		}
		lista = lista->next;
	}
	return initializareMelodie(-1, "-", "-", 0);
}

Melodie* inserareVector(Melodie* vector, int nr, Melodie m) {
	Melodie* vectorNou = (Melodie*)malloc(nr * sizeof(Melodie));
	for (int i = 0;i < nr - 1;i++) {
		//shallow copy
		vectorNou[i] = vector[i];
	}
	vectorNou[nr - 1] = m;
	free(vector);
	vector = NULL;
	return vectorNou;
}

Melodie* getAlbum(Nod* lista, const char* artist, int* nr) {
	*nr = 0;
	Melodie* vector = NULL;
	while (lista) {
		if (strcmp(lista->info.artist, artist) == 0) {
			(*nr)++;
			vector = inserareVector(vector, *nr, lista->info);
		}
		lista = lista->next;
	}
	//sortare vector dupa durata
	for (int i = 0;i < (*nr)-1;i++) {
		if (vector[i].durata > vector[i + 1].durata) {
			Melodie aux = vector[i];
			vector[i] = vector[i + 1];
			vector[i + 1] = aux;
		}
	}
	return vector;
}

void stergereLista(Nod** lista) {
	while ((*lista) != NULL) {
		free((*lista)->info.artist);
		free((*lista)->info.nume);
		Nod* aux = (*lista)->next;
		free(*lista);
		*lista = aux;
	}
	*lista = NULL;
}

int main() {
	Nod* lista = citireListaDinFisier("1melodii.txt");
	afisareLista(lista);
	float total = durataTotala(lista);
	printf("\nDurata totala este %.2f\n", total);
	Melodie m = cautaDupaId(lista, 3);
	printf("\nMelodia cu id-ul 3:\n");
	afisareMelodie(m);
	int nr;
	Melodie* album = getAlbum(lista, "Sabrina Carpenter", &nr);
	printf("\nAlbum:\n");
	for (int i = 0;i < nr;i++) {
		afisareMelodie(album[i]);
	}
	stergereLista(&lista);
	printf("\nLista dupa stergere: \n");
	afisareLista(lista);
	free(album);
	album = NULL;
	free(m.artist);
	m.artist = NULL;
	free(m.nume);
	m.nume = NULL;
	return 0;
}