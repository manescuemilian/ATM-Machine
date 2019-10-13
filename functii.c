#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functii.h"

/* MANESCU Emilian-Claudiu - 313 CB */

void eliberare_istoric (THistory **h) {
	THistory *aux;
	while (*h) {
		aux = *h;
		*h = (*h) -> urm;
		if (aux -> operatie != NULL) {
			free (aux -> operatie);
			free (aux -> rezultat);
		}
		free (aux);
	}
}

void eliberareLSC(aLSC card) {
	LSC aux;
	while (*card) {
		aux = *card;
		*card = (*card) -> urm;
		eliberare_istoric (&aux-> istoric);
		free (aux -> istoric);
		free (aux);
	}
}

void eliberareLC(ALC aL) {
	TLC aux;
	while (*aL) {
		aux = *aL;
		*aL = (*aL) -> urm;
		eliberareLSC (&aux -> info);
		free (aux -> info);
		free (aux);
	}
}

LSC CardAloca() {
	TCard *celula = (TCard *) malloc(sizeof(TCard));
	if (!celula) {
		return NULL;
	}
	celula -> balance = 0;
	celula -> numar_incercari = 0;
	celula -> este_copie = 0;
	celula -> istoric = (THistory *) malloc (sizeof (THistory));
	if (!celula -> istoric) {
		free (celula);
		return NULL;
	}
	celula -> istoric -> rezultat = NULL;
	celula -> istoric -> operatie = NULL;
	celula -> istoric -> urm = NULL;
	celula -> urm = NULL;
	return celula;
}

//aloca o celula reprezentand o sublista
LC *AlocaLC () {
	LC *lista = (LC *) malloc (sizeof (LC));
	if (!lista) {
		return NULL;
	}
	lista -> info = NULL;
	lista -> urm = NULL;
	lista -> nr_carduri = 0;
	return lista;
}

THistory *AlocaIstoric (char *succes, char *op) {
	int len1 = strlen (succes), len2 = strlen (op);
	THistory *his = (THistory *) malloc (sizeof (THistory));
	if (!his) {
		return NULL;
	}
	his -> rezultat = (char *) malloc (len1 + 1);
	if (!his -> rezultat) {
		free (his);
		return NULL;
	}

	his -> operatie = (char *) malloc (len2 + 1);
	if (!his -> operatie) {
		free (his);
		free (his -> rezultat);
		return NULL;
	}
	strcpy (his -> rezultat, succes);
	strcpy (his -> operatie, op);
	his -> urm = NULL;
	return his;
}

int suma_elemente (char *sir) {
	int i, suma = 0;
	for (i = 0; sir[i] != '\0'; i++) {
		suma += (sir[i] - '0');
	}
	return suma;
}

int InsIncLSC(TLC *L, TCard *info) {
	info -> urm = (*L) -> info;
	(*L) -> info = info;
	return 1;
}

int exista_card (char *sir, TLC lista) {
	TLC p;
	for (p = lista; p != NULL; p = p -> urm) {
		TCard *c = p -> info;
		for (; c != NULL; c = c -> urm) {
			if (strcmp (sir, c -> numar) == 0) {
				return 1;
			}
		}
	}
	return 0;
}

TLC pozitionare_carduri (int nr_max, TCard *card, ALC lista) {
	int pozitie, i;
	TLC aux, ant = NULL;
	pozitie = suma_elemente (card -> numar) % nr_max;
	i = 0, aux = *lista;
	while (i <= pozitie) {
		//daca lista e vida
		if (aux == NULL) {
			aux = AlocaLC ();
			if (!aux) return NULL;
			if (i == 0) {
				*lista = aux;
			} else {
				ant -> urm = aux;
			}
		}

		//daca am ajuns la celula corecta, nu mai actualizam
		if (i == pozitie) break;
		ant = aux;
		aux = aux -> urm;
		i++;
	}
	return aux;
}

void eliminare_duplicate (ALC lista, LSC card) {
	LSC p = NULL, ant;
	aLSC c;
	TLC a;
	int gasit = 0;
	for (a = *lista; a != NULL && gasit != 1; a = a -> urm) {
		c = &(a -> info);
		for (p = *c, ant = NULL; p != NULL; ant = p, p = p -> urm) {
			if (strcmp (card -> numar, p -> numar) == 0 && card != p) {
				gasit = 1;
				break;
			}
		}
	}
	if (p == NULL) return;
	//refacerea legaturilor
	if (ant == NULL) {
		*c = p -> urm;
	} else {
		ant -> urm = p -> urm;
	}

	//dezalocarea celulelor vechi
	free (p -> istoric);
	free (p);
}

