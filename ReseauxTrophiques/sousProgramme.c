#include "Header.h"

// Fonction pour ignorer une ligne de commentaire dans le fichier
void ignorerCommentaire(FILE *fichier) {
    int c;
    while ((c = fgetc(fichier)) != '\n' && c != EOF); // Avancer jusqu'à la fin de la ligne
}

// Charger le reseau trophique depuis un fichier
ReseauTrophique* chargerReseau(const char *nomFichier) {
    FILE *fichier = fopen(nomFichier, "r");
    if (!fichier) {
        perror("Erreur d'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    ReseauTrophique *reseau = malloc(sizeof(ReseauTrophique));
    reseau->nbSommets = 0;
    reseau->nbArcs = 0;

    char ligne[256];
    // Compter le nombre de sommets et d'arcs
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] == '/' && ligne[1] == '/') {
            continue; // Ignorer les lignes de commentaire
        } else if (strchr(ligne, '"')) {
            reseau->nbSommets++;
        } else if (isdigit(ligne[0])) {
            reseau->nbArcs++;
        }
    }

    // Allouer de la mémoire pour les sommets et les arcs
    reseau->sommets = malloc(reseau->nbSommets * sizeof(Sommet));
    reseau->arcs = malloc(reseau->nbArcs * sizeof(Arc));

    rewind(fichier); // Revenir au début du fichier pour relire les données

    int sommetIndex = 0, arcIndex = 0;

    // Lire les données du fichier
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] == '/' && ligne[1] == '/') {
            continue; // Ignorer les lignes de commentaire
        }

        if (strchr(ligne, '"')) {
            // Lire un sommet
            sscanf(ligne, "%d \"%[^\"]\" %s %d",
                   &reseau->sommets[sommetIndex].id,
                   reseau->sommets[sommetIndex].nom,
                   reseau->sommets[sommetIndex].type,
                   &reseau->sommets[sommetIndex].biomasse);
            sommetIndex++;
        } else if (isdigit(ligne[0])) {
            // Lire un arc
            sscanf(ligne, "%d %d %f",
                   &reseau->arcs[arcIndex].source,
                   &reseau->arcs[arcIndex].cible,
                   &reseau->arcs[arcIndex].poids);
            arcIndex++;
        }
    }

    fclose(fichier);
    return reseau;
}
void dfs(int sommet, int nbSommets, int *visites, Arc *arcs, int nbArcs, int direction) {
    visites[sommet] = 1;
    for (int i = 0; i < nbArcs; i++) {
        int voisin = (direction == 0) ? arcs[i].cible - 1 : arcs[i].source - 1;
        if (((direction == 0 && arcs[i].source - 1 == sommet) ||
             (direction == 1 && arcs[i].cible - 1 == sommet)) && !visites[voisin]) {
            dfs(voisin, nbSommets, visites, arcs, nbArcs, direction);
        }
    }
}
void verifierConnexiteEtFiltrer(ReseauTrophique *reseau) {
    int nbSommets = reseau->nbSommets;
    int *visites = calloc(nbSommets, sizeof(int));
    int *composante = calloc(nbSommets, sizeof(int));
    int composanteId = 0;

    // Identification des composantes fortement connexes
    for (int i = 0; i < nbSommets; i++) {
        if (!visites[i]) {
            composanteId++;
            dfs(i, nbSommets, visites, reseau->arcs, reseau->nbArcs, 0); // DFS dans une direction
            for (int j = 0; j < nbSommets; j++) {
                if (visites[j]) {
                    composante[j] = composanteId;
                    visites[j] = 0; // Réinitialiser pour exploration complète
                }
            }
        }
    }

    printf("\nLe réseau a %d composante(s) fortement connexe(s) :\n", composanteId);
    for (int i = 1; i <= composanteId; i++) {
        printf("Composante %d : ", i);
        for (int j = 0; j < nbSommets; j++) {
            if (composante[j] == i) {
                printf("[%d] %s, ", reseau->sommets[j].id, reseau->sommets[j].nom);
            }
        }
        printf("\n");
    }

    // Si le réseau n'est pas connexe, proposer de choisir une composante
    if (composanteId > 1) {
        int choix;
        printf("\nLe réseau n'est pas connexe. Choisissez une composante à étudier (1-%d) : ", composanteId);
        scanf("%d", &choix);
        if (choix < 1 || choix > composanteId) {
            printf("Choix invalide. Retour au menu principal.\n");
        } else {
            // Filtrage de la composante choisie
            ReseauTrophique *sousReseau = filtrerComposante(reseau, composante, choix);
            printf("\nÉtude de la composante %d :\n", choix);
            sucpre(sousReseau);
            libererReseau(sousReseau);
        }
    } else {
        printf("\nLe réseau est fortement connexe. Pas de filtration nécessaire.\n");
    }

    free(visites);
    free(composante);
}

