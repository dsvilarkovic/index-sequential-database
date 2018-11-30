#include "opcije.h"
#include "struktura.h"
#include "pomocneFunkcije.h"
#include <math.h>
//TODO pozatvarati u svim datotekama fajlove sa fclose()
void formirajPraznuDatoteku(){

   char imeDatoteke[FILENAMELENGTH + 1 + 4];
   FILE* fajlDatoteke;


   printf("Unesi ime datoteke: ");

   scanf("%s", imeDatoteke);

   strcat(imeDatoteke, ".bin");

   printf("***%s***\n", imeDatoteke);


   //write stvara fajl ako ne postoji, ili prebrise stari ako postoji sa istim imenom
   fajlDatoteke = safe_open(imeDatoteke, "wb", CREATE_FILE_FAILURE);

}




/**
 *  Funkcija koja sluzi za izbor aktivne datoteke
 */
void izborAktivneDatoteke(){
    char imeDatoteke[FILENAMELENGTH + 1 + 4];
    FILE* fajlDatoteke;

    printf("Unesi ime datoteke koju zelite da bude aktivna: \n");

    scanf("%s", imeDatoteke);

    strcat(imeDatoteke, ".bin");

    printf("***%s***\n", imeDatoteke);


    //otvara se i za citanje i  pisanje
    fajlDatoteke = safe_open(imeDatoteke, "rb+", SET_ACTIVE_FILE_FAILURE);

    //Ako je bila otvorena neka prethodno datoteka, zatvori je
    if(strcmp("", aktivnaDatoteka)){
        fclose(fajlAktivneDatoteke);
    }

    //TODO nepotrebna provera
    if(fajlDatoteke != NULL){
        fajlAktivneDatoteke = fajlDatoteke;
        strcpy(aktivnaDatoteka, imeDatoteke);
        printf("Aktivna je datoteka %s \n", aktivnaDatoteka);
    }
}

void prikazAktivneDatoteke(){
    
    printf("Aktivna datoteka je: %s \n", aktivnaDatoteka);  
}

//Zadatak: formira serijsku datoteku za unos
//Algoritam: pravi slog po slog i ubacuje u blok, na kraju faktora blokiranja, ih ubaci u datoteku
void formirajSerijskuDatoteku(){
    int noviEvidencioniBroj;

    char imeDatoteke[FILENAMELENGTH + 4 + 1]; //= "serijska.bin";
    printf("Unesi naziv datoteke u koju zelis da unosis:\n");
    
    scanf("%s", imeDatoteke);
    strcat(imeDatoteke, ".bin");

    //TODO izmeniti u wb, sad samo zbog ispisa ima ovaj oblik
    FILE* fajlSerijske = safe_open(imeDatoteke, "wb+", CREATE_FILE_FAILURE);

    //vodi evidenciju unetih slogova u blok, da zna kad da salje u disk podatke
    Blok* blok = (Blok*) malloc(sizeof(Blok));
    blok->trenutniBrojSlogova = 0;
    do{

        //kad dostignes sve unete slogove bloka,unesi ih u disk
        if(blok->trenutniBrojSlogova == FAKTOR_BLOKIRANJA){
            printf("Blok postignut, unosi se u datoteku!\n");
            fwrite(blok, sizeof(Blok), 1, fajlSerijske);
            
            oslobodiBlok(blok);
            
            blok = (Blok*) malloc(sizeof(Blok));
            blok->trenutniBrojSlogova = 0;

        }

        printf("Unesi evidencioni broj: (za izlazak, daj evidencioni manji od 1): \n");
        scanf(" %d", &noviEvidencioniBroj);
        getchar();
        fflush(stdin);

        //ako je validan, jedinstven kljuc, ubacuj u blok
        if(validacijaKljucaSerijske(noviEvidencioniBroj, fajlSerijske, blok) == 1){
            TvEmisija* slog = (TvEmisija*) malloc(sizeof(TvEmisija));
            slog->evidencioni_broj = noviEvidencioniBroj;
            napraviSlog(slog);

            blok->slog[blok->trenutniBrojSlogova++] = *slog;
        }     
        else 
            printf("Evidencioni broj nije jedinstven, probaj opet! \n");    
    }while(noviEvidencioniBroj >= MIN_KEY);    

    //ubaci na disk izostali blok
    if(blok->trenutniBrojSlogova > 0){
        fwrite(blok, sizeof(Blok), 1, fajlSerijske);
        oslobodiBlok(blok);
    }


    //fwrite(NULL, sizeof(Blok), )
    ispisiSerijsku(fajlSerijske);
}

//TODO obrisi listu na kraju upotrebe u sekvencijalnoj
void formirajSekvencijalnuDatoteku(){
    //pokazivac na glavu liste 
    CvorSlog_t *glava = NULL;
    int i;
    char imeSekvencijalne[FILENAME_MAX + 4 + 1];
    char imeSerijske[FILENAME_MAX + 4 + 1];
    printf("Unesi ime formirane sekvencijalne datoteke: ");
    scanf("%s", imeSekvencijalne);
    strcat(imeSekvencijalne, ".bin");

    //Rezim wb+, jer se posle upisa u njega, ispisuje kao krajni rezultat izrade
    FILE* fajlSekvencijalne = safe_open(imeSekvencijalne, "wb+", OPEN_CREATED_FILE_FAILURE);


    printf("Unesi ime serijske datoteke odakle zelis da izvlacis slogove: (bez .bin ekstenzije) ");
    scanf("%s", imeSerijske);
    strcat(imeSerijske, ".bin");

    //TODO (DONE) namestiti da bude rezim rb, jer se citaju slogovi 
    FILE* fajlSerijske = safe_open(imeSerijske, "rb", OPEN_CREATED_FILE_FAILURE);



    
    Blok blok;
    while(fread(&blok, sizeof(Blok), 1, fajlSerijske)){
        for(i = 0; i < blok.trenutniBrojSlogova; i++){
                unesiSlogListi(blok.slog[i],&glava);
        }
    }

    ispisiListu(glava);    

    //krece ubacivanje u sekvencijalnu

    int noviEvidencioniBroj;
    blok.trenutniBrojSlogova = 0;

    //mada je moglo i sa glavom ,al nek ostane
    CvorSlog_t* iterator = glava;
    while(iterator != NULL){   

        if(blok.trenutniBrojSlogova == FAKTOR_BLOKIRANJA){
            printf("Blok postignut, unosi se u datoteku!\n");
            fwrite(&blok, sizeof(Blok), 1, fajlSekvencijalne);
            blok.trenutniBrojSlogova = 0;
        }

        TvEmisija slog;
        slog = iterator->tvEmisija;
        blok.slog[blok.trenutniBrojSlogova++] = slog;
        iterator = iterator->sledeciCvor;
    }

    if(blok.trenutniBrojSlogova > 0){
        fwrite(&blok, sizeof(Blok), 1, fajlSekvencijalne);
    }

    obrisiListu(&glava);
    fflush(fajlSekvencijalne);
    //ispis sekvencijalne
    ispisiSerijsku(fajlSekvencijalne);
    fclose(fajlSekvencijalne);
    fclose(fajlSerijske);
    
}



