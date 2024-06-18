#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct Definitions
typedef struct Reteta {
    unsigned int id;
    unsigned char nrMedicamente;
    char** medicamente;
    unsigned char* cantitati;
    float* preturi;
    unsigned char* procCompensare;
    char* medic;
} Reteta;

typedef struct Nod {
    struct Nod* next;
    struct Nod* prev;
    Reteta info;
} Nod;

typedef struct ListaDubla {
    Nod* prim;
    Nod* ultim;
} ListaD;

// Function to initialize a Reteta
Reteta initializareReteta(unsigned int id, unsigned char nrMedicamente, const char** medicamente,
    unsigned char* cantitati, float* preturi, unsigned char* procCompensare, const char* medic) {

    Reteta r;
    r.id = id;
    r.nrMedicamente = nrMedicamente;
    r.medicamente = (char**)malloc(nrMedicamente * sizeof(char*));
    r.cantitati = (unsigned char*)malloc(nrMedicamente * sizeof(unsigned char));
    r.preturi = (float*)malloc(nrMedicamente * sizeof(float));
    r.procCompensare = (unsigned char*)malloc(nrMedicamente * sizeof(unsigned char));

    for (unsigned char i = 0; i < nrMedicamente; ++i) {
        r.medicamente[i] = (char*)malloc(strlen(medicamente[i]) + 1);
        strcpy(r.medicamente[i], medicamente[i]);
        r.cantitati[i] = cantitati[i];
        r.preturi[i] = preturi[i];
        r.procCompensare[i] = procCompensare[i];
    }

    r.medic = (char*)malloc(strlen(medic) + 1);
    strcpy(r.medic, medic);
    return r;
}

