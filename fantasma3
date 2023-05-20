#include "winsuport2.h"
#include "memoria.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



#define MIN_FIL 7		/* definir limits de variables globals */
#define MAX_FIL 25
#define MIN_COL 10
#define MAX_COL 80
#define MAX_FANTASMA 9   /* nombre màxim de fantasmes */


				/* definir estructures d'informacio */
typedef struct {		/* per un objecte (menjacocos o fantasma) */
	int f;				/* posicio actual: fila */
	int c;				/* posicio actual: columna */
	int d;				/* direccio actual: [0..3] */
  float r;            /* per indicar un retard relati */
	char a;				/* caracter anterior en pos. actual */
} objecte;


/* variables globals */
int n_fil1, n_col;		/* dimensions del camp de joc */
char tauler[70];		/* nom del fitxer amb el laberint de joc */
char c_req;			    /* caracter de pared del laberint */

objecte mc;      		/* informacio del menjacocos */
objecte f1;			    /* informacio del fantasma 1 */

int df[] = {-1, 0, 1, 0};	/* moviments de les 4 direccions possibles */
int dc[] = {0, -1, 0, 1};	/* dalt, esquerra, baix, dreta */

int cocos;			/* numero restant de cocos per menjar */
int retard;		    /* valor del retard de moviment, en mil.lisegons */

int fi1, fi2, rc, p;		/* variableees globalls*/


objecte fantasmes[MAX_FANTASMA]; /* posicions dels fantasmes */

int indexFantasmes;
int temps=0;

/* Variables Globals */
int *p_mem, *p_fi1, *p_fi2;
int numBytes;

int main(int n_args, char *ll_args[])
{ 
    //ficar set
    //memoria compartida els fi

    objecte fantasmes;
    objecte seg;
    int ret;
    int k, vk, nd, vd[3];
    
    int indexFantasmes;


   

    int *p_mem, id_mem;
    if (n_args != 6)
    { 
        fprintf(stderr,"proces: mp_car n_car n_vegades id_lletres\n");
        exit(0);
    }
    id_mem = atoi(ll_args[1]); //referencia tauler
    fantasmes.f = atoi(ll_args[2]); 
    fantasmes.c = atoi(ll_args[3]);
    fantasmes.d = atoi(ll_args[4]);
    fantasmes.r = atoi(ll_args[5]);
    fantasmes.a = atoi(ll_args[6]);
    indexFantasmes = atoi(ll_args[7]);
    //fi 1 i 2
    //nfil ncol

    
    
    p_mem = map_mem(id_mem); /*obtenir adres. de mem. compartida */
    if (p_mem == (int*) -1)
    { 
        fprintf(stderr,"proces (%d): error en identificador de memoria\n",
        getpid());
        exit(0);
    }


    win_set(p_mem, fantasmes.f , fantasmes.c ); //inicialitza el contingut de la finestra de dibuix i permet l'accés al procés pare

    setbuf(stdout,NULL); /* anular buffer de sortida estandard */
    srand(getpid());

        printf("Entraa xd");



  do			
  {

  int  numFantasma ;
  numFantasma = indexFantasmes;
 
  ret = 0; nd = 0;
 	 
    for (k=-1; k<=1; k++)		/* provar direccio actual i dir. veines */
    {
      vk = (fantasmes.d + k) % 4;		/* direccio veina */
      if (vk < 0) vk += 4;		/* corregeix negatius */
      seg.f = fantasmes.f + df[vk]; /* calcular posicio en la nova dir.*/
      seg.c = fantasmes.c + dc[vk];
      seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
      if ((seg.a==' ') || (seg.a=='.') || (seg.a=='0'))
      { vd[nd] = vk;			/* memoritza com a direccio possible */
        nd++;
      }
    }
  
  if (nd == 0)				/* si no pot continuar, */
  	fantasmes.d = (fantasmes.d + 2) % 4;		/* canvia totalment de sentit */
  else
  { 
    if (nd == 1)			/* si nomes pot en una direccio */
  	  fantasmes.d = vd[0];			/* li assigna aquesta */
    else				/* altrament */
    	fantasmes.d = vd[rand() % nd];		/* segueix una dir. aleatoria */

    seg.f = fantasmes.f + df[fantasmes.d];  /* calcular seguent posicio final */
    seg.c = fantasmes.c + dc[fantasmes.d];
    seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
    
    win_escricar(fantasmes.f,fantasmes.c,fantasmes.a,NO_INV);	/* esborra posicio anterior */
    
    fantasmes.f = seg.f; fantasmes.c = seg.c; fantasmes.a = seg.a;	/* actualitza posicio */
    
    win_escricar(fantasmes.f,fantasmes.c,'1'+numFantasma,NO_INV);		/* redibuixa fantasma */
    
    if (fantasmes.a == '0') ret = 1;		/* ha capturat menjacocos */
  }
	/**p++;  if ((p%2)==0)		 ralentitza fantasma a 2*retard */
    fi2 = ret;
    win_retard(retard);
  } while (!(*p_fi1)) && (!(*p_fi2));

}