void formirajIndeksSekvencijalnuDatoteku(){
    char imeSekvencijalne[FILENAME_MAX + 4 + 1];
    char imeIndeksSekvencijalne[FILENAME_MAX + 4 + 1];

    //imedatoteke_stablo.bin
    char imeStablaIndeksSekvencijalne[FILENAME_MAX + 4 + 1 + 6];
    //imedatoteke_prekoracioc.bin
    char imePrekoraciocaIndeksSekvencijalne[FILENAME_MAX + 4 + 1 + 11];

    printf("Unesi ime sekvencijalne datoteke odakle zelis da izvlacis slogove: (bez .bin ekstenzije) ");
    scanf("%s", imeSekvencijalne);
    strcat(imeSekvencijalne, ".bin");

    //TODO (DONE) namestiti da bude rezim rb, jer se citaju slogovi 
    FILE* fajlSekvencijalne = safe_open(imeSekvencijalne, "rb", OPEN_CREATED_FILE_FAILURE);


   //pretpostavljamo
   // if(ime)

   printf("Unesi ime indeks sekvencijalne datoteke koju cemo formirati na osnovu imena koje zadas: ");
   scanf("%s", imeIndeksSekvencijalne);
   strcpy(imeStablaIndeksSekvencijalne, imeIndeksSekvencijalne);
   strcpy(imePrekoraciocaIndeksSekvencijalne, imeIndeksSekvencijalne);

   strcat(imeStablaIndeksSekvencijalne, "_stablo.bin");
   strcat(imePrekoraciocaIndeksSekvencijalne, "_prekoracioc.bin");
   strcat(imeIndeksSekvencijalne, ".bin");


   //TODO promeniti u wb kasnije, sluzi samo za upis
   //ostavljam wb+, zbog ispisa koje izvrsavam, ionako ne koristim bafere preterano
   FILE* fajlIndeksSekvencijalne = safe_open(imeIndeksSekvencijalne, "wb+", CREATE_FILE_FAILURE);
   FILE* fajlStabla = safe_open(imeStablaIndeksSekvencijalne, "wb+", CREATE_FILE_FAILURE);
   FILE* fajlPrekoracioca = safe_open(imePrekoraciocaIndeksSekvencijalne, "wb+", CREATE_FILE_FAILURE);


   ZaglavljeIS* zaglavljeIS = malloc(sizeof(ZaglavljeIS));
  
   //pocinje od posle zaglavlja, za fseek ce se koristiti
   zaglavljeIS->adresaPrimarneZone = (int) sizeof(ZaglavljeIS);
   zaglavljeIS->brojBlokovaIndeksneZone = 0;
   zaglavljeIS->brojBlokovaPrimarneZone = 0;
   zaglavljeIS->brojBlokovaZonePrekoracenja = 0;

   //TODO (DONE)tek kasnije se dodaje zaglavlje
   //fwrite(zaglavljeIS, sizeof(ZaglavljeIS), 1, fajlIndeksSekvencijalne);


   Blok* blok = (Blok*) malloc(sizeof(Blok));
   PrimarniBlok* primarniBlok = (PrimarniBlok*) malloc(sizeof(PrimarniBlok));
   int i;
   //zaglavlje sa pravim informacijama dodajemo tek na kraju
   fseek(fajlIndeksSekvencijalne, sizeof(ZaglavljeIS), SEEK_SET);
   //kreiranje primarne zone
   //ucitavamo blok po blok
   while(fread(blok, sizeof(Blok), 1, fajlSekvencijalne)){
       //kopiramo niz slogova obicnog bloka u niz slogova primarnog bloka
       memcpy(primarniBlok->slog, blok->slog, sizeof(blok->slog));
       primarniBlok->trenutniBrojSlogova = blok->trenutniBrojSlogova;
       //inkrementiramo broj blokova primarne zone
       zaglavljeIS->brojBlokovaPrimarneZone++;

    //    printf("Blok-----\n");
    //    for(i = 0; i < primarniBlok->trenutniBrojSlogova; i++){
    //        printf("%d\n", primarniBlok->slog[i].evidencioni_broj);
    //    }

       //upis bloka u primarnu zonu datoteke na disku
       fwrite(primarniBlok, sizeof(PrimarniBlok), 1, fajlIndeksSekvencijalne);
   }
   free(blok);
    
   //sekvencijalna je uradila svoj posao, zatvaramo je
   fclose(fajlSekvencijalne);

   


    //Pocinjemo sa formiranjem zone indeksa
    //uzimamo trenutnu poziciju u fajlu posle unosa, i proglasavamo je za pocetak indeksne zone
    zaglavljeIS->adresaIndeksneZone = ftell(fajlIndeksSekvencijalne);

    //matematicki izraz za racunanje logaritma
    //sluzi da za razlicite n faktore blokiranja stabla daje tacna resenja
    int h = ceil(log((double)zaglavljeIS->brojBlokovaPrimarneZone) / log(N_FAKTOR));
    for(i = 1; i <= h; i++)
        zaglavljeIS->brojBlokovaIndeksneZone += ceil((double)zaglavljeIS->brojBlokovaPrimarneZone / pow(N_FAKTOR, h - i));
   
    //iscitavamo slogove primarne zone
    fseek(fajlIndeksSekvencijalne, zaglavljeIS->adresaPrimarneZone, SEEK_SET);

    int max_kljuc = MIN_KEY;
    int pozicijaPrimarnogBloka = zaglavljeIS->adresaPrimarneZone;
    IndeksniBlok* indeksniBlok = (IndeksniBlok*) malloc(sizeof(IndeksniBlok));

    //za cuvanje relativne adrese bloka u primarnoj zoni
    int relativnaAdresaBloka = -1;
    //za relativnu adresu sloga unutar bloka
    int pozicijaSloga;
    while(fread(primarniBlok, sizeof(PrimarniBlok), 1, fajlIndeksSekvencijalne)){
        relativnaAdresaBloka++;
        pozicijaSloga = relativnaAdresaBloka % N_FAKTOR;


        //trazimo najveci kljuc, koji cemo ubaciti u stablo kao list
        for(i = 0; i < primarniBlok->trenutniBrojSlogova; i++){
            if(max_kljuc < primarniBlok->slog[i].evidencioni_broj){
                max_kljuc = primarniBlok->slog[i].evidencioni_broj;
            }
        }

        //ako naidjemo na poslednji primarni blok, dodeljujemo indeksnom bloku maksimalnu 
        //vrednost koju kljuc moze da ima
        //on je uvek poslednji u primarnoj zoni pri ovakvom unosu i ima
        //potencijalno manje slogova, 
        //tako da smem ovako da ,,varam"
        if(primarniBlok->trenutniBrojSlogova < FAKTOR_BLOKIRANJA){
            max_kljuc = MAX_KEY;
        }
      

        //nasli maks kljuc, ubacujemo ga u datoteku ime_stablo.bin
        indeksniBlok->adresaPodredjenogPrimarneZone[pozicijaSloga] = relativnaAdresaBloka;
        //pretpostavka je da nema trenutno prekoracioca
        indeksniBlok->adresaPodredjenogZonePrekoracilaca[pozicijaSloga] = relativnaAdresaBloka; 
        indeksniBlok->evidencioniBrojPrimarneZone[pozicijaSloga] = max_kljuc;
        indeksniBlok->imaPrekoracioca[pozicijaSloga] = 0;
        if(relativnaAdresaBloka != zaglavljeIS->brojBlokovaPrimarneZone - 1)
            indeksniBlok->evidencioniBrojZonePrekoracilaca[pozicijaSloga] = max_kljuc;
        else
        //poslednja zona prekoracioca uvek ima ovu vrednost
            indeksniBlok->evidencioniBrojZonePrekoracilaca[pozicijaSloga] = MAX_KEY;
        //ako se stiglo do ispunjavanja bloka sa slogovima, ubaci u datoteku
        if(pozicijaSloga == N_FAKTOR - 1){
            fwrite(indeksniBlok, sizeof(IndeksniBlok), 1, fajlStabla);
            free(indeksniBlok);
            //koristim calloc za specijalne vrednosti, jer evidencioni broj ima >0
            //dakle ako bude trazio i naidje na nulu (ovde vraca sve sa 0), trazenje staje
            indeksniBlok = (IndeksniBlok*) calloc(1,sizeof(IndeksniBlok));

        }

    }
    free(primarniBlok);
    //ako je ucitani broj slogova u indeksnu zonu ubacen manji od n faktora, ubaci ostale
    if(pozicijaSloga < N_FAKTOR - 1){
        //i = 1;
        //printf("%d,%d,%d,%d \t", indeksniBlok->adresaPodredjenogPrimarneZone[i],indeksniBlok->adresaPodredjenogZonePrekoracilaca[i],indeksniBlok->evidencioniBrojPrimarneZone[i],indeksniBlok->evidencioniBrojZonePrekoracilaca[i]);
        fwrite(indeksniBlok, sizeof(IndeksniBlok), 1, fajlStabla);
    }


    // fseek(fajlStabla, 0, SEEK_SET);
    // while(fread(indeksniBlok, sizeof(IndeksniBlok),1,fajlStabla)){
    //     for(i = 0; i < N_FAKTOR; i++){
    //         printf("%d,%d,%d,%d \t", indeksniBlok->adresaPodredjenogPrimarneZone[i],indeksniBlok->adresaPodredjenogZonePrekoracilaca[i],indeksniBlok->evidencioniBrojPrimarneZone[i],indeksniBlok->evidencioniBrojZonePrekoracilaca[i]);
    //     }
    //     printf("\n");
    // }

    int n;
    int j;
    int B = zaglavljeIS->brojBlokovaPrimarneZone;
    free(indeksniBlok);
    indeksniBlok = calloc(1,sizeof(IndeksniBlok));
    IndeksniBlok* noviIndeksniBlok = (IndeksniBlok*) calloc(1,sizeof(IndeksniBlok));
    int sacuvanaPozicija;
    int korak;
    pozicijaSloga = 0;
    int k ;

    

    fseek(fajlStabla, 0, SEEK_SET);
    //svaki nivo citamo izuzev poslednjeg, jer je on poslednji
    for(i = h; i > 1; i--){
        //fseek(fajlStabla, ;
        korak = 0;
        //brojimo korak za koji treba da se pozicioniramo
        for(j = h; j > i; j--){
            korak += ceil((double) B / pow(N_FAKTOR, h - j + 1));
        }
        fseek(fajlStabla, korak*sizeof(IndeksniBlok), SEEK_SET);
        //printf("Relativna trenutna pozicija je: %d\n", ftell(fajlStabla)/sizeof(IndeksniBlok));
        max_kljuc = MIN_KEY;
        //ukupan broj blokova/cvorova stabla na nivou koji ce se citati
        n =ceil((double) B / pow(N_FAKTOR, h - i + 1));
        //pozicijaSlogaBloka indeksne za novi nivo
        pozicijaSloga = 0;
        for(j = 0; j < n; j++){
            pozicijaSloga = j % N_FAKTOR;
            //procita j. blok
            //printf("Relativna pozicija sa koje je ce se citati: %d\n", ftell(fajlStabla)/sizeof(IndeksniBlok));
            fread(indeksniBlok, sizeof(IndeksniBlok), 1, fajlStabla);
            //sacuva poziciju
            sacuvanaPozicija = ftell(fajlStabla);

            //prodji kroz sve slogove bloka koje citas i nadji max_kljuc i relativnu adresu podredjenog
            //pp da je prvi najveci   
            max_kljuc = indeksniBlok->evidencioniBrojPrimarneZone[0];
            for(k = 1; k < N_FAKTOR; k++){
                //uporedjujem samo od evidencionog na ovom nivou,jer nema prekoracioca
                if(max_kljuc < indeksniBlok->evidencioniBrojPrimarneZone[k]){
                    max_kljuc = indeksniBlok->evidencioniBrojPrimarneZone[k];
                }
            }

            noviIndeksniBlok->adresaPodredjenogPrimarneZone[pozicijaSloga] = -1 + 1.0*ftell(fajlStabla)/sizeof(IndeksniBlok);
            noviIndeksniBlok->adresaPodredjenogZonePrekoracilaca[pozicijaSloga] = 0;
            //ako citamo poslednji list, onda evidencioni broj ka njemu je MAX_KEY
            if(j == n - 1)
                noviIndeksniBlok->evidencioniBrojPrimarneZone[pozicijaSloga] = MAX_KEY;
            else
                noviIndeksniBlok->evidencioniBrojPrimarneZone[pozicijaSloga] = max_kljuc;
            noviIndeksniBlok->evidencioniBrojZonePrekoracilaca[pozicijaSloga] = 0;
            

            
            if(pozicijaSloga == N_FAKTOR - 1){
                //pozicioniraj se na nivo za upisivanje, bio si
                //na onom za citanje pozicije
                //fseek(fajlStabla, (n-j-1)*sizeof(IndeksniBlok), SEEK_CUR);
                fseek(fajlStabla, (n-j + floor(j/N_FAKTOR) - 1)*sizeof(IndeksniBlok),SEEK_CUR);
                // printf("ubacuje se blok u memoriju:");
                // printf("%d,%d,%d,%d \t", noviIndeksniBlok->adresaPodredjenogPrimarneZone[0],noviIndeksniBlok->adresaPodredjenogZonePrekoracilaca[0],noviIndeksniBlok->evidencioniBrojPrimarneZone[0],noviIndeksniBlok->evidencioniBrojZonePrekoracilaca[0]);
                // printf("%d,%d,%d,%d \n", noviIndeksniBlok->adresaPodredjenogPrimarneZone[1],noviIndeksniBlok->adresaPodredjenogZonePrekoracilaca[1],noviIndeksniBlok->evidencioniBrojPrimarneZone[1],noviIndeksniBlok->evidencioniBrojZonePrekoracilaca[1]);
                
                //printf("Relativna dresa u koju je upisivano je: %d\n", ftell(fajlStabla)/sizeof(IndeksniBlok));
                fwrite(noviIndeksniBlok, sizeof(IndeksniBlok), 1, fajlStabla);
                
                //vraca se na citanje
                fseek(fajlStabla, sacuvanaPozicija, SEEK_SET);
                //printf("Relativna dresa na koju smo se vratili: %d\n", ftell(fajlStabla)/sizeof(IndeksniBlok));
                free(noviIndeksniBlok);
                noviIndeksniBlok = (IndeksniBlok*) calloc(1,sizeof(IndeksniBlok));
            }
            

        }
        //ako je izostao jedan blok za punjenje,uvek poslednji
        if(pozicijaSloga < N_FAKTOR - 1){
            //ubaci zaostali blok
            fseek(fajlStabla, sizeof(IndeksniBlok), SEEK_CUR);
            //printf("Relativna dresa u koju je upisivano je: %d\n", ftell(fajlStabla)/sizeof(IndeksniBlok));
            fwrite(noviIndeksniBlok, sizeof(IndeksniBlok), 1, fajlStabla);
            //printf("%d,%d,%d,%d \t", noviIndeksniBlok->adresaPodredjenogPrimarneZone[0],noviIndeksniBlok->adresaPodredjenogZonePrekoracilaca[0],noviIndeksniBlok->evidencioniBrojPrimarneZone[0],noviIndeksniBlok->evidencioniBrojZonePrekoracilaca[0]);
            //printf("%d,%d,%d,%d \n", noviIndeksniBlok->adresaPodredjenogPrimarneZone[1],noviIndeksniBlok->adresaPodredjenogZonePrekoracilaca[1],noviIndeksniBlok->evidencioniBrojPrimarneZone[1],noviIndeksniBlok->evidencioniBrojZonePrekoracilaca[1]);
            free(noviIndeksniBlok);
            noviIndeksniBlok = (IndeksniBlok*) calloc(1,sizeof(IndeksniBlok));
            
        }
    }

    free(noviIndeksniBlok);
    //ZONA INDEKSA KREIRANA !!!!!!!!!!
    
    //pocetak kreiranja zone prekoracioca

    //SLEDI ZONA PREKORACENJA KOJA JE PO DIFOLTU PRAZNA ALI ALOCIRANA I SPREGNUTA I NAPRAVLJENA NE MORA
    //DA SE GLEDA SEM DA SE UBACI ZAGLAVLJE
    ZaglavljePrekoracioca* zaglavljePrekoracioca = malloc(sizeof(ZaglavljePrekoracioca));
    zaglavljePrekoracioca->sledeciSlobodniBlok = 0;
    BlokPrekoracioc* blokPrekoracioc = (BlokPrekoracioc*) malloc(sizeof(BlokPrekoracioc));
    TvEmisija tvEmisija;
    //STATICKO ALOCIRANJE 3 BLOKA i njihovo sprezanje

    fwrite(zaglavljePrekoracioca, sizeof(ZaglavljePrekoracioca), 1, fajlPrekoracioca);
    for(i = 0; i < STATICKA_ALOKACIJA; i++){
        //svaki sledeci je spregnut sa prethodnim
        blokPrekoracioc->adresaNarednog = i + 1;
        //samo da mu stavi -1 kao adresu, da ne postoji
        if(blokPrekoracioc->adresaNarednog == STATICKA_ALOKACIJA){
            blokPrekoracioc->adresaNarednog = -1;
        }
        //stavimo da je logicki neaktivan
        tvEmisija.statusPolje = 1;
        blokPrekoracioc->slog = tvEmisija;
        fwrite(blokPrekoracioc, sizeof(BlokPrekoracioc), 1, fajlPrekoracioca);
    }



    free(zaglavljePrekoracioca);
    free(blokPrekoracioc);


    //Na kraju, postavljam zaglavljeIS sa potrebnim informacijama na pocetak glavne datoteke
    fseek(fajlIndeksSekvencijalne, 0, SEEK_SET);
    fwrite(zaglavljeIS, sizeof(ZaglavljeIS), 1, fajlIndeksSekvencijalne);
    //indeks sekvencijalna kreirana, moze se zatvoriti
    fclose(fajlIndeksSekvencijalne);
    fclose(fajlStabla);
    fclose(fajlPrekoracioca);
}


