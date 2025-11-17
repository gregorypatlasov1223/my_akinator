#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "speech.h"
#include "graphics.h" 
#include "tree_tests.h"
#include "tree_error_type.h"

int main()
{
    initialization_graphics();
    printf("=== Graphics initialized successfully ===\n\n");

    test_akinator();
    printf("=== Database created successfully ===\n\n");

    tree_t tree = {};
    tree_error_type result = tree_constructor(&tree);

    const char* default_database = "akinator_database.txt";

    tree_error_type load_result = load_tree_from_file(&tree, default_database);

    if (load_result != TREE_NO_ERROR)
    {
        animate_question("Database isn't found. Creating new database...");
        printf("Database isn't found. Creating new database...");
        result = save_tree_to_file(&tree, default_database);

        if (result != TREE_NO_ERROR)
        {
            printf("Error creating database: %s\n", tree_error_translator(result));
            tree_destructor(&tree);
            close_graphics();
            return result;
        }
        printf("New database created successfully!\n");
    }
    else
    {
        animate_question("Database loaded successfully!");
        printf("Database loaded successfully! (%zu nodes)\n", tree.size);
    }

    int choice = 0;
    char filename[MAX_LENGTH_OF_FILENAME] = "akinator_tree.txt";

    do
    {
        print_menu();
        if (scanf("%d", &choice) != 1)
        {
            clear_input_buffer();
            printf("Invalid input. Please try again.\n");
            choice = 0;  // сбрасываем choice
            continue;
        }
        clear_input_buffer();

        switch (choice)
        {
            case 1:
                printf("\n=== Starting Game ===\n");
                animate_question("Let's play! Think of something!");
                printf("Think of something, and I'll try to guess it!\n");
                printf("Answer with 'yes' or 'no' to my questions.\n\n");

                result = akinator_play(&tree);
                if (result != TREE_NO_ERROR)
                {
                    speak_print_with_variable_number_of_parameters("Game error: %s\n", tree_error_translator(result));
                }
                break;

            case 2:
                animate_question("Saving database...");
                result = save_tree_to_file(&tree, filename);
                if (result == TREE_NO_ERROR)
                {
                    speak_print_with_variable_number_of_parameters("Tree successfully saved to %s\n", filename);
                }
                else
                {
                    speak_print_with_variable_number_of_parameters("Error saving tree: %s\n", tree_error_translator(result));
                }
                break;

            case 3:
                printf("\n=== Tree Structure ===\n");
                tree_common_dump(&tree);
                break;

            case 4:
                give_object_definition(&tree);
                break;

            case 5:
                compare_two_objects(&tree);
                break;

            case 6:
                animate_question("Goodbye! Thanks for playing!");
                speak_print_with_variable_number_of_parameters("Goodbye!\n");
                break;

            default:
                animate_question("Invalid option. Please try again.");
                speak_print_with_variable_number_of_parameters("Invalid option. Please try again.\n");
                break;
        }

    } while (choice != 6);

    animate_question("Saving database before exit...");
    speak_print_with_variable_number_of_parameters("Saving database before exit...\n");
    save_tree_to_file(&tree, default_database);

    tree_destructor(&tree);

    close_graphics();
    printf("=== Graphics closed successfully ===\n");

    return 0;
}
