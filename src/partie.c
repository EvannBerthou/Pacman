/******************************************************************************/
/* CHARGEMENT.c                                                               */
/******************************************************************************/
#include "./partie.h"


/******************************************************************************/
/* CHARGE PLAN                                                                */
/******************************************************************************/
Partie charge_plan(char *fichier)
    {
    Partie  p;
    FILE    *f = fopen(fichier,"r"); // Ouverture en lecture du fichier
    p.plateau = NULL;   // Le futur plan, à allouer dynamiquement.
    int     res, l, c;
    int     nbf;        // Nb de fantômes trouvés sur le plan
    int     nbb;        // Nb de bonus trouvés sur le plan
    char    ch;

    if(!f) // Si f== NULL, ouverture ratée
        {
        printf("Impossible d'ouvrir '%s'\n",fichier);
        exit(0);
        }

/* Lecture des dimensions du plan en en-tête                                  */
    res = fscanf(f,"%d %d\n",&p.L,&p.C); // Lecture de deux entiers

// Si on n'a pas pu lire deux entiers ou s'ils sont incorrects
    if(res !=2 || p.C<2 || p.L<2 || p.C >800 || p.L>600)
        {
        printf("Dimensions du tableau lues dans '%s' incorrectes\n",fichier);
        fclose(f);
        exit(0);
        }
    printf("Dimensions lues: %d x %d\n",p.L,p.C);

/* ALLOCATION DYNAMIQUE                                                       */
/* Allocation du tableau de *L pointeurs sur lignes                           */
    p.plateau = (char **) malloc(p.L * sizeof(char *));
    if(p.plateau == NULL)
        {
        printf("Allocation dynamique impossible\n");
        fclose(f);
        exit(0);
        }

/* Allocation des tableaux de *C caractères                                   */
    for(l=0;l!=p.L;l++)
        {
        p.plateau[l] = (char *) malloc(p.C * sizeof(char));
        if(p.plateau[l] == NULL)
            {
            printf("Allocation dynamique impossible\n");
            fclose(f);
            exit(0);
            }
        }


/* LECTURE DES LIGNES DU PLAN                                                 */
    l = 0;
    res = 0;
    nbf = 0;
    nbb = 0;
    while(res != EOF) // Lecture de chaque ligne
        {
        c=0;
        while(1)
            {
            res = fscanf(f,"%c",&ch); // Lecture d'un caractère
            if (res == EOF) // Si fin de fichier
                break; // Quittons la boucle interne

            if(c>p.C) // Si trop de colonnes...
                {
                printf("Ligne %d colonne %d: trop de colonnes\n",l,c);
                fclose(f);
                exit(0);
                }

            if(c==p.C) // Si fin de ligne supposée...
                {
                if(ch=='\n') // Si fin de ligne réelle, on quitte la boucle
                    {
                    break;
                    }
                else // Sinon trop de caractères
                    {
                    printf("Ligne %d: trop de caractères\n",l);
                    fclose(f);
                    //exit(0);
                    }
                }
/* ...sinon, nous ne sommes pas à la fin de la ligne.                         */
/* Si on lit un caractère interdit...                                         */
            if(ch!='.' && ch!=' ' && ch!= '*' && ch!='P' && ch!='F' && ch!='B' && ch!='C')
                {
                if(ch=='\n') // Si c'est un saut de ligne
                    printf("Ligne %d: trop peu de caractères\n",l);
                else
                    printf("Ligne %d: caractère '%c' incorrect\n",l,ch);

                fclose(f);
                exit(0);
                }

            if(ch=='P')
                {
                p.pacman.l = l;
                p.pacman.c = c;
                }
            else if(ch=='F')
                {
                if(nbf>NBFANTOMES)
                    {
                    printf("Ligne %d:  un fantôme de trop!\n",l);
                    fclose(f);
                    exit(0);
                    }
                p.fantomes[nbf].l = l;
                p.fantomes[nbf].c = c;
                nbf++;
                }
            else if(ch=='B')
                nbb++;

            p.plateau[l][c] = ch; // Ecriture dans le plan

            c++; // caractère suivant
            }
        l++; // ligne suivante
        }

    fclose(f); // Fermeture du flux de lecture du fichier

/* Si à la lecture de EOF on n'est pas sur la *V+1 ème ligne...               */
    if(l != p.L+1)
        {
        printf("Ligne %d: nb de lignes incorrect\n",l);
        exit(0);
        }

/* Si nb de fantômes incorrect...                                             */
    if(nbf!=NBFANTOMES)
        {
        printf("Nb de fantômes incorrect sur le plan\n");
        fclose(f);
        exit(0);
        }

/* Si pas de bonus....                                                       */
    if(nbb==0)
        {
        printf("Aucun bonus sur le plan!\n");
        fclose(f);
        exit(0);
        }
    p.nbbonus = nbb;

    p.taille_case[0] = 420 / p.C;
    p.taille_case[1] = 540 / p.L;

    return p;
    }

