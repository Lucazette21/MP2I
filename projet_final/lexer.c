#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"

// La définition du type maillon a été déplacée dans le fichier lexer.h

void ajoute_maillon_fin (maillon** pfin, char lex, char* arg){
    // Modifie la valeur pointé par pfin et ajoute un maillon
    maillon* nv_maillon = malloc(sizeof(maillon));
    nv_maillon->lexeme = lex; //Nouveau lexème
    nv_maillon->argument = arg; //Nouvel argument
    nv_maillon->suivant = NULL;
    (*pfin)->suivant = nv_maillon;
    *pfin = nv_maillon;
}

void affiche_liste (maillon* debut){
    //Fonction de déboggage : affiche les lexèmes et leurs arguments
    while (debut != NULL){
        printf("%c %s\n", debut->lexeme, debut->argument); //Attention au type de l'argument
        debut = debut-> suivant;
    }
}

void libere_liste (maillon* debut){
    //Libère tous les maillons
    if (debut != NULL){
        libere_liste(debut->suivant);
        // Le "hack" suivant ne marche que si l'argument est allloué dans le tas (avec malloc) :
        if (debut->argument != NULL){
            free(debut->argument);
        }
        free(debut);
    }
}

// Ici on définit des lexèmes qui nous intéressent. Si vous en rajoutez, pensez à changer la taille des tableaux.

const char ponctuation[] = {'(', ')', '{', '}', ' ', '\n', ';', ','};
const int len_ponctuation = 8;
const char* type[] = {"bool", "int", "void"};
const int len_type = 3; 
const char* motcle[] = {"while", "if"};
const int len_motcle = 2;
const char operateurs_simples[] = {'+', '-', '/', '*', '%'};
const int len_ops = 5;
const char operateurs_commentaires[] = {'/','*'}; // Rajout des commentaires
const int len_opc = 2;
const char operateurs_doubles[] = {'=', '|', '&','<','>','!'};
const int len_opd = 6;


bool char_in ( char c, const char tab[], const int len){
    //Teste si le caractère est dans le tableau (il nous faut sa longueur)
    for(int i = 0; i<len; i++){
        if (tab[i] == c){
            return true;
        }
    }
    return false;
}

bool string_in ( char* c, const char* tab[], const int len){
    //teste si la chaine est dans le tableau (il nous faut sa longueur)
    for(int i = 0; i<len; i++){
        if (!strcmp (c,tab[i])){
            return true;
        }
    }
    return false;
}    

char* cree_arg( char* buffer, const int len) {
    //transforme le buffer en une chaine de charactère correcte (avec un marqueur de fin). Il nous faut le nombre de cases occupées
    char* arg = malloc( (len+1)*sizeof(char));
    for ( int i=0; i<len; i++) {
        arg[i] = buffer[i];
    }
    arg[len]='\0';
    return arg;
}