void repozitionare (ALC lista, int nr_max) {
	TLC aux, p;
	aLSC card;
	LSC c;
	//inserarea la inceput in noua celula corespunzatoare
	for (p = *lista; p != NULL; p = p -> urm) {
		card = &(p -> info);
		if (*card != NULL) {
			for (c = *card; c != NULL; c = c -> urm) {
				aux = pozitionare_carduri (nr_max, c, lista);
				//noul card + copierea informatiei
				TCard *c1 = CardAloca();
				THistory h;
				memcpy (&h, c -> istoric, sizeof (THistory));
				strcpy (c1 -> numar, c -> numar);
				strcpy (c1 -> pin, c -> pin);
				strcpy (c1 -> expirare, c -> expirare);
				strcpy (c1 -> CVV, c -> CVV);
				strcpy (c1 -> status, c -> status);
				c1 -> balance = c -> balance;
				c1 -> numar_incercari = c -> numar_incercari;
				c1 -> este_copie = 1;
				memcpy (c1 -> istoric, &h, sizeof (THistory));
				c1 -> urm = NULL;
				InsIncLSC (&aux, c1);
			}
		}
	}

	//eliminarea cardurilor originale
	for (p = *lista; p != NULL; p = p -> urm) {
		card = &(p -> info);
		if (*card != NULL) {
			for (c = *card; c != NULL; c = c -> urm) {
				if (c -> este_copie == 1) {
					eliminare_duplicate (lista, c);
				}
			}
		}
	}
}

int adauga_card (char *sir, int *nr_max, ALC lista, FILE *o) {
	TCard *card = CardAloca ();
	if (!card) {
		return 0;
	}

	TLC aux;
	int element = 0;

	//copierea informatiilor in card (daca exista)
	while (sir != NULL) {
		if (element == 1) {
			strncpy (card -> numar, sir, 17);
			if (exista_card (card -> numar, *lista)) {
				eliberareLSC (&card);
				fprintf(o, "The card already exists\n");
				return 0;
			}
		} else if (element == 2) {
			strncpy (card -> pin, sir, 5);
		} else if (element == 3) {
			strncpy (card -> expirare, sir, 6);
		} else if (element == 4) {
			strncpy (card -> CVV, sir, 4);
		}
		sir = strtok (NULL, " \n");
		element++;
	}
	strncpy (card -> status, "NEW", 4);
	card -> CVV [3] = '\0';
	aux = pozitionare_carduri (*nr_max, card, lista);
	(*lista) -> nr_carduri++; //cresterea numarului de carduri din lista
	//cresterea numarului maxim in cazul depasirii
	if ((*lista) -> nr_carduri > *nr_max) {
		*nr_max = *nr_max * 2;
		//pozitia pe care vine elementul curent
		aux = pozitionare_carduri (*nr_max, card, lista);
		//repozitionarea tuturor celorlalte carduri
		repozitionare (lista, *nr_max);
	}
	//inserare la inceputul sublistei aux
	InsIncLSC (&aux, card);
	return 1;
}

void afisare_istoric (THistory *h, FILE *o) {
	fprintf(o, "history: [");
	for (; h -> urm != NULL; h = h -> urm) {
		if (h -> urm -> urm != NULL) {
			fprintf(o, "(%s, %s), ", h -> rezultat, h -> operatie);
		} else {
			fprintf(o, "(%s, %s)", h -> rezultat, h -> operatie);
		}
	}
	fprintf(o, "])\n");
}

void arata_carduri (TLC lista, FILE *f, char numar[17]) {
	if (numar != NULL) numar[16] = '\0';
	int i = 0, lista_nevida;
	for (; lista != NULL; lista = lista -> urm) {
		lista_nevida = 0;
		if (lista -> info == NULL && numar == NULL) {
			fprintf(f, "pos%d: []\n", i);
		} else {
			lista_nevida = 1;
			TCard *c = lista -> info;

			if (numar == NULL) {
				fprintf (f, "pos%d: [\n", i);
				for (; c != NULL; c = c -> urm) {
					fprintf (f, "(card number: %s, PIN: %s, expiry date: %s, ",
						c -> numar, c -> pin, c -> expirare);
					fprintf (f, "CVV: %s, balance: %d, status: %s, ",
						c -> CVV, c-> balance, c -> status);
					afisare_istoric (c -> istoric, f);
				}
			} else {
				lista_nevida = 0;
				for (; c != NULL; c = c -> urm) {
					if (strcmp (c -> numar, numar) == 0) {
						fprintf (f, "(card number: %s, PIN: %s,",
							c -> numar, c -> pin);
						fprintf(f, " expiry date: %s, ", c -> expirare);
						fprintf (f, "CVV: %s, balance: %d, status: %s, ",
							c -> CVV, c-> balance, c -> status);
						afisare_istoric (c -> istoric, f);							
					}
				}
			}			
		}
		if (lista_nevida == 1) {
			fprintf(f, "]\n");
		}
		i++;
	}
}