int mouvement_clavier(int sens){
    
    if (touche_a_ete_pressee(SDLK_UP)){//fleche du haut pressé
        return 1;
    }    
    else if (touche_a_ete_pressee(SDLK_DOWN)){//fleche du bas pressé 
        return 2;
    }        
    else if (touche_a_ete_pressee(SDLK_LEFT)){//fleche de gauche pressé 
        return 3;
    }
    else if (touche_a_ete_pressee(SDLK_RIGHT)){//fleche de droite pressé 
        return 4;
    }
    else {
        return sens;
    }
}

Pos mod_pos_pacman(Pos pacman, char ** plateau){//modification de L et C de pacman en fonction de l'environemen et des touche préssées
    switch (pacman.element.sens){
        case 1:
            if (plateau[pacman.l-1][pacman.c]!='*'){ //check upper
                pacman.l-=1;
            }
            else{
                pacman.element.sens=0;
            }
            break;
        case 2:
            if (plateau[pacman.l+1][pacman.c]!='*'){ //check lower
                pacman.l+=1;
            }
            else{
                pacman.element.sens=0;
            }
            break;
        case 3:
            if (plateau[pacman.l][pacman.c-1]!='*'){//check left 
                pacman.c-=1;
            }
            else{
                pacman.element.sens=0;
            }
            break;
        case 4:
            if (plateau[pacman.l][pacman.c+1]!='*'){//check right 
                pacman.c+=1;
            }
            else{
                pacman.element.sens=0;
            }
             break;
    }
    return pacman;

}

char ** maj_plateau(Pos pacman, char** plateau,char caractere){
    plateau[pacman.l][pacman.c]=caractere;
    return plateau;
}



void actualiser_partie(Partie *p) {
    p->pacman.element.sens=mouvement_clavier(p->pacman.element.sens);
    p->plateau=maj_plateau(p->pacman,p->plateau,' ');
    p->pacman=mod_pos_pacman(p->pacman,p->plateau);
    p->plateau=maj_plateau(p->pacman,p->plateau,'P');
}

void dessiner_grille(Partie *p) {
    int cx = p->taille_case[0];
    int cy = p->taille_case[1];
    for (int i = 0; i < p->L; i++) {
        for (int j = 0; j < p->C; j++) {
            Point pos = {j * cx, i * cy};
            char type = p->plateau[i][j];
            // Mur
            if (type == '*')
                dessiner_rectangle(pos, cx, cy, rouge);
            // Pacman
            else if (type == 'P')
                dessiner_rectangle(pos, cx, cy, vert);
            // Bonbon
            else if (type == '.'|| type == ' ')
                dessiner_rectangle(pos, cx, cy, gris);
            // Bonus
            else if (type == 'B')
                dessiner_rectangle(pos, cx, cy, orange);
            // Fantôme
            else if (type == 'F')
                dessiner_rectangle(pos, cx, cy, bleu);
        }
    }
}

void dessiner_partie(Partie *p) {
    // TODO: Déterminer la position sur le plateau
    //Point pacman = {p->pacman.l, p->pacman.c};
    //char pacman sens N/W/S/





    //dessiner_cercle(pacman, 5, jaune);

    dessiner_grille(p);
    actualiser();
}
