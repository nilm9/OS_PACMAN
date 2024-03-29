/*****************************************************************************/
/*									                                         */
/*				     cocos1.c				                                 */
/*									                                         */
/*     Programa inicial d'exemple per a les practiques 2.1 i 2.2 de FSO.     */
/*     Es tracta del joc del menjacocos: es dibuixa un laberint amb una      */
/*     serie de punts (cocos), els quals han de ser "menjats" pel menja-     */
/*     cocos. Aquest menjacocos es representara amb el caracter '0', i el    */
/*     moura l'usuari amb les tecles 'w' (adalt), 's' (abaix), 'd' (dreta)   */
/*     i 'a' (esquerra). Simultaniament hi haura un conjunt de fantasmes,    */
/*     representats per numeros de l'1 al 9, que intentaran capturar al      */
/*     menjacocos. En la primera versio del programa, nomes hi ha un fan-    */
/*     tasma.								                                 */
/*     Evidentment, es tracta de menjar tots els punts abans que algun fan-  */
/*     tasma atrapi al menjacocos.					                         */
/*									                                         */
/*  Arguments del programa:						                             */
/*     per controlar la posicio de tots els elements del joc, cal indicar    */
/*     el nom d'un fitxer de text que contindra la seguent informacio:	     */
/*		n_fil1 n_col fit_tauler creq				                         */
/*		mc_f mc_c mc_d mc_r						                             */
/*		f1_f f1_c f1_d f1_r						                             */
/*									                                         */
/*     on 'n_fil1', 'n_col' son les dimensions del taulell de joc, mes una   */
/*     fila pels missatges de text a l'ultima linia. "fit_tauler" es el nom  */
/*     d'un fitxer de text que contindra el dibuix del laberint, amb num. de */
/*     files igual a 'n_fil1'-1 i num. de columnes igual a 'n_col'. Dins     */
/*     d'aquest fitxer, hi hauran caracter ASCCII que es representaran en    */
/*     pantalla tal qual, excepte el caracters iguals a 'creq', que es visua-*/
/*     litzaran invertits per representar la paret.			                 */
/*     Els parametres 'mc_f', 'mc_c' indiquen la posicio inicial de fila i   */
/*     columna del menjacocos, aixi com la direccio inicial de moviment      */
/*     (0 -> amunt, 1-> esquerra, 2-> avall, 3-> dreta). Els parametres	     */
/*     'f1_f', 'f1_c' i 'f1_d' corresponen a la mateixa informacio per al    */
/*     fantasma 1. El programa verifica que la primera posicio del menja-    */
/*     cocos o del fantasma no coincideixi amb un bloc de paret del laberint.*/
/*	   'mc_r' 'f1_r' son dos reals que multipliquen el retard del moviment.  */
/*     A mes, es podra afegir un segon argument opcional per indicar el      */
/*     retard de moviment del menjacocos i dels fantasmes (en ms);           */
/*     el valor per defecte d'aquest parametre es 100 (1 decima de segon).   */
/*									                                         */
/*  Compilar i executar:					  	                             */
/*     El programa invoca les funcions definides a 'winsuport.h', les        */
/*     quals proporcionen una interficie senzilla per crear una finestra     */
/*     de text on es poden escriure caracters en posicions especifiques de   */
/*     la pantalla (basada en CURSES); per tant, el programa necessita ser   */
/*     compilat amb la llibreria 'curses':				                     */
/*									                                         */
/*	   $ gcc -Wall cocos0.c winsuport.o -o cocos0 -lcurses		             */
/*	   $ ./cocos0 fit_param [retard]				                         */
/*									                                         */
/*  Codis de retorn:						  	                             */
/*     El programa retorna algun dels seguents codis al SO:		             */
/*	0  ==>  funcionament normal					                             */
/*	1  ==>  numero d'arguments incorrecte 				                     */
/*	2  ==>  fitxer de configuracio no accessible			                 */
/*	3  ==>  dimensions del taulell incorrectes			                     */
/*	4  ==>  parametres del menjacocos incorrectes			                 */
/*	5  ==>  parametres d'algun fantasma incorrectes			                 */
/*	6  ==>  no s'ha pogut crear el camp de joc			                     */
/*	7  ==>  no s'ha pogut inicialitzar el joc			                     */
/*****************************************************************************/

