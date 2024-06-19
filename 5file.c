#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

typedef struct File File;
struct File {
	char* identifier;
	unsigned int size;
	char readOnly;
	char date[10];
	char* owner;
};

typedef struct Nod Nod;
struct Nod {
	Nod* stanga;
	Nod* dreapta;
	File info;
};

Nod* inserareArbore(Nod* rad, File f) {
	if (rad) {
		if (strcmp(rad->info.identifier, f.identifier) > 0) {
			rad->stanga = inserareArbore(rad->stanga, f);
		}
		else {
			rad->dreapta = inserareArbore(rad->dreapta, f);
		}
	}
	else {
		Nod* nod = (Nod*)malloc(sizeof(Nod));
		nod->dreapta = NULL;
		nod->stanga = NULL;
		nod->info = f;
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
		File f;
		char* token = strtok(buffer, separator);
		f.identifier = (char*)malloc(strlen(token) + 1);
		strcpy(f.identifier, token);
		token = strtok(NULL, separator);
		f.size = (unsigned int)atoi(token);
		token = strtok(NULL, separator);
		f.readOnly = token[0];
		token = strtok(NULL, separator);
		strcpy(f.date, token);
		token = strtok(NULL, separator);
		f.owner = (char*)malloc(strlen(token) + 1);
		strcpy(f.owner, token);
		rad = inserareArbore(rad, f);
	}
	fclose(file);
	return rad;
}

void afisareFile(File f) {
	printf("Identifier: %s\n", f.identifier);
	printf("Size: %u bytes\n", f.size);
	printf("Read-Only: %s\n", f.readOnly == '1' ? "Yes" : "No");
	printf("Date: %s\n", f.date);
	printf("Owner: %s\n", f.owner);
	printf("\n");
}

void afisarePreOrdine(Nod* rad) {
	if (rad) {
		afisareFile(rad->info);
		afisarePreOrdine(rad->stanga);
		afisarePreOrdine(rad->dreapta);
	}
}

int comparaDate(const char* data1, const char* data2) {
	int zi1, luna1, an1;
	int zi2, luna2, an2;

	sscanf(data1, "%d-%d-%d", &zi1, &luna1, &an1);
	sscanf(data2, "%d-%d-%d", &zi2, &luna2, &an2);

	if (an1 != an2) return an1 - an2;
	if (luna1 != luna2) return luna1 - luna2;
	return zi1 - zi2;
}

void afisareFisiereDataMaiMare(Nod* rad, char data[10]) {
	if (rad) {
		if (comparaDate(rad->info.date,data)>0) {
			afisareFile(rad->info);
		}
		afisareFisiereDataMaiMare(rad->stanga, data);
		afisareFisiereDataMaiMare(rad->dreapta, data);
	}
}

int numarFisiereReadOnly(Nod* rad) {
	if (rad) {
		int nr = 0;
		if (rad->info.readOnly == '1') {
			nr++;
		}
		nr += numarFisiereReadOnly(rad->stanga);
		nr += numarFisiereReadOnly(rad->dreapta);
		return nr;
	}
	return 0;
}

int numarFisier(Nod* rad) {
	if (rad) {
		return 1 + numarFisier(rad->stanga) + numarFisier(rad->dreapta);
	}
	return 0;
}

unsigned int sumaDimensiuni(Nod* rad) {
	if (rad) {
		return rad->info.size + sumaDimensiuni(rad->stanga) + sumaDimensiuni(rad->dreapta);
	}
	return 0;
}

float dimensiuneaMedie(Nod* rad) {
	int nrFisiere = numarFisier(rad);
	unsigned int suma = sumaDimensiuni(rad);
	return nrFisiere > 0 ? (float)suma / nrFisiere : 0.0;
}

File cautaDupaID(Nod* rad, const char* id) {
	if (rad) {
		rad->info.size *= 2;
		if (strcmp(rad->info.identifier, id) == 0) {
			return rad->info;
		}
		else {
			if (strcmp(rad->info.identifier, id) > 0) {
				return cautaDupaID(rad->stanga, id);
			}
			else {
				return cautaDupaID(rad->dreapta, id);
			}
		}
	}
	File f;
	f.identifier = "";
	strcpy(f.date, "-");
	f.size = -1;
	f.owner = "";
	f.readOnly = '0';
	return f;
}

