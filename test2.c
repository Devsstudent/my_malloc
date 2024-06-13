#include <criterion/criterion.h>  // Inclut la bibliothèque Criterion pour les tests unitaires
#include "my_secmalloc.private.h"  // Inclut les déclarations des fonctions et structures internes de l'allocateur
#include <sys/mman.h>  // Inclut les définitions de mmap et munmap


Test(my_malloc, stress_test) {
    const int N = 1000; // Nombre de répétitions
    void **ptrs = malloc(N * sizeof(void*)); // Tableau pour stocker les pointeurs alloués

    for (int i = 0; i < N; i++) {
        size_t size = (i % 10 + 1) * 10; // Taille d'allocation variable
        ptrs[i] = my_malloc(size);
        memset(ptrs[i], 0, size); // Initialise la mémoire allouée à zéro
        cr_assert_not_null(ptrs[i], "Allocation failed at iteration %d", i);
    }

    for (int i = 0; i < N; i++) {
        my_free(ptrs[i]); // Libération de la mémoire
    }

    free(ptrs); // Libérer le tableau des pointeurs
}
