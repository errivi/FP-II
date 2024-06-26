#include "sopa.h"
#include <windows.h>
#include <mmsystem.h>

void missatge_benvinguda() {
    printf("************************************************************************************\n");
    printf("\tUs donem la benvinguda al joc de la sopa de lletres!\n");
    printf("\tAutors: Eric Riveiro, Oriol Algar, Hugo Miranda\n");
    printf("\n\tNORMES: Troba les paraules de la sopa de lletres. \n\tPer fer una jugada digues si has trobat una paraula\n\to et rendeixes, despres indica les coordenades de la lletra inicial.\n");
    printf("\n\t 1.->\tesquerra cap a dreta\n\t-1.->\tdreta cap a esquerra\n\t 2.->\tde baix cap a dalt\n\t-2.->\tde dalt cap a baix\n");
    printf("************************************************************************************\n\n");
}

void carregar_paraules(char nom_fitx[], sopa_t *sopa) {
    FILE *f;
    f = fopen(nom_fitx, "r");
    int i = 0;

    if(f == NULL) {
        printf("Error al obrir l'arxiu.");
    }
    else {
        while(!feof(f) && i <= MAX_PARAULES) {
            fscanf(f, "%s", sopa->par[i].ll);
            i++;
        }
        sopa->n_par = i - 1;
    }
    fclose(f);
}

void ordenar(sopa_t *sopa){
    char ind[MAX_PARAULES];

    for(int i = 0; i < sopa->n_par; i++) {          // Ordenem les paraules per bombolla
        for(int j = i + 1; j < sopa->n_par; j++) {
            if (strcmp(sopa->par[i].ll, sopa->par[j].ll) > 0) {
                strcpy(ind, sopa->par[i].ll);
                strcpy(sopa->par[i].ll, sopa->par[j].ll);
                strcpy(sopa->par[j].ll, ind);
            }
        }
    }
}

bool hi_ha_lloc_dret(sopa_t *s, int longi, int posi) {
    int fila = (posi/s->dim) + 1;               // Obtenim la fila següent a la que li toca a posi
    int ul_posi = (fila * s->dim) - 1;          // Obtenim la primera posició de la fila següent i li restem 1 per obtenir la darrera posició de la fila que li correspon a la paraula
    int lloc = (ul_posi - posi);                // Restem la última posició amb la posició on va la primera lletra per quew ens donin el nombre de caselles que tenim abans de que salti de fila
    bool h_lloc = true;

    if((longi - 1) <= lloc) {
        for(int i = (posi + 1); i <= (posi + longi); i++) {
            if((s->lletres[i] != 0))
                h_lloc = false;
        }
    }
    else h_lloc = false;

    return (h_lloc);
}

bool hi_ha_lloc_esq(sopa_t *s, int longi, int posi) {
    int fila = (posi/s->dim) + 1;               // Calculem la fila on mirarem si hi ha lloc
    int prim_posi = (fila * s->dim) - s->dim;   // Calculem la posicio de la lletra inicial
    int lloc = (posi - prim_posi);              // El espai que hi ha lliure en la fila
    bool h_lloc = true;

    if((longi - 1) <= lloc) {                   // @pre Si la paraula cap en el lloc calculat
        for(int i = (posi - 1); i >= (posi - longi); i--) {
            if(s->lletres[i] != 0)
                h_lloc = false;
        }
    }
    else h_lloc = false;

    return (h_lloc);
}

bool hi_ha_lloc_amunt(sopa_t *s, int longi, int posi) {
    int ul_posi = posi - (s->dim * (longi - 1));
    bool h_lloc = true;

    if(ul_posi >= 0) {
        for(int i = (posi - s->dim); i >= ul_posi; i = i - s->dim) {
            if(s->lletres[i] != 0)
                h_lloc = false;
        }
    }
    else h_lloc = false;

    return (h_lloc);
}