void upisiSlogAktivnoj(){
    if(!strcmp("", aktivnaDatoteka)){
        printf("Ne postoji aktivna datoteka, odaberi prvo aktivnu datoteku! \n");
        return;
    }
    
    //cuva ime aktivne datoteke
    char privremena[FILENAME_MAX];
    memset(privremena, 0, FILENAME_MAX);
    memcpy(privremena, aktivnaDatoteka, strlen(aktivnaDatoteka) - 4);
    //prvo pristupamo aktivnom stablu
    //imedatoteke_stablo.bin
    char imeStablaAktivne[FILENAME_MAX + 4 + 1 + 6];
    memset(imeStablaAktivne, 0, FILENAME_MAX + 4 + 1 + 6);
    //imedatoteke_prekoracioc.bin
    //imamo i za svaki slucaj i prekoracioce otvorene
    char imePrekoraciocaIndeksSekvencijalne[FILENAME_MAX + 4 + 1 + 11];
    memset(imePrekoraciocaIndeksSekvencijalne, 0, FILENAME_MAX + 4 + 1 + 11);

    strcat(imeStablaAktivne, privremena);
    strcat(imeStablaAktivne, "_stablo.bin");
    strcat(imePrekoraciocaIndeksSekvencijalne, privremena);
    strcat(imePrekoraciocaIndeksSekvencijalne, "_prekoracioc.bin");


    FILE* fajlStablaAktivne = safe_open(imeStablaAktivne, "rb+", OPEN_CREATED_FILE_FAILURE);
    FILE* fajlPrekoraciocaAktivne  = safe_open(imePrekoraciocaIndeksSekvencijalne, "rb+", OPEN_CREATED_FILE_FAILURE); 
    //postavljanje fajlova na pocetak
    fseek(fajlAktivneDatoteke, 0, SEEK_SET);
    fseek(fajlStablaAktivne, 0, SEEK_SET);
    fseek(fajlPrekoraciocaAktivne, 0, SEEK_SET);
    int noviEvidencioniBroj;

    //ZaglavljeIS zaglavljeIS;
    //fread(&zaglavljeIS, sizeof(ZaglavljeIS), 1, fajlAktivneDatoteke);
    //printf("Pozicija %d; \n", ftell(fajlAktivneDatoteke)/sizeof(ZaglavljeIS));
    while(1){
        printf("Unesi evidencioni broj: ");
        fflush(stdin);
        scanf("%d", &noviEvidencioniBroj);
        if(noviEvidencioniBroj < MIN_KEY || noviEvidencioniBroj > MAX_KEY)
            continue;
        else
            break;
    }

    TvEmisija* tvEmisija = (TvEmisija*) malloc(sizeof(TvEmisija));
    //TODO interaktivno
    tvEmisija->evidencioni_broj = noviEvidencioniBroj;
    napraviSlog(tvEmisija);

    //PRVO pretraga po primarnoj zoni
    //ako uspesno nadje, znaci da kljuc nije jedinstven i da se ne moze
    //upisati slog u datoteku
    RezultatTrazenja rez = traziSlogAktivne(noviEvidencioniBroj, fajlStablaAktivne, fajlPrekoraciocaAktivne);
    if(rez.nasao){
        printf("Zeljeni slog se ne moze upisati u datoteku, nije jedinstven evidencioni broj! \n");
        return;
    } else if(rez.nasao == -1){
        return; //za gresku datoteke
    }


    int i;
    int pozicija;
    int relativnaPozicijaElementaIndeksa;
    ZaglavljePrekoracioca zaglavljePrekoracioca;
    BlokPrekoracioc blokPrekoracioc;
    //ako je resenje u primarnoj zoni
    if(rez.primaranJe){
        PrimarniBlok primarniBlok;
        fseek(fajlAktivneDatoteke, sizeof(ZaglavljeIS) + rez.relativnaAdresaBloka*sizeof(PrimarniBlok), SEEK_SET);
        fread(&primarniBlok, sizeof(PrimarniBlok), 1, fajlAktivneDatoteke);
        //u indeksni blok dodajemo izmene u najveci kljuc primarne zone
        relativnaPozicijaElementaIndeksa = rez.relativnaAdresaBloka % N_FAKTOR;
        IndeksniBlok indeksniBlok;
        fseek(fajlStablaAktivne, floor(rez.relativnaAdresaBloka/N_FAKTOR)*sizeof(IndeksniBlok) , SEEK_SET);
        fread(&indeksniBlok, sizeof(IndeksniBlok), 1, fajlStablaAktivne);
        //ako je blok kompletan, onda mora i zona prekoracenja da se pristupa
        if(primarniBlok.trenutniBrojSlogova == FAKTOR_BLOKIRANJA){
            
            //menja se nova maksimalna vrednost primarne zone, n - 1 vrednos bloka
            //pomeramo slogove unutar bloka
            //ovo je rezervni jer se izbija iz opsega
            //i on uvek ide u prekoracenje
            TvEmisija granicniSlog = primarniBlok.slog[FAKTOR_BLOKIRANJA - 1];
            for(i = primarniBlok.trenutniBrojSlogova - 1; i >= rez.relativnaAdresaSloga + 1; i--){
                primarniBlok.slog[i] = primarniBlok.slog[i - 1];
            }
            //postavljamo novi na njegovo mesto
            primarniBlok.slog[rez.relativnaAdresaSloga] = *tvEmisija;
            //DOLE je promena maks elementa u primarnoj zoni
            //i zona prekoracenja se menja
            //pristupam zaglavlju datoteke ogranicenja
            fread(&zaglavljePrekoracioca, sizeof(ZaglavljePrekoracioca), 1, fajlPrekoraciocaAktivne);
            //skacem na adresu prve slobodne lokacije
            //alociras prostor za dalji rad
            if(zaglavljePrekoracioca.sledeciSlobodniBlok == -1){
                    BlokPrekoracioc* blokAlokacije = (BlokPrekoracioc*) calloc(1, sizeof(BlokPrekoracioc));
                    //sacuva se pozicija pa pozicionira na kraj datoteke
                    pozicija = ftell(fajlPrekoraciocaAktivne);
                    fseek(fajlPrekoraciocaAktivne, 0, SEEK_END);
                    //alocira se prostor
                    zaglavljePrekoracioca.sledeciSlobodniBlok = (ftell(fajlPrekoraciocaAktivne)-sizeof(ZaglavljePrekoracioca))/sizeof(BlokPrekoracioc);
                    for(i = 0; i < STATICKA_ALOKACIJA; i++){
                        //ako nije poslednja lokacija,ako jeste,daj -1
                        if(i != STATICKA_ALOKACIJA - 1)
                            blokAlokacije->adresaNarednog = zaglavljePrekoracioca.sledeciSlobodniBlok + i + 1;
                        else    
                            blokAlokacije->adresaNarednog = -1;
                        blokAlokacije->slog.statusPolje = 1; //jos nije aktivan
                        blokAlokacije->slog.evidencioni_broj = 0; //kljuc invalidan
                        fwrite(blokAlokacije, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne);
                    }
                    free(blokAlokacije);
                    fseek(fajlPrekoraciocaAktivne, pozicija, SEEK_SET);
            } //prostor alociran

            BlokPrekoracioc noviBlokPrekoracioc;
            //skacem i citam slobodni blok prekoracioc
            fseek(fajlPrekoraciocaAktivne, zaglavljePrekoracioca.sledeciSlobodniBlok*sizeof(BlokPrekoracioc), SEEK_CUR);
            fread(&noviBlokPrekoracioc, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne);
            //zaglavlju dajem adresu sledeceg slobodnog, da ima
            zaglavljePrekoracioca.sledeciSlobodniBlok = noviBlokPrekoracioc.adresaNarednog;
            //naredni na kojeg novi blok prekoracioc pokazuje je onaj prekoracioc koji je 
            //ranije pokazivan, al vise ne, pod uslovom da uopste postoji
            //if(indeksniBlok.evidencioniBrojPrimarneZone[relativnaPozicijaElementaIndeksa] == indeksniBlok.evidencioniBrojZonePrekoracilaca[relativnaPozicijaElementaIndeksa]  && indeksniBlok.adresaPodredjenogZonePrekoracilaca[relativnaPozicijaElementaIndeksa] == indeksniBlok.adresaPodredjenogPrimarneZone[relativnaPozicijaElementaIndeksa]){
            if(!indeksniBlok.imaPrekoracioca[relativnaPozicijaElementaIndeksa]){
                //ako nema onda ni na sta ne pokazuje
                noviBlokPrekoracioc.adresaNarednog = -1;
            }
            else{
                noviBlokPrekoracioc.adresaNarednog = indeksniBlok.adresaPodredjenogZonePrekoracilaca[relativnaPozicijaElementaIndeksa];
            }
            //preventivno se javlja da zona prekoracenja ima clanova
            indeksniBlok.imaPrekoracioca[relativnaPozicijaElementaIndeksa] = 1;
            //menja se nova maksimalna vrednost primarne zone, poslednja/[n - 1] vrednost bloka je sad maksimalna
            indeksniBlok.evidencioniBrojPrimarneZone[relativnaPozicijaElementaIndeksa] = primarniBlok.slog[FAKTOR_BLOKIRANJA - 1].evidencioni_broj;
            noviBlokPrekoracioc.slog = granicniSlog;
            noviBlokPrekoracioc.slog.statusPolje = 0;
            //nova adresa za podredjeni blok je od onog bloka koji je sad napravljen
            indeksniBlok.adresaPodredjenogZonePrekoracilaca[relativnaPozicijaElementaIndeksa] = -1 + (ftell(fajlPrekoraciocaAktivne)-sizeof(ZaglavljePrekoracioca))/sizeof(BlokPrekoracioc);
            //upisujemo taj izmenjeni blok u listu prekoracenja
            //al se prvo pozicioniramo na tu zonu
            fseek(fajlPrekoraciocaAktivne, -sizeof(BlokPrekoracioc), SEEK_CUR);
            fwrite(&noviBlokPrekoracioc, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne);
            //i vracamo zaglavlje
            fseek(fajlPrekoraciocaAktivne, 0, SEEK_SET);
            fwrite(&zaglavljePrekoracioca, sizeof(ZaglavljePrekoracioca), 1, fajlPrekoraciocaAktivne);
            fflush(fajlPrekoraciocaAktivne);
        }
        //ako ne, onda ide samo obicno pomeranje unutar bloka
        else{
            //pomeramo slogove u bloku
            for(i = primarniBlok.trenutniBrojSlogova; i >= rez.relativnaAdresaSloga + 1; i--){
                primarniBlok.slog[i] = primarniBlok.slog[i - 1];
            }
            //konacno dodavanje sloga
            primarniBlok.slog[rez.relativnaAdresaSloga] = *tvEmisija;
            //i inkrementiranje jer smo dodali jedan slog
            primarniBlok.trenutniBrojSlogova++;
            //ako smo dostigli maks, menjamo da kljuc primarne
            //zone u slucaju poslednjeg ne bude MAX_KEY, vec kljuc poslednjeg sloga
            if(primarniBlok.trenutniBrojSlogova == FAKTOR_BLOKIRANJA){
                indeksniBlok.evidencioniBrojPrimarneZone[relativnaPozicijaElementaIndeksa] = primarniBlok.slog[FAKTOR_BLOKIRANJA - 1].evidencioni_broj;
            }
        } 

        //na kraju, upisujemo izmenjeni blok u memoriju i indeksni blok
        //prvo se pozicioniramo unazad za obe datoteke
        fseek(fajlAktivneDatoteke, -sizeof(PrimarniBlok), SEEK_CUR);
        fseek(fajlStablaAktivne, -sizeof(IndeksniBlok), SEEK_CUR);
        //pa onda upisemo sta zelimo
        fwrite(&primarniBlok, sizeof(PrimarniBlok), 1, fajlAktivneDatoteke);
        fwrite(&indeksniBlok, sizeof(IndeksniBlok), 1, fajlStablaAktivne);        
    }
    //ako je u zoni prekoracenja
    else{
        //ako je na prvom stao, onda ide drugacija procedura
        if(rez.relativnaAdresaPrethodnogBloka == -1){
            IndeksniBlok indeksniBlok;
            BlokPrekoracioc noviBlokPrekoracioc;
            fseek(fajlPrekoraciocaAktivne, 0, SEEK_SET);
            fread(&zaglavljePrekoracioca, sizeof(ZaglavljePrekoracioca), 1, fajlPrekoraciocaAktivne);
            //proveri odmah ima li prostora za novi blok prekoracioc
            //isti kod kao i gore
            if(zaglavljePrekoracioca.sledeciSlobodniBlok == -1){
                    BlokPrekoracioc* blokAlokacije = (BlokPrekoracioc*) calloc(1, sizeof(BlokPrekoracioc));
                    //sacuva se pozicija pa pozicionira na kraj datoteke
                    pozicija = ftell(fajlPrekoraciocaAktivne);
                    fseek(fajlPrekoraciocaAktivne, 0, SEEK_END);
                    //alocira se prostor
                    zaglavljePrekoracioca.sledeciSlobodniBlok = (ftell(fajlPrekoraciocaAktivne)-sizeof(ZaglavljePrekoracioca))/sizeof(BlokPrekoracioc);
                    for(i = 0; i < STATICKA_ALOKACIJA; i++){
                        //ako nije poslednja lokacija,ako jeste,daj -1
                        if(i != STATICKA_ALOKACIJA - 1)
                            blokAlokacije->adresaNarednog = zaglavljePrekoracioca.sledeciSlobodniBlok + i + 1;
                        else    
                            blokAlokacije->adresaNarednog = -1;
                        blokAlokacije->slog.statusPolje = 1; //jos nije aktivan
                        blokAlokacije->slog.evidencioni_broj = 0; //kljuc invalidan
                        fwrite(blokAlokacije, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne);
                    }
                    free(blokAlokacije);
                    fseek(fajlPrekoraciocaAktivne, pozicija, SEEK_SET);
            } //prostor alociran
            
            //pozicioniraj se na slobodni od zaglavlja
            fseek(fajlPrekoraciocaAktivne, zaglavljePrekoracioca.sledeciSlobodniBlok*sizeof(BlokPrekoracioc), SEEK_CUR);
            //na list smo vec pozicionirani u trazenju,samo citamo, al prvo vratimo unazad
            fseek(fajlStablaAktivne, -sizeof(IndeksniBlok), SEEK_CUR);
            fread(&indeksniBlok, sizeof(IndeksniBlok), 1, fajlStablaAktivne);
            //i ubacujemo da pokazuje zona prekoracenja na novi blok prekoracioc
            indeksniBlok.adresaPodredjenogZonePrekoracilaca[rez.relativnaZonaPrekoracioca] = zaglavljePrekoracioca.sledeciSlobodniBlok;
            //ovde upada i onaj kad nema prekoracioca   
            indeksniBlok.imaPrekoracioca[rez.relativnaZonaPrekoracioca] = 1;       
            //noviBlok dobija unesenu tvEmisiju
            fread(&noviBlokPrekoracioc, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne);
            noviBlokPrekoracioc.slog = *tvEmisija;
            //zaglavlje mora da pokazuje na novu slobodnu lokaciju
            //stoga ubacujemo da pokazuje na sledecu od onog na koji je pokazivao ranije
            zaglavljePrekoracioca.sledeciSlobodniBlok = noviBlokPrekoracioc.adresaNarednog;
            //stavljamo adresu tamo gde je trazenje stalo
            //ako je uopste postojala zona prekoracenja pre
            noviBlokPrekoracioc.adresaNarednog = rez.relativnaAdresaBloka;
            
            //kraj alokacije, smestamo blokove na mesta
            //vracamo se u stablu unazad pa onda upisujemo indeksniBlok
            fseek(fajlStablaAktivne, -sizeof(IndeksniBlok), SEEK_CUR);
            fwrite(&indeksniBlok, sizeof(IndeksniBlok), 1, fajlStablaAktivne);
            //upisujemo novi blok prekoracioc
            fseek(fajlPrekoraciocaAktivne, sizeof(ZaglavljePrekoracioca) +  sizeof(BlokPrekoracioc)*indeksniBlok.adresaPodredjenogZonePrekoracilaca[rez.relativnaZonaPrekoracioca], SEEK_SET);
            fwrite(&noviBlokPrekoracioc, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne);
            //upisujemo zaglavlje prkeoracioca
            fseek(fajlPrekoraciocaAktivne, 0, SEEK_SET);
            fwrite(&zaglavljePrekoracioca, sizeof(ZaglavljePrekoracioca), 1, fajlPrekoraciocaAktivne);
            //kraj upisa, azurirano sve
        }
        //ako je negde izmedju blokova, onda sprezemo 
        else{
            
            BlokPrekoracioc noviBlokPrekoracioc;
            fseek(fajlPrekoraciocaAktivne, 0, SEEK_SET);
            fread(&zaglavljePrekoracioca, sizeof(ZaglavljePrekoracioca), 1, fajlPrekoraciocaAktivne);
            //proveri odmah ima li prostora za novi blok prekoracioc
            //isti kod kao i gore
            if(zaglavljePrekoracioca.sledeciSlobodniBlok == -1){
                    BlokPrekoracioc* blokAlokacije = (BlokPrekoracioc*) calloc(1, sizeof(BlokPrekoracioc));
                    //sacuva se pozicija pa pozicionira na kraj datoteke
                    pozicija = ftell(fajlPrekoraciocaAktivne);
                    fseek(fajlPrekoraciocaAktivne, 0, SEEK_END);
                    //alocira se prostor
                    zaglavljePrekoracioca.sledeciSlobodniBlok = (ftell(fajlPrekoraciocaAktivne)-sizeof(ZaglavljePrekoracioca))/sizeof(BlokPrekoracioc);
                    for(i = 0; i < STATICKA_ALOKACIJA; i++){
                        //ako nije poslednja lokacija,ako jeste,daj -1
                        if(i != STATICKA_ALOKACIJA - 1)
                            blokAlokacije->adresaNarednog = zaglavljePrekoracioca.sledeciSlobodniBlok + i + 1;
                        else    
                            blokAlokacije->adresaNarednog = -1;
                        blokAlokacije->slog.statusPolje = 1; //jos nije aktivan
                        blokAlokacije->slog.evidencioni_broj = 0; //kljuc invalidan
                        fwrite(blokAlokacije, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne);
                    }
                    free(blokAlokacije);
                    fseek(fajlPrekoraciocaAktivne, pozicija, SEEK_SET);
            } //prostor alociran



            //za sprezanje cuvamo prethodni koji je prosao
            BlokPrekoracioc prethodniBlokPrekoracioc;
            fseek(fajlPrekoraciocaAktivne, rez.relativnaAdresaPrethodnogBloka*sizeof(BlokPrekoracioc), SEEK_CUR);
            fread(&prethodniBlokPrekoracioc, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne);
            //pozicioniramo se na slobodan blok
            fseek(fajlPrekoraciocaAktivne, sizeof(ZaglavljePrekoracioca) + zaglavljePrekoracioca.sledeciSlobodniBlok*sizeof(BlokPrekoracioc), SEEK_SET);
            //citamo blok prekoracioc koji je slobodan
            fread(&noviBlokPrekoracioc, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne);
            //sad sprezemo novi blok sa prethodnim i onim na kojem smo stali
            //i azuriramo zaglavlje
            prethodniBlokPrekoracioc.adresaNarednog = zaglavljePrekoracioca.sledeciSlobodniBlok;
            zaglavljePrekoracioca.sledeciSlobodniBlok = noviBlokPrekoracioc.adresaNarednog;
            noviBlokPrekoracioc.adresaNarednog = rez.relativnaAdresaBloka;
            noviBlokPrekoracioc.slog = *tvEmisija;
            //posao zavrsen, vratimo sve blokove u memoriju
            //prvo novi blok
            fseek(fajlPrekoraciocaAktivne, -sizeof(BlokPrekoracioc), SEEK_CUR);
            fwrite(&noviBlokPrekoracioc, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne);
            //pa onda prethodni
            fseek(fajlPrekoraciocaAktivne, sizeof(ZaglavljePrekoracioca) + rez.relativnaAdresaPrethodnogBloka*sizeof(BlokPrekoracioc), SEEK_SET);
            fwrite(&prethodniBlokPrekoracioc, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne);
            //pa zaglavlje
            fseek(fajlPrekoraciocaAktivne, 0, SEEK_SET);
            fwrite(&zaglavljePrekoracioca, sizeof(ZaglavljePrekoracioca), 1, fajlPrekoraciocaAktivne);
        }
    }


    //ispisiIndeksSekvencijalnu();
    fflush(fajlAktivneDatoteke); //ubaci iz bafera sve sto je izostalo
    fclose(fajlStablaAktivne);
    fclose(fajlPrekoraciocaAktivne);
}


