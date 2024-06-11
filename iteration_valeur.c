
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <pthread.h>

#include "iteration_valeur.h"


int pow2(int p) {
    return (1u << p);
}

void make_action(int action, int piece_num, int* pos, int* piece) {
    int dir = action % 4;
    (*pos) = action / 4;
    int pieces[7][4][8] = {
                            {{0,0, 1,0, 2,0, 3,0}, {0,3, 0,2, 0,1, 0,0}, {0,0, 1,0, 2,0, 3,0}, {0,3, 0,2, 0,1, 0,0}}, // I
                            {{0,1, 1,1, 1,0, 0,0}, {0,1, 1,1, 1,0, 0,0}, {0,1, 1,1, 1,0, 0,0}, {0,1, 1,1, 1,0, 0,0}}, // O
                            {{0,1, 1,1, 2,1, 1,0}, {0,1, 1,2, 1,1, 1,0}, {0,0, 1,0, 2,0, 1,1}, {0,2, 0,1, 0,0, 1,1}}, // T
                            {{0,0, 0,1, 1,1, 2,1}, {0,2, 1,2, 1,1, 1,0}, {0,0, 1,0, 2,0, 2,1}, {0,2, 0,1, 0,0, 1,0}}, // L
                            {{0,1, 1,1, 2,1, 2,0}, {0,0, 1,2, 1,1, 1,0}, {0,0, 1,0, 2,0, 0,1}, {0,2, 0,1, 0,0, 1,2}}, // J
                            {{0,1, 1,1, 1,0, 2,0}, {0,0, 0,1, 1,1, 1,2}, {0,1, 1,1, 1,0, 2,0}, {0,0, 0,1, 1,1, 1,2}}, // Z
                            {{0,0, 1,0, 1,1, 2,1}, {0,2, 0,1, 1,1, 1,0}, {0,0, 1,0, 1,1, 2,1}, {0,2, 0,1, 1,1, 1,0}}  // S
                          };
    for (int i=0; i<8; i++) {
        piece[i] = pieces[piece_num][dir][i];
    }
}

int reward(int real_state, int piece, int action, int width, int height) {
    float r = 0;
    int piece_num = piece;
    int game_state = real_state;
    game_state = joue(game_state, action, piece_num, width, height);
    for (int i=0; i<height; i++) {
        bool complete_line = true;
        for (int j=0; j<width; j++) {
            if ((game_state & 1) == 0) complete_line = false;
            game_state = game_state >> 1;
        }
        if (complete_line) r++;
    }
    return r;
}

int reward_inst(int real_state, int width, int height) {
    float r = 0;
    for (int i=0; i<height; i++) {
        bool complete_line = true;
        for (int j=0; j<width; j++) {
            if ((real_state & 1) == 0) complete_line = false;
            real_state = real_state >> 1;
        }
        if (complete_line) r++;
    }
    return r;
}

int update(int real_state, int width, int height) {
    int decalage = 0;
    for (int i=0; i<height; i++) {
        int nb_vide = 0;
        for (int j=0; j<width; j++) {
            if (get_pos(real_state, j + i*width) == false) nb_vide++;
        }
        if (nb_vide == 0) {
            for (int j=0; j<width; j++) {
                real_state = set_pos(real_state, j + i*width, 0);
            }
            decalage++;
        } else {
            for (int j=0; j<width; j++) {
                int tmp = get_pos(real_state, j+i*width);
                real_state = set_pos(real_state, j+i*width, 0);
                real_state = set_pos(real_state, j+(i-decalage)*width, tmp);
            }
        }
    }
    return real_state;
}

bool get_pos(int real_state, int index) {
    return ((real_state >> index) & 1) == 1;
}
int set_pos(int real_state, int index, int val) {
    if (val == 0) {
        return real_state - (real_state & (1u << index));
    } else {
        return (real_state | (1u << index));
    }
}

