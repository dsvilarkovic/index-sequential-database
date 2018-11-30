#ifndef POMOCNEFUNKCIJE_H_INCLUDED
#define POMOCNEFUNKCIJE_H_INCLUDED
#include <stdio.h>
#include "struktura.h"
#include "opcije.h"

FILE* safe_open(char*, char*, int);
int validacijaKljucaSerijske(int, FILE*, Blok*);
void oslobodiBlok(Blok*);
void napraviSlog(TvEmisija*);

//ovaj ispis se koristi i za sekvencijalnu, svakako je
//isti princip, samo sekvencijalna ima fizicki susedne slogove
void ispisiSerijsku(FILE*);

void unesiSlogListi(TvEmisija, CvorSlog_t**);
void ispisiDatum(datum);
void ispisiVreme(vreme);
void ispisiSlog(TvEmisija);
void ispisiIndeksSekvencijalnu();
void ispisiPrimarnuZonu(FILE*);
void ispisiIndeksnuZonu(FILE*);
void ispisiZonuPrekoracioca(FILE*);
RezultatTrazenja traziSlogAktivne();
void obrisiListu(CvorSlog_t **);

#endif