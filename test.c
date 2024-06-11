
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "tetris_heuristique.h"


int main() {
    srand( time( NULL ) );

    int width = 10;
    int height = 20;

    double coef_mon_heuristique[9] = {-2, -6, -58, -36, 7, -22, -61, -20, -6};
    double coef_mon_heuristique_entropie_croisee[9] = {-99.368309, -121.882760, -398.937069, -86.574341, 18.161392, -53.164912, -119.879491, -85.711275, -29.407215};

    double coef_dellacherie[6] = {-1, 1, -1, -1, -4, -1};
    double coef_dellacherie_entropie_croisee[6] = {-59.408966, 18.191485, -30.012043, -40.288439, -256.741967, -43.722236};

    printf("Heuristique de Dellacherie : \n");
    test(10, coef_dellacherie, true, width, height, dellacherie);

    printf("\nMon Heuristique issu de Dellacherie : \n");
    test(10, coef_mon_heuristique, true, width, height, mon_heuristique);


    printf("\nHeuristique de Dellacherie après entroie croisée: \n");
    test(10, coef_dellacherie_entropie_croisee, true, width, height, dellacherie);

    printf("\nMon Heuristique issu de Dellacherie après entropie croisée : \n");
    test(10, coef_mon_heuristique_entropie_croisee, true, width, height, mon_heuristique);

    return 0;    
}  