typedef struct Heap Heap;
struct Heap {
	File* vector;
	int dim;
};

File initializareFile(const char* id, unsigned int size, char readOnly, char date[10], const char* owner) {
	File f;
	f.identifier = (char*)malloc(strlen(id) + 1);
	strcpy(f.identifier, id);
	f.size = size;
	f.readOnly = readOnly;
	strcpy(f.date, date);
	f.owner = (char*)malloc(strlen(owner) + 1);
	strcpy(f.owner, owner);
	return f;
}

Heap filtrare(Heap heap, int pozRad) {
	int fs = pozRad * 2 + 1;
	int fd = pozRad * 2 + 2;
	int pozMax = pozRad;

	if (fs<heap.dim && heap.vector[fs].size>heap.vector[pozMax].size) {
		pozMax = fs;
	}

	if (fd<heap.dim && heap.vector[fd].size>heap.vector[pozMax].size) {
		pozMax = fd;
	}

	if (pozMax != pozRad) {
		File aux = heap.vector[pozRad];
		heap.vector[pozRad] = heap.vector[pozMax];
		heap.vector[pozMax] = aux;

		if (pozMax < (heap.dim - 2) / 2) {
			heap = filtrare(heap, pozMax);
		}
	}

	return heap;
}

Heap inserareHeap(Heap heap, File f) {
	heap.dim++;
	File* aux = (File*)malloc(heap.dim * sizeof(File));
	aux[0] = f;
	for (int i = 1;i < heap.dim;i++) {
		aux[i] = heap.vector[i - 1];
	}
	free(heap.vector);
	heap.vector = aux;
	heap = filtrare(heap, 0);
	return heap;
}


Heap conversieHeapRecursiv(Nod* rad, Heap heap) {
	if (rad) {
		File f = initializareFile(rad->info.identifier, rad->info.size, rad->info.readOnly, rad->info.date, rad->info.owner);
		heap = inserareHeap(heap, f);
		heap = conversieHeapRecursiv(rad->stanga, heap);
		heap = conversieHeapRecursiv(rad->dreapta, heap);
	}
	return heap;
}

Heap conversieHeap(Nod* rad) {
	Heap heap;
	heap.dim = 0;
	heap.vector = NULL;
	return conversieHeapRecursiv(rad, heap);
}



Nod* stergereArbore(Nod* rad) {
	if (rad) {
		rad->stanga = stergereArbore(rad->stanga);
		rad->dreapta = stergereArbore(rad->dreapta);
		free(rad->info.identifier);
		free(rad->info.owner);
		free(rad);
		rad = NULL;
	}
	return rad;
}

Heap stergereHeap(Heap heap) {
	for (int i = 0;i < heap.dim;i++) {
		free(heap.vector[i].identifier);
		free(heap.vector[i].owner);
	}
	free(heap.vector);
	heap.vector = NULL;
	heap.dim = 0;
	return heap;
}

int main() {
	Nod* rad = citireFisier("5files.txt");
	afisarePreOrdine(rad);
	printf("Fisierele cu data mai mare decat 01-04-2023\n");
	afisareFisiereDataMaiMare(rad, "01-04-2023");
	int nr = numarFisiereReadOnly(rad);
	printf("Fisiere read only: %d\n", nr);
	float medie = dimensiuneaMedie(rad);
	printf("Dimensiunea medie a fisierelor: %.2f B", medie);
	File cautat = cautaDupaID(rad, "file10.mp4");
	printf("\n\nFisierul cautat:\n");
	afisareFile(cautat);
	printf("Arborele modificat:\n");
	afisarePreOrdine(rad);

	printf("\n\n\nHeap:\n");
	Heap heap = conversieHeap(rad);
	for (int i = 0;i < heap.dim;i++) {
		afisareFile(heap.vector[i]);
	}

	rad = stergereArbore(rad);
	heap = stergereHeap(heap);
	return 0;
}