void traziSlogAktivneOpcija(){
    if(!strcmp("", aktivnaDatoteka)){
        printf("Ne postoji aktivna datoteka, odaberi prvo aktivnu datoteku! \n");
        return;
    }
    
    //cuva ime aktivne datoteke
    char privremena[FILENAME_MAX];
    memset(privremena, 0, FILENAME_MAX);
    memcpy(privremena, aktivnaDatoteka, strlen(aktivnaDatoteka) - 4);
    //prvo pristupamo aktivnom stablu
    //imedatoteke_stablo.bin
    char imeStablaAktivne[FILENAME_MAX + 4 + 1 + 6];
    memset(imeStablaAktivne, 0, FILENAME_MAX + 4 + 1 + 6);
    //imedatoteke_prekoracioc.bin
    //imamo i za svaki slucaj i prekoracioce otvorene
    char imePrekoraciocaIndeksSekvencijalne[FILENAME_MAX + 4 + 1 + 11];
    memset(imePrekoraciocaIndeksSekvencijalne, 0, FILENAME_MAX + 4 + 1 + 11);

    strcat(imeStablaAktivne, privremena);
    strcat(imeStablaAktivne, "_stablo.bin");
    strcat(imePrekoraciocaIndeksSekvencijalne, privremena);
    strcat(imePrekoraciocaIndeksSekvencijalne, "_prekoracioc.bin");


    FILE* fajlStablaAktivne = safe_open(imeStablaAktivne, "rb+", OPEN_CREATED_FILE_FAILURE);
    FILE* fajlPrekoraciocaAktivne  = safe_open(imePrekoraciocaIndeksSekvencijalne, "rb+", OPEN_CREATED_FILE_FAILURE); 
    //postavljanje fajlova na pocetak
    fseek(fajlAktivneDatoteke, 0, SEEK_SET);
    fseek(fajlStablaAktivne, 0, SEEK_SET);
    fseek(fajlPrekoraciocaAktivne, 0, SEEK_SET);
    int noviEvidencioniBroj;

    while(1){
        printf("Unesi evidencioni broj: ");
        fflush(stdin);
        scanf("%d", &noviEvidencioniBroj);
        if(noviEvidencioniBroj < MIN_KEY || noviEvidencioniBroj > MAX_KEY)
            continue;
        else
            break;
    }

    RezultatTrazenja rez = traziSlogAktivne(noviEvidencioniBroj, fajlStablaAktivne, fajlPrekoraciocaAktivne);
    if(rez.nasao){
        printf("Zeljeni slog se ne moze upisati u datoteku, nije jedinstven evidencioni broj! \n");
        return;
    } else if(rez.nasao == -1){
        return; //za gresku datoteke
    }

    fclose(fajlStablaAktivne);
    fclose(fajlPrekoraciocaAktivne);
}


