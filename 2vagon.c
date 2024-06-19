#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

typedef struct Vagon Vagon;
struct Vagon {
	int numarVagon;
	char* firmaTransport;
	int numarBileteVandute;
	int capacitateVagon;
};

typedef struct Nod Nod;
struct Nod {
	Nod* next;
	Nod* prev;
	Vagon info;
};

typedef struct ListaDubla ListaD;
struct ListaDubla {
	Nod* prim;
	Nod* ultim;
};

ListaD inserareLista(ListaD lista, Vagon v) {
	Nod* nod = (Nod*)malloc(sizeof(Nod));
	nod->info = v;
	nod->prev = NULL;
	nod->next = lista.prim;
	if (lista.prim) {
		lista.prim->prev = nod;
	}
	else {
		lista.ultim = nod;
	}
	lista.prim = nod;
	return lista;
}

ListaD citireFisier(const char* numeFisier) {
	FILE* file = fopen(numeFisier, "r");
	ListaD lista;
	lista.prim = NULL;
	lista.ultim = NULL;
	char buffer[100];
	char separator[] = ",\n";
	while (fgets(buffer, 100, file)) {
		Vagon v;
		char* token = strtok(buffer, separator);
		v.numarVagon = atoi(token);
		token = strtok(NULL, separator);
		v.firmaTransport = (char*)malloc(strlen(token) + 1);
		strcpy(v.firmaTransport, token);
		token = strtok(NULL, separator);
		v.numarBileteVandute = atoi(token);
		token = strtok(NULL, separator);
		v.capacitateVagon = atoi(token);
		lista = inserareLista(lista, v);
	}
	fclose(file);
	return lista;
}

void afisareVagon(Vagon v) {
	printf("Vagonul %d de la %s are capacitate de %d calatori si %d bilete vandute\n", v.numarVagon, v.firmaTransport, v.capacitateVagon, v.numarBileteVandute);
}

void afisareDeLaInceput(ListaD lista) {
	while (lista.prim) {
		afisareVagon(lista.prim->info);
		lista.prim = lista.prim->next;
	}
}

void afisareDeLaSfarsit(ListaD lista) {
	while (lista.ultim) {
		afisareVagon(lista.ultim->info);
		lista.ultim = lista.ultim->prev;
	}
}

ListaD stergereMinim(ListaD lista) {
	Nod* aux = lista.prim->next;
	int min = lista.prim->info.numarBileteVandute;
	while (aux) {
		if (aux->info.numarBileteVandute < min) {
			min = aux->info.numarBileteVandute;
		}
		aux = aux->next;
	}
	Nod* nod = lista.prim;
	while (nod) {
		if (nod->info.numarBileteVandute == min) {
			if (nod->prev == NULL) {
				if (nod->next == NULL) {
					lista.prim = NULL;
					lista.ultim = NULL;
				}
				else {
					nod->next->prev = NULL;
					lista.prim = nod->next;
				}
			}
			else {
				if (nod->next == NULL) {
					nod->prev->next = NULL;
					lista.ultim = NULL;
				}
				else {
					nod->prev->next = nod->next;
					nod->next->prev = nod->prev;
				}
			}
			free(nod->info.firmaTransport);
			free(nod);
			nod = NULL;
		}
		else {
			nod = nod->next;
		}
	}
	return lista;
}

typedef struct MinHeap Heap;
struct MinHeap {
	Vagon* vector;
	int dim;
};

float gradOcupare(Vagon v) {
	return (float)v.numarBileteVandute / v.capacitateVagon;
}


Heap filtrareHeap(Heap heap, int rad) {
	int fs = rad * 2 + 1;
	int fd = rad * 2 + 2;
	int min = rad;
	if (fs < heap.dim && gradOcupare(heap.vector[fs]) < gradOcupare(heap.vector[min])) {
		min = fs;
	}
	if (fd < heap.dim && gradOcupare(heap.vector[fd]) < gradOcupare(heap.vector[min])) {
		min = fd;
	}
	if (min != rad) {
		Vagon aux = heap.vector[min];
		heap.vector[min] = heap.vector[rad];
		heap.vector[rad] = aux;

		if (min <= (heap.dim - 2) / 2) {
			heap = filtrareHeap(heap,min);
		}
	}
	return heap;
}

Heap initializareHeap(int dim) {
	Heap heap;
	heap.dim = dim;
	heap.vector = (Vagon*)malloc(dim * sizeof(Vagon));
	return heap;
}

int nrNoduri(ListaD lista) {
	int nr = 0;
	while (lista.prim) {
		nr++;
		lista.prim = lista.prim->next;
	}
	return nr;
}

Vagon initializareVagon(int numar, int capacitate, int bileteV, const char* firma) {
	Vagon v;
	v.numarVagon = numar;
	v.capacitateVagon = capacitate;
	v.numarBileteVandute = bileteV;
	v.firmaTransport = (char*)malloc(strlen(firma) + 1);
	strcpy(v.firmaTransport, firma);
	return v;
}

Heap conversieHeap(ListaD lista) {
	int dim = nrNoduri(lista);
	Heap heap = initializareHeap(dim);
	int k = 0;
	while (lista.prim) {
		heap.vector[k++] = initializareVagon(lista.prim->info.numarVagon, lista.prim->info.capacitateVagon, lista.prim->info.numarBileteVandute, lista.prim->info.firmaTransport);
		lista.prim = lista.prim->next;
	}
	for (int i = (heap.dim - 2) / 2;i >= 0;i--) {
		heap = filtrareHeap(heap, i);
	}
	return heap;
}

void afisareHeap(Heap heap) {
	for (int i = 0;i < heap.dim;i++) {
		printf("\n%.2f - ", gradOcupare(heap.vector[i]));
		afisareVagon(heap.vector[i]);
	}
}

Heap modificaNumarBileteVandute(Heap heap, int nrVagon, int nrNouBilete) {
	for (int i = 0;i < heap.dim;i++) {
		if (heap.vector[i].numarVagon == nrVagon) {
			heap.vector[i].numarBileteVandute = nrNouBilete;
			i = heap.dim;
		}
	}
	for (int i = (heap.dim - 2) / 2;i >= 0;i--) {
		heap = filtrareHeap(heap, i);
	}
	return heap;
}

ListaD dezalocareLista(ListaD lista) {
	while (lista.prim) {
		Nod* aux = lista.prim->next;
		free(lista.prim->info.firmaTransport);
		free(lista.prim);
		lista.prim = aux;
	}
	lista.prim = NULL;
	lista.ultim = NULL;
	return lista;
}

Heap dezalocareHeap(Heap heap) {
	for (int i = 0;i < heap.dim;i++) {
		free(heap.vector[i].firmaTransport);
	}
	free(heap.vector);
	heap.vector = NULL;
	heap.dim = 0;
	return heap;
}

int main() {
	ListaD lista = citireFisier("2vagoane.txt");
	afisareDeLaInceput(lista);
	printf("\n");
	afisareDeLaSfarsit(lista);
	lista = stergereMinim(lista);
	printf("\nDupa stergere:\n");
	afisareDeLaInceput(lista);
	printf("\n");
	afisareDeLaSfarsit(lista);
	Heap heap = conversieHeap(lista);
	afisareHeap(heap);
	heap = modificaNumarBileteVandute(heap, 2, 50);
	printf("\nDupa modificare:\n");
	afisareHeap(heap);
	lista = dezalocareLista(lista);
	heap = dezalocareHeap(heap);
	afisareDeLaInceput(lista);
	afisareHeap(heap);
	return 0;
}