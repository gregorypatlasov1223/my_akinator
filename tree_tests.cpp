#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "tree_error_type.h"

void test_akinator_interactive()
{
    tree_t tree = {};
    tree_constructor(&tree);

    printf("\n=== Testing Akinator Interactive Game ===\n");

    printf("Building basic tree...\n");
    {
        node_t* current = tree.root;
        tree_split_node(&tree, current, "has tail", "cat");

        node_t* cat_node = tree.root -> yes;
        tree_split_node(&tree, cat_node, "barks", "dog");

        node_t* nothing_node = tree.root -> no;
        tree_split_node(&tree, nothing_node, "can fly", "bird");
    }

    printf("Basic tree built with %zu elements\n", tree.size);
    tree_common_dump(&tree);

    printf("\n=== Simulating Game Session ===\n");

    tree_destructor(&tree);
}


void test_akinator()
{
    tree_t tree = {};
    tree_constructor(&tree);

    const char* folder_name = "log_TestAkinator";
    initialization_of_tree_log(folder_name);

    tree_dump(&tree, folder_name);

    printf("=== Starting Akinator Test ===\n");

    // 1.Проверка с кошкой кошку
    printf("Session 1: Adding 'cat'\n");
    {
        node_t* current = tree.root;

        tree_split_node(&tree, current, "has tail", "cat");
        tree_dump(&tree, folder_name);
    }

    // 2.Проверка собаки
    printf("Session 2: Adding 'dog'\n");
    {
        node_t* cat_node = tree.root -> yes;

        tree_split_node(&tree, cat_node, "barks", "dog");
        tree_dump(&tree, folder_name);
    }

    // 3. Проверка птицы
    printf("Session 3: Adding 'bird'\n");
    {
        node_t* nothing_node = tree.root -> no;

        tree_split_node(&tree, nothing_node, "can fly", "bird");
        tree_dump(&tree, folder_name);
    }

    // 4. Проверка рыбы
    printf("Session 4: Adding 'fish'\n");
    {
        node_t* no_fly_node = tree.root -> no -> no;

        tree_split_node(&tree, no_fly_node, "can swim", "fish");
        tree_dump(&tree, folder_name);
    }

    // 5. Проверяем змею
    printf("Session 5: Adding 'snake'\n");
    {
        node_t* no_bark_node = tree.root -> yes -> no;

        tree_split_node(&tree, no_bark_node, "has legs", "snake");
        tree_dump(&tree, folder_name);
    }

    printf("Final tree with %zu elements\n", tree.size);
    tree_dump(&tree, folder_name);

    tree_error_type verify_result = tree_verify(&tree);
    printf("Tree verification: %s\n", tree_error_translator(verify_result));

    save_tree_to_file(&tree, "akinator_test_tree.txt");
    printf("Tree saved to akinator_test_tree.txt\n");

    close_tree_log(folder_name);
    tree_destructor(&tree);

    printf("=== Akinator Test Completed ===\n");
}


void all_tests()
{
    test_akinator();
    test_akinator_interactive();
}


