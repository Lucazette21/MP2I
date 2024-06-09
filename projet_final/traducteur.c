#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "lexer.h" 




const char* boolean[] = {"true", "false"};
const int len_boolean = 2;
const char* important[] = {"main"};
const int len_important = 1;


// int nombre_tabulations = 0;  // Définition du nombre de tabulations à effectuer quand on reprend un ligne,
                             // Variable définie globalement car indispensable dans le code


bool is_function(maillon* debut){ // Teste si c'est une fonction à venir
    assert(debut->lexeme == 'V');
    if((debut->suivant->argument[0] == '(')||
       ((debut->suivant->suivant != NULL)&&(debut->suivant->argument[0] == ' ')&&(debut->suivant->suivant->argument[0] == ')')))
    {
        return true;
    }
    return false;
}

maillon* traitement_int(FILE* file, maillon* debut){
    assert(debut->lexeme == '0');
    maillon* maillon_actuel = debut;
    while(maillon_actuel->lexeme == '0'){
        fprintf( file, " %s", maillon_actuel->argument);
        maillon_actuel = maillon_actuel->suivant;
    }
    return maillon_actuel;
}

maillon* traitement_ops(FILE* file, maillon* debut){ // Traitement des opérateurs simples
    assert(debut-> lexeme == 'O');
    if (debut->argument[0] == '%')
    {
        fprintf( file, " mod"); // On suppose qu'il n'y a pas d'espace entre les opérateurs simples et ce sur quoi ça agit, comme dans l'exemple
        
    }
    else
    {
        fprintf(file, " %s", debut->argument);
    }
    return debut->suivant;
}

maillon* traitement_opd(FILE* file, maillon* debut){ // Traitement des opérateurs simples
    assert(debut->lexeme == 'B');
    int taille = strlen(debut->argument);
    if ((debut->argument[0] == '!')&&(taille == 1))
    {
        fprintf( file, " not"); 
        
    }
    else if ((debut->argument[0] == '!')&&(taille == 2)&&(debut->argument[1] == '='))
    {
        fprintf( file, " <>"); 
        
    }
    else if ((debut->argument[0] == '=')&&(taille == 2)&&(debut->argument[1] == '='))
    {
        fprintf( file, " ="); 
        
    }
    else
    {
        fprintf(file, " %s", debut->argument);
    }
    return debut->suivant;
}

maillon* traitement_opc(FILE* file, maillon* debut){ // Traitement des commentaires
    assert(debut->lexeme == 'C');
    fprintf( file, "(*%s*)", debut->argument);
    printf("%c", debut->lexeme);
    return debut->suivant;
}


maillon* traitement_ponctuation_variable(FILE* file, maillon* debut){
    assert(debut->lexeme == 'P');
    if (debut->argument[0] == '\n')
    {
        fprintf( file, "\n");
    }
    else if ((debut->argument[0] == '(')||(debut->argument[0] == ')'))
    {
        fprintf( file, " %s", debut->argument);
    }
    return debut->suivant;
}

maillon* traitement_fonction_entree(FILE* file, maillon* debut){ // Traite les arguments de la fonction quand elle est appelée
    maillon* maillon_actuel = debut;
    while (maillon_actuel->argument[0] != '(')
    {
        maillon_actuel = maillon_actuel->suivant;
    }
    maillon_actuel = maillon_actuel->suivant;
    while (maillon_actuel->argument[0] != ')')
        {
            if ((maillon_actuel->lexeme == '0')||(string_in (maillon_actuel->argument, boolean, len_boolean)))
            {
                fprintf(file," %s", maillon_actuel->argument);
            }
            else if (maillon_actuel->lexeme == 'V')
            {
                fprintf(file," !%s", maillon_actuel->argument);
            }
            maillon_actuel = maillon_actuel->suivant;
        }
    while (maillon_actuel->argument[0] != ';')
    {
        maillon_actuel = maillon_actuel->suivant;
    }
    return maillon_actuel->suivant;
}

maillon* traitement_lettres(FILE* file, maillon* debut){ // Traitement d'un enchaînement de lettres
    assert(debut->lexeme == 'V');
    maillon* maillon_actuel = debut;
    if ( string_in (maillon_actuel->argument, boolean, len_boolean)) // C'est un type "bool"
    {
        fprintf( file, " %s", maillon_actuel->argument);
    }
    else if (is_function(maillon_actuel))
    {
    {
        fprintf(file, "(%s", maillon_actuel->argument);
        maillon_actuel = traitement_fonction_entree(file, maillon_actuel);
        fprintf(file, ")");
        
    }
    }
    else // C'est un type "int"
    {
        fprintf( file, " !%s", maillon_actuel->argument);
    }
    return maillon_actuel->suivant;
    
}


