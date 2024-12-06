#include "Header.h"

int main() {
    char nomFichier[100];
    printf("Entrez le nom du fichier contenant le reseau trophique : ");
    scanf("%s", nomFichier);

    ReseauTrophique *reseau = chargerReseau(nomFichier);

    int choix, iterations;
    do {
        printf("\n--- Menu ---\n");
        printf("1. Afficher le reseau trophique\n");
        printf("2. Rechercher les producteurs\n");
        printf("3. Rechercher les consommateurs\n");
        printf("4. Simuler la dynamique des populations\n");
        printf("5. Afficher la complexitée du reseau\n");
        printf("6. Verifier la connexite \n");
        printf("7. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                afficherReseau(reseau);
                break;
            case 2:
                rechercherMaillons(reseau, "producteur");
                break;
            case 3:
                rechercherMaillons(reseau, "consommateur");
                break;
            case 4:
                printf("Entrez le nombre d'itérations : ");
                scanf("%d", &iterations);
                simulerDynamique(reseau, iterations);
                break;
            case 5:
                afficherComplexite(reseau);
                break;
            case 6:
                verifierConnexiteEtFiltrer(reseau);
                break;

            case 7:
                printf("Au revoir !\n");
                break;
            default:
                printf("Choix invalide. Réessayez.\n");
        }
    } while (choix != 7);

    libererReseau(reseau);
    return 0;
}
