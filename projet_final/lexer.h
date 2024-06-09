#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Vous regardez un fichier de header .h. Son but est de décrire les types et les fonctions utilisés dans le fichier de code .c pour pouvoir les utiliser dans d'autres fichiers.

struct maillon {
    char lexeme; // le nom du lexème
    char* argument;  // sa valeur, on prendra NULL s'il n'y a pas besoin d'argument
                    // pour l'instant ce sera un char* mais on pourra imaginer d'autres types en utilisant void* 
    struct maillon* suivant; // on chaine de maniere classique
};

typedef struct maillon maillon;

void ajoute_maillon_fin (maillon** pfin, char lex, char* arg);

void affiche_liste (maillon* debut);

void libere_liste (maillon* debut);;

bool char_in ( char c, const char tab[], const int len);

bool string_in ( char* c, const char* tab[], const int len);

char* cree_arg( char* buffer, const int len);

maillon* lexeur (FILE* fichier);
