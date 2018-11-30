#include "pomocneFunkcije.h"
#include <math.h>

//malo je besmisleno da ogranicavam na blok slanje u disk
//pa da ovako validiram sa proverom svakog unesenog kljuca pristupom disku
//al sta je tu je
//Pokazivac na fajl koristimo zbog pregleda prethodnih blokova
//a blok za pregled trenutnog bloka
int validacijaKljucaSerijske(int noviEvidencioniBroj, FILE* fajlSerijske, Blok* blok){
    Blok* blokDiska= (Blok*) malloc(sizeof(Blok));
    int i;

    //za cuvanje broj bajtova od pocetka, na kraju 
    //se vratimo na taj broj kad sve ispregledamo


    //printf("Kljuc: %d i %d -> %d \n", noviEvidencioniBroj, MIN_KEY);
    int trenutniOffset = ftell(fajlSerijske);

    //provera jel >0
    if(noviEvidencioniBroj < MIN_KEY || noviEvidencioniBroj > MAX_KEY)
        return 0;

    //provera trenutnog bloka u OM
    //printf("Provera OM\n");
    for(i = 0; i < blok->trenutniBrojSlogova; i++){
        if(noviEvidencioniBroj == blok->slog[i].evidencioni_broj ){
                return 0;
        }
        else{
            //printf("Porede se %d i %d\n", noviEvidencioniBroj, blok->slog[i].evidencioni_broj);
        }
    }


   // printf("Provera u disku\n");

    //postavka na pocetak fajla
   
    fseek(fajlSerijske, 0, SEEK_SET);
    //provera blokova u disku


    while(fread(blokDiska, sizeof(Blok), 1, fajlSerijske)){
        for(i = 0; i < blokDiska->trenutniBrojSlogova; i++){
            if(noviEvidencioniBroj == blokDiska->slog[i].evidencioni_broj ){
                return 0;
            }
            else{
                //printf("Porede se %d i %d\n", noviEvidencioniBroj, blokDiska->slog[i].evidencioni_broj);
            }
        }
    }

    //vracamo se na poziciju od koje upisujemo dalje
 
    fseek(fajlSerijske, trenutniOffset, SEEK_SET);
    free(blokDiska);
    //uspesna provera jedinstvenosti
    return 1;

    
}

int validacijaKljucaIndeksSekvencijalne(int noviEvidencioniBroj, FILE* f){
    if(noviEvidencioniBroj < MIN_KEY || noviEvidencioniBroj > MAX_KEY){
        return 0;
    }
}

//nekad davno sluzila
void oslobodiBlok(Blok* blok){

    free(blok);
}