maillon* lexeur (FILE* fichier){
    maillon* debut = malloc(sizeof(maillon)); //Premier maillon
    maillon* fin = debut; //Dernier maillon
    debut->argument = malloc (6*sizeof(char));
    strcpy (debut->argument, "DEBUT"); //On met un argument spécial au début
    debut->lexeme = 'D'; 
    debut->suivant = NULL;
    
    // Instruction pour récupérer les caractères, un par un. c vaut EOF lorsque le fichier est fini.
    char c = fgetc(fichier);
    // Le buffer sert à stocker des caractères pour créer les arguments.
    char buffer[100];
    int len_buffer;
    
    while (c != EOF){
        len_buffer=0;
        //Cas 1 : on récupère un entier, il faut le lire en totalité
        if ( c>= '0' && c <= '9'){
            while ( c>= '0' && c <= '9'){
            buffer[len_buffer] = c;
            len_buffer++;
            c = fgetc(fichier);
            }
            ajoute_maillon_fin (&fin, '0', cree_arg (buffer, len_buffer));
            // '0' pour indiquer un entier
        }
        // Cas 2 : on récupère de la ponctuation
        else if ( char_in (c, ponctuation, len_ponctuation)){
            char* chaine= cree_arg(&c, 1);
            ajoute_maillon_fin (&fin,  'P' , chaine);
            // 'P' pour indiquer de la ponctuation
            c = fgetc(fichier);
        }
        // Cas 3 : on récupère un opérateur qui n'apparait que seul (et les commentaires)

        else if ( char_in (c, operateurs_simples, len_ops)){
            if ( c == '/') // Cas X : on récupère les commentaires
            { 
                char d = c;
                c = fgetc(fichier);
                if ( char_in (c, operateurs_commentaires, len_opc))
                {
                    if (c == '/') // ici, on a l'enchaînement '//'
                    {
                        c = fgetc(fichier);
                        len_buffer = 0;
                        while( (c != '\n')&&(c != EOF)){
                            buffer[len_buffer] = c;
                            c = fgetc(fichier);
                            len_buffer++;
                        }
                        ajoute_maillon_fin (&fin, 'C', cree_arg(buffer, len_buffer));

                    }
                    else // dans ce cas, on a l'enchaînement '/*'
                    {
                        d = fgetc(fichier);
                        c = fgetc(fichier);
                        len_buffer = 0;
                        while( (d != '*')||(c != '/')){
                            buffer[len_buffer] = d;
                            len_buffer++;
                            d = c;
                            c = fgetc(fichier);
                        }
                        ajoute_maillon_fin (&fin, 'C', cree_arg(buffer, len_buffer));
                        c = fgetc(fichier);
                    }
                    
                    
                }
                else
                {
                   char* chaine= cree_arg(&d, 1);
                    ajoute_maillon_fin (&fin, 'O' , chaine);
                    // 'O' pour indiquer un opérateur
                }
            }
            else
            {
                char* chaine= cree_arg(&c, 1);
                ajoute_maillon_fin (&fin, 'O' , chaine);
                // 'O' pour indiquer un opérateur
                c = fgetc(fichier);
            }
            
            
            
    
            
            /*
            if (char_in (c, operateurs_commentaires,len_opm)) { // Cas C : pour les commentaires
                char d = c;
                c = fgetc(fichier);
                if (d == '*'){  // FONCTION A RACCOURCIR EN REUNISSANT DES CONDITIONS
                    if (c == '/'){
                        buffer[0]=d;
                        buffer[1]=c;
                        len_buffer = 2;
                        ajoute_maillon_fin (&fin, 'C', cree_arg(buffer, len_buffer));
                        c = fgetc(fichier);
                    }
                    else{
                        char* chaine= cree_arg(&d, 1);
                        ajoute_maillon_fin (&fin, 'O' , chaine);
                        // 'O' pour indiquer un opérateur
                    }
                }
                else if (d == '/'){
                    if ( char_in (c, operateurs_commentaires,len_opm)){
                        buffer[0]=d;
                        buffer[1]=c;
                        len_buffer = 2;
                        ajoute_maillon_fin (&fin, 'C', cree_arg(buffer, len_buffer));
                        c = fgetc(fichier);
                    }
                    else{
                        char* chaine= cree_arg(&d, 1);
                        ajoute_maillon_fin (&fin, 'd' , chaine);
                        // 'O' pour indiquer un opérateur
                    }
                }
                else{
                    char* chaine= cree_arg(&d, 1);
                    ajoute_maillon_fin (&fin, 'O' , chaine);
                    // 'O' pour indiquer un opérateur
                    c = fgetc(fichier);
                }

            }
            else{
                char* chaine= cree_arg(&c, 1);
                ajoute_maillon_fin (&fin, 'O' , chaine);
                // 'O' pour indiquer un opérateur
                c = fgetc(fichier);
            }
            */


        }
        // Cas 4 : on récupère un opérateur qui peut apparaitre seul et/ou à côté d'un autre
        else if ( char_in (c, operateurs_doubles, len_opd)){
            char d = c;
            c = fgetc(fichier);
            if(char_in (c, operateurs_doubles, len_opd)){
                buffer[0]=d;
                buffer[1]=c;
                len_buffer = 2;
                ajoute_maillon_fin (&fin, 'B', cree_arg(buffer, len_buffer));
                // 'B' pour indiquer un opérateur binaire
                c = fgetc(fichier);
            }
            else if (d == '='){
                char* chaine= cree_arg(&d, 1);
                ajoute_maillon_fin (&fin, 'E', chaine);
                // On sépare l'opérateur d'affectation des autres. 'E' indique l'affectation
            }
            else {
                char* chaine= cree_arg(&d, 1);
                ajoute_maillon_fin (&fin, 'B', chaine);
            }
        }
        // Cas 5 : On est face à une chaine de caractère, on la lit en entier.
        else if (c == '"') {
            buffer[len_buffer] = c;
            len_buffer++;
            c = fgetc(fichier);
            while (c!='"'){
                buffer[len_buffer] = c;
                len_buffer++;
                c = fgetc(fichier);
            }
            buffer[len_buffer] = c;
            len_buffer++;
            c = fgetc(fichier);
            ajoute_maillon_fin (&fin, 'S', cree_arg(buffer, len_buffer));
            // 'S' pour indiquer une chaine de caractère
        }
        //Cas 6 : on est face à quelque chose qui s'écrit avec des lettres : un mot-clé, une constante, une variable, ...
        else if ((c >= 'a' && c<='z') || (c >= 'A' && c<='Z')){
            while ((c >= 'a' && c<='z') || (c >= 'A' && c<='Z')){
                buffer[len_buffer] = c;
                len_buffer++;
                c = fgetc(fichier);
            }
            char* chaine = cree_arg(buffer, len_buffer);
            if (string_in(chaine, type, len_type)){
                ajoute_maillon_fin (&fin, 'T', chaine);
                // 'T' pour indiquer un type
            }
            else if (string_in(chaine, motcle, len_motcle)){
                ajoute_maillon_fin (&fin, 'M', chaine);
                // 'M' pour indiquer un mot clé
            }
            else {ajoute_maillon_fin (&fin, 'V', chaine);}
            // 'V' pour indiquer une variable
        }
        // Cas 7 : On est face à un dièse
        else if(c == '#'){
            char* chaine= cree_arg(&c, 1);
            ajoute_maillon_fin (&fin, 'H', chaine);
            c = fgetc(fichier);
        }
        else if (c == '.'){ // Cas 8 ; passer les "." qu'on trouve dans les "include"
            c = fgetc(fichier);
        }

        //Dernier cas : on est face à quelque chose d'anormal
        else{
            fprintf(stderr, "Le charactère %c de numéro %d n'a pas été reconnu.", c, (int) c);
            exit(1);
        }
    }
    return debut;
}