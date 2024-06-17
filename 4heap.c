#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct Capsula Capsula;
struct Capsula {
	int intensitate;
	char* marca;
	float ml;
};

typedef struct MaxHeap Heap;
struct MaxHeap {
	int dim;
	Capsula* vector;
};

Heap initializareHeap(int dim) {
	Heap heap;
	heap.dim = dim;
	heap.vector = (Capsula*)malloc(sizeof(Capsula) * dim);
	return heap;
}

Heap filtrare(Heap heap, int rad) {
	int fs = 2 * rad + 1;
	int fd = 2 * rad + 2;
	int max = rad;

	//verific daca radacina are descendenti mai mari decat ea
	if (fs<heap.dim && heap.vector[fs].intensitate>heap.vector[max].intensitate) {
		max = fs;
	}

	if (fd<heap.dim && heap.vector[fd].intensitate>heap.vector[max].intensitate) {
		max = fd;
	}

	//daca da, ii interschimb (aduc nodul cu valoare mai mare in fata)
	if (max != rad) {
		Capsula aux = heap.vector[max];
		heap.vector[max] = heap.vector[rad];
		heap.vector[rad] = aux;

		if (max < (heap.dim - 2) / 2) {
			heap = filtrare(heap, max);
		}
	}

	return heap;
}

Heap inserareHeap(Heap heap, Capsula c) {
	//fac un vector mai mare cu un element si pun pe prima pozitie elementul pe care vreau sa il inserez 
	Capsula* aux = (Capsula*)malloc((heap.dim + 1) * sizeof(Capsula));
	aux[0] = c;
	for (int i = 1;i <= heap.dim;i++) {
		aux[i] = heap.vector[i - 1];
	}
	//modific heap
	free(heap.vector);
	heap.vector = aux;
	heap.dim++;
	//filtrez dupa pozitia pe care am inserat
	heap = filtrare(heap, 0);
	return heap;
}

Heap citireHeapFisier(const char* numeFisier) {
	FILE* file = fopen(numeFisier, "r");
	Heap heap;
	heap.dim = 0;
	heap.vector = NULL;
	char buffer[100];
	char separator[] = ",\n";
	while (fgets(buffer, 100, file)) {
		Capsula c;
		char* token = strtok(buffer, separator);
		c.intensitate = atoi(token);
		token = strtok(NULL, separator);
		c.marca = (char*)malloc(strlen(token) + 1);
		strcpy(c.marca, token);
		token = strtok(NULL, separator);
		c.ml = atof(token);
		heap = inserareHeap(heap, c);
	}
	fclose(file);
	return heap;
}

void afisareCapsula(Capsula c) {
	printf("Capsula %s produce %.2f ml cafea si are intensitatea %d\n", c.marca, c.ml, c.intensitate);
}

void afisareHeap(Heap heap) {
	for (int i = 0;i < heap.dim;i++) {
		afisareCapsula(heap.vector[i]);
	}
}

Capsula extragereMax(Heap* heap) {
	if (heap->dim > 0) {
		//extrag maximul
		Capsula extras = heap->vector[0];

		//mut elementele ramase in alt vector mai mic cu un element
		Capsula* aux = (Capsula*)(malloc(sizeof(Capsula) * ((*heap).dim - 1)));
		for (int i = 1; i < heap->dim; i++) {
			aux[i - 1] = heap->vector[i];
		}

		// modific heap
		free(heap->vector);
		heap->vector = aux;
		heap->dim--;

		for (int i = (heap->dim - 2) / 2; i >= 0; i--) {
			*heap = filtrare((*heap), i);
		}

		return extras;
	}
}

Heap stergereHeap(Heap heap) {
	for (int i = 0;i < heap.dim;i++) {
		free(heap.vector[i].marca);
	}
	free(heap.vector);
	heap.vector = NULL;
	heap.dim = 0;
	return heap;
}

int main() {
	Heap heap=citireHeapFisier("4capsule.txt");
	afisareHeap(heap);
	Capsula extras = extragereMax(&heap);
	printf("\nElementul extras:\n");
	afisareCapsula(extras);
	printf("\nHeap dupa extragere:\n");
	afisareHeap(heap);
	heap = stergereHeap(heap);
	printf("\nDupa stergere:\n");
	afisareHeap(heap);
	return 0;
}