//TODO preimenuj ovo serijsko u nesto drugo, posto i serijska i sekvencijalna koriste
void ispisiSerijsku(FILE* f){
    printf("Datoteka uneta, ovo su podaci: \n");

    fseek(f, 0, SEEK_SET);
    Blok* blok =(Blok*) malloc(sizeof(Blok));
    int i;

    while(fread(blok, sizeof(Blok), 1, f)){
        printf("Blok %d\n", ftell(f)/sizeof(Blok) - 1);
        for(i = 0; i < blok->trenutniBrojSlogova; i++){
            printf("\tSlog %d\n", i);
            //printf("%d\n", blok->slog[i].evidencioni_broj);
            ispisiSlog(blok->slog[i]);
        }
    }

    free(blok);
}
void napraviSlog(TvEmisija* slog){
    
    //difoltno ubacivanje vrednosti, bitan samo evidencioni broj trenutno
    printf("Da li zelis da dalje informacije unosis? 1/0");
    printf("(uvedeno da se ne bi gubilo vreme na testiranje)\n");
    int izbor;
    do{
        fflush(stdin);
        scanf("%d", &izbor);
    }while(izbor != 0 && izbor != 1);

    if(!izbor){
        strcpy(slog->imeVoditelja,"Pera"); 
        strcpy(slog->prezimeVoditelja,"Peric");
        slog->datumEmisije.dd = 10;
        slog->datumEmisije.mm = 01;
        slog->datumEmisije.yyyy = 1994;
        slog->vremeEmisije.hh = 20;
        slog->vremeEmisije.mm = 45;
        strcpy(slog->vrsteEmisije, "Sportsko-politickog karaktera");
        slog->statusPolje = 0;
        slog->trajanjeEmisije = 100;
    }
    else{
        char imeVoditelja[IME_VODITELJA];
        char prezimeVoditelja[PREZIME_VODITELJA];
        datum dat;
        vreme vrem;
        int trajanje;
        char privremeni[20]; //za formate

        printf("Unesi ime i prezime voditelja: ");
        scanf("%s %s", slog->imeVoditelja, slog->prezimeVoditelja);
        printf("Unesi datum emisije u formatu dd/mm/yyyy: ");
        scanf("%s", privremeni);
        fflush(stdin);
        sscanf(privremeni, "%d/%d/%d", &dat.dd, &dat.mm, &dat.yyyy);
        slog->datumEmisije = dat;
        printf("Unesi vreme emisije u formatu hh:mm : ");
        scanf("%s", privremeni);
        fflush(stdin);
        sscanf(privremeni, "%d:%d", &vrem.hh, &vrem.mm);
        slog->vremeEmisije = vrem;
        printf("Unesi trajanje emisije: ");
        do{
            fflush(stdin);
            scanf("%u", &slog->trajanjeEmisije);
        } while(slog->trajanjeEmisije > TRAJANJE_EMISIJE);
        fflush(stdin);
        printf("Vrsta emisije: ");
        scanf(" %30[^\n]s", slog->vrsteEmisije);//cim se ubaci vrednost vec od 30, cudno se ponasa
        fflush(stdin);
        slog->statusPolje = 0; //aktivna je
        printf("Kreiranje gotovo, uneseni podaci su: \n");
        ispisiSlog(*slog);
    }
}






FILE* safe_open(char *ime, char *mod, int greska) {
	FILE *f = fopen(ime, mod);
	if (f == NULL) {
        printf("Uocena greska broja %d \n", greska);
        fclose(f);
		exit(greska);
	}
    else{
        printf("Uspesno otvorena datoteka %s za svrhu %s! \n",ime, mod);
    }
	return f;
}


//u buducnosti za neku list.c datoteku
void unesiSlogListi(TvEmisija slog, CvorSlog_t** glava){
    CvorSlog_t* noviCvor = (CvorSlog_t*) calloc(1,sizeof(CvorSlog_t));
    CvorSlog_t* trenutni;
    noviCvor->tvEmisija = slog;

    if(*glava == NULL || (*glava)->tvEmisija.evidencioni_broj > noviCvor->tvEmisija.evidencioni_broj){
        noviCvor->sledeciCvor = *glava;
        *glava = noviCvor;
    }
    else{
        trenutni = *glava;
        while(trenutni->sledeciCvor != NULL && trenutni->sledeciCvor->tvEmisija.evidencioni_broj < noviCvor->tvEmisija.evidencioni_broj){
            trenutni = trenutni->sledeciCvor;
        }

        noviCvor->sledeciCvor = trenutni->sledeciCvor;
        trenutni->sledeciCvor = noviCvor;
    }
    //free(noviCvor); <- NIKAKO OVO
}

void ispisiListu(CvorSlog_t* glava){
    printf("Ispis liste: ");
    while(glava != NULL){
        ispisiSlog(glava->tvEmisija);
        glava = glava->sledeciCvor;
    }
}

void ispisiDatum(datum dat){
    printf("\t\tDatum emisije: %02d/%02d/%02d\n", dat.dd, dat.mm, dat.yyyy);
}
void ispisiVreme(vreme vrem){
    printf("\t\tVreme emisije: %02d:%02d\n", vrem.hh, vrem.mm);
}