#include <stdio.h>     /* incloure definicions de funcions estandard */
#include <stdlib.h>    /* per exit() */
#include <unistd.h>    /* per getpid() */
#include <pthread.h>   /* per getpid() */
#include "winsuport.h" /* incloure definicions de funcions propies */

#define MIN_FIL 7 /* definir limits de variables globals */
#define MAX_FIL 25
#define MIN_COL 10
#define MAX_COL 80
#define MAX_FANTASMA 9 /* nombre màxim de fantasmes */

/* definir estructures d'informacio */
typedef struct
{            /* per un objecte (menjacocos o fantasma) */
    int f;   /* posicio actual: fila */
    int c;   /* posicio actual: columna */
    int d;   /* direccio actual: [0..3] */
    float r; /* per indicar un retard relati */
    char a;  /* caracter anterior en pos. actual */
} objecte;

/* variables globals */
int n_fil1, n_col; /* dimensions del camp de joc */
char tauler[70];   /* nom del fitxer amb el laberint de joc */
char c_req;        /* caracter de pared del laberint */

objecte mc; /* informacio del menjacocos */
objecte f1; /* informacio del fantasma 1 */

int df[] = {-1, 0, 1, 0}; /* moviments de les 4 direccions possibles */
int dc[] = {0, -1, 0, 1}; /* dalt, esquerra, baix, dreta */

int cocos;  /* numero restant de cocos per menjar */
int retard; /* valor del retard de moviment, en mil.lisegons */

int fi1, fi2, rc, p; /* variableees globalls*/

objecte fantasmes[MAX_FANTASMA]; /* posicions dels fantasmes */

int index = 0;

