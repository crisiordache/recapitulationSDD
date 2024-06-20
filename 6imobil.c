#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Imobil Imobil;
struct Imobil {
    unsigned int id;
    char* numeStrada;
    unsigned int nrStrada;
    int nrApartamente;
    int* nrPersoanaAp;
    float* intretinereAp;
};

typedef struct Nod Nod;
struct Nod {
    Imobil info;
    Nod* stanga;
    Nod* dreapta;
};

int inaltimeArbore(Nod* rad) {
    if (rad) {
        int hs = inaltimeArbore(rad->stanga);
        int hd = inaltimeArbore(rad->dreapta);
        return 1 + (hd > hs ? hd : hs);
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

Nod* inserareArboreAVL(Nod* rad, Imobil i) {
    if (rad) {
        if (rad->info.id > i.id) {
            rad->stanga = inserareArboreAVL(rad->stanga, i);
        }
        else {
            rad->dreapta = inserareArboreAVL(rad->dreapta, i);
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
        Nod* nod = (Nod*)malloc(sizeof(Nod));
        nod->info = i;
        nod->dreapta = NULL;
        nod->stanga = NULL;
        rad = nod;
    }
    return rad;
}

Nod* citireFisier(const char* numeFisier) {
    FILE* file = fopen(numeFisier, "r");
    Nod* rad = NULL;
    char buffer[200];
    char separator[] = ",\n";
    while (fgets(buffer, 200, file)) {
        Imobil i;
        char* token = strtok(buffer, separator);
        i.id = atoi(token);

        token = strtok(NULL, separator);
        i.numeStrada = (char*)malloc(strlen(token) + 1);
        strcpy(i.numeStrada, token);

        token = strtok(NULL, separator);
        i.nrStrada = atoi(token);

        token = strtok(NULL, separator);
        i.nrApartamente = atoi(token);

        i.nrPersoanaAp = (int*)malloc(sizeof(int) * i.nrApartamente);
        i.intretinereAp = (float*)malloc(sizeof(float) * i.nrApartamente);
        for (int j = 0; j < i.nrApartamente; j++) {
            token = strtok(NULL, separator);
            i.nrPersoanaAp[j] = atoi(token);

            token = strtok(NULL, separator);
            i.intretinereAp[j] = atof(token);
        }
        rad = inserareArboreAVL(rad, i);
    }
    fclose(file);
    return rad;
}

void afisareImobil(Imobil imobil) {
    printf("Imobil ID: %u\n", imobil.id);
    printf("Strada: %s, Nr: %u\n", imobil.numeStrada, imobil.nrStrada);
    printf("Numar apartamente: %d\n", imobil.nrApartamente);

    for (int i = 0; i < imobil.nrApartamente; i++) {
        printf("  Apartament %d:\n", i + 1);
        printf("    Numar persoane: %d\n", imobil.nrPersoanaAp[i]);
        printf("    Intretinere: %.2f\n", imobil.intretinereAp[i]);
    }
}

void afisareInordine(Nod* rad) {
    if (rad) {
        afisareInordine(rad->stanga);
        afisareImobil(rad->info);
        afisareInordine(rad->dreapta);
    }
}

int nrImobileDupaStrada(Nod* rad, const char* strada) {
    if (rad) {
        int nr = 0;
        if (strcmp(rad->info.numeStrada, strada) == 0) {
            nr++;
        }
        nr += nrImobileDupaStrada(rad->stanga,strada);
        nr += nrImobileDupaStrada(rad->dreapta, strada);
        return nr;
    }
    return 0;
}

float sumaIntretinereBloc(Nod* rad, const char* strada, unsigned int nrBloc) {
    if (rad) {
        if (strcmp(rad->info.numeStrada, strada) == 0 && rad->info.nrStrada == nrBloc) {
            float suma = 0;
            for (int i = 0; i < rad->info.nrApartamente; ++i) {
                suma += rad->info.intretinereAp[i];
            }
            return suma;
        }

        float sumaStanga = sumaIntretinereBloc(rad->stanga, strada, nrBloc);
        if (sumaStanga != 0) {
            return sumaStanga;
        }

        return sumaIntretinereBloc(rad->dreapta, strada, nrBloc);
    }
    return 0;
}

int nrPersoaneBloc(Nod* rad, const char* strada, unsigned int nrBloc) {
    if (rad) {
        if (strcmp(rad->info.numeStrada, strada) == 0 && rad->info.nrStrada == nrBloc) {
            int nr = 0;
            for (int i = 0; i < rad->info.nrApartamente; ++i) {
                nr += rad->info.nrPersoanaAp[i];
            }
            return nr;
        }

        int nrStanga = nrPersoaneBloc(rad->stanga, strada, nrBloc);
        if (nrStanga != 0) {
            return nrStanga;
        }

        return nrPersoaneBloc(rad->dreapta, strada, nrBloc);
    }
    return 0;
}

float intretinerePePersoane(Nod* rad, const char* strada, unsigned int nrBloc) {
    int nr = nrPersoaneBloc(rad, strada, nrBloc);
    if (nr != 0) {
        return sumaIntretinereBloc(rad, strada, nrBloc) / nr;
    }
    else {
        return 0;
    }
}

Nod* stergereRadacina(Nod* rad) {
    if (rad) {
        Nod* aux = rad;
        if (aux->stanga) {
            rad = aux->stanga;
            if (aux->dreapta) {
                Nod* temp = aux->stanga;
                while (temp->dreapta) {
                    temp = temp->dreapta;
                }
                temp->dreapta = aux->dreapta;
            }
        }
        else {
            if (aux->dreapta) {
                rad = aux->dreapta;
            }
            else {
                rad = NULL;
            }
        }
        free(aux->info.intretinereAp);
        free(aux->info.nrPersoanaAp);
        free(aux->info.numeStrada);
        free(aux);
        return rad;
    }
    return NULL;
}

Nod* eliberareNod(Nod* nod) {
    free(nod->info.numeStrada);
    free(nod->info.nrPersoanaAp);
    free(nod->info.intretinereAp);
    free(nod);
    return NULL;
}

Nod* stergeFrunzeNivel(Nod* rad, int nivelCurent, int nivelTarget) {
    if (rad == NULL) {
        return NULL;
    }

    if (nivelCurent == nivelTarget - 1) {
        if (rad->stanga && rad->stanga->stanga == NULL && rad->stanga->dreapta == NULL) {
            rad->stanga = eliberareNod(rad->stanga);
        }
        if (rad->dreapta && rad->dreapta->stanga == NULL && rad->dreapta->dreapta == NULL) {
            rad->dreapta = eliberareNod(rad->dreapta);
        }
    }
    else {
        rad->stanga = stergeFrunzeNivel(rad->stanga, nivelCurent + 1, nivelTarget);
        rad->dreapta = stergeFrunzeNivel(rad->dreapta, nivelCurent + 1, nivelTarget);
    }

    return rad;
}

Nod* copieFaraNod(Nod* radNou, Nod* radVechi, unsigned int id) {
    if (radVechi) {
        if (radVechi->info.id != id) {
            radNou = inserareArboreAVL(radNou, radVechi->info);
        }
        radNou = copieFaraNod(radNou, radVechi->stanga, id);
        radNou = copieFaraNod(radNou, radVechi->dreapta, id);
    }
    return radNou;
}

int main() {
    Nod* rad = citireFisier("6imobil.txt");
    afisareInordine(rad);
    int nr = nrImobileDupaStrada(rad, "Strada Libertatii");
    printf("\nPe Strada Libertatii sunt %d imobile\n", nr);
    float val = intretinerePePersoane(rad, "Strada Unirii",15);
    printf("\nValoare intretinerii pe persoana este %.2f\n", val);
    /*rad = stergereRadacina(rad);
    printf("\n\n");
    afisareInordine(rad);
    rad = stergeFrunzeNivel(rad, 0, 2);
    printf("\n\n");
    afisareInordine(rad);
    return 0;*/
    Nod* radNou=NULL;
    radNou = copieFaraNod(radNou, rad, 10);
    printf("\n\n");
    afisareInordine(radNou);
}