int stergere_card (ALC lista, char *numar, int nr_max) {
	int pozitie = suma_elemente (numar) % nr_max;
	int i = 0;
	TLC p = *lista;
	LSC ant, aux;
	aLSC card_lista;
	for (; p != NULL; p = p -> urm) {
		if (i == pozitie) break;
		i++;
	}
	// p este acum un pointer spre celula de inceput a listei
	// in care se afla elementul
	card_lista = &(p -> info);
	aux = *card_lista;

	for (ant = NULL; aux != NULL; ant = aux, aux = aux -> urm) {
		if (strcmp (numar, aux -> numar) == 0) {
			break;
		}
	}
	if (aux == NULL) return 0;
	if (ant == NULL) {
		*card_lista = aux -> urm;
	} else {
		ant -> urm = aux -> urm;
	}
	(*lista) -> nr_carduri--;

	//eliberarea cardului sters
	eliberare_istoric (&aux -> istoric);
	free (aux);
	aux = NULL;
	return 1;
}

int InsIncIstoric(aLSC L, char *succes, char *comanda) {
	THistory *aux = AlocaIstoric (succes, comanda);
	if (!aux) {
		return 0;
	}
	aux -> urm = (*L) -> istoric;
	(*L) -> istoric = aux;
	return 1;
}

int inserare_card(char *numar, char *pin, char s[1000], TLC lista, FILE *o) {
	LSC c;
	int val_return = 1;
	int gasit = 0;
	for (; lista != NULL && gasit == 0; lista = lista -> urm) {
		c = lista -> info;
		for (; c != NULL; c = c -> urm) {
			if (strcmp (c -> numar, numar) == 0) {
				gasit = 1;
				break;
			}
		}
	}

	if (strcmp (c -> pin, pin) != 0 && c -> numar_incercari < 3) {
		fprintf(o, "Invalid PIN\n");
		c -> numar_incercari++;
		val_return = 0;
	} else if (c -> numar_incercari < 3) {
		c -> numar_incercari = 0;
	}

	if (c -> numar_incercari >= 3) {
		fprintf(o, "The card is blocked. Please contact the administrator.\n");
		strcpy (c -> status, "LOCKED");
		val_return = 0;
	}
	if (strcmp (c -> status, "NEW") == 0 && val_return == 1) {
		fprintf(o, "You must change your PIN.\n");
	}

	//adaugare in istoric
	if (val_return == 0) {
		InsIncIstoric (&c, "FAIL", s);
	} else {
		InsIncIstoric (&c, "SUCCESS", s);
	}
	return val_return;
}

void deconectare (char *numar, TLC lista, char s[1000]) {
	TCard *c;
	int gasit = 0;
	for (; lista != NULL && gasit == 0; lista = lista -> urm) {
		c = lista -> info;
		for (; c != NULL; c = c -> urm) {
			if (strcmp (c -> numar, numar) == 0) {
				gasit = 1;
				break;
			}
		}
	}

	if (s != NULL) {
		InsIncIstoric (&c,"SUCCESS", s);
	}
}

void deblocare_card (char *numar, TLC lista) {
	LSC c;
	int gasit = 0;
	//cautarea cardului
	for (; lista != NULL && gasit != 1; lista = lista -> urm) {
		c = lista -> info;
		for (; c != NULL; c = c -> urm) {
			if (strcmp (c -> numar, numar) == 0) {
				gasit = 1;
				break;
			}
		}
	}
	c -> numar_incercari = 0;
	strcpy (c -> status, "ACTIVE");
}

int pin_corect (char *sir) {
	int len = strlen (sir), i;
	if (len != 4) {
		return 0;
	}
	for (i = 0; sir[i] != '\0'; i++) {
		if (sir[i] < '0' || sir[i] > '9') {
			return 0;
		}
	}
	return 1;
}