ReseauTrophique* filtrerComposante(ReseauTrophique *reseau, int *composante, int composanteId) {
    int nbSommets = 0;
    for (int i = 0; i < reseau->nbSommets; i++) {
        if (composante[i] == composanteId) {
            nbSommets++;
        }
    }

    ReseauTrophique *sousReseau = malloc(sizeof(ReseauTrophique));
    sousReseau->sommets = malloc(nbSommets * sizeof(Sommet));
    sousReseau->arcs = malloc(reseau->nbArcs * sizeof(Arc));
    sousReseau->nbSommets = 0;
    sousReseau->nbArcs = 0;

    int *nouveauxIds = malloc(reseau->nbSommets * sizeof(int));
    for (int i = 0; i < reseau->nbSommets; i++) {
        if (composante[i] == composanteId) {
            sousReseau->sommets[sousReseau->nbSommets] = reseau->sommets[i];
            nouveauxIds[reseau->sommets[i].id - 1] = sousReseau->nbSommets + 1;
            sousReseau->nbSommets++;
        }
    }

    for (int i = 0; i < reseau->nbArcs; i++) {
        int source = reseau->arcs[i].source - 1;
        int cible = reseau->arcs[i].cible - 1;
        if (composante[source] == composanteId && composante[cible] == composanteId) {
            sousReseau->arcs[sousReseau->nbArcs] = reseau->arcs[i];
            sousReseau->arcs[sousReseau->nbArcs].source = nouveauxIds[source];
            sousReseau->arcs[sousReseau->nbArcs].cible = nouveauxIds[cible];
            sousReseau->nbArcs++;
        }
    }

    free(nouveauxIds);
    return sousReseau;
}



// Afficher le reseau trophique
void sucpre(ReseauTrophique *reseau) {
    printf("\n--- Relations d'adjacence ---\n");
    for (int i = 0; i < reseau->nbSommets; i++) {
        printf("\nSommet [%d] %s (%s) :\n", reseau->sommets[i].id, reseau->sommets[i].nom, reseau->sommets[i].type);
        printf("  Successeurs : ");
        int aucunSuccesseur = 1;
        for (int j = 0; j < reseau->nbArcs; j++) {
            if (reseau->arcs[j].source == reseau->sommets[i].id) {
                printf("[%d] %s, ", reseau->arcs[j].cible, reseau->sommets[reseau->arcs[j].cible - 1].nom);
                aucunSuccesseur = 0;
            }
        }
        if (aucunSuccesseur) printf("Aucun");
        printf("\n  Prédécesseurs : ");
        int aucunPredecesseur = 1;
        for (int j = 0; j < reseau->nbArcs; j++) {
            if (reseau->arcs[j].cible == reseau->sommets[i].id) {
                printf("[%d] %s, ", reseau->arcs[j].source, reseau->sommets[reseau->arcs[j].source - 1].nom);
                aucunPredecesseur = 0;
            }
        }
        if (aucunPredecesseur) printf("Aucun");
        printf("\n");
    }
}
void afficherReseau(ReseauTrophique *reseau) {
    printf("\n--- Réseau Trophique ---\n");
    printf("\nSommets :\n");
    for (int i = 0; i < reseau->nbSommets; i++) {
        printf("[%d] %s (%s) - Biomasse : %d\n", reseau->sommets[i].id, reseau->sommets[i].nom, reseau->sommets[i].type, reseau->sommets[i].biomasse);
    }
    printf("\nArcs :\n");
    for (int i = 0; i < reseau->nbArcs; i++) {
        printf("[%d -> %d] Poids : %.2f\n", reseau->arcs[i].source, reseau->arcs[i].cible, reseau->arcs[i].poids);
    }
    sucpre(reseau);
}

