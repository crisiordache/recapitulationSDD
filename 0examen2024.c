#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

typedef struct RaportAnalizeMedicale Raport;
struct RaportAnalizeMedicale {
	unsigned int codRaport;
	unsigned int codPacient;
	unsigned char nrAnalize;
	char** denumiriAnalize;
	float* valoriReferinta;
	float* valoriAnalize;
};

typedef struct Nod Nod;
struct Nod {
	Nod* next;
	Nod* prev;
	Raport info;
};

typedef struct ListaDubluInlantuita Lista;
struct ListaDubluInlantuita {
	Nod* prim;
	Nod* ultim;
};

Lista inserareLista(Lista lista, Raport r) {
	Nod* nod = (Nod*)malloc(sizeof(Nod));
	nod->info = r;
	nod->next = NULL;
	nod->prev = NULL;
	//1. lista goala
	if (!lista.prim) {
		lista.prim = nod;
		lista.ultim = nod;
	}
	else {
		Nod* curent = lista.prim;
		while (curent && curent->info.codPacient < r.codPacient) {
			curent = curent->next;
		}
		//2. daca e ultimul
		if (!curent) { 
			nod->prev = lista.ultim;
			lista.ultim->next = nod;
			lista.ultim = nod;
		}
		//3. daca e primul
		else if (!curent->prev) { 
			nod->next = curent;
			curent->prev = nod;
			lista.prim = nod;
		}
		//4. daca e in interior
		else {
			nod->prev = curent->prev;
			nod->next = curent;
			curent->prev->next = nod;
			curent->prev = nod;
		}
	}
	return lista;
}

Lista citireFisier(const char* numeF) {
	FILE* file = fopen(numeF, "r");
	Lista lista;
	lista.prim = NULL;
	lista.ultim = NULL;
	char buffer[200];
	char separator[] = ",\n";
	while (fgets(buffer, 200, file)) {
		Raport r;
		char* token = strtok(buffer, separator);
		r.codRaport = (unsigned int)atoi(token);
		token = strtok(NULL, separator);
		r.codPacient = (unsigned int)atoi(token);
		token = strtok(NULL, separator);
		r.nrAnalize = (unsigned char)atoi(token);
		r.denumiriAnalize = (char**)malloc(sizeof(char*) * r.nrAnalize);
		r.valoriReferinta = (float*)malloc(sizeof(float) * r.nrAnalize);
		r.valoriAnalize = (float*)malloc(sizeof(float) * r.nrAnalize);
		for (unsigned char i = 0;i < r.nrAnalize;i++) {
			token = strtok(NULL, separator);
			r.denumiriAnalize[i] = (char*)malloc(strlen(token) + 1);
			strcpy(r.denumiriAnalize[i], token);
			token = strtok(NULL, separator);
			r.valoriReferinta[i] = atof(token);
			token = strtok(NULL, separator);
			r.valoriAnalize[i] = atof(token);
		}
		lista = inserareLista(lista, r);
	}
	fclose(file);
	return lista;
}

int main() {
	Lista lista = citireFisier("0raport.txt");
	Nod* aux = lista.prim;
	while (aux) {
		printf("Raport %u pentru pacientul %u:\n", aux->info.codRaport, aux->info.codPacient);
		for (unsigned char i = 0; i < aux->info.nrAnalize; i++) {
			printf("\tAnaliza: %s, Valoare referinta: %.2f, Valoare obtinuta: %.2f\n",
				aux->info.denumiriAnalize[i], aux->info.valoriReferinta[i], aux->info.valoriAnalize[i]);
		}
		aux = aux->next;
	}
	return 0;
}