void schimba_pin (char *numar, char *pin, TLC lista, FILE *o, char s[1000]) {
	LSC c;
	int gasit = 0;
	for (; lista != NULL && gasit != 1; lista = lista -> urm) {
		c = lista -> info;
		for (; c != NULL; c = c -> urm) {
			if (strcmp (c -> numar, numar) == 0) {
				gasit = 1;
				break;
			}
		}
	}
	if (pin_corect (pin) == 0) {
		fprintf(o, "Invalid PIN\n");
		InsIncIstoric (&c,"FAIL", s);
	} else {
		strcpy (c -> pin, pin);
		InsIncIstoric (&c,"SUCCESS", s);
		strcpy (c -> status, "ACTIVE");
	}
}

void sold_curent (char *numar, TLC lista, FILE *o, char s[1000]) {
	LSC c;
	int gasit = 0;
	for (; lista != NULL && gasit != 1; lista = lista -> urm) {
		c = lista -> info;
		for (; c != NULL; c = c -> urm) {
			if (strcmp (c -> numar, numar) == 0) {
				gasit = 1;
				break;
			}
		}
	}
	fprintf(o, "%d\n", c -> balance);
	InsIncIstoric (&c,"SUCCESS", s);
}

void reincarcare (char *numar, int suma, TLC lista, FILE *o, char s[1000]) {
	LSC c;
	int gasit = 0;
	for (; lista != NULL && gasit == 0; lista = lista -> urm) {
			c = lista -> info;
			for (; c != NULL; c = c -> urm) {
				if (strcmp (c -> numar, numar) == 0) {
					gasit = 1;
					break;
				}
			}
		}

	if (suma % 10 != 0) {
		fprintf (o, "The added amount must be multiple of 10\n");
		InsIncIstoric (&c,"FAIL", s);
		return;
	}

	c -> balance += suma;
	InsIncIstoric (&c,"SUCCESS", s);
	fprintf (o, "%d\n", c -> balance);
}

void retragere (char *numar, int suma, TLC lista, FILE *o, char s[1000]) {
	LSC c;
	int gasit = 0;
	for (; lista != NULL && gasit != 1; lista = lista -> urm) {
			c = lista -> info;
			for (; c != NULL; c = c -> urm) {
				if (strcmp (c -> numar, numar) == 0) {
					gasit = 1;
					break;
				}
			}
		}

	if (suma % 10 != 0) {
		fprintf (o, "The requested amount must be multiple of 10\n");
		InsIncIstoric (&c,"FAIL", s);
		return;
	}

	if (suma > c -> balance) {
		fprintf (o, "Insufficient funds\n");
		InsIncIstoric (&c,"FAIL", s);
		return;
	}

	c -> balance -= suma;
	InsIncIstoric (&c,"SUCCESS", s);
	fprintf (o, "%d\n", c -> balance);
}

void transfera (char *src, char *dest, int suma, 
				TLC lista, FILE *o, char s[1000]) {
	LSC c1, c2, c;
	int gasit1 = 0, gasit2 = 0;
	TLC p;

	//cautarea cardului sursa
	for (p = lista; p != NULL && gasit1 == 0; p = p -> urm) {
		c = p -> info;
		for (; c != NULL; c = c -> urm) {
			if (strcmp (c -> numar, src) == 0) {
				gasit1 = 1;
				c1 = c;
				break;
			}
		}
	}

	//cautarea cardului destinatie
	for (p = lista; p != NULL && gasit2 == 0; p = p -> urm) {
		c = p -> info;
		for (; c != NULL; c = c -> urm) {
			if (strcmp (c -> numar, dest) == 0) {
				gasit2 = 1;
				c2 = c;
				break;
			}
		}
	}

	if (suma % 10 != 0) {
		fprintf(o, "The transferred amount must be multiple of 10\n");
		InsIncIstoric (&c1,"FAIL", s);
		deconectare (src, lista, NULL);
		return;
	} else if (c1 -> balance < suma) {
		fprintf(o, "Insufficient funds\n");
		InsIncIstoric (&c1,"FAIL", s);
		deconectare (src, lista, NULL);
		return;
	}
	c1 -> balance -= suma;
	c2 -> balance += suma;
	fprintf(o, "%d\n", c1 -> balance);
	//inserarea in istoricul celor doua carduri a operatiei cu succes
	InsIncIstoric (&c1,"SUCCESS", s); InsIncIstoric (&c2,"SUCCESS", s);
}

