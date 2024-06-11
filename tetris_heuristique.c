
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

#include "tetris_heuristique.h"

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

float mabs(float a) {
    if (a<0) return -a;
    else return a; 
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

bool joue(bool* state, int action, int piece_num, int width, int height) {
    if (action == -1) return false;
    if (legal_action(piece_num, action, width) == false) {
        return false;
    }

    int pos;
    int* piece = (int*) malloc(8*sizeof(int));
    make_action(action, piece_num, &pos, piece);

    int y = height;
    bool valid_y = true;
    while (valid_y) {
        for (int i=0; i<4; i++) {
            int index = pos+piece[i*2] + (y+piece[i*2+1])*width;
            if (y+piece[i*2+1] >= 0 && y+piece[i*2+1] < height && state[index]) valid_y = false;
        }
        if (valid_y) y--;
        if (y==-1) valid_y = false;
    }
    y++;
    for (int i=0; i<4; i++) {
        int index = pos+piece[i*2] + (y+piece[i*2+1])*width;
        if (y+piece[i*2+1] >= 0 && y+piece[i*2+1] < height) {
            state[index] = true;
        } else {
            // END GAME
            free(piece);
            return false;
        }
    }
    free(piece);
    return true;
}

bool joue_get_piece_pos(bool* state, int action, int piece_num, int width, int height, int* piece_pos) {
    if (action == -1) return false;
    if (legal_action(piece_num, action, width) == false) {
        return false;
    }

    int pos;
    int* piece = (int*) malloc(8*sizeof(int));
    make_action(action, piece_num, &pos, piece);

    int y = height;
    bool valid_y = true;
    while (valid_y) {
        for (int i=0; i<4; i++) {
            int index = pos+piece[i*2] + (y+piece[i*2+1])*width;
            if (y+piece[i*2+1] >= 0 && y+piece[i*2+1] < height && state[index]) valid_y = false;
        }
        if (valid_y) y--;
        if (y==-1) valid_y = false;
    }
    y++;
    for (int i=0; i<4; i++) {
        int index = pos+piece[i*2] + (y+piece[i*2+1])*width;
        if (y+piece[i*2+1] >= 0 && y+piece[i*2+1] < height) {
            piece_pos[i] = index;
            state[index] = true;
        } else {
            // END GAME
            free(piece);
            return false;
        }
    }
    free(piece);
    return true;
}

void update(bool* state, int width, int height) {
    int decalage = 0;
    for (int i=0; i<height; i++) {
        int nb_vide = 0;
        for (int j=0; j<width; j++) {
            if (state[j + i*width] == false) nb_vide++;
        }
        if (nb_vide == 0) {
            for (int j=0; j<width; j++) {
                state[j + i*width] = false;
            }
            decalage++;
        } else {
            for (int j=0; j<width; j++) {
                bool tmp = state[j+i*width];
                state[j+i*width] = false;
                state[j+(i-decalage)*width] = tmp;
            }
        }
    }
}

int nb_ligne_fill(bool* state, int width, int height) {
    float nb_ligne = 0;
    for (int i=0; i<height; i++) {
        bool complete_line = true;
        for (int j=0; j<width; j++) {
            int index = j+i*width;
            if (state[index] == false) complete_line = false;
        }
        if (complete_line) nb_ligne++;
    }
    return nb_ligne;
}

int reward_inst(bool* state, int width, int height) {
    return nb_ligne_fill(state, width, height);
}

float dellacherie(bool* state, int* piece_pos, int width, int height, bool print, double* coef, double* detail) {
    bool* new_state = (bool*) malloc(sizeof(bool)*width*height);
    for (int i=0; i<width*height; i++) {
        new_state[i] = state[i];
    }

    int max_height = 0;
    for (int i=0; i<4; i++) {
        int h = (int) piece_pos[i]/width;
        if (h > max_height) max_height = h;
    }

    int nb_ligne = 0;
    int nb_case_sup_in_last_piece = 0;
    for (int i=0; i<height; i++) {
        bool complete_line = true;
        for (int j=0; j<width; j++) {
            int index = j+i*width;
            if (new_state[index] == false) complete_line = false;
        }
        if (complete_line) {
            for (int j=0; j<4; j++) {
                int h = (int) piece_pos[j]/width;
                if (h == i) nb_case_sup_in_last_piece++;
            }
            nb_ligne++;
        }
    }
    int erosion = nb_ligne * nb_case_sup_in_last_piece;

    update(new_state, width, height);

    int transition_ligne = 0;
    int transition_col = 0;
    for (int i=0; i<width; i++) {
        for (int j=0; j<height; j++) {
            if (i<width-1 && new_state[i+j*width] ^ new_state[(i+1)+j*width]) transition_ligne++;
            if (j<height-1 && new_state[i+j*width] ^ new_state[i+(j+1)*width]) transition_col++;
        }
    }

    int trous = 0;
    for (int i=0; i<width; i++) {
        bool recouver = false;
        for (int j=height-1; j>=0; j--) {
            if (recouver && new_state[i+j*width] == false) trous++;
            if (new_state[i+j*width]) recouver = true;
        }
    }

    int puits = 0;
    for (int i=0; i<width; i++) {
        int h = 0;
        for (int j=height-1; j>=0; j--) {
            if (new_state[i+j*width]==false && (i==0 || new_state[i-1+j*width]) && (i==width-1 || new_state[i+1+j*width])) {
                h++;
                puits += h;
            }
            if (new_state[i+j*width]) j=0;
        }
    }

    if (print) {
        printf("max_height : %d\nerosion : %d\ntransition ligne : %d\ntransition colonnes : %d\ntrous : %d\npuits : %d\n", max_height, erosion, transition_ligne, transition_col, trous, puits);
    }

    if (detail != NULL) {
        detail[0] = max_height;
        detail[1] = erosion;
        detail[2] = transition_ligne;
        detail[3] = transition_col;
        detail[4] = trous;
        detail[5] = puits;
    }

    free(new_state);

    return coef[0]*max_height+coef[1]*erosion+coef[2]*transition_ligne+coef[3]*transition_col+coef[4]*trous+coef[5]*puits;
}

float bertsekas_ioffe_modifier(bool* state, int* piece_pos, int width, int height, bool print, double* coef, double* detail) {
    bool* new_state = (bool*) malloc(sizeof(bool)*width*height);
    for (int i=0; i<width*height; i++) {
        new_state[i] = state[i];
    }
    update(new_state, width, height);

    int maxH = 0;
    int* hk = (int*) malloc(sizeof(int)*width);
    int* deltaHk = (int*) malloc(sizeof(int)*(width-1));
    int l = 0;
    for (int i=0; i<width; i++) {
        int k = height-1;
        while (k>=0 && new_state[i + k*width] == false) k--;
        hk[i] = k+1;
        if (i>=1) deltaHk[i-1] = abs(hk[i-1]-hk[i]);
        if (hk[i]>maxH) maxH = hk[i];
        while (k>=0) {
            if (new_state[i+k*width] == false) l++;
            k--;
        }
    }
    double val = coef[2*width-1]*maxH + coef[2*width]*l;
    if (detail != NULL) {
        detail[2*width-1] = maxH;
        detail[2*width] = l;
    }
    for (int i=0; i<width; i++) {
        val += coef[i]*hk[i];
        if (i<width-1) val += coef[i+width]*deltaHk[i];
        if (detail != NULL) {
            detail[i] = hk[i];
            if (i<width-1) detail[i+width] = deltaHk[i];
        }
    }
    if (print) {
        printf("hk : ");
        for (int i=0; i<width; i++) printf("%d, ", hk[i]);
        printf("\nDeltaHK : ");
        for (int i=0; i<width-1; i++) printf("%d, ", deltaHk[i]);
        printf("\nmaxH : %d\nL : %d\n", maxH, l);
        printf("val : %f\n", val);
    }
    free(hk);
    free(deltaHk);
    free(new_state);
    return val;
}

float mon_heuristique(bool* state, int* piece_pos, int width, int height, bool print, double* coef, double* detail) {

    bool* new_state = (bool*) malloc(sizeof(bool)*width*height);
    for (int i=0; i<width*height; i++) {
        new_state[i] = state[i];
    }

    int max_height_piece = 0;
    for (int i=0; i<4; i++) {
        int h = ((int) piece_pos[i]/width) + 1;
        if (h > max_height_piece) max_height_piece = h;
    }

    int nb_ligne = 0;
    int nb_case_sup_in_last_piece = 0;
    for (int i=0; i<height; i++) {
        bool complete_line = true;
        for (int j=0; j<width; j++) {
            int index = j+i*width;
            if (new_state[index] == false) complete_line = false;
        }
        if (complete_line) {
            for (int j=0; j<4; j++) {
                int h = (int) piece_pos[j]/width;
                if (h == i) nb_case_sup_in_last_piece++;
            }
            nb_ligne++;
        }
    }
    int erosion = nb_ligne * nb_case_sup_in_last_piece;

    update(new_state, width, height);

    float moy_height = 0;
    float moy_col_diff = 0;
    int last_col = -1;
    for (int i=0; i<width; i++) {
        bool nex_col = false;
        int j = height-1;
        while (nex_col == false) {
            int ind = i+j*width;
            if (new_state[ind]) {
                moy_height += j+1;
                nex_col = true;
                if (last_col != -1) {
                    float v = mabs(last_col-(j+1));
                    moy_col_diff += v;
                }
                last_col = j+1;
            } else if (j==0) {
                nex_col = true;
                if (last_col != -1) {
                    float v = last_col;
                    moy_col_diff += v;
                }
                last_col = 0;
            }
            j--;
        }
    }
    moy_height /= (float) width;
    moy_col_diff /= (float) (width-1);

    int transition_ligne = 0;
    int transition_col = 0;
    for (int i=0; i<width; i++) {
        for (int j=0; j<height; j++) {
            if (i<width-1 && new_state[i+j*width] ^ new_state[(i+1)+j*width]) transition_ligne++;
            if (j<height-1 && new_state[i+j*width] ^ new_state[i+(j+1)*width]) transition_col++;
        }
    }

    int trous = 0;
    for (int i=0; i<width; i++) {
        bool recouver = false;
        for (int j=height-1; j>=0; j--) {
            if (recouver && new_state[i+j*width] == false) trous++;
            if (new_state[i+j*width]) recouver = true;
        }
    }

    int puits = 0;
    for (int i=0; i<width; i++) {
        int h = 0;
        for (int j=height-1; j>=0; j--) {
            if (new_state[i+j*width]==false && (i==0 || new_state[i-1+j*width]) && (i==width-1 || new_state[i+1+j*width])) {
                h++;
                puits += h;
            }
            if (new_state[i+j*width]) j=0;
        }
    }

    int taille_trous = 0;
    for (int i=0; i<width; i++) {
        int h = 0;
        bool debut = false;
        for (int j=height-1; j>=0; j--) {
            if (new_state[i+j*width]) {
                debut = true;
                if (h!=0) taille_trous += h-1;
                h=0;
            } else {
                if (debut) h++;
            }
        }
        if (debut && h!=0) taille_trous += h-1;
    }

    free(new_state);

    if (print) {
        printf("moy_height : %f\nmoy_col_diff : %f\ntrous : %d\nmax_height_piece : %d\nerosion : %d\ntransition_ligne : %d\ntransition_col : %d\npuits : %d\ntaille_trous : %d\n", moy_height, moy_col_diff, trous, max_height_piece, erosion, transition_ligne, transition_col, puits, taille_trous);
    }

    if (detail != NULL) {
        detail[0] = moy_height;
        detail[1] = moy_col_diff;
        detail[2] = trous;
        detail[3] = max_height_piece;
        detail[4] = erosion;
        detail[5] = transition_ligne;
        detail[6] = transition_col;
        detail[7] = puits;
        detail[8] = taille_trous;
    }

    return coef[0]*moy_height+coef[1]*moy_col_diff+coef[2]*trous+coef[3]*max_height_piece+coef[4]*erosion+coef[5]*transition_ligne+coef[6]*transition_col+coef[7]*puits+coef[8]*taille_trous;
}

float get_val_detail(bool* state, int* piece_pos, int width, int height, bool print, double* coef, double* detail, float (*fnc)(bool*,int*,int,int,bool,double*,double*)) {
    return fnc(state, piece_pos, width, height, print, coef, detail);
}

float get_val_2(bool* state, int* piece_pos, int width, int height, bool print, double* coef, float (*fnc)(bool*,int*,int,int,bool,double*,double*)) {
    return get_val_detail(state, piece_pos, width, height, print, coef, NULL, fnc);
}

void print_game(bool* state, int width, int height) {
    for (int i=0; i<width; i++) printf("-");
    printf("\n");
    for (int i=0; i<height; i++) {
        int ii = height-i-1;
        for (int j=0; j<width; j++) {
            printf("%c", (state[j+ii*width] == false)?' ':'X');
        }
        printf("\n");
    }
    for (int i=0; i<width; i++) printf("-");
    printf("\n");
}

int get_best_action(bool* state, int piece, int width, int height, int nb_action, double* coef, float (*fnc)(bool*,int*,int,int,bool,double*,double*)) {
    float max_val = 0;
    int max_i = -1;
    for (int i=0; i<nb_action; i++) {

        bool* new_state = (bool*) malloc(sizeof(bool)*width*height);
        for (int i=0; i<width*height; i++) {
            new_state[i] = state[i];
        }
        
        int* piece_pos = (int*) malloc(sizeof(int)*4);
        for (int j=0; j<4; j++) {
            piece_pos[j] = -1;
        }
        bool valide_action = joue_get_piece_pos(new_state, i, piece, width, height, piece_pos);

        if (valide_action) {
            float val = get_val_2(new_state, piece_pos, width, height, false, coef, fnc) + reward_inst(new_state, width, height);
            if (max_i == -1 || val > max_val) {
                max_val = val;
                max_i = i;
            }
        }
        free(piece_pos);
        free(new_state);
    }

    return max_i;
}

struct param_s {
    int nb_test;
    int height;
    int width;
    int nb_piece;
    int nb_action;
    float* score_moy;
    double* coef;
    float (*fnc)(bool*,int*,int,int,bool,double*,double*);
};
typedef struct param_s param_t;

pthread_mutex_t mutex;

void *compute(void* argv) {
    pthread_mutex_lock(&mutex);
    param_t* arg = (param_t*) argv;
    int nb_test = arg->nb_test;
    int height = arg->height;
    int width = arg->width;
    int nb_piece = arg->nb_piece;
    int nb_action = arg->nb_action;
    float (*fnc)(bool*,int*,int,int,bool,double*,double*) = arg->fnc;
    int score_tot = 0;
    int min = -1;
    int max = -1;
    double* coef = arg->coef;
    pthread_mutex_unlock(&mutex);

    bool* state = (bool*) malloc(sizeof(bool)*height*width);

    for (int i=0; i<nb_test; i++) {
        for (int i=0; i<height*width; i++) {
            state[i] = false;
        }
        bool fini = false;
        int score = 0;
        while (fini == false) {
            int piece = rand()%nb_piece;
            int action = get_best_action(state, piece, width, height, nb_action, coef, fnc);
            fini = joue(state, action, piece, width, height) == false;
            score += reward_inst(state, width, height);
            update(state, width, height);
        }
        if (min == -1 || score < min) min = score;
        if (max == -1 || score > max) max = score;
        score_tot += score;
    }
    free(state);
    pthread_exit((void *)score_tot);
}

float test(int nb_test, double* coef, bool print, int width, int height, float (*fnc)(bool*,int*,int,int,bool,double*,double*)) {
    // int nb_test_th = nb_test + NB_THREAD-nb_test%NB_THREAD;
    // if (nb_test%NB_THREAD == 0) nb_test_th = nb_test;
    if (print) printf("nb_test : %d\n", nb_test);
    // param_t* arg = malloc(sizeof(param_t));
    // arg->nb_test = nb_test_th/NB_THREAD;
    // arg->score_moy = NULL;
    // arg->height = height;
    // arg->width = width;
    // arg->nb_action = arg->width*4;
    // arg->nb_piece = 7;
    // arg->coef = coef;
    // arg->fnc = fnc;

    param_t** args = (param_t**) malloc(sizeof(param_t*)*NB_THREAD);

    pthread_mutex_init(&mutex, NULL);

    pthread_t threads[NB_THREAD];
    int nbt = nb_test/NB_THREAD;
    for (int i=0; i<NB_THREAD; i++) {
        args[i] = malloc(sizeof(param_t));
        if (i == NB_THREAD-1) args[i]->nb_test = nb_test - nbt*(NB_THREAD-1);
        else args[i]->nb_test = nbt;
        args[i]->score_moy = NULL;
        args[i]->height = height;
        args[i]->width = width;
        args[i]->nb_action = args[i]->width*4;
        args[i]->nb_piece = 7;
        args[i]->coef = coef;
        args[i]->fnc = fnc;
        int rc = pthread_create(&threads[i], NULL, compute, (void *)args[i]);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    int score_tot = 0;
    for (int i=0; i<NB_THREAD; i++) {
        int* score = 0;
        pthread_join(threads[i], (void*)&score);
        score_tot += (int)score;
    }

    for (int i=0; i<NB_THREAD; i++) free(args[i]);
    free(args);

    float score_moy = ((float) score_tot) / ((float) (nb_test));
    if (print) printf("score moyen : %f\n", score_moy);
    return score_moy;
}

struct file {
    bool* state;
    int* piece_pos;
    struct file* next;
};
typedef struct file file;

void simul_partie(double* r, int*** pieces_pos, bool*** states, int* t, int width, int height, float (*fnc)(bool*,int*,int,int,bool,double*,double*)) {
    int nb_piece = 7;
    int nb_action = width*4;
    double* coef = r;

    bool* state = (bool*) malloc(sizeof(bool)*height*width);

    for (int i=0; i<height*width; i++) {
        state[i] = false;
    }

    file* tr = malloc(sizeof(file));
    tr->state = (bool*) malloc(sizeof(bool)*width*height);
    for (int i=0; i<width*height; i++) {
        tr->state[i] = state[i];
    }
    tr->piece_pos = (int*) malloc(sizeof(int)*4);
    for (int j=0; j<4; j++) {
        tr->piece_pos[j] = 0;
    }

    int taille = 1;

    bool fini = false;
    while (fini == false) {
        int piece = rand()%nb_piece;
        int action = get_best_action(state, piece, width, height, nb_action, coef, fnc);

        bool* new_state = (bool*) malloc(sizeof(bool)*width*height);
        for (int i=0; i<width*height; i++) {
            new_state[i] = state[i];
        }
        int* piece_pos = (int*) malloc(sizeof(int)*4);
        for (int j=0; j<4; j++) {
            piece_pos[j] = -1;
        }
        joue_get_piece_pos(new_state, action, piece, width, height, piece_pos);
        file* ntr = malloc(sizeof(file));
        ntr->next = tr;
        ntr->piece_pos = piece_pos;
        ntr->state = new_state;

        tr = ntr;

        fini = joue(state, action, piece, width, height) == false;
        update(state, width, height);
        taille++;
    }

    free(state);

    (*pieces_pos) = (int**) malloc(sizeof(int*)*taille);
    (*states) = (bool**) malloc(sizeof(bool*)*taille);
    (*t) = taille;
    for (int i=taille-1; i>=0; i--) {
        (*states)[i] = tr->state;
        (*pieces_pos)[i] = tr->piece_pos;
        file* tmp = tr->next;
        free(tr);
        tr = tmp;
    }
}