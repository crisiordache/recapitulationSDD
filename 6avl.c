#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

typedef struct Plus Plus;
struct Plus {
    int id;
    char* nume;
};

typedef struct Nod Nod;
struct Nod {
    Nod* stanga;
    Nod* dreapta;
    Plus info;
};

int inaltimeArbore(Nod* rad) {
    if (rad) {
        int hs = inaltimeArbore(rad->stanga);
        int hd = inaltimeArbore(rad->dreapta);
        return 1 + (hs > hd ? hs : hd);
    }
    return 0;
}

int gradEchilibru(Nod* rad) {
    if (rad) {
        int hs = inaltimeArbore(rad->stanga);
        int hd = inaltimeArbore(rad->dreapta);
        return hs - hd;
    }
    return 0;
}

Nod* rotireDreapta(Nod* rad) {
    Nod* aux = NULL;
    if (rad) {
        aux = rad->stanga;
        rad->stanga = aux->dreapta;
        aux->dreapta = rad;
    }
    return aux;
}

Nod* rotireStanga(Nod* rad) {
    Nod* aux = NULL;
    if (rad) {
        aux = rad->dreapta;
        rad->dreapta = aux->stanga;
        aux->stanga = rad;
    }
    return aux;
}

Nod* inserareArbore(Nod* rad, Plus p) {
    if (rad) {
        if (p.id < rad->info.id) {
            rad->stanga = inserareArbore(rad->stanga, p);
        }
        else {
            rad->dreapta = inserareArbore(rad->dreapta, p);
        }

        int ge = gradEchilibru(rad);
        if (ge == 2) {
            if (gradEchilibru(rad->stanga) == -1) {
                rad->stanga = rotireStanga(rad->stanga);
            }
            rad = rotireDreapta(rad);
        }

        if (ge == -2) {
            if (gradEchilibru(rad->dreapta) == 1) {
                rad->dreapta = rotireDreapta(rad->dreapta);
            }
            rad = rotireStanga(rad);
        }
    }
    else {
        Nod* aux = (Nod*)malloc(sizeof(Nod));
        aux->stanga = NULL;
        aux->dreapta = NULL;
        aux->info = p;
        rad = aux;
    }
    return rad;
}

Nod* citireAVLfisier(const char* numeFisier) {
    FILE* file = fopen(numeFisier, "r");
    Nod* rad = NULL;
    char buffer[100];
    char separator[] = ",\n";
    while (fgets(buffer, 100, file)) {
        Plus p;
        char* token = strtok(buffer, separator);
        p.id = atoi(token);
        token = strtok(NULL, separator);
        p.nume = (char*)malloc(strlen(token) + 1);
        strcpy(p.nume, token);
        rad = inserareArbore(rad, p);
    }
    fclose(file);
    return rad;
}

void afisarePlus(Plus p) {
    printf("%d. %s\n", p.id, p.nume);
}

void afisarePreOrdine(Nod* rad) {
    if (rad) {
        afisarePlus(rad->info);
        afisarePreOrdine(rad->stanga);
        afisarePreOrdine(rad->dreapta);
    }
}

Nod* stergereArbore(Nod* rad) {
    if (rad) {
        stergereArbore(rad->stanga);
        stergereArbore(rad->dreapta);
        free(rad->info.nume);
        free(rad);
        rad = NULL;
    }
    return rad;
}

int main() {
    Nod* rad = citireAVLfisier("6plusuri.txt");
    afisarePreOrdine(rad);
    rad = stergereArbore(rad);
    printf("\nDupa stergere: ");
    afisarePreOrdine(rad);
    return 0;
}