void EliminareIstoric (char *src, char *dest, int suma, THistory **h) {
	THistory  *p, *ant;
	char *sir;
	char sum[10];
	int len1, len2, len3, len4;
	sprintf (sum, "%d", suma);
	len1 = strlen(src), len2 = strlen(src), len3 = strlen(sum);
	len4 = strlen ("transfer_funds");
	sir = (char *) malloc (len1 + len2 + len3 + len4 + 4);
	sprintf (sir, "transfer_funds %s %s %s", src, dest, sum);
	for (p = *h, ant = NULL; p != NULL; ant = p, p = p -> urm) {
		if (strcmp (sir, p -> operatie) == 0) {
			break;
		}
	}
	if (ant == NULL) {
		*h = p -> urm;
	} else {
		ant -> urm = p -> urm;
	}
	free (p -> operatie); free (p -> rezultat);
	free (p);
	free (sir);
}

void anuleaza(char *src, char *dest, int suma, FILE *o, 
			TLC lista, char s[1000]) {
	LSC c1, c2, c; TLC p;
	int gasit1 = 0, gasit2 = 0;
	for (p = lista; p != NULL && gasit1 == 0; p = p -> urm) {
		c = p -> info;
		for (; c != NULL; c = c -> urm) {
			if (strcmp (c -> numar, src) == 0) {
				gasit1 = 1;
				c1 = c;
				break;
			}
		}
	}

	for (p = lista; p != NULL && gasit2 == 0; p = p -> urm) {
		c = p -> info;
		for (; c != NULL; c = c -> urm) {
			if (strcmp (c -> numar, dest) == 0) {
				gasit2 = 1;
				c2 = c;
				break;
			}
		}
	}
	if (c2 -> balance < suma) {
		fprintf(o, "The transaction cannot be reversed\n");
		return;
	}

	InsIncIstoric (&c1,"SUCCESS", s);
	c2 -> balance -= suma;
	c1 -> balance += suma;
	EliminareIstoric (src, dest, suma, &c2 -> istoric);
}

void executare_comenzi (char fisier[1000][1000], int linii, 
						int *nr_max, ALC lista, FILE *o) {
	int i, suma;
	char *sir, *numar, *pin, *dest;
	char copie[1000];
	for (i = 0; i < linii; i++) {
		strcpy (copie, fisier[i]);
		copie[strlen (copie) - 1] = '\0';
		sir = strtok (fisier[i], " ");
		if(strcmp (sir, "add_card") == 0) {
			adauga_card (sir, nr_max, lista, o);
		} else if (strcmp (sir,  "show") == 0
			|| strcmp (sir, "show\n") == 0) {
			sir = strtok (NULL, " ");
			arata_carduri(*lista, o, sir);
		} else if (strcmp (sir, "delete_card") == 0) {
			sir = strtok (NULL, " ");
			sir [strlen (sir) - 1] = '\0'; //pentru a sterge newline-ul
			stergere_card(lista, sir, *nr_max);
		} else if (strcmp (sir, "reverse_transaction") == 0) {
			numar = strtok (NULL, " \n");
			dest = strtok (NULL, " \n");
			suma = atoi(strtok (NULL, " \n"));
			anuleaza (numar, dest, suma, o, *lista, copie);
		} else if (strcmp (sir, "unblock_card") == 0) {
			numar = strtok (NULL, "\n");
			deblocare_card (numar, *lista);
		} else if (strcmp (sir, "insert_card") == 0) {
			numar = strtok (NULL, " \n");
			pin = strtok (NULL, " \n");
			inserare_card(numar, pin, copie, *lista, o);
		} else if (strcmp (sir, "cancel") == 0) {
			numar = strtok (NULL, " \n");
			deconectare (numar, *lista, copie);
		} else if (strcmp (sir, "pin_change") == 0) {
			numar = strtok (NULL, " \n");
			pin = strtok (NULL, " \n");
			schimba_pin (numar, pin, *lista, o, copie);
			deconectare (numar, *lista, NULL);
		} else if (strcmp (sir, "balance_inquiry") == 0) {
			numar = strtok (NULL, " \n");
			sold_curent (numar, *lista, o, copie);
			deconectare (numar, *lista, NULL);
		} else if (strcmp (sir, "recharge") == 0) {
			numar = strtok (NULL, " \n");
			suma = atoi(strtok (NULL, " \n"));
			reincarcare (numar, suma, *lista, o, copie);
			deconectare (numar, *lista, NULL);
		} else if (strcmp (sir, "cash_withdrawal") == 0) {
			numar = strtok (NULL, " \n");
			suma = atoi(strtok (NULL, " \n"));
			retragere (numar, suma, *lista, o, copie);
			deconectare (numar, *lista, NULL);
		} else if (strcmp (sir, "transfer_funds") == 0) {
			numar = strtok (NULL, " \n");
			dest = strtok (NULL, " \n");
			suma = atoi(strtok (NULL, " \n"));
			transfera (numar, dest, suma, *lista, o, copie);
		}
	}
}