//za ispis slogova, kasnije biti dodatno implementirano
void ispisiSlog(TvEmisija slog){
    //printf("%d \t", slog.evidencioni_broj);
    //printf("%d", slog.statusPolje);
    if(slog.statusPolje == 0){
        printf("\n\t\tEvidencioni broj: %d \n", slog.evidencioni_broj);
        printf("\t\tIme i prezime voditelja: %s %s\n", slog.imeVoditelja, slog.prezimeVoditelja);
        ispisiDatum(slog.datumEmisije);
        ispisiVreme(slog.vremeEmisije);
        printf("\t\tTrajanje emitovanja: %d minuta\n", slog.trajanjeEmisije);
        printf("\t\tVrsta emisije: %s\n", slog.vrsteEmisije);
    }
    else{
        printf("\n\t\tEvidencioni broj: / \n", slog.evidencioni_broj);
        printf("\t\tIme i prezime voditelja: / \n", slog.imeVoditelja, slog.prezimeVoditelja);
        ispisiDatum(slog.datumEmisije);
        ispisiVreme(slog.vremeEmisije);
        printf("\t\tTrajanje emitovanja: / \n", slog.trajanjeEmisije);
        printf("\t\tVrsta emisije: / \n", slog.vrsteEmisije);
    }
}

void ispisiIndeksSekvencijalnu(){
    char imeIndeksSekvencijalne[FILENAME_MAX + 4 + 1];
    //imedatoteke_stablo.bin
    char imeStablaIndeksSekvencijalne[FILENAME_MAX + 4 + 1 + 6];
    //imedatoteke_prekoracioc.bin
    char imePrekoraciocaIndeksSekvencijalne[FILENAME_MAX + 4 + 1 + 11];


   printf("Unesi ime indeks sekvencijalne datoteke koju zelis da otvoris: ");
   scanf("%s", imeIndeksSekvencijalne);
   strcpy(imeStablaIndeksSekvencijalne, imeIndeksSekvencijalne);
   strcpy(imePrekoraciocaIndeksSekvencijalne, imeIndeksSekvencijalne);

   strcat(imeStablaIndeksSekvencijalne, "_stablo.bin");
   strcat(imePrekoraciocaIndeksSekvencijalne, "_prekoracioc.bin");
   strcat(imeIndeksSekvencijalne, ".bin");

   FILE* fajlIndeksSekvencijalne = safe_open(imeIndeksSekvencijalne, "rb", CREATE_FILE_FAILURE);
   FILE* fajlStabla = safe_open(imeStablaIndeksSekvencijalne, "rb", CREATE_FILE_FAILURE);
   FILE* fajlPrekoracioca = safe_open(imePrekoraciocaIndeksSekvencijalne, "rb", CREATE_FILE_FAILURE);


   
   ispisiPrimarnuZonu(fajlIndeksSekvencijalne);
   ispisiIndeksnuZonu(fajlStabla);
   ispisiZonuPrekoracioca(fajlPrekoracioca);


   fclose(fajlIndeksSekvencijalne);
   fclose(fajlStabla);
   fclose(fajlPrekoracioca);
}


void ispisiPrimarnuZonu(FILE* f){
    printf("PRIMARNA ZONA: \n");
    PrimarniBlok* primarniBlok = malloc(sizeof(PrimarniBlok));
    ZaglavljeIS* zaglavljeIS = malloc(sizeof(ZaglavljeIS));
    fseek(f, 0, SEEK_SET);
    fread(zaglavljeIS, sizeof(ZaglavljeIS), 1, f);
    printf("Zaglavlje datoteke: \n");
    printf("%d\n", zaglavljeIS->brojBlokovaPrimarneZone);
    int i;
    //postavljanje na pocetak za citanje
    fseek(f, sizeof(ZaglavljeIS), SEEK_SET);
    while(fread(primarniBlok, sizeof(PrimarniBlok), 1, f)){
       printf("Blok %d:\n", (ftell(f) -  sizeof(ZaglavljeIS))/sizeof(PrimarniBlok) - 1);
       for(i = 0; i < primarniBlok->trenutniBrojSlogova; i++){
           printf("\tSlog %d: ", i);
           ispisiSlog(primarniBlok->slog[i]);     
           printf("\n");
       }
       printf("\n");
   }

   free(primarniBlok);
   free(zaglavljeIS);
}



