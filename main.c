#include "sopa.h"

int main(int argc, char *argv[]) {

                if(argc == 2)              // Comprovem nombre arguments sino acaba el programa
                {
                    srand(time(NULL)); //inicialitzem la llavor
                    sopa_t sopa;
                    solucio_t sol[MAX_PARAULES];

                    missatge_benvinguda();
                    carregar_paraules(argv[1], &sopa);
                    ordenar(&sopa);
                    while((sopa.dim < DIM_MIN) || (sopa.dim > DIM_MAX)) {   //bucle per a que el jugador només introdueixi valors del 10 al 40 que son la dimensió mínima i la màxima
                        printf("Indica la dimensio de la sopa de lletres (ha de ser un valor entre 10 i 40):\t");
                        scanf("%d", &sopa.dim);
                    }
                    sopa.n_encerts = 0;
                    genera_sopa(&sopa, sol);
                    mostra_sopa(&sopa);
                    has_trobat(&sopa, sol);
                }

    return 0;
}