/* funcio per realitzar la carrega dels parametres de joc emmagatzemats */
/* dins d'un fitxer de text, el nom del qual es passa per referencia a  */
/* 'nom_fit'; si es detecta algun problema, la funcio avorta l'execucio */
/* enviant un missatge per la sortida d'error i retornant el codi per-	*/
/* tinent al SO (segons comentaris al principi del programa).		    */
void carrega_parametres(const char *nom_fit)
{

    FILE *fit;

    fit = fopen(nom_fit, "rt"); /* intenta obrir fitxer */
    if (fit == NULL)
    {
        fprintf(stderr, "No s'ha pogut obrir el fitxer \'%s\'\n", nom_fit);
        exit(2);
    }

    if (!feof(fit))
        fscanf(fit, "%d %d %s %c\n", &n_fil1, &n_col, tauler, &c_req);
    else
    {
        fprintf(stderr, "Falten parametres al fitxer \'%s\'\n", nom_fit);
        fclose(fit);
        exit(2);
    }
    if ((n_fil1 < MIN_FIL) || (n_fil1 > MAX_FIL) ||
        (n_col < MIN_COL) || (n_col > MAX_COL))
    {
        fprintf(stderr, "Error: dimensions del camp de joc incorrectes:\n");
        fprintf(stderr, "\t%d =< n_fil1 (%d) =< %d\n", MIN_FIL, n_fil1, MAX_FIL);
        fprintf(stderr, "\t%d =< n_col (%d) =< %d\n", MIN_COL, n_col, MAX_COL);
        fclose(fit);
        exit(3);
    }

    if (!feof(fit))
        fscanf(fit, "%d %d %d %f\n", &mc.f, &mc.c, &mc.d, &mc.r);
    else
    {
        fprintf(stderr, "Falten parametres al fitxer \'%s\'\n", nom_fit);
        fclose(fit);
        exit(2);
    }
    if ((mc.f < 1) || (mc.f > n_fil1 - 3) ||
        (mc.c < 1) || (mc.c > n_col - 2) ||
        (mc.d < 0) || (mc.d > 3))
    {
        fprintf(stderr, "Error: parametres menjacocos incorrectes:\n");
        fprintf(stderr, "\t1 =< mc.f (%d) =< n_fil1-3 (%d)\n", mc.f, (n_fil1 - 3));
        fprintf(stderr, "\t1 =< mc.c (%d) =< n_col-2 (%d)\n", mc.c, (n_col - 2));
        fprintf(stderr, "\t0 =< mc.d (%d) =< 3\n", mc.d);
        fclose(fit);
        exit(4);
    }

    if (feof(fit))
    {
        fprintf(stderr, "Falten parametres al fitxer \'%s\'\n", nom_fit);
        fclose(fit);
        exit(2);
    }

    int i = 0;
    while (!feof(fit))
    {

        fscanf(fit, "%d %d %d %f\n", &fantasmes[i].f, &fantasmes[i].c, &fantasmes[i].d, &fantasmes[i].r);

        if ((fantasmes[i].f < 1) || (fantasmes[i].f > n_fil1 - 3) ||
            (fantasmes[i].c < 1) || (fantasmes[i].c > n_col - 2) ||
            (fantasmes[i].d < 0) || (fantasmes[i].d > 3))
        {
            fprintf(stderr, "Error: parametres fantasma i incorrectes:\n");
            fprintf(stderr, "\t1 =< fantasma.f (%d) =< n_fil1-3 (%d)\n", fantasmes[i].f, (n_fil1 - 3));
            fprintf(stderr, "\t1 =< fantasma.c (%d) =< n_col-2 (%d)\n", fantasmes[i].c, (n_col - 2));
            fprintf(stderr, "\t0 =< fantasma.d (%d) =< 3\n", fantasmes[i].d);
            fclose(fit);
            exit(5);
        }
        i++;
    }

    fprintf(stderr, "%d %d %d %f\n", fantasmes[0].f, fantasmes[0].c, fantasmes[0].d, fantasmes[0].r);
    fprintf(stderr, "%d %d %d %f\n", fantasmes[1].f, fantasmes[1].c, fantasmes[1].d, fantasmes[1].r);
    fprintf(stderr, "%d %d %d %f\n", fantasmes[2].f, fantasmes[2].c, fantasmes[2].d, fantasmes[2].r);
    fprintf(stderr, "%d %d %d %f\n", fantasmes[3].f, fantasmes[3].c, fantasmes[3].d, fantasmes[3].r);

    fclose(fit); /* fitxer carregat: tot OK! */
    printf("Joc del MenjaCocos\n\tTecles: \'%c\', \'%c\', \'%c\', \'%c\', RETURN-> sortir\n",
           TEC_AMUNT, TEC_AVALL, TEC_DRETA, TEC_ESQUER);
    printf("prem una tecla per continuar:\n");
    getchar();
    index = i;
}

// setejar contador de fantasmes

