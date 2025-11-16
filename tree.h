#ifndef TREE_H_
#define TREE_H_

#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include "tree_error_type.h"

#define MAX_LENGTH_OF_ADDRESS 128
#define MAX_LENGTH_OF_ANSWER 1024
#define MAX_PATH_DEPTH 512
#define MAX_LENGTH_OF_FILENAME 256
#define MAX_LENGTH_OF_SYSTEM_COMMAND 512

#define OPERATION_FAILED       0
#define COMPLETED_SUCCESSFULLY 1

#define BASE_EDGE_LENGTH 1.0
#define DEPTH_SPREAD_FACTOR 0.5
#define ZERO_RANK 0

struct node_t
{
    char* question;
    node_t* yes;
    node_t* no;
    node_t* parent;
};

struct tree_t
{
    node_t* root;
    size_t size;
};

struct path_step
{
    node_t* question_node;
    bool answer; // true да, false нет
};

// Основные функции дерева
tree_error_type tree_constructor(tree_t* tree);
tree_error_type tree_destructor(tree_t* tree);
tree_error_type tree_verify(tree_t* tree);
tree_error_type tree_common_dump(tree_t* tree);

// Функции работы с узлами
bool is_leaf(node_t* node);
tree_error_type tree_create_node(node_t** node_ptr, const char* phrase);
tree_error_type tree_set_parent(node_t* child, node_t* parent);
tree_error_type tree_split_node(tree_t* tree, node_t* old_node, const char* feature, const char* new_object);
node_t* find_leaf_by_phrase(node_t* node, const char* phrase);
tree_error_type find_and_validate_object(tree_t* tree, const char* object, node_t** found_node);
tree_error_type build_path_from_leaf_to_root(node_t* leaf, path_step* path, int* step_count);
void print_definition(const path_step* path, int step_count);
tree_error_type print_object_path(tree_t* tree, const char* object);
void give_object_definition(tree_t* tree);
tree_error_type find_common_and_different_features(tree_t* tree, const char* object1, const char* object2);
void print_comparison_results(const char* object1, const char* object2,
                              path_step* path1, int steps1,
                              path_step* path2, int steps2,
                              int common_steps);
int find_common_steps(path_step* path1, int steps1, path_step* path2, int steps2);
void compare_two_objects(tree_t* tree);
void format_node_part(char* part_buffer, size_t buffer_size, const char* label, node_t* child_node);

// Функции сохранения и вывода
tree_error_type save_tree_to_file_recursive(const node_t* node, FILE* file);
tree_error_type save_tree_to_file(const tree_t* tree, const char* filename);
tree_error_type print_tree_node(const node_t* node);
void move_position_until_get_not_space(const char** position);
tree_error_type check_symbol(const char** position, char expected_symbol);
tree_error_type read_child_node(const char** position, node_t** parent, node_t** child);
tree_error_type read_nil_node(const char** position, node_t** node);
tree_error_type create_node_and_read_children(const char** position, node_t** node, const char* phrase);
tree_error_type read_phrase_in_quote(const char** position, char* phrase_buffer);
tree_error_type read_node(const char** position, node_t** node);
tree_error_type read_file_to_buffer(const char* filename, char** buffer);
tree_error_type validate_no_extra_chars(const char* position, node_t* tree_root);
void replace_tree(tree_t* tree, node_t* new_root);
tree_error_type load_tree_from_file(tree_t* tree, const char* filename);


// Акинатор
void print_menu();
char* string_to_lower_copy(const char* str);
int contains_negative_words(const char* str);
void get_input_without_negatives(const char* input_message, char* buffer, size_t buffer_size);
void validate_yes_no_input(char* answer, size_t answer_size);
node_t* ask_questions_until_leaf(node_t* current, char* answer, size_t answer_size);
tree_error_type learn_new_object(tree_t* tree, node_t* current_node);
void clear_input_buffer();
tree_error_type akinator_play(tree_t* tree);

// Функции дампа и логирования
tree_error_type tree_dump(tree_t* tree, const char* filename);
tree_error_type initialization_of_tree_log(const char* filename);
tree_error_type close_tree_log(const char* filename);

// Вспомогательные функции
// void speak_print_with_variable_number_of_parameters(const char* format, ...);
const char* tree_error_translator(tree_error_type error);
tree_error_type tree_destroy_recursive(node_t* node);
size_t count_nodes_recursive(node_t* node);
size_t get_file_size(FILE *file);

// Функции для дампа
void write_dump_header(FILE* htm_file, time_t now);
void write_information_about_tree(FILE* htm_file, tree_t* tree);
tree_error_type execute_graphviz_command(const char* input_file, const char* output_file);
tree_error_type make_folder_name(const char* base_name, char* folder_name, size_t folder_name_size);
tree_error_type make_directory(const char* folder_name);
tree_error_type write_tree_nodes_table_recursive(node_t* node, FILE* htm_file);
void write_tree_nodes_table(FILE* htm_file, tree_t* tree);
int is_root_node(tree_t* tree, node_t* node);
tree_error_type create_dot_tree_recursive(tree_t* tree, node_t* node, FILE* dot_file, int level);
tree_error_type create_tree_dot_header(FILE* dot_file);
tree_error_type create_dot_file_tree(tree_t* tree, const char* filename);
tree_error_type create_graph_visualization_tree(tree_t* tree, FILE* htm_file, const char* folder_name, time_t now);
tree_error_type tree_dump_to_htm(tree_t* tree, FILE* htm_file, const char* folder_name);

#endif // TREE_H_
