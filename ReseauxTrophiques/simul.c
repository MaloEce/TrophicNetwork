#include "Header.h"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Couleur pour chaque courbe, il y a don un maximum despeces
unsigned int generer_couleur(int index, int nbSommets) {
    int couleurs_disponibles[12][3] = {
            {0, 255, 0},    // Vert
            {0, 0, 255},    // Bleu
            {255, 0, 0},    // Rouge
            {0, 255, 255},  // Cyan
            {255, 0, 255},  // Magenta
            {255, 255, 0},  // Jaune
            {255, 165, 0},  // Orange
            {128, 128, 128}, // Gris
            {128, 0, 128},  // Violet
            {0, 0, 0},      // Noir
            {255, 255, 255}, // Blanc
            {255, 192, 203}  // Rose
    };

    //si trop despece on modifie
    if (index >= 12) {
        int rouge = (index * 50) % 256;
        int vert = (index * 80) % 256;
        int bleu = (index * 120) % 256;
        return makecol(rouge, vert, bleu);
    }

    return makecol(couleurs_disponibles[index][0], couleurs_disponibles[index][1], couleurs_disponibles[index][2]);
}

void dessiner_legende(Sommet *sommets, int nbSommets) {
    int x = SCREEN_WIDTH - 150;
    int y = 10;
    for (int i = 0; i < nbSommets; i++) {
        // Dessiner un carré avec la couleur de l'espèce
        rectfill(screen, x, y, x + 20, y + 20, generer_couleur(i, nbSommets));
        // Afficher le nom de l'espèce
        textout_ex(screen, font, sommets[i].nom, x + 25, y, makecol(255, 255, 255), -1);
        y += 25;
    }
}
void afficher_graphique(ReseauTrophique *reseau, int **resultats, int iterations) {
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

    BITMAP *buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    clear_to_color(buffer, makecol(255, 255, 255)); // Fond blanc

    // biomasse maximale utilisee pour faire la premiere echelle, pour tout voir a louverture du graph
    int max_biomasse = 0;
    for (int i = 0; i < reseau->nbSommets; i++) {
        for (int t = 0; t <= iterations; t++) {
            if (resultats[i][t] > max_biomasse) {
                max_biomasse = resultats[i][t];
            }
        }
    }
    if (max_biomasse == 0) max_biomasse = 1; // Éviter division par zéro

    float facteur_zoom = 1.0;

    while (!key[KEY_ESC]) {
        clear_to_color(buffer, makecol(255, 255, 255));

        // Échelles
        float echelle_x = (float)(SCREEN_WIDTH - 150) / iterations; // Axe des abscisses
        float echelle_y = (float)(SCREEN_HEIGHT - 100) / (max_biomasse * facteur_zoom); // Axe des ordonnées

        // Dessiner les axes
        line(buffer, 50, SCREEN_HEIGHT - 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 50, makecol(0, 0, 0)); // Axe X
        line(buffer, 50, SCREEN_HEIGHT - 50, 50, 50, makecol(0, 0, 0)); // Axe Y

        // Divisions sur les axes
        for (int t = 0; t <= iterations; t++) {
            int x = 50 + t * echelle_x;
            line(buffer, x, SCREEN_HEIGHT - 50, x, SCREEN_HEIGHT - 45, makecol(0, 0, 0)); // Graduation
            textprintf_ex(buffer, font, x - 10, SCREEN_HEIGHT - 40, makecol(0, 0, 0), -1, "%d", t); // Itérations
        }
        for (int i = 0; i <= 10; i++) {
            int y = SCREEN_HEIGHT - 50 - i * (SCREEN_HEIGHT - 100) / 10;
            float valeur_biomasse = i * max_biomasse * facteur_zoom/ 10.0;
            line(buffer, 45, y, 50, y, makecol(0, 0, 0)); // Graduation
            textprintf_ex(buffer, font, 10, y - 5, makecol(0, 0, 0), -1, "%.1f", valeur_biomasse); // Valeur
        }

        // Dessiner les courbes
        for (int i = 0; i < reseau->nbSommets; i++) {
            int couleur = generer_couleur(i, reseau->nbSommets);
            for (int t = 0; t < iterations; t++) {
                int x1 = 50 + t * echelle_x;
                int y1 = SCREEN_HEIGHT - 50 - resultats[i][t] * echelle_y;
                int x2 = 50 + (t + 1) * echelle_x;
                int y2 = SCREEN_HEIGHT - 50 - resultats[i][t + 1] * echelle_y;

                line(buffer, x1, y1, x2, y2, couleur); // Relier les points
                circlefill(buffer, x1, y1, 3, couleur); // Point
                if (key[KEY_SPACE]) {
                    textprintf_ex(buffer, font, x1 + 2, y1 - 10, makecol(0, 0, 0), -1, "%d", resultats[i][t]);
                }
            }
            int x_last = 50 + iterations * echelle_x;
            int y_last = SCREEN_HEIGHT - 50 - resultats[i][iterations] * echelle_y;
            circlefill(buffer, x_last, y_last, 3, couleur); // Dernier point
        }

        // Dessiner la legende
        int x_legende = SCREEN_WIDTH - 140;
        int y_legende = 60;
        for (int i = 0; i < reseau->nbSommets; i++) {
            int couleur = generer_couleur(i, reseau->nbSommets);
            rectfill(buffer, x_legende, y_legende, x_legende + 20, y_legende + 20, couleur); // Carré couleur
            textout_ex(buffer, font, reseau->sommets[i].nom, x_legende + 30, y_legende + 5, makecol(0, 0, 0), -1);
            y_legende += 25;
        }

        // copier dans l'ecran principal
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        // gerer le zoom
        if (key[KEY_A]) { // Zoom avant
            facteur_zoom *= 1.001;
        }
        if (key[KEY_S]) { // Zoom arrière
            facteur_zoom /= 1.001;
        }
    }

    destroy_bitmap(buffer);
    allegro_exit();
}