int joue(int real_state, int action, int piece_num, int width, int height) {
    if (legal_action(piece_num, action, width) == false) {
        return -1;
    }

    int pos;
    int* piece = (int*) malloc(8*sizeof(int));
    make_action(action, piece_num, &pos, piece);

    int y = height;
    bool valid_y = true;
    while (valid_y) {
        for (int i=0; i<4; i++) {
            int index = pos+piece[i*2] + (y+piece[i*2+1])*width;
            if (y+piece[i*2+1] >= 0 && y+piece[i*2+1] < height && get_pos(real_state, index)) valid_y = false;
        }
        if (valid_y) y--;
        if (y==-1) valid_y = false;
    }
    y++;
    for (int i=0; i<4; i++) {
        int index = pos+piece[i*2] + (y+piece[i*2+1])*width;
        if (y+piece[i*2+1] >= 0 && y+piece[i*2+1] < height) {
            real_state = (real_state | (1u << index));
        } else {
            // END GAME
            free(piece);
            return -1;
        }
    }
    free(piece);
    return real_state;
}

void print_game(int real_state, int width, int height) {
    for (int i=0; i<width; i++) printf("-");
    printf("\n");
    for (int i=0; i<height; i++) {
        int ii = height-i-1;
        for (int j=0; j<width; j++) {
            printf("%c", (get_pos(real_state, j+ii*width) == 0)?' ':'X');
        }
        printf("\n");
    }
    for (int i=0; i<width; i++) printf("-");
    printf("\n");
}

bool legal_action(int piece_num, int action, int width) {
    int pos;
    int* piece = (int*) malloc(8*sizeof(int));
    make_action(action, piece_num, &pos, piece);
    for (int i=0; i<4; i++) {
        if (piece[i*2] + pos < 0 || piece[i*2] + pos >= width) {
            free(piece);
            return false;
        }
    }
    free(piece);
    return true;
}

float mabs(float a) {
    if (a>0) return a;
    else return -a;
}
float mmax(float a, float b) {
    if (a>b) return a;
    else return b;
}

void save_model(vecteur* v, char* file) {
    FILE* a = fopen(file, "w");
    printf("start save\n");
    for (int i = 0; i<v->size; i++) {
        if (i%(v->size/1000) == 0) {
            printf("\r%f", (double)i*100/(v->size));
            fflush(stdout);
        }
        fprintf(a, "%f\n", v->data[i]);
    }
    printf("\nend save\n");
    fclose(a);
}

void load_model(vecteur* v, char* file) {
    FILE* a = fopen(file, "r");
    printf("load save\n");
    for (int i = 0; i<v->size; i++) {
        if (i%(v->size/1000) == 0) {
            printf("\r%f", (double)i*100/(v->size));
            fflush(stdout);
        }
        fscanf(a, "%f\n", &v->data[i]);
    }
    printf("\nend\n");
    fclose(a);
}

void save_model_i(vecteur_int* v, char* file) {
    FILE* a = fopen(file, "w");
    printf("start save\n");
    for (int i = 0; i<v->size; i++) {
        if (i%(v->size/1000) == 0) {
            printf("\r%f", (double)i*100/(v->size));
            fflush(stdout);
        }
        fprintf(a, "%d\n", v->data[i]);
    }
    printf("\nend save\n");
    fclose(a);
}

void load_model_i(vecteur_int* v, char* file) {
    FILE* a = fopen(file, "r");
    printf("load\n");
    for (int i = 0; i<v->size; i++) {
        if (i%(v->size/1000) == 0) {
            printf("\r%f", (double)i*100/(v->size));
            fflush(stdout);
        }
        fscanf(a, "%d\n", &v->data[i]);
    }
    printf("\nend\n");
    fclose(a);
}