/* funcio per inicialitar les variables i visualitzar l'estat inicial del joc */
void inicialitza_joc(void)
{
    int r, i, j;
    char strin[12];

    r = win_carregatauler(tauler, n_fil1 - 1, n_col, c_req);
    if (r == 0)
    {
        mc.a = win_quincar(mc.f, mc.c);
        if (mc.a == c_req)
            r = -6; /* error: menjacocos sobre pared */
        else
        {
            // CANVI
            for (int j = 0; j < index; j++)
            {
                fantasmes[j].a = win_quincar(fantasmes[j].f, fantasmes[j].c);
                if (fantasmes[j].a == c_req)
                {
                    r = -7;
                    /* error: fantasma sobre pared */
                }
            }

            cocos = 0; /* compta el numero total de cocos */
            for (i = 0; i < n_fil1 - 1; i++)
                for (j = 0; j < n_col; j++)
                    if (win_quincar(i, j) == '.')
                        cocos++;

            win_escricar(mc.f, mc.c, '0', NO_INV);
            // CANVI
            for (int j = 0; j < index; j++)
            {
                win_escricar(fantasmes[j].f, fantasmes[j].c, '1', NO_INV);
            }

            if (mc.a == '.')
                cocos--; /* menja primer coco */

            sprintf(strin, "Cocos: %d", cocos);
            win_escristr(strin);
        }
    }
    if (r != 0)
    {
        win_fi();
        fprintf(stderr, "Error: no s'ha pogut inicialitzar el joc:\n");
        switch (r)
        {
        case -1:
            fprintf(stderr, "  nom de fitxer erroni\n");
            break;
        case -2:
            fprintf(stderr, "  numero de columnes d'alguna fila no coincideix amb l'amplada del tauler de joc\n");
            break;
        case -3:
            fprintf(stderr, "  numero de columnes del laberint incorrecte\n");
            break;
        case -4:
            fprintf(stderr, "  numero de files del laberint incorrecte\n");
            break;
        case -5:
            fprintf(stderr, "  finestra de camp de joc no oberta\n");
            break;
        case -6:
            fprintf(stderr, "  posicio inicial del menjacocos damunt la pared del laberint\n");
            break;
        case -7:
            fprintf(stderr, "  posicio inicial del fantasma damunt la pared del laberint\n");
            break;
        }
        exit(7);
    }
}

/* funcio per moure un fantasma una posicio; retorna 1 si el fantasma   */
/* captura al menjacocos, 0 altrament					*/
void *mou_fantasma(void *index)
{
    objecte seg;
    int ret;
    int k, vk, nd, vd[3];

    do
    {
        /**cast**/
        int i = (intptr_t)index;

        ret = 0;
        nd = 0;
        /*p++; if ((p%2)==0) ralentitza fantasma a 2*retard */

        for (k = -1; k <= 1; k++) /* provar direccio actual i dir. veines */
        {
            vk = (f1.d + k) % 4; /* direccio veina */
            if (vk < 0)
                vk += 4;           /* corregeix negatius */
            seg.f = f1.f + df[vk]; /* calcular posicio en la nova dir.*/
            seg.c = f1.c + dc[vk];
            seg.a = win_quincar(seg.f, seg.c); /* calcular caracter seguent posicio */
            if ((seg.a == ' ') || (seg.a == '.') || (seg.a == '0'))
            {
                vd[nd] = vk; /* memoritza com a direccio possible */
                nd++;
            }
        }

        if (nd == 0)               /* si no pot continuar, */
            f1.d = (f1.d + 2) % 4; /* canvia totalment de sentit */
        else
        {
            if (nd == 1)                /* si nomes pot en una direccio */
                f1.d = vd[0];           /* li assigna aquesta */
            else                        /* altrament */
                f1.d = vd[rand() % nd]; /* segueix una dir. aleatoria */

            seg.f = f1.f + df[f1.d]; /* calcular seguent posicio final */
            seg.c = f1.c + dc[f1.d];
            seg.a = win_quincar(seg.f, seg.c);      /* calcular caracter seguent posicio */
            win_escricar(f1.f, f1.c, f1.a, NO_INV); /* esborra posicio anterior */
            f1.f = seg.f;
            f1.c = seg.c;
            f1.a = seg.a;                          /* actualitza posicio */
            win_escricar(f1.f, f1.c, '1', NO_INV); /* redibuixa fantasma */
            if (f1.a == '0')
                ret = 1; /* ha capturat menjacocos */
        }
        /**p++;  if ((p%2)==0)		 ralentitza fantasma a 2*retard */
        fi2 = ret;
        win_retard(retard);
    } while (!fi1 && !fi2);
    return;
}

