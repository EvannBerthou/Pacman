/******************************************************************************/
/* MAIN.c                                                                     */
/******************************************************************************/
#include "./main.h"
#include "../lib/libgraphique.h"
#include "timer.h"
#include "leaderboard.h"

/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

SCENE scene_active = SCENE_ACCUEIL;

int main(int argc, char **argv) {
/* argc indique le nombre d'arguments,
   argv est un tableau de mots contenant ces arguments. */
    Partie  p;      // la structure contenant les données d'une partie

    /** Traitement des arguments **************************************************/
    if(argc!=2) { // Si le nombre d'arguments est différent de 2
        printf("Usage: %s fichier_plan\n", argv[0]); // mode d'emploi et sortie
        return 0;
    }
    /******************************************************************************/

    /* Chargement du plan à partir du fichier fourni en paramètre                 */
    printf("Chargement du plan...\n");
    p = charge_plan(argv[1]);
    /* Si problème lors du chargement du plan...                                  */
    if(p.plateau == NULL)
        return 1;
    if (charger_sprites() == -1)
        return 1;
    calculer_voisins(&p);

#ifdef DEBUG
    /* Affichage du plan lu                                                       */
    int i,j;
    printf("Affichage du plan...\n");
    for(i=0;i!=p.L;i++) {
        for(j=0;j!=p.C;j++)
            printf("%c",p.plateau[i][j]);
        printf("\n");
    }
    printf("Bonbons et bonus : %d\n", p.nbbonus);
#endif

/******************************************************************************/
/* A PARTIR D'ICI...C'EST A VOUS DE JOUER!                                    */
/******************************************************************************/

    // TODO: Devra être affiché sur l'écran d'accueil
    afficher_leaderboard();

    ouvrir_fenetre(600, 540);
    Timer timer = nouveau_timer();
    while(1) {
        tick_timer(&timer);
#ifdef DEBUG
        print_fps(&timer);
#endif
        traiter_evenements();
        //actualiser_partie(&p, &timer);
        //dessiner_partie(&p);
        actualiser_jeu(&p, &timer);
        dessiner_jeu(&p);
        reinitialiser_evenements();
    }
    return 0;
}

void actualiser_jeu(Partie *p, Timer *t) {
    switch(scene_active) {
    case SCENE_ACCUEIL:
        actualiser_accueil(p, t);
        break;
    case SCENE_NIVEAU:
        actualiser_partie(p, t);
        break;
    }
}

void dessiner_jeu(Partie *p) {
    switch(scene_active) {
    case SCENE_ACCUEIL:
        actualiser();
        break;
    case SCENE_NIVEAU:
        dessiner_partie(p);
        break;
    }
}

void actualiser_accueil(Partie *p, Timer *t) {
    if (touche_a_ete_pressee(SDLK_j)) {
        scene_active = SCENE_NIVEAU;
        actualiser_partie(p, t);
    }
}