float parcour(int real_state, vecteur* vi, vecteur* vim1, vecteur_int* pi, int nb_piece, int nb_action, float gamma, int width, int height, vecteur_int* vu) {
    float max_diff = 0;

    for (int piece = 0; piece < nb_piece; piece++) {
        float max = 0;
        int max_a = 0;

        int full_state = real_state*nb_piece + piece;
        if (vu->data[full_state] == 0) {
            vu->data[full_state] = 1;

            float sm = 0;
            int real_sp_act_0 = joue(real_state, 0, piece, width, height);
            if (real_sp_act_0 != -1) {
                real_sp_act_0 = update(real_sp_act_0, width, height);
                for (int p=0; p<nb_piece; p++) {
                    sm += vim1->data[real_sp_act_0*nb_piece + p]/nb_piece;
                }
                max = ((float) reward(real_state, piece, 0, width, height)) + gamma*sm;
            }

            for (int a=1; a<nb_action; a++) {
                float nsm = 0;
                int real_sp_act = joue(real_state, a, piece, width, height);
                if (real_sp_act != -1) {
                    real_sp_act = update(real_sp_act, width, height);
                    for (int p=0; p<nb_piece; p++) {
                        nsm += vim1->data[real_sp_act*nb_piece + p]/nb_piece;
                    }
                    float nv = ((float) reward(real_state, piece, a, width, height)) + gamma*nsm;
                    if (nv > max) {
                        max = nv;
                        max_a = a;
                    }
                }
            }
            
            vi->data[full_state] = max;
            pi->data[full_state] = max_a;
            float diff = mabs(vi->data[full_state] - vim1->data[full_state]);

            for (int a=0; a<nb_action; a++) {
                int next_state = joue(real_state, a, piece, width, height);
                float next_diff = 0;
                if (next_state != -1) {
                    next_state = update(next_state, width, height);
                    next_diff = parcour(next_state, vi, vim1, pi, nb_piece, nb_action, gamma, width, height, vu);
                }

                float part_max_diff = mmax(diff, next_diff);
                if (part_max_diff > max_diff) max_diff = part_max_diff;
            }
        }
    }

    return max_diff;
}

int main() {
    srand( time( NULL ) );

    int width = 4;
    int height = 5;

    int nb_piece = 7;
    int nb_real_state = pow2(width*height);
    int nb_state = nb_real_state*nb_piece;

    int nb_action = width*4;

    float gamma = 1;
    
    vecteur* vi = (vecteur*) malloc(sizeof(vecteur));
    vi->size = nb_state;
    vi->data = (float*) malloc(sizeof(float)*nb_state);
    for (int i=0; i<vi->size; i++) {
        vi->data[i] = 0;
    }

    vecteur* vim1 = (vecteur*) malloc(sizeof(vecteur));
    vim1->size = nb_state;
    vim1->data = (float*) malloc(sizeof(float)*nb_state);
    for (int i=0; i<vim1->size; i++) {
        vim1->data[i] = 0;
    }

    vecteur_int* pi = (vecteur_int*) malloc(sizeof(vecteur_int));
    pi->size = nb_state;
    pi->data = (int*) malloc(sizeof(int)*nb_state);
    for (int i=0; i<pi->size; i++) {
        pi->data[i] = 0;
    }

    int max_it = 6;
    int it = 0;
    float epsilon = 1;

    bool loop = true;  // True pour entrainer le model False sinon

    while (loop && it<max_it) {
        it++;

        vecteur_int* vu = (vecteur_int*) malloc(sizeof(vecteur_int));
        vu->size = nb_state;
        vu->data = (int*) malloc(sizeof(int)*nb_state);
        for (int i=0; i<vu->size; i++) {
            vu->data[i] = 0;
        }

        float max_diff = parcour(0, vi, vim1, pi, nb_piece, nb_action, gamma, width, height, vu);

        free(vu->data);
        free(vu);

        for (int i=0; i<vim1->size; i++) {
            vim1->data[i] = vi->data[i];
        }

        loop = epsilon <= max_diff;
        printf("epsilon : %f, max_diff : %f\n", epsilon, max_diff);
    }
    free(vi->data);
    free(vi);

    // Sauvegarde le model
    save_model(vim1, "model4x5.txt");
    save_model_i(pi, "model_pi4x5.txt");
    // Charge un model
    // load_model(vim1, "model4x5.txt");
    // load_model_i(pi, "model_pi4x5.txt");


    float score_moy = 0;
    int nb_parti = 10000;
    for (int i=0; i<nb_parti; i++) {
        int real_state = 0;
        while (real_state != -1) {
            int piece = rand()%nb_piece;
            int action = pi->data[real_state*nb_piece + piece];
            if (action != -1) {
                real_state = joue(real_state, action, piece, width, height);
                if (real_state != -1) {
                    score_moy += reward_inst(real_state, width, height);
                    real_state = update(real_state, width, height);
                }
            } else {
                real_state = -1;
            }
        }
    }
    score_moy /= nb_parti;
    printf("score moyen sur %d parties : %f\n", nb_parti, score_moy);

    free(vim1->data);
    free(vim1);
    free(pi->data);
    free(pi);

    return 0;
}