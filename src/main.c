/******************************************************************************/
/* MAIN.c                                                                     */
/******************************************************************************/
#include <dirent.h>
#include "./main.h"
#include "../lib/libgraphique.h"
#include "timer.h"
#include "leaderboard.h"
#include "Bouton.h"
#include "entite.h"
#include "editeur.h"
#include "pacman.h"
#include "fantome.h"

/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

SCENE scene_active = SCENE_ACCUEIL;
#define NOMBRE_BOUTONS 4
BoutonAccueil boutons[NOMBRE_BOUTONS];
int bouton_selectionne = 0;
SDL_Joystick *manette = NULL;

static Uint8 *audio_pos; // avance de la lecture du fichier audio
static Uint32 audio_len; // taille restante à être lue

// Permet d'avancer la lecture du fichier audio
// Provient de la doc SDL1
void my_audio_callback(void *userdata, Uint8 *stream, int len) {
	if (audio_len == 0)
		return;

	len = ( len > audio_len ? audio_len : len );
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another

	audio_pos += len;
	audio_len -= len;
}

int main(int argc, char **argv) {
    ouvrir_fenetre(ECRAN_W, ECRAN_H);

    // Chargement de la manette si disponible
    int num_joy = SDL_NumJoysticks();

#ifdef DEBUG
    printf("%d manettes détectés\n", num_joy);
#endif

    if (num_joy > 0) {
        printf("Utilisation de la manette 0 (%s)\n", SDL_JoystickName(0));
        // Active la manette
        manette = SDL_JoystickOpen(0);
        // Active la mise à jour des evenements de la manette (sera appelé dans traiter_evenement
        // par la libgraphique)
        SDL_JoystickEventState(SDL_ENABLE);
    }


    // Le code lié à l'audio est directement pris depuis la doc de la SDL1
	static Uint32 wav_length; // taille du fichier audio
	static Uint8 *wav_buffer; // contenue du fichier audio
	static SDL_AudioSpec wav_spec; // Caractéristique du fichier audio

    // Charge le fichier musique
	if (SDL_LoadWAV("music.wav", &wav_spec, &wav_buffer, &wav_length) != NULL ){
        wav_spec.callback = my_audio_callback;
        wav_spec.userdata = NULL;
        audio_pos = wav_buffer;
        audio_len = wav_length;

        // Ouvre le périphérique audio
        if (SDL_OpenAudio(&wav_spec, NULL) < 0 ){
        #ifdef DEBUG
          fprintf(stderr, "Erreur lors de l'ouverture de l'audio: %s\n", SDL_GetError());
        #endif
        }
        else {
            SDL_PauseAudio(0);
        }
    }

    // Création des boutons
    boutons[0] = nouveau_bouton((Point){ECRAN_W / 2, 200}, blanc, "Jouer", 26);
    boutons[1] = nouveau_bouton((Point){ECRAN_W / 2, 250}, blanc, "Classement", 26);
    boutons[2] = nouveau_bouton((Point){ECRAN_W / 2, 300}, blanc, "Editeur", 26);
    boutons[3] = nouveau_bouton((Point){ECRAN_W / 2, 350}, blanc, "Quitter", 26);

    if (charger_sprites() == -1)
        return 1;

    Timer timer = nouveau_timer();
    Partie p;
    // Boucle principale
    // On peut faire une boucle infinie car lorsque le signal d'arrêt est envoyé, la libgraphique
    // termine directement le programme, toute la mémoire alloué est directement libérée par le système
    while(1) {
        tick_timer(&timer);
#ifdef DEBUG
        print_fps(&timer);
#endif
        traiter_evenements();
        actualiser_jeu(&p, &timer);
        dessiner_jeu(&p);
        reinitialiser_evenements();
    }
    return 0;
}

// Actualise la scene active
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

