#ifndef AKINATOR_APP_H_
#define AKINATOR_APP_H_

#include "tree.h"
#include "tree_error_type.h"


bool initialize_akinator_app(void);
bool load_or_create_database(tree_t* tree);
void handle_play_game(tree_t* tree);
void handle_save_database(tree_t* tree);
void handle_show_tree(tree_t* tree);
void handle_object_definition(tree_t* tree);
void handle_object_comparison(tree_t* tree);
void handle_exit_program(tree_t* tree);
void handle_invalid_choice();
void handle_menu_choice(tree_t* tree, int choice);
int get_user_choice();
void run_akinator_loop(tree_t* tree);
void save_before_exit(tree_t* tree);
void cleanup_akinator_app(tree_t* tree);

#endif // AKINATOR_APP_H_
