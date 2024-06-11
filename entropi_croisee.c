
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include <unistd.h>

#include "tetris_heuristique.h"


struct couple {
    double* vects;
    float val;
};
typedef struct couple couple;

#define TWOPI 2.0*3.141592653589793238462643383279502884197169399375

// Trie décroissant du tableau tab en utilisant val
void sort(couple* tab, int size) {
    for (int i=0; i<size; i++) {
        float max = tab[i].val;
        int maxi = i;
        for (int j=i; j<size; j++) {
            if (tab[j].val > max) {
                max = tab[j].val;
                maxi = j;
            }
        }
        couple tmp = tab[i];
        tab[i] = tab[maxi];
        tab[maxi] = tmp;
    }
}

// Renvoie une valeur générer par une loi normal de paramètre mu et sigma
double loi_normale(double mu, double sigma) {
    double U = ((double) rand())/((double) RAND_MAX);
    double V = ((double) rand())/((double) RAND_MAX);
    double unif_centrer_reduit = sqrt(-2.0*log(U))*cos(TWOPI*V);
    return (sigma*unif_centrer_reduit + mu);
}

// Renvoie N tableau de taille m dont chaque valeur suit une loi normale de paramètre mu[i] et sigma[i]
double** generer_vecteurs(int N, int m, double* mu, double* sigma) {
    double** vects = (double**) malloc(sizeof(double*)*N);
    for (int i=0; i<N; i++) {
        vects[i] = (double*) malloc(sizeof(double)*m);
        for (int j=0; j<m; j++) {
            vects[i][j] = loi_normale(mu[j], sigma[j]);
        }
    }
    return vects;
}

// Évalue N vecteurs en jouant k partie de Tetris pour chaque vecteur
float* evaluer(double** vecteur, int N, int k, int width, int height, float (*fnc)(bool*,int*,int,int,bool,double*,double*)) {
    float* val = (float*) malloc(sizeof(float)*N);
    for (int i=0; i<N; i++) {
        float v = test(k, vecteur[i], false, width, height, fnc);
        val[i] = v;
    }
    return val;
}

float bruit(int t) {
    return 4.0;
}

int main() {
    srand( time( NULL ) );

    int m = 9; // nombre de param
    float (*fonction_valeur)(bool*,int*,int,int,bool,double*,double*) = mon_heuristique; // Fonction de valeur utilisée

    double* mu = (double*) malloc(sizeof(double)*m);
    double* sigma = (double*) malloc(sizeof(double)*m);
    for (int i=0; i<m; i++) {
        mu[i] = 0;
        sigma[i] = 100;
    }

    int t = 0;
    int N = 110;
    float rho = 0.1;
    int k = 1;
    int width = 10;
    int height = 10;

    while (true) {
        t++;
        printf("iter : %d\n", t);
        double** vecteurs = generer_vecteurs(N, m, mu, sigma);

        float* value = evaluer(vecteurs, N, k, width, height, fonction_valeur);
        couple* c = (couple*) malloc(sizeof(couple)*N);
        for (int i=0; i<N; i++) {
            c[i].val = value[i];
            c[i].vects = vecteurs[i];
        }
        
        sort(c, N);
        
        int nb_select = rho*N;
        for (int i=0; i<m; i++) {
            mu[i] = 0;
            for (int j=0; j<nb_select; j++) {
                mu[i] += c[j].vects[i];
            }
            mu[i] /= nb_select;
            float var = 0;
            for (int j=0; j<nb_select; j++) {
                float v = c[j].vects[i] - mu[i];
                var += v*v;
            }
            var /= nb_select;
            sigma[i] = sqrtf(var + bruit(t));
        }
        free(value);
        for (int i=0; i<N; i++) {
            free(vecteurs[i]);
        }
        free(c);
        free(vecteurs);
        
        printf("mu : ");
        for (int i=0; i<m; i++) {
            printf("%f, ", mu[i]);
        }
        printf("\nsigma : ");
        for (int i=0; i<m; i++) {
            printf("%f, ", sigma[i]);
        }
        printf("\n");
    }
    

    free(mu);
    free(sigma);

    return 0;
}