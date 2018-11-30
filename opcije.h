#ifndef OPCIJE_H_INCLUDED
#define OPCIJE_H_INCLUDED

#define FILENAMELENGTH 30
#define CREATE_FILE_FAILURE 1
#define SET_ACTIVE_FILE_FAILURE 2
#define OPEN_CREATED_FILE_FAILURE 3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "struktura.h"


//cuva puno ime aktivne datoteke
char aktivnaDatoteka[FILENAMELENGTH + 1 + 4];
FILE* fajlAktivneDatoteke ;


void formirajPraznuDatoteku();
void izborAktivneDatoteke();
void prikazAktivneDatoteke();
void formirajSerijskuDatoteku();
void formirajSekvencijalnuDatoteku();

void formirajIndeksSekvencijalnuDatoteku();
void upisiSlogAktivnoj();
void traziSlogAktivneOpcija();
void ispisiSvePodatkeAktivne();
void logickoBrisanje();

void cheat();


#endif
