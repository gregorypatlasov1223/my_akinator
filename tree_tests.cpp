#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "tree_error_type.h"

void test_akinator()
{
    tree_t tree = {};
    tree_constructor(&tree);

    const char* folder_name = "log_TestAkinator";
    initialization_of_tree_log(folder_name);

    tree_dump(&tree, folder_name);

    printf("=== Starting Akinator Test ===\n");

    printf("Session 1: Adding 'cat'\n");
    {
        node_t* current = tree.root;

        tree_split_node(&tree, current, "has tail", "cat");
        tree_dump(&tree, folder_name);
    }

    printf("Session 2: Adding 'dog'\n");
    {
        node_t* cat_node = tree.root -> yes;

        tree_split_node(&tree, cat_node, "barks", "dog");
        tree_dump(&tree, folder_name);
    }

    printf("Session 3: Adding 'bird'\n");
    {
        node_t* nothing_node = tree.root -> no;

        tree_split_node(&tree, nothing_node, "can fly", "bird");
        tree_dump(&tree, folder_name);
    }

    printf("Session 4: Adding 'fish'\n");
    {
        node_t* no_fly_node = tree.root -> no -> no;

        tree_split_node(&tree, no_fly_node, "can swim", "fish");
        tree_dump(&tree, folder_name);
    }

    printf("Session 5: Adding 'snake'\n");
    {
        node_t* no_bark_node = tree.root -> yes -> no;

        tree_split_node(&tree, no_bark_node, "live in Thailand", "snake");
        tree_dump(&tree, folder_name);
    }

    printf("Final tree with %zu elements\n", tree.size);
    tree_dump(&tree, folder_name);

    tree_error_type verify_result = tree_verify(&tree);
    printf("Tree verification: %s\n", tree_error_translator(verify_result));

    save_tree_to_file(&tree, "akinator_database.txt");
    printf("Tree saved to akinator_database.txt\n");

    close_tree_log(folder_name);
    tree_destructor(&tree);

    printf("=== Akinator Test Completed ===\n");
}