ListaD inserareListaInceput(ListaD lista, Reteta r) {
    Nod* nod = (Nod*)malloc(sizeof(Nod));
  
    nod->info = r;
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

ListaD citireListaFisier(const char* numeFisier) {
    FILE* file = fopen(numeFisier, "r");
    ListaD lista;
    lista.prim = NULL;
    lista.ultim = NULL;
    char buffer[200];
    char separator[] = ",\n";
    while (fgets(buffer, 200, file)) {
        Reteta r;
        char* token = strtok(buffer, separator);
        r.id = atoi(token);

        token = strtok(NULL, separator);
        r.nrMedicamente = (unsigned char)atoi(token);
        r.medicamente = (char**)malloc(r.nrMedicamente * sizeof(char*));
        r.cantitati = (unsigned char*)malloc(r.nrMedicamente * sizeof(unsigned char));
        r.preturi = (float*)malloc(r.nrMedicamente * sizeof(float));
        r.procCompensare = (unsigned char*)malloc(r.nrMedicamente * sizeof(unsigned char));

        for (unsigned char i = 0; i < r.nrMedicamente; ++i) {
            token = strtok(NULL, separator);
            r.medicamente[i] = (char*)malloc(strlen(token) + 1);
            strcpy(r.medicamente[i], token);

            token = strtok(NULL, separator);
            r.cantitati[i] = (unsigned char)atoi(token);

            token = strtok(NULL, separator);
            r.preturi[i] = atof(token);

            token = strtok(NULL, ",\n");
            r.procCompensare[i] = (unsigned char)atoi(token);
        }

        token = strtok(NULL, separator);
        r.medic = (char*)malloc(strlen(token) + 1);
        strcpy(r.medic, token);

        lista = inserareListaInceput(lista, r);
    }

    fclose(file);
    return lista;
}

void afisareReteta(Reteta r) {
    printf("%d. %s a prescris %d medicamente:\n", r.id, r.medic, r.nrMedicamente);
    for (unsigned char i = 0; i < r.nrMedicamente; ++i) {
        printf("\t%d %s - %.2f lei, compensat %d%%\n", r.cantitati[i], r.medicamente[i], r.preturi[i], r.procCompensare[i]);
    }
}

void afisareInceput(ListaD lista) {
    while (lista.prim) {
        afisareReteta(lista.prim->info);
        lista.prim = lista.prim->next;
    }
}

void afisareSfarsit(ListaD lista) {
    while (lista.ultim) {
        afisareReteta(lista.ultim->info);
        lista.ultim = lista.ultim->prev;
    }
}

void eliberareReteta(Reteta r) {
    for (unsigned char i = 0; i < r.nrMedicamente; ++i) {
        free(r.medicamente[i]);
    }
    free(r.medicamente);
    free(r.cantitati);
    free(r.preturi);
    free(r.procCompensare);
    free(r.medic);
}

float incasariMedicament(ListaD lista, const char* medicament) {
    float total = 0;
    while (lista.prim) {
        for (unsigned char i = 0;i < lista.prim->info.nrMedicamente;i++) {
            if (strcmp(lista.prim->info.medicamente[i], medicament) == 0) {
                total += lista.prim->info.preturi[i] * lista.prim->info.cantitati[i];
            }
        }
        lista.prim = lista.prim->next;
    }
    return total;
}


Reteta* reteteDupaMedic(int* dim, const char* medic, ListaD lista) {
    *dim = 0;
    Nod* curent = lista.prim;

    while (curent) {
        if (strcmp(curent->info.medic, medic) == 0) {
            (*dim)++;
        }
        curent = curent->next;
    }

    Reteta* vector = (Reteta*)malloc((*dim) * sizeof(Reteta));
    int k = 0;
    curent = lista.prim;

    while (curent) {
        if (strcmp(curent->info.medic, medic) == 0) {
            vector[k++] = initializareReteta(curent->info.id, curent->info.nrMedicamente, (const char**)curent->info.medicamente, curent->info.cantitati, curent->info.preturi, curent->info.procCompensare, curent->info.medic);
        }
        curent = curent->next;
    }
    return vector;
}

float valoareCompensata(Reteta* vector, int dim) {
    float val = 0;
    for (int i = 0; i < dim; i++) {
        for (unsigned char j = 0; j < vector[i].nrMedicamente; j++) {
            val += (vector[i].cantitati[j] * vector[i].preturi[j]) * vector[i].procCompensare[j] / 100;
        }
    }
    return val;
}

int cantitateVanduta(ListaD lista, const char* medicament) {
    int total = 0;
    Nod* curent = lista.prim;
    while (curent) {
        for (unsigned char i = 0; i < curent->info.nrMedicamente; ++i) {
            if (strcmp(curent->info.medicamente[i], medicament) == 0) {
                total += (int)curent->info.cantitati[i];
            }
        }
        curent = curent->next;
    }
    return total;
}

ListaD stergereLista(ListaD lista) {
    while (lista.prim) {
        Nod* aux = lista.prim->next;
        for (unsigned char i = 0;i < lista.prim->info.nrMedicamente;i++) {
            free(lista.prim->info.medicamente[i]);
        }
        free(lista.prim->info.medicamente);
        free(lista.prim->info.cantitati);
        free(lista.prim->info.preturi);
        free(lista.prim->info.procCompensare);
        free(lista.prim);
        lista.prim = aux;
    }
    lista.prim = NULL;
    lista.ultim = NULL;
    return lista;
}

Reteta* stergereVector(Reteta* vector, int* dim) {
    for (int i = 0;i < *dim;i++) {
        for (unsigned char j = 0;j < vector[i].nrMedicamente;j++) {
            free(vector[i].medicamente[j]);
        }
        free(vector[i].medicamente);
        free(vector[i].cantitati);
        free(vector[i].preturi);
        free(vector[i].procCompensare);
    }
    free(vector);
    vector = NULL;
    *dim = 0;
    return vector;
}

int main() {
    ListaD lista = citireListaFisier("2retete.txt");
    afisareSfarsit(lista);
    float total = incasariMedicament(lista, "Aspirin");
    printf("\nIncasarile totale de Aspirina: %.2f lei", total);
    int dim;
    printf("\nRetetele medicului John Doe:\n");
    Reteta* vector = reteteDupaMedic(&dim, "Cris", lista);
    for (int i = 0;i < dim;i++) {
        afisareReteta(vector[i]);
    }
    float val = valoareCompensata(vector, dim);
    printf("\nValoarea compensata: %.2f", val);
    int cant = cantitateVanduta(lista, "Ibuprofen");
    printf("\nCantiatea totala de Ibuprofen vanduta: %d buc", cant);
    return 0;
    lista = stergereLista(lista);
    vector = stergereVector(vector, &dim);
}