void simuler_dynamique(ReseauTrophique *reseau, int iterations, float r) {
    int **resultats = malloc(reseau->nbSommets * sizeof(int *));
    for (int i = 0; i < reseau->nbSommets; i++) {
        resultats[i] = malloc((iterations + 1) * sizeof(int));
        resultats[i][0] = reseau->sommets[i].biomasse; // Biomasse initiale
    }

    for (int t = 1; t <= iterations; t++) {
        for (int i = 0; i < reseau->nbSommets; i++) {
            Sommet *sommet = &reseau->sommets[i];
            float K = 0; // Capacite de portage

            // si cest un producteur alors lui donner une constante en plus, car il na pas dentree
            if (strcmp(sommet->type, "producteur") == 0) {
                K += 500;
            }

            // Calcul des apports entrants (arcs vers ce sommet)
            for (int j = 0; j < reseau->nbArcs; j++) {
                Arc *arc = &reseau->arcs[j];
                if (arc->cible == sommet->id) {
                    K += arc->poids * reseau->sommets[arc->source].biomasse;
                }
            }

            if (K == 0) K = 1; // Eviter division par zéro

            // Dynamique
            int N_t = sommet->biomasse;
            int N_t1 = N_t + r * N_t * (1 - (float)N_t / K);

            // Ajustement pour les interactions sortantes
            for (int j = 0; j < reseau->nbArcs; j++) {
                Arc *arc = &reseau->arcs[j];
                if (arc->source == sommet->id) {
                    N_t1 += arc->poids * reseau->sommets[arc->cible].biomasse;
                }
            }

            if (N_t1 < 0) N_t1 = 0;

            resultats[i][t] = N_t1;
            sommet->biomasse = N_t1; // Mise à jour pour la prochaine iteration
        }
    }

    // Afficher les resultats
    printf("\n--- Resultats de la simulation ---\n");
    for (int t = 0; t <= iterations; t++) {
        printf("Iteration %d:\n", t);
        for (int i = 0; i < reseau->nbSommets; i++) {
            printf(" - %s: %d\n", reseau->sommets[i].nom, resultats[i][t]);
        }
        printf("\n");
    }
    int oui;

    do {
        printf("Voulez vous l'affichage Graphique? 1 oui 2 non \n ");
        scanf("%d", &oui);
        switch (oui) {
            case 1:
                afficher_graphique(reseau, resultats, iterations);
                oui=2;
                break;

            case 2:
                break;

            default:
                printf("Choix invalide, reessayez\n");
        }
    }while(oui!=2);


// Liberer la memoire
    for (int i = 0; i < reseau->nbSommets; i++) {
        free(resultats[i]);
    }
    free(resultats);
}