void ispisiSvePodatkeAktivne(){
    if(!strcmp("", aktivnaDatoteka)){
        printf("Ne postoji aktivna datoteka, odaberi prvo aktivnu datoteku! \n");
        return;
    }

    //cuva ime aktivne datoteke
    char privremena[FILENAME_MAX];
    memset(privremena, 0, FILENAME_MAX);
    memcpy(privremena, aktivnaDatoteka, strlen(aktivnaDatoteka) - 4);
    //prvo pristupamo aktivnom stablu
    //imedatoteke_stablo.bin
    char imeStablaAktivne[FILENAME_MAX + 4 + 1 + 6];
    memset(imeStablaAktivne, 0, FILENAME_MAX + 4 + 1 + 6);
    //imedatoteke_prekoracioc.bin
    //imamo i za svaki slucaj i prekoracioce otvorene
    char imePrekoraciocaIndeksSekvencijalne[FILENAME_MAX + 4 + 1 + 11];
    memset(imePrekoraciocaIndeksSekvencijalne, 0, FILENAME_MAX + 4 + 1 + 11);

    strcat(imeStablaAktivne, privremena);
    strcat(imeStablaAktivne, "_stablo.bin");
    strcat(imePrekoraciocaIndeksSekvencijalne, privremena);
    strcat(imePrekoraciocaIndeksSekvencijalne, "_prekoracioc.bin");


    FILE* fajlStablaAktivne = safe_open(imeStablaAktivne, "rb+", OPEN_CREATED_FILE_FAILURE);
    FILE* fajlPrekoraciocaAktivne  = safe_open(imePrekoraciocaIndeksSekvencijalne, "rb+", OPEN_CREATED_FILE_FAILURE);

    ispisiPrimarnuZonu(fajlAktivneDatoteke);
    ispisiIndeksnuZonu(fajlStablaAktivne);
    ispisiZonuPrekoracioca(fajlPrekoraciocaAktivne);


    fclose(fajlStablaAktivne);
    fclose(fajlPrekoraciocaAktivne);
}
//za rucno namestanje nekih stvari u datokama koje mi smetaju
//TODO obrisati
void cheat(){
    FILE* f = safe_open("ahmed_prekoracioc.bin", "rb+", 1);
    ZaglavljePrekoracioca zp;
    fseek(f, 0, SEEK_SET);
    fread(&zp, sizeof(ZaglavljePrekoracioca), 1, f);
    zp.sledeciSlobodniBlok = 1;
    fwrite(&zp, sizeof(ZaglavljePrekoracioca), 1, f);
    fflush(f);
    fclose(f);
}