// Dessine la scene active
void dessiner_jeu(Partie *p) {
    switch(scene_active) {
    case SCENE_ACCUEIL:
        dessiner_accueil();
        break;
    case SCENE_NIVEAU:
        dessiner_partie(p);
        break;
    }
}

// Fonctions d'aide pour vérifier si une manette est branchée et qu'une touche est appuyée
static int touche_manete(int bouton) {
    if (manette == NULL) return 0;
    return SDL_JoystickGetButton(manette, bouton);
}

static Uint8 croix_manette() {
    if (manette == NULL) return 0;
    return SDL_JoystickGetHat(manette, 0);
}

int nouvelle_touche() {
    static int derniere_touche = 0;
    static int derniere_croix  = 0;

    int touches[] = {SDLK_DOWN, SDLK_UP, SDLK_LEFT, SDLK_RIGHT, SDLK_RETURN, SDLK_q};
    int croix[] = {SDL_HAT_DOWN, SDL_HAT_UP, SDL_HAT_LEFT, SDL_HAT_RIGHT};
    int boutons[] = {0,1};

    int touche = attendre_touche_duree(10);
    Uint8 croix_presse = croix_manette();

    // Vérifie les autres touches
    // On peut le mettre en dehors du prochain if car dès qu'un bouton est pressé, il y a
    // un changement de menu et donc la fonctin ne sera pas rappelée directement
    for (int i = 0; i < 2; i++) {
        if (touche == touches[4 + i] || touche_manete(boutons[i])) {
            return touches[4 + i];
        }
    }

    // Vérifie si la touche détecté était déjà appuyé lors du dernier appel, évite
    // la répétition de la touche (ou de la croix)
    if (touche == derniere_touche && croix_presse == derniere_croix) return 0;

    derniere_touche = touche;
    derniere_croix = croix_presse;

    // Vérifie les directions
    for (int i = 0; i < 4; i++) {
        if (touche == touches[i] || croix_presse & croix[i]) {
            return touches[i];
        }
    }
    // Aucune touche pressée
    return 0;
}

void actualiser_accueil(Partie *p, Timer *t) {
    int touche = nouvelle_touche();
    if (touche == SDLK_DOWN) {
        bouton_selectionne = (bouton_selectionne + 1) % NOMBRE_BOUTONS;
    }
    else if (touche == SDLK_UP) {
        bouton_selectionne--;
        if (bouton_selectionne < 0)
            bouton_selectionne = NOMBRE_BOUTONS - 1;
    }
    else if (touche == SDLK_RETURN) {
        activer_bouton(p, t);
    }
}

void activer_bouton(Partie *p, Timer *t) {
    // Animation de pacman qui mange le bouton avant de changer de menu
    manger_bouton();
    // Transition écran noir
    switch(bouton_selectionne) {
    case 0: {
        char *chemin = selectionner_niveau();
        if (chemin == NULL) {
            break;
        }
        // Charge le niveau
        if (charger_niveau(p, chemin)) {
            printf("Erreur lors du chargement du niveau\n");
            free(chemin);
            break;
        }
        scene_active = SCENE_NIVEAU;
        // Arrete la musique de l'accueil
        if (SDL_GetAudioStatus() == SDL_AUDIO_PLAYING)
            SDL_PauseAudio(1);
        free(chemin);
        break;
    }
    case 1: afficher_leaderboard(); break;
    case 2: {
        char *chemin = selectionner_niveau();
        if (chemin == NULL) {
            break;
        }
        lancer_editeur(chemin);
        break;
    }
    case 3: exit(0); break;
    }
}

void dessiner_accueil() {
    dessiner_rectangle((Point){0, 0}, 800, ECRAN_W, noir);
    // TODO: Dessiner un logo au lieu d'un texte
    afficher_texte("Pacman", 46, (Point){800 / 2 - 26 * 7, 50}, jaune);
    for (int i = 0; i < NOMBRE_BOUTONS; i++) {
        BoutonAccueil b = boutons[i];
        afficher_texte(b.texte, b.taille, (Point){b.rect.x, b.rect.y}, b.c);
        // Si le bouton en cours d'affichage est le bouton selectionné
        if (i == bouton_selectionne) {
            afficher_image("data/sprites/pacman30.bmp", (Point){b.rect.x - 30, b.rect.y + 7});
        }
    }
    actualiser();
}