void ispisiIndeksnuZonu(FILE* f){
    printf("INDEKSNA ZONA: \n");
    IndeksniBlok* indeksniBlok = malloc(sizeof(IndeksniBlok));
    int i;
    fseek(f, 0, SEEK_SET);
    while(fread(indeksniBlok, sizeof(IndeksniBlok), 1, f)){
       printf("Blok %d:\n", ftell(f)/sizeof(IndeksniBlok) - 1);
      for(i = 0; i < N_FAKTOR; i++){
          //ovo ne treba u posebnoj fji da se pravi
          //ako je zabranjena adresa za prekoracioc tj ako nije list, ne ispisuj
          printf("\tSlog %d:\n", i);
          
          printf("\tKljuc podstabla/primarnog bloka: %d Adresa podstabla/primarnog bloka: %d", indeksniBlok->evidencioniBrojPrimarneZone[i], indeksniBlok->adresaPodredjenogPrimarneZone[i]);
          if(indeksniBlok->evidencioniBrojZonePrekoracilaca[i] > 0){
              printf("\n\tKljuc Zone prekoracioca: %d Adresa Zone prekoracioca: %d \n", indeksniBlok->evidencioniBrojZonePrekoracilaca[i], indeksniBlok->adresaPodredjenogZonePrekoracilaca[i]);
          }
          else{
              printf("\n");
          }
      }
       printf("\n");
   }

   free(indeksniBlok);
}


void ispisiZonuPrekoracioca(FILE* f){
    printf("ZONA PREKORACIOCA: \n");
    BlokPrekoracioc blokPrekoracioc;
    int i;
    ZaglavljePrekoracioca zaglavljePrekoracioca;
    fseek(f, 0, SEEK_SET);
    fread(&zaglavljePrekoracioca, sizeof(ZaglavljePrekoracioca), 1, f);
    printf("\nZaglavlje prekoracenja: \n");
    if(zaglavljePrekoracioca.sledeciSlobodniBlok > -1){
        printf("Adresa sledeceg slobodnog bloka: %d \n\n" , zaglavljePrekoracioca.sledeciSlobodniBlok);
    }
    else{
        printf("Adresa sledeceg slobodnog bloka: * \n\n");
    }
    
    while(fread(&blokPrekoracioc, sizeof(BlokPrekoracioc), 1, f)){
       printf("Blok %d:\n", -1 + (ftell(f) - sizeof(ZaglavljePrekoracioca))/sizeof(BlokPrekoracioc));
       //printf("\tKljuc: %d\t", blokPrekoracioc.slog.evidencioni_broj);
       //printf("Aktivan: %d\t", blokPrekoracioc.slog.statusPolje);
       if(blokPrekoracioc.adresaNarednog > -1)
          printf("Adresa narednog:%d", blokPrekoracioc.adresaNarednog);
       else
          printf("Adresa narednog: *");
       printf("\n");
       ispisiSlog(blokPrekoracioc.slog);
   }

}