bool hi_ha_lloc_abaix(sopa_t *s, int longi, int posi) {
    int ul_posi = posi + (s->dim * (longi - 1));
    bool h_lloc = true;

    if(ul_posi < (s->dim*s->dim)) {
        for(int i = (posi + s->dim); i <= ul_posi ; i = i + (s->dim)) {
            if((s->lletres[i] != 0))
                h_lloc = false;
        }
    }
    else h_lloc = false;

    return (h_lloc);
}

void insertar_paraula_dret(sopa_t *sopa, unsigned short pos_1, int longi, int aux_par) {
    int t = pos_1;

    for(int i = 0; i < longi; i++) {
      sopa->lletres[t] = sopa->par[aux_par].ll[i];        // Inserim la paraula lletra per lletra
      sopa->encertades[t] = true;
      t++;
    }
}

void insertar_paraula_esq(sopa_t *sopa, unsigned short pos_1, int longi, int aux_par) {
    int t = pos_1;

    for(int i = 0; i < longi; i++) {
      sopa->lletres[t] = sopa->par[aux_par].ll[i];
      sopa->encertades[t] = true;        // Inserim la paraula lletra per lletra
      t--;
    }
}

void insertar_paraula_amunt(sopa_t *sopa, unsigned short pos_1, int longi, int aux_par) {
    int t = pos_1;

    for(int i = 0; i < longi; i++) {
      sopa->lletres[t] = sopa->par[aux_par].ll[i];        // Inserim la paraula lletra per lletra
      sopa->encertades[t] = true;
      t = t - sopa->dim;
    }
}

void insertar_paraula_abaix(sopa_t *sopa, unsigned short pos_1, int longi, int aux_par) {
    int t = pos_1;

    for(int i = 0; i < longi; i++) {
      sopa->lletres[t] = sopa->par[aux_par].ll[i];        // Inserim la paraula lletra per lletra
      sopa->encertades[t] = true;
      t = t + sopa->dim;
    }
}

void crear_posicio(sopa_t *sopa, solucio_t soluc[]) {
    bool cap;
    int direc;
    int pos_1;      // Posició inicial primera lletra paraula

    for(int i = 0; i < sopa->n_par; i++) {
      int longi = strlen(sopa->par[i].ll);
      cap = false;
      direc = rand() % 4;

        while(!cap) {
            switch(direc) {
                case 0: pos_1 = rand() % ((sopa->dim * sopa->dim) - 1);
                        while((sopa->lletres[pos_1] != 0) || (!hi_ha_lloc_dret(sopa, longi, pos_1))) {
                            pos_1 = rand() % ((sopa->dim * sopa->dim) - 1);      // Comprovem que la primera posició no hi ha res
                        }
                        insertar_paraula_dret(sopa, pos_1, longi, i);
                        cap = true;
                        soluc[i].pos_ini = pos_1;
                        soluc[i].dir = 1;
                        break;

                case 1: pos_1 = rand() % ((sopa->dim * sopa->dim) - 1);
                        while((sopa->lletres[pos_1] != 0) || (!hi_ha_lloc_esq(sopa, longi, pos_1))) {
                            pos_1 = rand() % ((sopa->dim * sopa->dim) - 1);      // Comprovem que la primera posició no hi ha res
                        }
                        insertar_paraula_esq(sopa, pos_1, longi, i);
                        cap = true;
                        soluc[i].pos_ini = pos_1;
                        soluc[i].dir = (-1);
                        break;

                case 2: pos_1 = rand() % ((sopa->dim * sopa->dim) - 1);
                        while((sopa->lletres[pos_1] != 0) || (!hi_ha_lloc_amunt(sopa, longi, pos_1))) {
                            pos_1 = rand() % ((sopa->dim * sopa->dim) - 1);      // Comprovem que la primera posició no hi ha res
                        }
                        insertar_paraula_amunt(sopa, pos_1, longi, i);
                        cap = true;
                        soluc[i].pos_ini = pos_1;
                        soluc[i].dir = 2;
                        break;

                case 3: pos_1 = rand() % ((sopa->dim * sopa->dim) - 1);
                        while((sopa->lletres[pos_1] != 0) || (!hi_ha_lloc_abaix(sopa, longi, pos_1))) {
                            pos_1 = rand() % ((sopa->dim * sopa->dim) - 1);      // Comprovem que la primera posició no hi ha res
                        }
                        insertar_paraula_abaix(sopa, pos_1, longi, i);
                        cap = true;
                        soluc[i].pos_ini = pos_1;
                        soluc[i].dir = (-2);
                        break;
            }
        }
    }
}