maillon* traitement_contenu_variable(FILE* file, maillon* debut){ // Applicable aussi aux fonctions et aux conditions
    maillon* maillon_actuel = debut;
    if (maillon_actuel->lexeme == 'O')
    {
        maillon_actuel = traitement_ops(file , maillon_actuel);
    }
    else if (maillon_actuel->lexeme == 'B')
    {
        maillon_actuel = traitement_opd(file , maillon_actuel);
    }
    else if (maillon_actuel->lexeme == 'C')
    {
        maillon_actuel = traitement_opc(file , maillon_actuel);
    }
    else if (maillon_actuel->lexeme == 'P')
    {
        maillon_actuel = traitement_ponctuation_variable(file , maillon_actuel);
    }
    else if (maillon_actuel->lexeme == 'V')
    {
        maillon_actuel = traitement_lettres(file , maillon_actuel);
    }
    else if (maillon_actuel->lexeme == '0')
    {
        maillon_actuel = traitement_int(file , maillon_actuel);
    }
    return maillon_actuel;
    
}



maillon* traitement_V(FILE* file, maillon* debut){ //traitement en début de ligne du lexème 'V'
    assert(debut->lexeme == 'V');
    assert(debut->suivant != NULL);
    maillon* maillon_actuel = debut;
    if (is_function(maillon_actuel))
    { // Dans ce cas, c'est un appel de fonction
        fprintf(file, "(%s", maillon_actuel->argument);
        maillon_actuel = traitement_fonction_entree(file, maillon_actuel);
        fprintf(file, ");");

    }
    else if (strcmp("return", maillon_actuel-> argument) == 0)
    {
        maillon_actuel = maillon_actuel->suivant->suivant;
        fprintf(file, "(");
        while (maillon_actuel->argument[0] != ';')
        {
            maillon_actuel = traitement_contenu_variable(file, maillon_actuel);
        }
        fprintf(file, ");");
        maillon_actuel = maillon_actuel->suivant;
        
    }
    else
    { // Dans ce cas, c'est forcément une variable à modifier
        fprintf( file, "%s :=", maillon_actuel->argument);
    while(maillon_actuel->argument[0] != '=')
    {
        maillon_actuel = maillon_actuel->suivant;
    }
    maillon_actuel = maillon_actuel->suivant;
    while(maillon_actuel->argument[0] != ';')
    {
        printf("%c", maillon_actuel->lexeme);
        maillon_actuel = traitement_contenu_variable(file , maillon_actuel);
    }
    fprintf( file, ";"); // Problème dans un cas où il n'y a rien après le "in"
    return maillon_actuel->suivant;
    }
    
    return maillon_actuel;
}

maillon* traitement_variable_creation(FILE* file, maillon* debut){
    maillon* maillon_actuel = debut;
    maillon_actuel = maillon_actuel->suivant->suivant;
    fprintf( file, "let %s = ref", maillon_actuel->argument);
    while(maillon_actuel->argument[0] != '=')
    {
        maillon_actuel = maillon_actuel->suivant;
    }
    maillon_actuel = maillon_actuel->suivant;
    while(maillon_actuel->argument[0] != ';')
    {
        printf("%c", maillon_actuel->lexeme);
        maillon_actuel = traitement_contenu_variable(file , maillon_actuel);
    }
    fprintf( file, ";;");
    return maillon_actuel->suivant;
}





maillon* traitement_debut_ligne(FILE* file, maillon* debut, int tabulation){
    assert(debut->lexeme != 'M');
    for(int i = 0; i < 3*tabulation; i++){
        printf(" ");
    }
    maillon* maillon_actuel = debut;
    printf("%s", maillon_actuel->argument);
    if (maillon_actuel->lexeme == 'T')
    {
        maillon_actuel = traitement_variable_creation(file , maillon_actuel);


    }

    else if (maillon_actuel->argument[0] == ' ')
    {
        while((maillon_actuel != NULL)&&(maillon_actuel->argument[0] == ' '))
        {
            maillon_actuel = maillon_actuel->suivant; // Pour passer les tabulations qui pourraient même parfois être étranges
        }
        
    }
    else if (maillon_actuel->argument[0] == '\n')
    {
        fprintf(file, "\n");
        maillon_actuel = maillon_actuel->suivant;
    }
    else if (maillon_actuel->lexeme == 'V')
    {
        maillon_actuel = traitement_V(file, maillon_actuel);
    }
    
    else if (maillon_actuel->lexeme == 'M')
    {
        
    }

    else if (maillon_actuel->lexeme == 'C')
    {
        maillon_actuel = traitement_opc(file , maillon_actuel);
    }
    else if (maillon_actuel->argument[0] == '}')
    {
        maillon_actuel = maillon_actuel->suivant;
    }
    return maillon_actuel;
}