RezultatTrazenja traziSlogAktivne(int noviEvidencioniBroj, FILE* fajlStablaAktivne, FILE* fajlPrekoraciocaAktivne){
    RezultatTrazenja rez;
    //pretpostavimo da nije nadjeno i postavimo  na neke difolt vrednosti
    rez.nasao = -1;
    rez.evidencioni_broj = noviEvidencioniBroj;
    rez.primaranJe = 0;
    rez.relativnaAdresaBloka = 0;
    //prvo ulazimo u stablo
    //koren se nalazi na kraju datoteke stabla
    IndeksniBlok indeksniBlok;
    //pozicioniramo se 
    if(fajlAktivneDatoteke == NULL || !strcmp(aktivnaDatoteka, "")){
        printf("Nije odabrana aktivna datoteka, izaberi! \n");
        //return -1;
        return rez;
    }
    fseek(fajlStablaAktivne, -sizeof(IndeksniBlok), SEEK_END);
    
    

    //procitamo zaglavlje iz glavne datoteke da dobijemo info o broju i visini stabla
    //i da se pomerimo unapred
    ZaglavljeIS zaglavljeIS;
    fread(&zaglavljeIS, sizeof(ZaglavljeIS), 1, fajlAktivneDatoteke);
    int h = ceil(log((double)zaglavljeIS.brojBlokovaPrimarneZone) / log(N_FAKTOR));
    int i;
    int relativnaAdresaZaSkok;
    //sad imamo sve informacije za pretragu po stablu
    //trazi dok ne dostignes list stabla, tad je drugacija procedura
    while(h-- > 1){
        fread(&indeksniBlok, sizeof(IndeksniBlok), 1, fajlStablaAktivne);


        if(noviEvidencioniBroj <= indeksniBlok.evidencioniBrojPrimarneZone[0]){
            fseek(fajlStablaAktivne, indeksniBlok.adresaPodredjenogPrimarneZone[0]*sizeof(IndeksniBlok), SEEK_SET);
            continue;
        }
        
        for(i = 1; i < N_FAKTOR; i++){
            if(indeksniBlok.evidencioniBrojPrimarneZone[i - 1] < noviEvidencioniBroj && noviEvidencioniBroj <= indeksniBlok.evidencioniBrojPrimarneZone[i]){
                fseek(fajlStablaAktivne, indeksniBlok.adresaPodredjenogPrimarneZone[i]*sizeof(IndeksniBlok), SEEK_SET);
                break;
            }
        }        
    }

    //ucitavanje kad smo stigli do lista
    fread(&indeksniBlok, sizeof(IndeksniBlok), 1, fajlStablaAktivne);
    //provera da li je u primarnoj zoni
    //bice potrebno za dalje trazenje
    int primarniJe; 
    //MOZE I SA SEEK_CUR, jer smo se vec pozicionirali u primarnoj zoni na blokove
    //ovde citamo iz listova relativnu adresu blokova u primarnoj zoni
    if(noviEvidencioniBroj <= indeksniBlok.evidencioniBrojPrimarneZone[0]){
        fseek(fajlAktivneDatoteke, sizeof(ZaglavljeIS) + indeksniBlok.adresaPodredjenogPrimarneZone[0]*sizeof(PrimarniBlok), SEEK_SET);
        primarniJe = 1;
    }
    //ako nije gledaj da li je u zoni prekoracioca
    else if(indeksniBlok.evidencioniBrojPrimarneZone[0] < noviEvidencioniBroj && noviEvidencioniBroj <= indeksniBlok.evidencioniBrojZonePrekoracilaca[0]){
        //ako jeste,pozicioniraj se u zoni prekoracioca
        //i ako nije to ista zona kao i kod indeksa
        if(indeksniBlok.imaPrekoracioca[0]){
        //if(indeksniBlok.evidencioniBrojPrimarneZone[0] != indeksniBlok.evidencioniBrojZonePrekoracilaca[0]){
            fseek(fajlPrekoraciocaAktivne, sizeof(ZaglavljePrekoracioca) + indeksniBlok.adresaPodredjenogZonePrekoracilaca[0]*sizeof(BlokPrekoracioc), SEEK_SET);
            primarniJe = 0; //u prekoraciocu je
            rez.relativnaZonaPrekoracioca = 0;
        //TODO ovde ispod
        }
        else{
            primarniJe = 0;
            rez.nasao = 0;
            rez.primaranJe = primarniJe;
            rez.relativnaAdresaBloka = -1; //u tom slucaju nema sledeceg
            rez.relativnaAdresaSloga = -1;
            rez.relativnaZonaPrekoracioca = 0;
            rez.relativnaAdresaPrethodnogBloka = -1; // da bi trigerovao povoljan nacin
            return rez;
        }
    }

    for(i = 1; i < N_FAKTOR; i++){
        if(indeksniBlok.evidencioniBrojZonePrekoracilaca[i - 1] < noviEvidencioniBroj && noviEvidencioniBroj <= indeksniBlok.evidencioniBrojPrimarneZone[i]){
            fseek(fajlAktivneDatoteke, sizeof(ZaglavljeIS) + indeksniBlok.adresaPodredjenogPrimarneZone[i]*sizeof(PrimarniBlok), SEEK_SET);
            primarniJe = 1;
            break;
        }
        //ako nije gledaj da li je u zoni prekoracioca
        else if(indeksniBlok.evidencioniBrojPrimarneZone[i] < noviEvidencioniBroj && noviEvidencioniBroj <= indeksniBlok.evidencioniBrojZonePrekoracilaca[i]){
            //ako jeste,pozicioniraj se u zoni prekoracioca
            //i ako nije ista zona kao i kod indeksa
            rez.relativnaZonaPrekoracioca = i;
            //if(indeksniBlok.evidencioniBrojPrimarneZone[i] != indeksniBlok.evidencioniBrojZonePrekoracilaca[i]){
            if(indeksniBlok.imaPrekoracioca[i]){
               fseek(fajlPrekoraciocaAktivne, sizeof(ZaglavljePrekoracioca) + indeksniBlok.adresaPodredjenogZonePrekoracilaca[i]*sizeof(BlokPrekoracioc), SEEK_SET);
                primarniJe = 0;
                break;
            //}
            }
            else{
                primarniJe = 0;
                rez.nasao = 0;
                rez.primaranJe = primarniJe;
                rez.relativnaAdresaBloka = -1; //u tom slucaju nema sledeceg
                rez.relativnaAdresaSloga = -1;
                rez.relativnaZonaPrekoracioca = i;
                rez.relativnaAdresaPrethodnogBloka = -1; // da bi trigerovao povoljan nacin
                return rez;
            }
        }
        //ako je veci i od poslednje zone prekoracioca
        /*
        else if(noviEvidencioniBroj > indeksniBlok.evidencioniBrojZonePrekoracilaca[i]){

        }
        */
    }    

    //ako je u primarnoj zoni, tamo ga trazi
    if(primarniJe){
        //citanje primarnog bloka iz pozicioniranog
        PrimarniBlok primarniBlok;
        fread(&primarniBlok, sizeof(PrimarniBlok), 1, fajlAktivneDatoteke);
        for(i = 0; i < primarniBlok.trenutniBrojSlogova; i++){
            //trazenje uspesno, vracaj 1
            if(noviEvidencioniBroj == primarniBlok.slog[i].evidencioni_broj){
                //prikaz sloga zajedno sa adresom bloka i rednim brojem sloga u bloku
                //mora jos provera da li je taj logicki brisan
                if(primarniBlok.slog[i].statusPolje == 0){
                    printf("Slog nadjen uspesno!\n");
                    printf("Blok %d: \n", -1 + (ftell(fajlAktivneDatoteke) - sizeof(ZaglavljeIS))/sizeof(PrimarniBlok));
                    printf("\t Slog %d: ", i);
                    ispisiSlog(primarniBlok.slog[i]);
                    printf("\n");
                    //return 1;
                    rez.nasao = 1;
                    rez.primaranJe = primarniJe;
                    rez.relativnaAdresaBloka = -1 + (ftell(fajlAktivneDatoteke) - sizeof(ZaglavljeIS))/sizeof(PrimarniBlok);
                    rez.relativnaAdresaSloga = i;
                    return rez;
                }
                //ako jeste logick, obrisan onda trazi dalje, jer se moze desiti da iza
                //stoji neobrisani sa istim kljucem
            }
            //ako je izmedju, onda ga sigurno nema
            else if(noviEvidencioniBroj < primarniBlok.slog[i].evidencioni_broj){
                printf("Slog nije nadjen!\n");
                rez.nasao = 0;
                rez.primaranJe = primarniJe;
                rez.relativnaAdresaBloka = -1 + (ftell(fajlAktivneDatoteke) - sizeof(ZaglavljeIS))/sizeof(PrimarniBlok);
                rez.relativnaAdresaSloga = i;
                return rez;
            }
        }
        //preventivno, vraca da nije nasao
        printf("Slog nije nadjen!\n");
        //return 0;
        rez.nasao = 0;
        rez.primaranJe = primarniJe;
        rez.relativnaAdresaBloka = -1 + (ftell(fajlAktivneDatoteke) - sizeof(ZaglavljeIS))/sizeof(PrimarniBlok);
        rez.relativnaAdresaSloga = i;
        return rez;
    }
    //ako je u zoni prekoracioca, idi direktnim trazenjem kroz blokove

    else{
        BlokPrekoracioc blokPrekoracioc;
        //relativna adresa prethodno nadjenog bloka, sluzice za uvezivanje novog bloka
        //pretpostavljam da na prvom bloku staje , uspesno ili neuspesno
        rez.relativnaAdresaPrethodnogBloka = -1;
        //citaj dok god se ne zaustavis uspesno ili neuspesno,
        while(fread(&blokPrekoracioc, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne)){
            if(blokPrekoracioc.slog.evidencioni_broj == noviEvidencioniBroj){
                //ako si nasao isti evidencioni broj i nije logicki obrisan
                //ako jesi, trazices dalje dole
                if(blokPrekoracioc.slog.statusPolje == 0){
                    printf("Slog uspesno nadjen! \n");
                    printf("Blok %d: \n", -1 + (ftell(fajlPrekoraciocaAktivne) - sizeof(ZaglavljePrekoracioca))/sizeof(BlokPrekoracioc));
                    ispisiSlog(blokPrekoracioc.slog);
                    printf("\n");
                    //return 1;
                    rez.nasao = 1;
                    rez.primaranJe = primarniJe;
                    rez.relativnaAdresaBloka = -1 + (ftell(fajlPrekoraciocaAktivne) - sizeof(ZaglavljePrekoracioca))/sizeof(BlokPrekoracioc);
                    rez.relativnaAdresaSloga = -1;
                    return rez;
                }
            }
            //ako je manji od sledeceg bloka, neuspesno trazenje
            else if(noviEvidencioniBroj < blokPrekoracioc.slog.evidencioni_broj){
                printf("Slog nije nadjen!\n");
                //return 0;
                rez.nasao = 0;
                rez.primaranJe = primarniJe;
                rez.relativnaAdresaBloka = -1 + (ftell(fajlPrekoraciocaAktivne) - sizeof(ZaglavljePrekoracioca))/sizeof(BlokPrekoracioc);
                rez.relativnaAdresaSloga = -1;
                return rez;
            }
            //ako je nijedan od data dva slucaja, onda nastavlja dalje direktno trazenje
            //cuvamo adresu prethodnog bloka
            //ako je stigao do kraja, onda prekida direktno trazenje
            rez.relativnaAdresaPrethodnogBloka = -1 + (ftell(fajlPrekoraciocaAktivne) - sizeof(ZaglavljePrekoracioca))/sizeof(BlokPrekoracioc);
            //i postavljamo se na novu adresu bloka sa koje cemo citati
            //ukoliko sledeca adresa ne postoji, izlazimo iz petlje, 
            //da ne bi doslo do infinity loop-a
            if(blokPrekoracioc.adresaNarednog != -1)
                fseek(fajlPrekoraciocaAktivne, sizeof(ZaglavljePrekoracioca) + blokPrekoracioc.adresaNarednog*sizeof(BlokPrekoracioc), SEEK_SET);
            else{
                //sledeci u kojeg ga ubacis, jeste upravo kraj
                rez.relativnaAdresaBloka = -1;
                break;
            }
        }
        //ako je stigao do kraja i nije nasao bloka, onda prijavljujemo neuspesno trazenje
        //neka greska
        printf("Slog nije nadjen! \n");
        rez.nasao = 0;
        rez.primaranJe = primarniJe;
        //rez.relativnaAdresaBloka = -1 + (ftell(fajlPrekoraciocaAktivne) - sizeof(ZaglavljePrekoracioca))/sizeof(BlokPrekoracioc);
        //rez.relativnaAdresaSloga = -1;
        return rez;
    }
}

void obrisiListu(CvorSlog_t **glava){
    CvorSlog_t* trenutni = *glava;
    CvorSlog_t* sledeci;
    while(trenutni != NULL){
        sledeci = trenutni->sledeciCvor;
        free(trenutni);
        trenutni = sledeci;
    }

    *glava = NULL;
}



