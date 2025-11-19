#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "speech.h"
#include "graphics.h"
#include "tree_tests.h"
#include "akinator_app.h"
#include "tree_error_type.h"


static const char* DEFAULT_DATABASE = "akinator_database.txt";
static const char* EXPORT_FILENAME = "akinator_tree.txt";


bool initialize_akinator_app()
{
    initialization_graphics();
    printf("Graphics initialized successfully\n\n");

    test_akinator();
    printf("Database created successfully\n\n");

    return true;
}


bool load_or_create_database(tree_t* tree)
{
    tree_error_type result = tree_constructor(tree);
    if (result != TREE_NO_ERROR)
    {
        printf("Error initializing tree: %s\n", tree_error_translator(result));
        return false;
    }

    tree_error_type load_result = load_tree_from_file(tree, DEFAULT_DATABASE);

    if (load_result != TREE_NO_ERROR)
    {
        animate_question("Database isn't found. Creating new database...");
        printf("Database isn't found. Creating new database...");

        result = save_tree_to_file(tree, DEFAULT_DATABASE);
        if (result != TREE_NO_ERROR)
        {
            printf("Error creating database: %s\n", tree_error_translator(result));
            return false;
        }
        printf("New database created successfully!\n");
    }
    else
    {
        animate_question("Database loaded successfully!");
        printf("Database loaded successfully! (%zu nodes)\n", tree -> size);
    }

    return true;
}


void handle_play_game(tree_t* tree)
{
    set_game_state_background(STATE_PLAYING);

    printf("\nStarting Game\n");
    animate_question("Let's play! Think of something!");
    printf("Think of something, and I'll try to guess it!\n");
    printf("Answer with 'yes' or 'no' to my questions.\n\n");

    tree_error_type result = akinator_play(tree);
    if (result != TREE_NO_ERROR)
    {
        speak_print_with_variable_number_of_parameters("Game error: %s\n", tree_error_translator(result));
    }
}


void handle_save_database(tree_t* tree)
{
    set_game_state_background(STATE_SAVING);
    animate_question("Saving database...");

    tree_error_type result = save_tree_to_file(tree, EXPORT_FILENAME);
    if (result == TREE_NO_ERROR)
    {
        speak_print_with_variable_number_of_parameters("Tree successfully saved to %s\n", EXPORT_FILENAME);
    }
    else
    {
        speak_print_with_variable_number_of_parameters("Error saving tree: %s\n", tree_error_translator(result));
    }
}


void handle_show_tree(tree_t* tree)
{
    set_game_state_background(STATE_SHOW_TREE);
    printf("\nTree Structure\n");
    tree_common_dump(tree);
}


void handle_object_definition(tree_t* tree)
{
    set_game_state_background(STATE_DEFINITION);
    give_object_definition(tree);
}


void handle_object_comparison(tree_t* tree)
{
    set_game_state_background(STATE_COMPARISON);
    compare_two_objects(tree);
}


void handle_exit_program(tree_t* tree)
{
    animate_question("Goodbye! Thanks for playing!");
    speak_print_with_variable_number_of_parameters("Goodbye!\n");
}


void handle_invalid_choice()
{
    animate_question("Invalid option. Please try again.");
    speak_print_with_variable_number_of_parameters("Invalid option. Please try again.\n");
}


void handle_menu_choice(tree_t* tree, int choice)
{
    switch (choice)
    {
        case 1: handle_play_game(tree);         break;
        case 2: handle_save_database(tree);     break;
        case 3: handle_show_tree(tree);         break;
        case 4: handle_object_definition(tree); break;
        case 5: handle_object_comparison(tree); break;
        case 6: handle_exit_program(tree);      break;
        default: handle_invalid_choice();       break;
    }
}


int get_user_choice()
{
    int choice = 0;

    print_menu();
    if (scanf("%d", &choice) != 1)
    {
        clear_input_buffer();
        printf("Invalid input. Please try again.\n");

        return 0;
    }
    clear_input_buffer();

    return choice;
}


void run_akinator_loop(tree_t* tree)
{
    int choice = 0;

    do
    {
        set_game_state_background(STATE_MAIN_MENU);

        choice = get_user_choice();
        if (choice == 0)
            continue;

        handle_menu_choice(tree, choice);

    } while (choice != 6);
}


void save_before_exit(tree_t* tree)
{
    animate_question("Saving database before exit...");
    speak_print_with_variable_number_of_parameters("Saving database before exit...\n");
    save_tree_to_file(tree, DEFAULT_DATABASE);
}


void cleanup_akinator_app(tree_t* tree)
{
    save_before_exit(tree);
    tree_destructor(tree);
    close_graphics();

    printf("Graphics closed successfully\n");
}