maillon* traitement_motcle(FILE* file, maillon* debut){
    maillon* maillon_actuel = debut;
    while(maillon_actuel->argument[0] != '{')
    {
        maillon_actuel = maillon_actuel->suivant;
    }
    if (file == NULL){ // A ENLEVER
        return debut;
    }
    return maillon_actuel->suivant;
}

maillon* traitement_fonction_creation(FILE* file, maillon* debut){
    assert(debut->lexeme == 'T');
    maillon* maillon_actuel = debut->suivant->suivant;
    printf("%c", maillon_actuel->lexeme);
    fprintf(file, "let %s", maillon_actuel->argument);
    maillon_actuel = maillon_actuel->suivant;
    while (maillon_actuel->argument[0] != '{')
        {
            if (maillon_actuel->lexeme == 'V')
            {
                printf("%s", maillon_actuel->argument);
                fprintf(file," %s", maillon_actuel->argument);
            }
            maillon_actuel = maillon_actuel->suivant;
        }
    fprintf(file, " =\n");
    maillon_actuel = maillon_actuel->suivant;
    while(maillon_actuel->argument[0] != '}'){
        maillon_actuel = traitement_debut_ligne(file, maillon_actuel, 1);
    }
    fprintf(file, ";");


    
    return maillon_actuel->suivant;
}
    
maillon* traitement_avant_main(FILE* file, maillon* debut){
    maillon* maillon_actuel = debut;
    if ((maillon_actuel->lexeme == 'H'))
    {
        while ((maillon_actuel != NULL)&&(maillon_actuel->argument[0] != '\n'))
        {
            maillon_actuel = maillon_actuel->suivant;
        }
        if (maillon_actuel != NULL)
        {
            maillon_actuel = maillon_actuel->suivant;
        }
    }
    else if (maillon_actuel->lexeme == 'C')
    {
        maillon_actuel = traitement_opc(file , maillon_actuel);
    }
    else if (maillon_actuel->argument[0] == ' ')
    {
        //maillon_actuel = traitement_fonction(file , maillon_actuel);
        maillon_actuel = maillon_actuel->suivant; // TEMPORAIRE
    }
    else if(maillon_actuel->lexeme == 'T'){
        printf("bizarre");
        if (!string_in(maillon_actuel->argument, important, len_important)){
            printf("bizarre");
            maillon_actuel = traitement_fonction_creation(file, maillon_actuel);
        }
    }
    else if (maillon_actuel->argument[0] == '\n')
    {
        fprintf(file, "\n");
        maillon_actuel = maillon_actuel->suivant;
    }

    return maillon_actuel;

    
}

void traduction(FILE* file, maillon* debut){
    maillon* maillon_actuel = debut;
    while ( (maillon_actuel != NULL)&&(!string_in(maillon_actuel->suivant->suivant->argument, important, len_important)))
    {
        maillon_actuel = traitement_avant_main(file, maillon_actuel);
    }
    
    while( (maillon_actuel != NULL)&&(maillon_actuel->argument[0] != '{'))
    {
        maillon_actuel = maillon_actuel->suivant;
    }
    if (maillon_actuel != NULL)
    {
        maillon_actuel = maillon_actuel->suivant;
    }
    

    while(maillon_actuel != NULL)
    {
        maillon_actuel = traitement_debut_ligne(file , maillon_actuel, 0);
    }
    
}


int main(){
    // Ouvrir le fichier de sortie
    FILE* cible = fopen("fichier_ex.ml", "w");
    if (cible == NULL) {
        perror("Erreur lors de l'ouverture du fichier de sortie truc.ml");
        fclose(cible);
        return EXIT_FAILURE;
    }

    // Ouvrir le fichier d'entrée
    FILE* test = fopen("fichier_ex.c", "r");
    if (test == NULL) {
        perror("Erreur lors de l'ouverture du fichier d'entrée test.c");
        fclose(test);
        return EXIT_FAILURE;
    }

    // Appeler la fonction lexeur
    maillon* lex = lexeur(test);
    if (lex == NULL) {
        fprintf(stderr, "Erreur: lexeur a retourné NULL\n");
        fclose(cible);
        fclose(test);
        return EXIT_FAILURE;
    }
    lex = lex->suivant; // On passe le DEBUT
    
    traduction(cible, lex);


    fclose(cible);
    fclose(test);
}