void logickoBrisanje(){
    if(!strcmp("", aktivnaDatoteka)){
        printf("Ne postoji aktivna datoteka, odaberi prvo aktivnu datoteku! \n");
        return;
    }
    
    //cuva ime aktivne datoteke
    char privremena[FILENAME_MAX];
    memset(privremena, 0, FILENAME_MAX);
    memcpy(privremena, aktivnaDatoteka, strlen(aktivnaDatoteka) - 4);
    //prvo pristupamo aktivnom stablu
    //imedatoteke_stablo.bin
    char imeStablaAktivne[FILENAME_MAX + 4 + 1 + 6];
    memset(imeStablaAktivne, 0, FILENAME_MAX + 4 + 1 + 6);
    //imedatoteke_prekoracioc.bin
    //imamo i za svaki slucaj i prekoracioce otvorene
    char imePrekoraciocaIndeksSekvencijalne[FILENAME_MAX + 4 + 1 + 11];
    memset(imePrekoraciocaIndeksSekvencijalne, 0, FILENAME_MAX + 4 + 1 + 11);

    strcat(imeStablaAktivne, privremena);
    strcat(imeStablaAktivne, "_stablo.bin");
    strcat(imePrekoraciocaIndeksSekvencijalne, privremena);
    strcat(imePrekoraciocaIndeksSekvencijalne, "_prekoracioc.bin");


    FILE* fajlStablaAktivne = safe_open(imeStablaAktivne, "rb+", OPEN_CREATED_FILE_FAILURE);
    FILE* fajlPrekoraciocaAktivne  = safe_open(imePrekoraciocaIndeksSekvencijalne, "rb+", OPEN_CREATED_FILE_FAILURE); 
    //postavljanje fajlova na pocetak
    fseek(fajlAktivneDatoteke, 0, SEEK_SET);
    fseek(fajlStablaAktivne, 0, SEEK_SET);
    fseek(fajlPrekoraciocaAktivne, 0, SEEK_SET);
    int noviEvidencioniBroj;

    while(1){
        printf("Unesi evidencioni broj: ");
        fflush(stdin);
        scanf("%d", &noviEvidencioniBroj);
        if(noviEvidencioniBroj < MIN_KEY || noviEvidencioniBroj > MAX_KEY)
            continue;
        else
            break;
    }

    RezultatTrazenja rez = traziSlogAktivne(noviEvidencioniBroj, fajlStablaAktivne, fajlPrekoraciocaAktivne);
  
    if(!rez.nasao){
        printf("Ne moze se brisati nepostojece");
        return;
    }
    else if(rez.nasao == -1){
        return; //za gresku datoteke
    }
    //treba nam da znamo da li je slog iz prekoracioca
    //ili primarne zone

    //ako je iz prekoracioca,dodelio sam mu -1
    if(rez.relativnaAdresaSloga == -1){
        BlokPrekoracioc blokPrekoracioc;
        //pozicioniramo se na trazeni blok
        fseek(fajlPrekoraciocaAktivne, sizeof(ZaglavljePrekoracioca) + rez.relativnaAdresaBloka*sizeof(BlokPrekoracioc), SEEK_SET);
        //procitamo ga
        fread(&blokPrekoracioc, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne);
        //postavimo da je neaktivan, ako nije
        if(blokPrekoracioc.slog.statusPolje == 0)
            blokPrekoracioc.slog.statusPolje = 1;
        else{ //u suporotnom je neka greska
            printf("Slog je vec neaktivan! \n");
            return;
        }

        //postavljamo ga nazad
        fseek(fajlPrekoraciocaAktivne, -sizeof(BlokPrekoracioc), SEEK_CUR);
        fwrite(&blokPrekoracioc, sizeof(BlokPrekoracioc), 1, fajlPrekoraciocaAktivne);
    }
    //ako je ipak u primarnoj zoni
    else{
        PrimarniBlok primarniBlok;
        //postavljamo se u dati blok primarne zone
        fseek(fajlAktivneDatoteke, sizeof(ZaglavljeIS) + rez.relativnaAdresaBloka*sizeof(PrimarniBlok), SEEK_SET);
        //citamo blok
        fread(&primarniBlok, sizeof(PrimarniBlok), 1, fajlAktivneDatoteke);
        //onda se pozicioniramo na slog u njemu i dodelimo da je neaktivan
        //ako nije
        if(primarniBlok.slog[rez.relativnaAdresaSloga].statusPolje == 0){
            primarniBlok.slog[rez.relativnaAdresaSloga].statusPolje = 1;
        }
        else{//izbila neka greska
            printf("Paznja, slog vec bio neaktivan, tj logicki obrisan! \n");
            return;
        }

        //i upisujemo blok tamo gde mu je mesto
        //al ga prvo pozicioniramo
        fseek(fajlAktivneDatoteke, -sizeof(PrimarniBlok), SEEK_CUR);
        fwrite(&primarniBlok, sizeof(PrimarniBlok), 1, fajlAktivneDatoteke);
    }

    fclose(fajlPrekoraciocaAktivne);
    fclose(fajlStablaAktivne);
} 