void manger_bouton() {
    SDL_Surface *images[] = {
        sprite_pacman(3, 0),
        sprite_pacman(3, 1),
    };
    BoutonAccueil b = boutons[bouton_selectionne];
    Point p = {b.rect.x - 30, b.rect.y + 7};
    Point debut = p;
    int cible = b.rect.x + b.rect.w;
    while (p.x < cible) {
        dessiner_rectangle(debut, (p.x - debut.x + 10), 25, noir);
        p.x += 1;
        // Change d'image toutes les 15 frames
        afficher_surface(images[p.x % 30 / 15], p);
        attente(5);
        actualiser();
    }
}

int charger_niveau(Partie *p, char *chemin) {
    /* Chargement du plan à partir du fichier fourni en paramètre                 */
    printf("Chargement du plan...\n");
    char chemin_complet[100];
    sprintf(chemin_complet, "data/maps/%s", chemin);
    int err = charger_plan(chemin_complet, p);
    if (err == -1) {
        charger_accueil();
        return 1;
    }
    else {
        calculer_voisins(p);
        reset_timer_fantomes();
    }

#ifdef DEBUG
    /* Affichage du plan lu                                                       */
    int i, j;
    printf("Affichage du plan...\n");
    for (i = 0; i != p->L; i++) {
        for (j = 0; j != p->C; j++)
            printf("%c", p->plateau[i][j]);
        printf("\n");
    }
    printf("Bonbons et bonus : %d\n", p->nbbonus);
#endif
    return 0;
}

void charger_accueil() {
    bouton_selectionne = 0;
    scene_active = SCENE_ACCUEIL;
}

char* selectionner_niveau() {
    // Chargement des fichiers
    char *niveaux[100]; // TODO: Probleme de taille fixe
    int nb = 0; // nombre de niveaux dans la liste
    DIR *dossier;
    struct dirent *fichier;
    dossier = opendir("data/maps");
    if (dossier != NULL) {
        while ((fichier = readdir(dossier)) != NULL) {
            if (fichier->d_type == DT_REG) { // Si fichier (et non dossier)
                niveaux[nb] = strdup(fichier->d_name);
                nb++;
            }
        }
        closedir(dossier);
    }
    else {
        return NULL;
    }

    int curseur = 0; // Quel niveau est selectionné
    while (1) {
        traiter_evenements();
        int touche = nouvelle_touche();
        if (touche == SDLK_DOWN) {
            curseur = (curseur + 1) % nb;
        }
        else if (touche == SDLK_UP) {
            curseur--;
            if (curseur < 0) curseur = nb - 1;
        }
        else if (touche == SDLK_RETURN) {
            // Garde en mémoire le niveau selectionne pour pas qu'il se fasse free
            char *selectionne = strdup(niveaux[curseur]);
            // libère la mémoire alloué pour les noms de fichier
            for (int i = 0; i < nb; i++) {
                free(niveaux[i]);
            }
            return selectionne;
        }
        else if (touche == SDLK_q) {
            for (int i = 0; i < nb; i++) {
                free(niveaux[i]);
            }
            return NULL;
        }

        afficher_liste_niveaux(niveaux, nb, curseur);
        reinitialiser_evenements();
    }
}

void afficher_liste_niveaux(char **liste, int n, int curseur) {
    const int font = 26;
    dessiner_rectangle((Point){0,0}, ECRAN_W, ECRAN_H, noir);
    for (int i = 0; i < n; i++) {
        Point p = {0, font * i + 10};
        Couleur c = i == curseur ? rouge : blanc;
        afficher_texte(liste[i], font, p, c);
    }
    actualiser();
}