/* funcio per moure el menjacocos una posicio, en funcio de la direccio de   */
/* moviment actual; retorna -1 si s'ha premut RETURN, 1 si s'ha menjat tots  */
/* els cocos, i 0 altrament */
void *mou_menjacocos(void *null)
{
    char strin[12];
    objecte seg;
    int tec, ret;
    do
    {
        // fprintf(stderr,"itreracio coco!\n");
        ret = 0;
        tec = win_gettec();
        if (tec != 0)
            switch (tec) /* modificar direccio menjacocos segons tecla */
            {
            case TEC_AMUNT:
                mc.d = 0;
                break;
            case TEC_ESQUER:
                mc.d = 1;
                break;
            case TEC_AVALL:
                mc.d = 2;
                break;
            case TEC_DRETA:
                mc.d = 3;
                break;
            case TEC_RETURN:
                ret = -1;
                break;
            }
        seg.f = mc.f + df[mc.d]; /* calcular seguent posicio */
        seg.c = mc.c + dc[mc.d];
        seg.a = win_quincar(seg.f, seg.c); /* calcular caracter seguent posicio */
        if ((seg.a == ' ') || (seg.a == '.'))
        {
            win_escricar(mc.f, mc.c, ' ', NO_INV); /* esborra posicio anterior */
            mc.f = seg.f;
            mc.c = seg.c;                          /* actualitza posicio */
            win_escricar(mc.f, mc.c, '0', NO_INV); /* redibuixa menjacocos */
            if (seg.a == '.')
            {
                cocos--;
                sprintf(strin, "Cocos: %d", cocos);
                win_escristr(strin);
                if (cocos == 0)
                    ret = 1;
            }
        }

        /*if ((p%2)==0)		 ralentitza fantasma a 2*retard **/
        fi1 = ret;
        win_retard(retard);
    } while (!fi1 && !fi2);
    return;
}

/* programa principal				    */
int main(int n_args, const char *ll_args[])
{

    srand(getpid()); /* inicialitza numeros aleatoris */

    if ((n_args != 2) && (n_args != 3))
    {
        fprintf(stderr, "Comanda: cocos0 fit_param [retard]\n");
        exit(1);
    }
    carrega_parametres(ll_args[1]);

    if (n_args == 3)
        retard = atoi(ll_args[2]);
    else
        retard = 100;

    rc = win_ini(&n_fil1, &n_col, '+', INVERS); /* intenta crear taulell */
    if (rc == 0)                                /* si aconsegueix accedir a l'entorn CURSES */
    {
        inicialitza_joc();
        p = 0;
        fi1 = 0;
        fi2 = 0;

        /**
       do
          {
          fi1 = mou_menjacocos();
          p++; if ((p%2)==0)
              fi2 = mou_fantasma();
          win_retard(retard);
          } while (!fi1 && !fi2);
          win_fi();
        **/

        pthread_t thread1;
        pthread_t thread2;

        fi1 = pthread_create(&thread1, NULL, mou_menjacocos, NULL);
        fi2 = pthread_create(&thread2, NULL, mou_fantasma, (void *)(intptr_t)index);

        pthread_join(thread1, (void **)&fi1);
        pthread_join(thread2, (void **)&fi2);

        win_fi();

        if (fi1 == -1)
            printf("S'ha aturat el joc amb tecla RETURN!\n");
        else
        {
            if (fi1)
                printf("Ha guanyat l'usuari!\n");
            else
                printf("Ha guanyat l'ordinador!\n");
        }
    }
    else
    {
        fprintf(stderr, "Error: no s'ha pogut crear el taulell:\n");
        switch (rc)
        {
        case -1:
            fprintf(stderr, "camp de joc ja creat!\n");
            break;
        case -2:
            fprintf(stderr, "no s'ha pogut inicialitzar l'entorn de curses!\n");
            break;
        case -3:
            fprintf(stderr, "les mides del camp demanades son massa grans!\n");
            break;
        case -4:
            fprintf(stderr, "no s'ha pogut crear la finestra!\n");
            break;
        }
        exit(6);
    }
    return (0);
}