void genera_sopa(sopa_t *s, solucio_t solu[]) {
    s->lletres = calloc(s->dim * s->dim, sizeof(char));     // Espai per a les lletres
    s->encertades = calloc(s->dim * s->dim, sizeof(char));  // Per saber si una lletra correspon a encert

    crear_posicio(s, solu);
    for(int i = 0; i < (s->dim * s->dim); i++) {
        s->encertades[i] = false;
        // Generem una lletra aleatoriament
        if(s->lletres[i] == 0) {
            s->lletres[i] = 'A' + (rand() % ('Z'-'A' + 1));
        }
    }
}

void mostra_sopa (sopa_t *s) {
    // Mostrem els numeros de columna
    printf("\033[0;31m");   // Color
    printf("  ");
    for (int i = 10; i < s->dim + 1; i+=10)
    {
        for (int j=0; j < 18; j++)
            printf(" ");
        printf(" %d", i/10);

    }

    printf("\n  ");
    for (int i = 0; i < s->dim; i++)
    {
        int p = (i % 10) + 1;
        p != 10 ? printf(" %d", p) : printf(" 0");
    }

    printf("\n");
    printf("\033[0m");  // Tornem al color per defecte


    // Mostrem les lletres. Cada lletra ocupa dos espais:
    // Si correspon a un encert, es marca
    for (int i = 0; i < s->dim ; i++)
    {
        printf("\033[0;31m");   // Color
        printf("%-2d", i + 1);  // Mostrar numero de linia
        printf("\033[0m");  // Tornem al color per defecte

        for (int j = 0; j < s->dim; j++)
        {
            if (s->encertades[i * s->dim + j])
            {
                printf("\033[0;42m");   // Color verd de fons
                printf(" %c", s->lletres[i * s->dim + j]);
                printf("\033[0m");  // Tornem al color per defecte
            }
            else
            {
                printf(" %c", s->lletres[i * s->dim + j]);
            }
        }
        printf("\n");
    }
    printf("\n");

    printf("Portes %d encerts.\n", s->n_encerts);
    printf("Paraules a trobar: %d\n", s->n_par - s->n_encerts);
    for (int i = 0; i < s->n_par; i++)
    {
        if (!s->par[i].enc)
            printf("%s\n", s->par[i].ll);
    }
}

void mostrar_solu(sopa_t *sopa, solucio_t solu[]) {

    for(int i = 0; i <= (sopa->n_par + 1); i++) {
        int longi = strlen(sopa->par[i].ll);

        switch(solu[i].dir) {
            case 2: for(int z = solu[i].pos_ini; z > (solu[i].pos_ini - longi*sopa->dim); z = z - sopa->dim) {
                        sopa->encertades[z] = true;
                     }
                     break;

            case -1: for(int z = solu[i].pos_ini; z > (solu[i].pos_ini - longi); z--) {
                        sopa->encertades[z] = true;
                     }
                     break;

            case 1: for(int z = solu[i].pos_ini; z < (solu[i].pos_ini + longi); z++) {
                        sopa->encertades[z] = true;
                     }
                     break;

            case -2: for(int z = solu[i].pos_ini; z < (solu[i].pos_ini + longi*sopa->dim); z = z + sopa->dim) {
                        sopa->encertades[z] = true;
                     }
                     break;
        }
    }
}

