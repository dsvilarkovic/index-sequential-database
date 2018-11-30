#ifndef STRUKTURA_H_INCLUDED
#define STRUKTURA_H_INCLUDED

#define IME_VODITELJA 50
#define PREZIME_VODITELJA 50
#define VRSTA_EMISIJE 30
// #define IME_VODITELJA_S "50"
// #define PREZIME_VODITELJA_S "50"
// #define VRSTA_EMISIJE_S "30"
#define TRAJANJE_EMISIJE 210
#define FAKTOR_BLOKIRANJA 5
#define N_FAKTOR 2
#define FAKTOR_ZONE_PREKORACENJA 1
#define MIN_KEY 1
#define MAX_KEY 9999999
#define STATICKA_ALOKACIJA 3

typedef struct{
    int hh;
    int mm;
}  vreme;

typedef struct{
    int dd;
    int mm;
    int yyyy;

} datum;


//slog datoteke
typedef struct {
    int evidencioni_broj;
    char imeVoditelja[IME_VODITELJA + 1] ;
    char prezimeVoditelja[PREZIME_VODITELJA + 1];
    datum datumEmisije;
    vreme vremeEmisije;
    short trajanjeEmisije;
    char vrsteEmisije[VRSTA_EMISIJE + 1];

    int statusPolje; //za logicko brisanje, 0 aktivan, 1 neaktivan

} TvEmisija;

//blok datoteke
typedef struct{
    TvEmisija slog[FAKTOR_BLOKIRANJA];
    int trenutniBrojSlogova;
} Blok;

//dinamicka lista u koju ce se unositi slogovi serijske datoteke
//lista ce biti namestena 
typedef struct CvorSlog{
    TvEmisija tvEmisija;
    struct CvorSlog* sledeciCvor;
}CvorSlog_t;


//INDEKS SEKVENCIJALNA DATOTEKA
typedef struct ZaglavljeIS{
    //relativne adrese u odnosu na zaglavlje
    int adresaPrimarneZone;
    int adresaIndeksneZone;
    int brojBlokovaPrimarneZone;
    int brojBlokovaIndeksneZone;
    int brojBlokovaZonePrekoracenja;
}ZaglavljeIS;

typedef struct {
    TvEmisija slog[FAKTOR_BLOKIRANJA];
    int trenutniBrojSlogova;
}PrimarniBlok;  

typedef struct {
    int evidencioniBrojPrimarneZone[N_FAKTOR];
    int adresaPodredjenogPrimarneZone[N_FAKTOR];
    int evidencioniBrojZonePrekoracilaca[N_FAKTOR];
    int adresaPodredjenogZonePrekoracilaca[N_FAKTOR];
    int imaPrekoracioca[N_FAKTOR]; //0 nema, 1 ima
}IndeksniBlok;

typedef struct {
    TvEmisija slog;
    int adresaNarednog;
}BlokPrekoracioc;

typedef struct {
    int sledeciSlobodniBlok;
}ZaglavljePrekoracioca;

typedef struct{
    //ako je 1, nadjeno, ako 0 , nije nadjeno
    int nasao;
    //relativna adresa bloka primarne zone ili zone prekoracenja
    int relativnaAdresaBloka;
    //da li je u primarnoj zoni ili zoni prekoracenja
    int primaranJe;
    //kljuc koji ce se mozda koristiti
    int evidencioni_broj;
    //za primarnu zonu ako je tamo nadjen/nenadjen, ako nije
    //onda je vrednost -1
    int relativnaAdresaSloga;
    //za relativnu adresu prethodno nadjenog bloka,
    //sluzi za sprezanje kod zone prekoracenja
    int relativnaAdresaPrethodnogBloka;
    //sluzi za koju zonu prekoracenja je trazenje upalo
    //u odnosu na list
    int relativnaZonaPrekoracioca;
}RezultatTrazenja;




#endif // STRUKTURA_H_INCLUDED
