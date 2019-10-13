#ifndef FUNCTII
#define FUNCTII

typedef struct istoric {
	char *rezultat;
	char *operatie;
	struct istoric *urm;
} THistory;

typedef struct card {
	char numar[17];
	char pin[5];
	char expirare[6];
	char CVV[4];
	char status[7];
	int balance;
	int numar_incercari;
	int este_copie;
	THistory *istoric;
	struct card *urm;
} TCard, *LSC, **aLSC;


typedef struct lista {
	TCard *info; //pointer catre inceputul sublistei
	struct lista *urm;	
	int nr_carduri;
} LC, *TLC, **ALC;

void eliberare_istoric (THistory **h);
void eliberareLSC(aLSC card);
void eliberareLC(ALC aL);

LSC CardAloca();
LC *AlocaLC ();
THistory *AlocaIstoric (char *succes, char *op);

//suma cifrelor din sirul reprezentand numarul cardului
int suma_elemente (char *sir);

int InsIncLSC(TLC *L, TCard *info);

int exista_card (char *sir, TLC lista);

//returneaza celula LC in care va fi inserat cardul
TLC pozitionare_carduri (int nr_max, TCard *card, ALC lista);

//elimina celulele vechi (copiile raman alocate in lista)
void eliminare_duplicate (ALC lista, LSC card);

//repozitionarea cardurilor dupa cresterea numarului maxim
void repozitionare (ALC lista, int nr_max);

void afisare_istoric (THistory *h, FILE *o);

int adauga_card (char *sir, int *nr_max, ALC lista, FILE *o);

void arata_carduri (TLC lista, FILE *f, char numar[17]);

int stergere_card (ALC lista, char *numar, int nr_max);

int InsIncIstoric(aLSC L, char *succes, char *comanda);

int inserare_card(char *numar, char *pin, char s[1000], TLC lista, FILE *o);

void deconectare (char *numar, TLC lista, char s[1000]);

void deblocare_card (char *numar, TLC lista);

int pin_corect (char *sir);

void schimba_pin (char *numar, char *pin, TLC lista, FILE *o, char s[1000]);

void sold_curent (char *numar, TLC lista, FILE *o, char s[1000]);

void reincarcare (char *numar, int suma, TLC lista, FILE *o, char s[1000]);

void retragere (char *numar, int suma, TLC lista, FILE *o, char s[1000]);

void transfera (char *src, char *dest, int suma, TLC lista, FILE *o, char s[1000]);

void anuleaza(char *src, char *dest, int suma, FILE *o, TLC lista, char s[1000]);

void EliminareIstoric (char *src, char *dest, int suma, THistory **h);

void executare_comenzi (char fisier[1000][1000], int linii,
						int *nr_max, ALC lista, FILE *o);
#endif