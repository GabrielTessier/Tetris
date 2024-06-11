
#ifndef TETRIS_HEURISTIQUE_H
#define TETRIS_HEURISTIQUE_H

#define NB_THREAD 2

float test(int nb_test, double* coef, bool print, int width, int height, float (*fnc)(bool*,int*,int,int,bool,double*,double*));
float get_val_2(bool* state, int* piece_pos, int width, int height, bool print, double* coef, float (*fnc)(bool*,int*,int,int,bool,double*,double*));
int reward_inst(bool* state, int width, int height);

float dellacherie(bool* state, int* piece_pos, int width, int height, bool print, double* coef, double* detail);
float bertsekas_ioffe_modifier(bool* state, int* piece_pos, int width, int height, bool print, double* coef, double* detail);
float mon_heuristique(bool* state, int* piece_pos, int width, int height, bool print, double* coef, double* detail);

float get_val_detail(bool* state, int* piece_pos, int width, int height, bool print, double* coef, double* detail, float (*fnc)(bool*,int*,int,int,bool,double*,double*));
void simul_partie(double* r, int*** pieces_pos, bool*** states, int* t, int width, int height, float (*fnc)(bool*,int*,int,int,bool,double*,double*));
void print_game(bool* state, int width, int height);
void update(bool* state, int width, int height);

#endif