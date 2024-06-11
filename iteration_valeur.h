
#ifndef TEST_H
#define TEST_H

struct vecteur_s {
    int size;
    float* data;
};
typedef struct vecteur_s vecteur;

struct vecteur_int_s {
    int size;
    int* data;
};
typedef struct vecteur_int_s vecteur_int;

struct matris_s {
    int size;
    vecteur** data;
};
typedef struct matris_s matris;

// Renvoie 2 puissance p
int pow2(int p);

// Renvoie les coordonées des cases d'une pièce en fonction de l'action effectuée
void make_action(int action, int piece_num, int* pos, int* piece);

// Renvoie la récompence associé à un état et une action
int reward(int real_state, int piece, int action, int width, int height);

// Renvoie le nombre de ligne pleine dans l'état real_state
int reward_inst(int real_state, int width, int height);

// Retire toute les lignes pleine d'une grille
int update(int real_state, int width, int height);

// Renvoie la valeur du bit en position index dans real_state
bool get_pos(int real_state, int index);

// Set la valeur du bit en position index dans real_state
int set_pos(int real_state, int index, int val);

// Renvoie le nouvelle état après avoir effectué l'action action dans l'état real_state
// Renvoie -1 si l'action n'est pas possible
int joue(int real_state, int action, int piece_num, int width, int height);

// Renvoie la probabilité de passée de l'état state à l'état next_state en jouant l'action action
//float proba_T(int state, int action, int next_state, int width, int height, int nb_piece);

// Print le jeu dans la sortie standard
void print_game(int real_state, int width, int height);

// Renvoie faux si l'action place la pièce en dehors de la grille
bool legal_action(int piece_num, int action, int width);

// Sauvegarde le model V dans le fichier file
void save_model(vecteur* v, char* file);

// Charge le model V depuis le fichier file
void load_model(vecteur* v, char* file);

// Sauvegarde la politique (l'action à effectuer pour chaque état possible) dans le fichier file
void save_model_i(vecteur_int* v, char* file);

// Charge la politique (l'action à effectuer pour chaque état possible) depuis le fichier file
void load_model_i(vecteur_int* v, char* file);

// Fait un parcours en profondeur sur l'ensemble des états depuis real_state
// Pour chaque état exécute une itération de l'algorithme d'itération de la valeur
float parcour(int real_state, vecteur* vi, vecteur* vim1, vecteur_int* pi, int nb_piece, int nb_action, float gamma, int width, int height, vecteur_int* vu);


#endif // TEST_H