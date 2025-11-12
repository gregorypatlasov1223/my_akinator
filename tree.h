#ifndef TREE_H_
#define TREE_H_

#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include "tree_error_type.h"

#define MAX_LENGTH_OF_ADDRESS 32
#define MAX_LENGTH_OF_ANSWER 256
#define MAX_LENGTH_OF_FILENAME 256
#define MAX_LENGTH_OF_SYSTEM_COMMAND 512

#define OPERATION_FAILED       0
#define COMPLETED_SUCCESSFULLY 1

#define BASE_EDGE_LENGTH 1.0
#define DEPTH_SPREAD_FACTOR 0.5
#define ZERO_RANK 0

typedef struct node_t
{
    char* question;
    struct node_t* yes;
    struct node_t* no;
    struct node_t* parent;
} node_t;

typedef struct tree_t
{
    node_t* root;
    size_t size;
} tree_t;

// Основные функции дерева
tree_error_type tree_constructor(tree_t* tree);
tree_error_type tree_destructor(tree_t* tree);
tree_error_type tree_verify(tree_t* tree);
tree_error_type tree_common_dump(tree_t* tree);

// Функции работы с узлами
tree_error_type tree_create_node(node_t** node_ptr, const char* phrase);
tree_error_type tree_set_parent(node_t* child, node_t* parent);
tree_error_type tree_split_node(tree_t* tree, node_t* old_node, const char* feature, const char* new_object);
void format_node_part(char* part_buffer, size_t buffer_size, const char* label, node_t* child_node);

// Функции сохранения и вывода
tree_error_type save_tree_to_file_recursive(const node_t* node, FILE* file);
tree_error_type save_tree_to_file(const tree_t* tree, const char* filename);
tree_error_type print_tree_node(const node_t* node);

// Акинатор
void print_menu();
char* string_to_lower_copy(const char* string);
int contains_negative_words(const char* string);
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
const char* tree_error_translator(tree_error_type error);
tree_error_type tree_destroy_recursive(node_t* node);
size_t count_nodes_recursive(node_t* node);

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
tree_error_type create_dot_file_tree(tree_t* tree, const char* filename);
tree_error_type create_graph_visualization_tree(tree_t* tree, FILE* htm_file, const char* folder_name, time_t now);
tree_error_type tree_dump_to_htm(tree_t* tree, FILE* htm_file, const char* folder_name);

#endif // TREE_H_
