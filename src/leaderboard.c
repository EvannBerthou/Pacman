#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <netdb.h>

// Fonctions d'aide pour gérer les erreurs
static int err(int result, const char *msg) {
    if (result < 0) {
#ifdef DEBUG
        fprintf(stderr, "%s\n", msg);
#endif
        exit(1);
    }
    return result;
}

static void * errp(void *p, const char *msg) {
    if (p == NULL) {
#ifdef DEBUG
        fprintf(stderr, "%s\n", msg);
#endif
        exit(1);
    }
    return p;
}

// Envoie une requete à un serveur et renvoie la réponse du serveur
char *envoyer_requete(const char *host, int port, const char *req) {

    // Création du socket
    int sockfd = err(socket(AF_INET, SOCK_STREAM, 0), "Erreur lors de la création du socket");
    struct hostent *server = errp(gethostbyname(host), "Serveur non trouvé");

    // Initialisation du socket pour qu'il se connecte au serveur
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr.s_addr,server->h_addr,server->h_length);

    // Connection au serveur
    err(connect(sockfd,(struct sockaddr *)&addr,sizeof(addr)), "Erreur lors de la connection");


    // Envoie de la requete tout en gérant les éventuelles paquets perdus
    int total = strlen(req);
    int envoye = 0;
    do {
        int bytes = err(send(sockfd, req + envoye, total - envoye, 0), "Erreur lors de l'envoie");
        if (bytes == 0) break; // Connection au serveur perdue
        envoye+=bytes;
    } while (envoye < total);

    // Réponse du serveur
    total = 4096;
    char *reponse = calloc(total, sizeof(char));
    int recu = 0;
    do {
        int bytes = err(recv(sockfd, reponse + recu, total - recu, 0), "Erreur lors de la lecture");
        if (bytes == 0) break; // Connection au serveurperdue
        recu += bytes;
    } while (recu < total);

    // Fermeture du socket
    close(sockfd);

    return reponse;
}

void afficher_leaderboard() {
    const char *get_req = 
            "GET / HTTP/1.0\r\n"
            "Host: pacman-leaderboard.herokuapp.com\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n"
            "Content-Length: %d\r\n\r\n"
            "%s\r\n";

    char req[2048];
    const char get_params[] = "joueur=c";
    sprintf(req, get_req, strlen(get_params), get_params);

    char *reponse = envoyer_requete("pacman-leaderboard.herokuapp.com", 80, req);
    char *body = strstr(reponse, "\r\n\r\n");
    char *state;
    char *line = strtok_r(body, "\r\n", &state);
    while (line != NULL) {
        char *partie;
        const char *joueur = strtok_r(line, ":", &partie);
        const char *points = strtok_r(NULL, ":", &partie);
        printf("joueur : %s Points : %s\n", joueur, points);
        line = strtok_r(NULL, "\r\n", &state);
    }
    free(reponse);
}