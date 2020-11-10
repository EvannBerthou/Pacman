/******************************************************************************/
/* MAIN.c                                                                     */
/******************************************************************************/
#include "./main.h"
#include "../lib/libgraphique.h"
#include "timer.h"
#include "leaderboard.h"
#include "Bouton.h"
#include "entite.h"

/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

SCENE scene_active = SCENE_ACCUEIL;
#define NOMBRE_BOUTONS 3
BoutonAccueil boutons[NOMBRE_BOUTONS];
int bouton_selectionne = 0;
int musique = 1;

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
    SDL_Joystick *manette = NULL;
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
	if (SDL_LoadWAV("music.wav", &wav_spec, &wav_buffer, &wav_length) == NULL ){
        musique = 0;
	}
    if (musique) {
        wav_spec.callback = my_audio_callback;
        wav_spec.userdata = NULL;
        audio_pos = wav_buffer;
        audio_len = wav_length;

        // Ouvre le périphérique audio
        if (SDL_OpenAudio(&wav_spec, NULL) < 0 ){
          fprintf(stderr, "Erreur lors de l'ouverture de l'audio: %s\n", SDL_GetError());
          musique = 0;
        }
        else {
            SDL_PauseAudio(0);
        }
    }

    // Création des boutons
    boutons[0] = nouveau_bouton((Point){ECRAN_W / 2, 200}, blanc, "Jouer", 26);
    boutons[1] = nouveau_bouton((Point){ECRAN_W / 2, 250}, blanc, "Classement", 26);
    boutons[2] = nouveau_bouton((Point){ECRAN_W / 2, 300}, blanc, "Quitter", 26);

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
        actualiser_jeu(&p, &timer, manette);
        dessiner_jeu(&p);
        reinitialiser_evenements();
    }
    return 0;
}

// Actualise la scene active
void actualiser_jeu(Partie *p, Timer *t, SDL_Joystick *manette) {
    switch(scene_active) {
    case SCENE_ACCUEIL:
        actualiser_accueil(p, t, manette);
        break;
    case SCENE_NIVEAU:
        actualiser_partie(p, t, manette);
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
static int touche_manete(SDL_Joystick *manette, int bouton) {
    if (manette == NULL) return 0;
    return SDL_JoystickGetButton(manette, bouton);
}

static Uint8 croix_manette(SDL_Joystick *manette) {
    if (manette == NULL) return 0;
    return SDL_JoystickGetHat(manette, 0);
}

void actualiser_accueil(Partie *p, Timer *t, SDL_Joystick *manette) {
    // Evite la répétition de touche
    // Touche appuyé lors du dernier appel
    static int derniere_touche = 0;
    static Uint8 derniere_croix = 0;
    // Touche appuyés à cet appel
    int touche = attendre_touche_duree(10); // Touche clavier
    Uint8 croix = croix_manette(manette);  // Croix manette
    int entrer = touche_manete(manette, 0); // Touche X manette (manette ps4)
    // Si la touche appuyé est la meme que celui du dernier appel, cela veut dire que la touche
    // n'a pas été relachée
    if (touche == derniere_touche && croix == derniere_croix && entrer == 0) return;

    // Sauvegarde les touches appuyés au dernier appel
    derniere_touche = touche;
    derniere_croix = croix;

    if (touche == SDLK_DOWN || croix & SDL_HAT_DOWN) {
        bouton_selectionne = (bouton_selectionne + 1) % NOMBRE_BOUTONS;
    }

    if (touche == SDLK_UP || croix & SDL_HAT_UP) {
        bouton_selectionne--;
        if (bouton_selectionne < 0)
            bouton_selectionne = NOMBRE_BOUTONS - 1;
    }

    if (touche == SDLK_RETURN || entrer) {
        activer_bouton(p, t, manette);
    }
}

void activer_bouton(Partie *p, Timer *t, SDL_Joystick *manette) {
    // Animation de pacman qui mange le bouton avant de changer de menu
    manger_bouton();
    // Transition écran noir
    switch(bouton_selectionne) {
    case 0:
        // Charge le niveau
        if (charger_niveau(p)) {
            printf("Erreur lors du chargement du niveau\n");
            exit(1);
        }
        scene_active = SCENE_NIVEAU;
        // Arrete la musique de l'accueil
        //if (SDL_GetAudioStatus() == SDL_AUDIO_PLAYING)
            SDL_PauseAudio(1);
        break;
    case 1: afficher_leaderboard(); break;
    case 2: exit(0); break;
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
        charger_sprite(ENTITE_PACMAN, 3, 0),
        charger_sprite(ENTITE_PACMAN, 3, 1),
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

int charger_niveau(Partie *p) {
    /* Chargement du plan à partir du fichier fourni en paramètre                 */
    printf("Chargement du plan...\n");
    // TODO: Charger le niveau dynamiquement
    *p = charge_plan("data/test.txt");
    /* Si problème lors du chargement du plan...                                  */
    if (p->plateau == NULL)
        return 1;
    calculer_voisins(p);

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