void fer_jugada(sopa_t *sopa, solucio_t solu[], int fil_res, int col_res, int direc_res) {
    int posi_jug = ((fil_res - 1)*sopa->dim + (col_res - 1));     // Restem '-1' per concordar les coordenades de la sopa que veu el usuari amb les del programa intern
    bool falla_jugada = true;

    for(int i = 0; i < sopa->n_par; i++) {
        if((solu[i].pos_ini == posi_jug) && (solu[i].dir == direc_res)) {
            int longi = strlen(sopa->par[i].ll);

            switch(solu[i].dir) {
                case 2: for(int z = solu[i].pos_ini; z > (solu[i].pos_ini - (longi*sopa->dim)); z = z - sopa->dim) {
                            sopa->encertades[z] = true;
                        }
                        PlaySound("C:\\Users\\Rivi\\Music\\acierto.wav", NULL, SND_FILENAME);
                        break;

                case -1: for(int z = solu[i].pos_ini; z > (solu[i].pos_ini - longi); z--) {
                            sopa->encertades[z] = true;

                        }
                        PlaySound("C:\\Users\\Rivi\\Music\\acierto.wav", NULL, SND_FILENAME);
                        break;

                case 1:  for(int z = solu[i].pos_ini; z < (solu[i].pos_ini + longi); z++) {
                            sopa->encertades[z] = true;
                        }
                        PlaySound("C:\\Users\\Rivi\\Music\\acierto.wav", NULL, SND_FILENAME);
                        break;

                case -2: for(int z = solu[i].pos_ini; z < (solu[i].pos_ini + (longi*sopa->dim)); z = z + sopa->dim) {
                            sopa->encertades[z] = true;
                        }
                        PlaySound("C:\\Users\\Rivi\\Music\\acierto.wav", NULL, SND_FILENAME);
                        break;
            }
            sopa->par[i].enc = true;
            sopa->n_encerts++;
            printf("\n\t>>>> Ben fet, has encertat la paraula!\n");
            falla_jugada = false;
        }
    }
    if(falla_jugada)
    {
        printf("\n\t>>>> Has fallat, torna a provar...\n");
        PlaySound("C:\\Users\\Rivi\\Music\\error.wav", NULL, SND_FILENAME);
    }
}

void has_trobat(sopa_t *s, solucio_t solu[]) {
    char user_res[MAX_LLETRES];
    int fil_res, col_res, dir_res;

    do {
        do {
            printf("\nSi has trobat una paraula introdueix 'SI', sino introdueix 'RENDICIO' per veure la solucio final:\n");
            scanf("%s", user_res);

        } while((strcmp(user_res, "SI") != 0) && (strcmp(user_res, "RENDICIO") != 0));

        if(strcmp(user_res, "SI") == 0) {
            do {
                printf("Introdueix la fila de la lletra inicial: ");
                scanf("%d", &fil_res);
                fflush(stdin);
            } while(fil_res < 0 || fil_res > s->dim);
            do {
                printf("Introdueix la columna de la lletra inicial: ");
                scanf("%d", &col_res);
                fflush(stdin);
            } while(col_res < 0 || col_res > s->dim);
            do {
                printf("Introdueix la direccio de la paraula: ");
                scanf("%d", &dir_res);
                fflush(stdin);
            } while(dir_res < -2 || dir_res > 2 || dir_res == 0);
            fer_jugada(s, solu, fil_res, col_res, dir_res);
            mostra_sopa(s);
        }
        else {      // Per la opció 'RENDICIO'
            printf("La solucio es:\n");
            mostrar_solu(s, solu);
            mostra_sopa(s);
            printf("\n\t>>>>>> Gracies per intentar, ens veiem en la proxima partida...\n");
            PlaySound("C:\\Users\\Rivi\\Music\\end.wav", NULL, SND_FILENAME);
        }
    } while((s->n_encerts < s->n_par) && (strcmp(user_res, "RENDICIO") != 0));
    if(s->n_encerts == s->n_par){
        printf("\t>>>>>> Enhorabona, has guanyat!!!\n ");
        PlaySound("C:\\Users\\Rivi\\Music\\win.wav", NULL, SND_FILENAME);
    }
}
