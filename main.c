#include <stdio.h>
#include "opcije.h"
#include "struktura.h"



void printMenu();

int main()
{
    int choice;
    RezultatTrazenja rez;

    do{
        printMenu();

        scanf("%d", &choice);

        switch(choice){
            case 1:
                formirajPraznuDatoteku();
                break;
            case 2:
                izborAktivneDatoteke();
                break;
            case 3:
                prikazAktivneDatoteke();
                break;
            case 4:
                formirajSerijskuDatoteku();
                break;
            case 5:
                formirajSekvencijalnuDatoteku();
                break;
            case 6:
                formirajIndeksSekvencijalnuDatoteku();
                break;
            case 7:
                upisiSlogAktivnoj();
                break;
            case 8:
                traziSlogAktivneOpcija();
                break;
            case 9:
                logickoBrisanje();
                break;
            case 10:
                ispisiSvePodatkeAktivne();
                break;
            case 11:
                ispisiIndeksSekvencijalnu();
                break;
            case 555333:
                cheat();
                break;             
                
            default:
                printf("Dodavace se \n");
        }
    }while(choice != -1);

    return 0;
}


void printMenu(){
    printf("Izaberi opciju \n");

    printf("1. formiranje prazne datoteke \n"
            "2. izbor aktivne datoteke zadavanjem njenog naziva\n"
            "3. prikaz naziva aktivne datoteke\n"
            "4. formiranje serijske datoteke direktnim unosom podataka \n"
            "5. formiranje sekvencijalne datoteke tako što će se učitati slogovi iz serijske datoteke \n"
            "6. formiranje aktivne datoteke iz sekvencijalne datoteke \n"
            "7. upis novog sloga u aktivnu datoteku direktnim unosom podataka u realnom vremenu\n"
            "8. traženje proizvoljnog sloga u aktivnoj datoteci i njegov prikaz \n"
            "9. logičko brisanje aktuelnog sloga iz aktivne datoteke \n"
            "10. prikaz svih slogova aktivne datoteke \n"
          );
    printf("11. Ispisi indeks sekvencijalnu \n");

}