// Rechercher des maillons (par exemple : producteurs)
void rechercherMaillons(ReseauTrophique *reseau, const char *type) {
    printf("\nRecherche des %s :\n", type);
    for (int i = 0; i < reseau->nbSommets; i++) {
        if (strcmp(reseau->sommets[i].type, type) == 0) {
            printf("[%d] %s - Biomasse : %d\n",
                   reseau->sommets[i].id,
                   reseau->sommets[i].nom,
                   reseau->sommets[i].biomasse);
        }
    }
}

// Simuler la dynamique des populations
void simulerDynamique(ReseauTrophique *reseau, int iterations) {
    printf("\nSimulation de la dynamique des populations sur %d iterations :\n", iterations);

    for (int t = 0; t < iterations; t++) {
        printf("\nIteration %d :\n", t + 1);

        int *nouvellesBiomasses = malloc(reseau->nbSommets * sizeof(int));
        for (int i = 0; i < reseau->nbSommets; i++) {
            nouvellesBiomasses[i] = reseau->sommets[i].biomasse; // Copie
        }

        for (int i = 0; i < reseau->nbArcs; i++) {
            Arc arc = reseau->arcs[i];
            int source = arc.source - 1;
            int cible = arc.cible - 1;

            int transfert = (int)(reseau->sommets[source].biomasse * arc.poids);
            nouvellesBiomasses[source] -= transfert;
            nouvellesBiomasses[cible] += transfert;
        }

        for (int i = 0; i < reseau->nbSommets; i++) {
            reseau->sommets[i].biomasse = nouvellesBiomasses[i];
            printf("[%d] %s - Biomasse : %d\n",
                   reseau->sommets[i].id,
                   reseau->sommets[i].nom,
                   reseau->sommets[i].biomasse);
        }

        free(nouvellesBiomasses);
    }
}
void distributionDesDegres(ReseauTrophique *reseau) {
    int *degresEntrants = calloc(reseau->nbSommets, sizeof(int));
    int *degresSortants = calloc(reseau->nbSommets, sizeof(int));

    for (int i = 0; i < reseau->nbArcs; i++) {
        degresSortants[reseau->arcs[i].source - 1]++;
        degresEntrants[reseau->arcs[i].cible - 1]++;
    }

    printf("\nDistribution des degrés :\n");
    for (int i = 0; i < reseau->nbSommets; i++) {
        printf("[%d] %s - Degré entrant : %d, Degré sortant : %d\n",
               reseau->sommets[i].id,
               reseau->sommets[i].nom,
               degresEntrants[i],
               degresSortants[i]);
    }

    free(degresEntrants);
    free(degresSortants);
}
int calculerHauteurTrophiqueRec(ReseauTrophique *reseau, int sommetId, int *visites) {
    visites[sommetId] = 1;
    int maxHauteur = 0;

    for (int i = 0; i < reseau->nbArcs; i++) {
        if (reseau->arcs[i].source == sommetId + 1 && !visites[reseau->arcs[i].cible - 1]) {
            int hauteur = calculerHauteurTrophiqueRec(reseau, reseau->arcs[i].cible - 1, visites);
            if (hauteur > maxHauteur) {
                maxHauteur = hauteur;
            }
        }
    }

    visites[sommetId] = 0;
    return maxHauteur + 1;
}

int calculerHauteurTrophique(ReseauTrophique *reseau) {
    int *visites = calloc(reseau->nbSommets, sizeof(int));
    int maxHauteur = 0;

    for (int i = 0; i < reseau->nbSommets; i++) {
        int hauteur = calculerHauteurTrophiqueRec(reseau, i, visites);
        if (hauteur > maxHauteur) {
            maxHauteur = hauteur;
        }
    }

    free(visites);
    return maxHauteur;
}
double calculerDensite(ReseauTrophique *reseau) {
    int nbMaxArcs = reseau->nbSommets * (reseau->nbSommets - 1);
    return (double)reseau->nbArcs / nbMaxArcs;
}
void afficherComplexite(ReseauTrophique *reseau) {
    printf("\n--- Complexité du réseau ---\n");
    printf("Nombre d'espèces : %d\n", reseau->nbSommets);

    int hauteurTrophique = calculerHauteurTrophique(reseau);
    printf("Hauteur trophique (nombre de niveaux) : %d\n", hauteurTrophique);

    double densite = calculerDensite(reseau);
    printf("Densité de liaison : %.2f\n", densite);

    distributionDesDegres(reseau);
}

// Liberer la memoire allouee
void libererReseau(ReseauTrophique *reseau) {
    free(reseau->sommets);
    free(reseau->arcs);
    free(reseau);
}
