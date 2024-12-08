#include "Header.h"
int main() {
    char nomFichier[100];
    printf("Entrez le nom du fichier contenant le reseau trophique : ");
    scanf("%s", nomFichier);

    ReseauTrophique *reseau = chargerReseau(nomFichier);
    connexite(&reseau);
//notre menu, comment on passe detape a etape
        int choix, choix2;
    do {
        printf("\n--- Menu ---\n");
        printf("1. Afficher le reseau trophique\n");
        printf("2. Rechercher les sommets particuliers\n");
        printf("3. Afficher la complexitee du reseau\n");
        printf("4. Simuler la dynamique des population\n");
        printf("5. Quitter\n");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                afficherReseau(reseau);
                break;

            case 2: //sous menu pour les sommets particuliers
                choix=0;
                do {
                    printf("\n--- Recherche des sommets particuliers ---\n");
                    printf("1. Rechercher les consommateurs\n");
                    printf("2. Rechercher les producteurs\n");
                    printf("3. Especes n'ayant qu'une source d'alimentation\n");
                    printf("4. Rechercher tous les predecesseurs\n");
                    printf("5. Revenir en arriere\n");
                    scanf("%d", &choix2);
                    switch (choix2) {
                        case 1:
                            rechercherMaillons(reseau, "consommateur");
                            break;

                        case 2:
                            rechercherMaillons(reseau, "producteur");
                            break;

                        case 3:
                            unpre(reseau);
                            break;

                        case 4:
                            toutpre(reseau);
                            break;

                        case 5:

                            break;

                        default:
                            printf("Choix invalide, reessayez.\n");
                    }
                }while(choix2!=5);
                break;

            case 3:
                afficherComplexite(reseau);
                break;

            case 4:
                printf("Simulation de la dynamique des populations\n");

                int iterations;
                float r;

                printf("Entrez le nombre d'iterations  : ");// envoi des variables pour la dyna, jaurai pu les demander apres
                scanf("%d", &iterations);

                printf("Entrez le rythme de croissance  : ");
                scanf("%f", &r);

                simuler_dynamique(reseau, iterations, r);
                break;

            case 5:
                printf("Au revoir !\n");
                break;

            default:
                printf("Choix invalide, reessayez.\n");
        }
    } while (choix != 5);

    libererReseau(reseau);
    return 0;
}
END_OF_MAIN()
