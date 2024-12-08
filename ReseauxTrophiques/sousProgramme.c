#include "Header.h"


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
void unpre(ReseauTrophique *reseau) {
    printf("\n--- Especes n'ayant qu'une source d'alimentation ---\n");

    for (int i = 0; i < reseau->nbSommets; i++) {
        // Compter le nombre de prédécesseurs du sommet actuel
        int nbPredecesseurs = 0;

        for (int j = 0; j < reseau->nbArcs; j++) {
            if (reseau->arcs[j].cible == reseau->sommets[i].id) {
                nbPredecesseurs++;
            }
        }

        // Afficher uniquement les sommets ayant exactement un prédécesseur
        if (nbPredecesseurs == 1) {
            printf("\nSommet [%d] %s (%s) :\n", reseau->sommets[i].id, reseau->sommets[i].nom, reseau->sommets[i].type);
            printf("  Prédécesseur : ");

            for (int j = 0; j < reseau->nbArcs; j++) {
                if (reseau->arcs[j].cible == reseau->sommets[i].id) {
                    printf("[%d] %s\n",
                           reseau->arcs[j].source,
                           reseau->sommets[reseau->arcs[j].source - 1].nom
                    );
                    break; // On affiche le seul prédécesseur trouvé
                }
            }
        }
    }
}

// Fonction DFS inversée pour explorer tous les chemins menant jusqu'à la cible
void dfsInverse(ReseauTrophique *reseau, int sommetActuel, int chemin[], int index) {
    chemin[index] = sommetActuel; // Enregistrer le sommet actuel dans le chemin

    // Vérifier s'il existe des arcs menant à ce sommet
    int trouvePred = 0;
    for (int i = 0; i < reseau->nbArcs; i++) {
        if (reseau->arcs[i].cible == sommetActuel) {
            trouvePred = 1; // Un prédécesseur existe
            dfsInverse(reseau, reseau->arcs[i].source, chemin, index + 1); // Continuer la recherche
        }
    }

    // Afficher le chemin lorsque l'on remonte dans la récursion
    if (!trouvePred) { // On est arrivé à une source sans prédécesseur
        printf("\nChemin trouvé : ");
        for (int i = index; i >= 0; i--) {
            printf("%s", reseau->sommets[chemin[i] - 1].nom);
            if (i > 0) printf(" -> ");
        }
        printf("\n");
    }
}

// Sous-programme pour l'interface utilisateur
void toutpre(ReseauTrophique *reseau) {
    int sommetRecherche;
    int chemin[100]; // Tableau pour sauvegarder le chemin actuel
    int aDesPred = 0;

    // Afficher la liste des sommets disponibles
    printf("\nSommets disponibles dans le réseau trophique :\n");
    for (int i = 0; i < reseau->nbSommets; i++) {
        printf("[%d] %s (%s)\n", reseau->sommets[i].id, reseau->sommets[i].nom, reseau->sommets[i].type);
    }

    // Demander un sommet cible
    printf("\nEntrez l'ID du sommet que vous souhaitez explorer : ");
    scanf("%d", &sommetRecherche);
    // Vérifier si le sommet a des prédécesseurs
    for (int i = 0; i < reseau->nbArcs; i++) {
        if (reseau->arcs[i].cible == sommetRecherche) {
            aDesPred = 1;
            break;
        }
    }

    if (aDesPred==0) {
        printf("\nLe sommet %s n'a aucun prédécesseur dans le réseau trophique.\n");
    }
    if (aDesPred==1) {
        // Lancer la recherche inversée avec DFS
        printf("\nExploration des chemins menant jusqu'à %s\n", reseau->sommets[sommetRecherche - 1].nom);
        dfsInverse(reseau, sommetRecherche, chemin, 0);
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
