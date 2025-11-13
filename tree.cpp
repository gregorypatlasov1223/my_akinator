#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "tree.h"
#include "tree_error_type.h"

const char* tree_error_translator(tree_error_type error)
{
    switch(error)
    {
        case TREE_NO_ERROR:              return "There is no error";
        case TREE_ERROR_ALLOCATION:      return "Incorrect memory allocation";
        case TREE_ERROR_NULL_PTR:        return "A null pointer is used";
        case TREE_ERROR_CONSTRUCTOR:     return "Error in the constructor";
        case TREE_ERROR_OPENING_FILE:    return "Error when opening a file";
        case TREE_ERROR_SIZE_MISMATCH:   return "Tree size doesn't match actual node count";
        case TREE_ERROR_STRUCTURE:       return "The tree structure is broken";
        case TREE_ERROR_SYNTAX:          return "Get unexpected symbol";
        default:                         return "Unknown error";
    }
}


bool is_leaf(node_t* node)
{
    return node != NULL && node -> no == NULL && node -> yes == NULL;
}


size_t count_nodes_recursive(node_t* node)
{
    if (node == NULL)
        return 0;

    return count_nodes_recursive(node -> yes) + count_nodes_recursive(node -> no) + 1;
}


tree_error_type tree_verify(tree_t* tree)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    size_t actual_size = count_nodes_recursive(tree -> root);

    if (actual_size != tree -> size)
        return TREE_ERROR_SIZE_MISMATCH;

    return TREE_NO_ERROR;
}


tree_error_type tree_destroy_recursive(node_t* node)
{
    if (node == NULL)
        return TREE_NO_ERROR; // для рекурсии это норм

    tree_destroy_recursive(node -> yes);
    tree_destroy_recursive(node -> no);

    free(node -> question);
    free(node);

    return TREE_NO_ERROR;
}


tree_error_type print_tree_node(const node_t* node)
{
    if (node == NULL)
    {
        printf("nil");
        return TREE_NO_ERROR;
    }

    printf("(\"%s\" ", node -> question);

    print_tree_node(node -> yes);
    printf(" ");
    print_tree_node(node -> no);

    printf(")");

    return TREE_NO_ERROR;
}


tree_error_type tree_set_parent(node_t* child, node_t* parent)
{
    if (child == NULL)
        return TREE_NO_ERROR; // для parent нужна проверка?

    child -> parent = parent;

    return TREE_NO_ERROR;
}


tree_error_type tree_create_node(node_t** node_ptr, const char* phrase)
{
    assert(phrase != NULL);

    if (node_ptr == NULL)
        return TREE_ERROR_NULL_PTR;;

    *node_ptr = (node_t*)calloc(1, sizeof(node_t));
    if (*node_ptr == NULL)
        return TREE_ERROR_ALLOCATION;

    (*node_ptr) -> question = strdup(phrase);
    if ((*node_ptr) -> question == NULL)
    {
        free(*node_ptr);
        return TREE_ERROR_ALLOCATION;
    }

    (*node_ptr) -> yes    = NULL;
    (*node_ptr) -> no     = NULL;
    (*node_ptr) -> parent = NULL;

    return TREE_NO_ERROR;
}


tree_error_type tree_split_node(tree_t* tree, node_t* old_node, const char* feature, const char* new_object)
{
    assert(tree       != NULL);
    assert(feature    != NULL);
    assert(old_node   != NULL);
    assert(new_object != NULL);

    char* old_object = strdup(old_node -> question);
    if (old_object == NULL)
        return TREE_ERROR_ALLOCATION;

    free(old_node -> question);
    old_node -> question = strdup(feature);

    if (old_node -> question == NULL)
    {
        free(old_object);
        return TREE_ERROR_ALLOCATION;
    }

    tree_error_type result_yes = tree_create_node(&(old_node -> yes), new_object);
    if (result_yes != TREE_NO_ERROR)
    {
        free(old_object);
        return result_yes;
    }

    tree_error_type result_no = tree_create_node(&(old_node -> no), old_object);
    free(old_object);

    if (result_no != TREE_NO_ERROR)
    {
        tree_destroy_recursive(old_node -> yes);
        old_node -> yes = NULL;
        return result_no;
    }

    tree_set_parent(old_node -> yes, old_node);
    tree_set_parent(old_node -> no,  old_node);

    tree -> size += 2;

    return TREE_NO_ERROR;
}


tree_error_type tree_constructor(tree_t* tree)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    tree -> root = NULL;
    tree -> size = 0;

    tree_error_type result = tree_create_node(&(tree -> root), "nothing");
    if (result == TREE_NO_ERROR)
    {
        tree -> size = 1; // корневой узел
    }

    return result;
}


tree_error_type tree_destructor(tree_t* tree)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    tree_destroy_recursive(tree -> root);

    tree -> root = NULL;
    tree -> size = 0;

    return TREE_NO_ERROR;
}


tree_error_type tree_common_dump(tree_t* tree)
{
    if (tree == NULL)
    {
        printf("Tree is NULL");
        return TREE_ERROR_NULL_PTR;
    }

    printf("=====TREE DUMP=====\n");
    printf("Tree size = %zu\n", tree -> size);

    tree_error_type verify_result = tree_verify(tree);
    printf("Tree verification: %s\n", tree_error_translator(verify_result));

    printf("Tree structure:\n");
    if (tree -> root == NULL)
        printf("EMPTY TREE\n");
    else
        print_tree_node(tree -> root);

    putchar('\n');
    return verify_result;
}


// // Функция для проверки, является ли подстрока отдельным словом
// int is_whole_word(const char* string, const char* word, size_t position, int word_length)
// {
//     if (position > 0 && !isspace(string[position-1]))
//         return OPERATION_FAILED;
//
//     if (string[position + word_length] != '\0' &&
//         !isspace((unsigned char)string[position + word_length]))
//         return OPERATION_FAILED;
//
//     return COMPLETED_SUCCESSFULLY;
// }


char* string_to_lower_copy(const char* string)
{
    if (string == NULL)
        return NULL;

    char* lower_string = strdup(string);
    if (lower_string == NULL)
        return NULL;

    // Приводим всю строку к нижнему регистру
    for (char* letter = lower_string; *letter; letter++)
    {
        *letter = tolower((unsigned char)*letter);
    }

    return lower_string;
}


int contains_negative_words(const char* string)
{
    if (string == NULL)
        return OPERATION_FAILED;

    const char* forbidden_phrases[] = {"do not", "is not", "does not", "did not",
        "don't", "isn't", "doesn't", "didn't"};

    size_t number_of_phrases = sizeof(forbidden_phrases) / sizeof(forbidden_phrases[0]);

    char* lower_input = string_to_lower_copy(string);

    if (lower_input == NULL)
        return OPERATION_FAILED;

    int found_phrases = 0;

    for (size_t i = 0; i < number_of_phrases; i++)
    {
        if (strstr(lower_input, forbidden_phrases[i]) != NULL)
        {
            found_phrases = 1;
            break;
        }
    }

    free(lower_input);
    return found_phrases;
}


void get_input_without_negatives(const char* input_message, char* buffer, size_t buffer_size)
{
    assert(buffer        != NULL);
    assert(input_message != NULL);

    bool valid = false;

    while (!valid)
    {
        if (input_message != NULL)
            printf("%s", input_message);

        fgets(buffer, (int)buffer_size, stdin);
        // TODO: strlen here
        buffer[strcspn(buffer, "\n")] = '\0'; // убираем символ \n который fgets() добавляет в конец введенной строки

        if (!contains_negative_words(buffer))
            valid = true;
        else
            printf("Please avoid negative phrases. Try again: ");
    }
}


void validate_yes_no_input(char* answer, size_t answer_size)
{
    while (strcmp(answer, "yes") != 0 && strcmp(answer, "no") != 0)
    {
        printf("Please answer only 'yes' or 'no': ");
        get_input_without_negatives("", answer, answer_size);
    }
}


node_t* ask_questions_until_leaf(node_t* current, char* answer, size_t answer_size)
{
    assert(answer  != NULL);
    assert(current != NULL);

    while (current -> yes != NULL && current -> no != NULL)
    {
        printf("%s? (yes/no): ", current -> question);
        get_input_without_negatives("", answer, answer_size);

        validate_yes_no_input(answer, answer_size);

        if (strcmp(answer, "yes") == 0)
            current = current -> yes;
        else
            current = current -> no;
    }

    return current;
}


tree_error_type learn_new_object(tree_t* tree, node_t* current_node)
{
    assert(tree         != NULL); // по-хорошему верификатор
    assert(current_node != NULL);

    char new_object[MAX_LENGTH_OF_ANSWER] = {};
    char feature[MAX_LENGTH_OF_ANSWER]    = {}; // ответ

    get_input_without_negatives("Who was it?: ", new_object, sizeof(new_object));

    char feature_input_message[MAX_LENGTH_OF_ANSWER] = {}; // приглашение для ввода признака
    snprintf(feature_input_message, sizeof(feature_input_message),
             "How is %s different? It... ", new_object);

    get_input_without_negatives(feature_input_message, feature, sizeof(feature));
    tree_split_node(tree, current_node, feature, new_object);

    printf("I'll remember that!\n");
    return TREE_NO_ERROR;
}


tree_error_type save_tree_to_file_recursive(const node_t* node, FILE* file)
{
    if (node == NULL)
    {
        fprintf(file, "nil");
        return TREE_NO_ERROR;
    }

    fprintf(file, "(\"%s\" ", node -> question);
    save_tree_to_file_recursive(node -> yes, file);
    fprintf(file, " ");
    save_tree_to_file_recursive(node -> no, file);
    fprintf(file, ")");

    return TREE_NO_ERROR;
}


tree_error_type save_tree_to_file(const tree_t* tree, const char* filename)
{
    assert(tree     != NULL);
    assert(filename != NULL);

    FILE* file = fopen(filename, "w");
    if (file == NULL)
        return TREE_ERROR_OPENING_FILE;

    save_tree_to_file_recursive(tree -> root, file);

    fclose(file);

    return TREE_NO_ERROR;
}


void print_menu()
{
    printf("\n=== AKINATOR GAME ===\n");
    printf("1. Play game\n");
    printf("2. Save tree to file\n");
    printf("3. Show tree structure\n");
    printf("4. Give definition\n");
    printf("5. Exit\n");
    printf("Choose option: ");
}


void clear_input_buffer()
{
    int symbol = 0;
    while ((symbol = getchar()) != '\n' && symbol != EOF);
}


node_t* find_leaf_by_phrase(node_t* node, const char* phrase)
{
    if (node == NULL)
        return NULL;

    if (is_leaf(node))
    {
        char* phrase_lower      = string_to_lower_copy(phrase);
        char* node_phrase_lower = string_to_lower_copy(node -> question);

        if (phrase_lower == NULL || node_phrase_lower == NULL)
        {
            free(node_phrase_lower);
            free(phrase_lower);

            return NULL;
        }

        int compare_result = strcmp(phrase_lower, node_phrase_lower);
        free(node_phrase_lower);
        free(phrase_lower);

        if (compare_result == 0)
            return node;
    }

    node_t* yes_result = find_leaf_by_phrase(node -> yes, phrase);
    if (yes_result != NULL)
        return yes_result;

    node_t* no_result = find_leaf_by_phrase(node -> no, phrase);
    return no_result;
}


tree_error_type find_and_validate_object(tree_t* tree, const char* object, node_t** found_node)
{
    if (tree == NULL || object == NULL)
    {
        printf("Error: No tree or object specified.\n");
        return TREE_ERROR_NULL_PTR;
    }

    *found_node = find_leaf_by_phrase(tree -> root, object);
    if (*found_node == NULL)
    {
        printf("Object \"%s\" not found in the database.\n", object);
        return TREE_NO_ERROR;
    }

    return TREE_NO_ERROR;
}


tree_error_type build_path_from_leaf_to_root(node_t* leaf, path_step* path, int* step_count)
{
    if (leaf == NULL)
        return TREE_ERROR_NULL_PTR;

    *step_count = 0;
    node_t* current = leaf;

    // Поднимаемся от листа к корню
    while (current -> parent != NULL && *step_count < MAX_PATH_DEPTH)
    {
        node_t* parent = current -> parent;

        if (parent -> yes == current)
        {
            path[*step_count].question_node = parent;
            path[*step_count].answer = true;
        }
        else if (parent -> no == current)
        {
            path[*step_count].question_node = parent;
            path[*step_count].answer = false;
        }
        else
        {
            printf("Error: The tree structure is broken.\n");
            return TREE_ERROR_STRUCTURE;
        }

        (*step_count)++;
        current = parent;
    }

    return TREE_NO_ERROR;
}


void print_definition(const path_step* path, int step_count)
{
    printf("The definition:\n");
    for (int i = step_count - 1; i >= 0; i--)
    {
        if (path[i].answer)
            printf("%s", path[i].question_node -> question);
        else
            printf("not %s", path[i].question_node -> question);

        if (i > 0)
            printf(", ");
    }
    printf("\n");
}


tree_error_type print_object_path(tree_t* tree, const char* object)
{
    assert(tree   != NULL);
    assert(object != NULL);

    node_t* found = NULL;
    tree_error_type validation_result = find_and_validate_object(tree, object, &found);

    if (validation_result != TREE_NO_ERROR)
        return validation_result;

    // Добавляем эту проверку
    if (found == NULL)
    {
        printf("Object \"%s\" not found in the database.\n", object);
        return TREE_NO_ERROR;
    }

    path_step path[MAX_PATH_DEPTH] = {};
    int step_count = 0;

    tree_error_type path_result = build_path_from_leaf_to_root(found, path, &step_count);
    if (path_result != TREE_NO_ERROR)
        return path_result;

    if (step_count == 0)
    {
        printf("This is the root object: %s\n", found -> question);
    }
    else
    {
        print_definition(path, step_count);
    }

    return TREE_NO_ERROR;
}


void give_object_definition(tree_t* tree)
{
    if (tree == NULL || tree -> root == NULL)
    {
        printf("The tree is not initialized!\n");
        return;
    }

    char object_name[MAX_LENGTH_OF_ANSWER] = {};
    get_input_without_negatives("Enter the name of the object to search for: ",
                                 object_name, sizeof(object_name));

    print_object_path(tree, object_name);
}


tree_error_type read_node(const char** position, node_t** node)
{
    assert(position  != NULL);
    assert(*position != NULL);

    if (node == NULL)
        return TREE_ERROR_NULL_PTR;

    while (isspace((unsigned char)**position))
        (*position)++;

    if (**position == '(')
    {
        (*position)++;

        while (isspace((unsigned char)**position))
            (*position)++;

        if (**position != '"')
            return TREE_ERROR_SYNTAX;
        (*position)++;

        int chars_read = 0;
        char phrase[MAX_LENGTH_OF_ANSWER] = {};

        if (sscanf(*position, "%[^\"]%n", phrase, &chars_read) != 1)
            return TREE_ERROR_SYNTAX;

        (*position) += chars_read;

        if (**position != '"')
            return TREE_ERROR_SYNTAX;
        (*position)++;

        while (isspace((unsigned char)**position))
            (*position)++;

        tree_error_type result = tree_create_node(node, phrase);
        if (result != TREE_NO_ERROR)
            return result;

        result = read_node(position, &((*node)->yes));
        if (result != TREE_NO_ERROR)
        {
            tree_destroy_recursive(*node);
            *node = NULL;
            return result;
        }

        if ((*node)->yes != NULL)
            (*node)->yes->parent = *node;

        while (isspace((unsigned char)**position))
            (*position)++;

        result = read_node(position, &((*node)->no));
        if (result != TREE_NO_ERROR)
        {
            tree_destroy_recursive(*node);
            *node = NULL;
            return result;
        }

        if ((*node)->no != NULL)
            (*node)->no->parent = *node;


        while (isspace((unsigned char)**position))
            (*position)++;

        if (**position != ')')
            return TREE_ERROR_SYNTAX;
        (*position)++;

        return TREE_NO_ERROR;
    }

    while (isspace((unsigned char)**position))
        (*position)++;

    if (strncmp(*position, "nil", 3) == 0)
    {
        *position += 3;
        *node = NULL;
        return TREE_NO_ERROR;
    }

    return TREE_ERROR_SYNTAX;
}


size_t get_file_size(FILE *file)
{
    assert(file != NULL);

    struct stat stat_buffer = {};

    int file_descriptor = fileno(file);
    if (file_descriptor == -1)
    {
        fprintf(stderr, "Error: Cannot get file descriptor\n");
        return 0;
    }

    if (fstat(file_descriptor, &stat_buffer) != 0)
    {
        fprintf(stderr, "Error: Cannot get file stats\n");
        return 0;
    }

    return (size_t)stat_buffer.st_size;
}


tree_error_type load_tree_from_file(tree_t* tree, const char* filename)
{
    assert(tree     != NULL);
    assert(filename != NULL);

    FILE* file = fopen(filename, "r");
    if (file == NULL)
        return TREE_ERROR_OPENING_FILE;

    size_t file_size = get_file_size(file);
    if (file_size == 0)
    {
        fclose(file);
        printf("Error: File is empty or cannot get file size\n");
        return TREE_ERROR_OPENING_FILE;
    }

    char* buffer = (char*)calloc(file_size + 1, sizeof(char)); // +1 для \0
    if (buffer == NULL)
    {
        fclose(file);
        return TREE_ERROR_ALLOCATION;
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read != file_size)
        printf("Warning: Read only %zu bytes out of %zu\n", bytes_read, file_size);

    buffer[bytes_read] = '\0';
    fclose(file);

    const char* position = buffer;
    node_t* new_root = NULL; // корень нового дерева

    tree_error_type result = read_node(&position, &new_root);

    if (result == TREE_NO_ERROR)
    {
        while (isspace((unsigned char)*position))
            position++;

        if (*position != '\0')
        {
            tree_destroy_recursive(new_root);
            result = TREE_ERROR_SYNTAX;
            printf("Syntax error: extra characters after tree: '%s'\n", position);
        }
    }

    free(buffer);

    if (result != TREE_NO_ERROR)
    {
        printf("Error loading tree from file: %s\n", tree_error_translator(result));
        return result;
    }

    // заменяем старое дерево новым
    tree_destructor(tree);
    tree -> root = new_root;
    tree -> size = count_nodes_recursive(new_root);

    printf("Tree successfully loaded from %s (%zu nodes)\n", filename, tree -> size);
    return TREE_NO_ERROR;
}


tree_error_type akinator_play(tree_t* tree)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    node_t* current = tree -> root;
    char answer[MAX_LENGTH_OF_ANSWER] = {};

    // Проходим по дереву вопросов
    current = ask_questions_until_leaf(current, answer, sizeof(answer));

    printf("Is it %s? (yes/no): ", current -> question);
    get_input_without_negatives("", answer, sizeof(answer));

    validate_yes_no_input(answer, sizeof(answer));

    if (strcmp(answer, "yes") == 0)
    {
        printf("AI wins!\n");
        return TREE_NO_ERROR;
    }
    else
        return learn_new_object(tree, current);

    return TREE_NO_ERROR;
}

// ============================GRAPHIC_DUMP===========================================

void write_dump_header(FILE* htm_file, time_t now)
{
    assert(htm_file != NULL);

    fprintf(htm_file, "<div style='border:2px solid #ccc; margin:10px; padding:15px; background:#f9f9f9;'>\n");
    fprintf(htm_file, "<h2 style='color:#333;'>Tree Dump at %s</h2>\n", ctime(&now));
}


void write_information_about_tree(FILE* htm_file, tree_t* tree)
{
    assert(tree     != NULL);
    assert(htm_file != NULL);

    fprintf(htm_file, "<div style='margin-bottom:15px;'>\n");
    fprintf(htm_file, "<p><b>Tree size:</b> %zu</p>\n", tree -> size);
    fprintf(htm_file, "<p><b>Root address:</b> %p</p>\n", (void*)tree -> root);
    fprintf(htm_file, "</div>\n");
}


tree_error_type write_tree_nodes_table_recursive(node_t* node, FILE* htm_file)
{
    assert(htm_file != NULL);
    if (node == NULL)
        return TREE_NO_ERROR;

    fprintf(htm_file, "<tr><td>%p</td><td>%s</td><td>%p</td><td>%p</td><td>%p</td></tr>\n",
                      (void*)node, node -> question, (void*)node -> yes, (void*)node -> no, (void*)node -> parent);

    tree_error_type result = TREE_NO_ERROR;

    result = write_tree_nodes_table_recursive(node -> yes,  htm_file);
    if (result != TREE_NO_ERROR) return result;

    result = write_tree_nodes_table_recursive(node -> no,  htm_file);
    if (result != TREE_NO_ERROR) return result;

    return TREE_NO_ERROR;
}


void write_tree_nodes_table(FILE* htm_file, tree_t* tree)
{
    assert(tree     != NULL);
    assert(htm_file != NULL);

    fprintf(htm_file, "<table border='1' style='border-collapse:collapse; width:100%%; margin-top:15px;'>\n");
    fprintf(htm_file, "<tr><th>Address</th><th>Question</th><th>Yes</th><th>No</th><th>Parent</th></tr>\n");

    if (tree -> root != NULL)
        write_tree_nodes_table_recursive(tree -> root, htm_file);

    fprintf(htm_file, "</table>\n");
}


int is_root_node(tree_t* tree, node_t* node)
{
    return (tree -> root == node) ? 1 : 0;
}


void format_node_part(char* part_buffer, size_t buffer_size, const char* label, node_t* child_node)
{
    assert(label       != NULL);
    assert(part_buffer != NULL);

    if (child_node == NULL)
        snprintf(part_buffer, buffer_size, "O");
    else
        snprintf(part_buffer, buffer_size, "%s: %p", label, (void*)child_node);
}


// tree_error_type create_dot_edge(FILE* dot_file, node_t* parent_node, node_t* child_node,
//                                const char* port, const char* colour, const char* label,
//                                double distance, tree_t* tree, int level)
// {
//     if (child_node == NULL)
//         return TREE_NO_ERROR;
//
//     fprintf(dot_file, "    node_%p:%s -> node_%p [color=%s, minlen=%.1f, label=\"%s\"];\n",
//                       (void*)parent_node, port, (void*)child_node, colour, distance, label);
//
//     return TREE_NO_ERROR;
// }


tree_error_type create_dot_tree_recursive(tree_t* tree, node_t* node, FILE* dot_file, int level)
{
    if (node == NULL)
        return TREE_ERROR_NULL_PTR;

    const char* fill_color = is_root_node(tree, node) ? "lightblue" : "white";
    const char* shape = "Mrecord";

    char yes_part[MAX_LENGTH_OF_ADDRESS] = {};
    char no_part[MAX_LENGTH_OF_ADDRESS]  = {};

    format_node_part(yes_part, sizeof(yes_part), "YES", node -> yes);
    format_node_part(no_part, sizeof(no_part), "NO", node -> no);

    fprintf(dot_file, "    node_%p [label=\"{%s | {<f0> %s | <f1> %s}}\", shape=%s, style=filled, fillcolor=%s, color=black];\n",
                      (void*)node, node -> question, yes_part, no_part, shape, fill_color);

    double distance = BASE_EDGE_LENGTH + (level * DEPTH_SPREAD_FACTOR); // distance - min расстояние между узлом и листом

    if (node -> yes != NULL)
    {
        fprintf(dot_file, "    node_%p:<f0> -> node_%p [colour=green, minlen=%.1f, label=\"YES\"];\n",
                          (void*)node, (void*)node -> yes, distance);
        create_dot_tree_recursive(tree, node -> yes, dot_file, level + 1);
    }

    if (node -> no != NULL)
    {
        fprintf(dot_file, "    node_%p:<f1> -> node_%p [color=red, minlen=%.1f, label=\"NO\"];\n",
                          (void*)node, (void*)node -> no, distance);
        create_dot_tree_recursive(tree, node -> no, dot_file, level + 1);
    }

    return TREE_NO_ERROR;
}


tree_error_type create_dot_file_tree(tree_t* tree, const char* filename)
{
    assert(filename != NULL);
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    FILE* dot_file = fopen(filename, "w");
    if (dot_file == NULL)
        return TREE_ERROR_OPENING_FILE;

    // TODO: create_tree_dot_header
    fprintf(dot_file, "digraph AkinatorTree {\n");
    fprintf(dot_file, "    rankdir=TB;\n");
    fprintf(dot_file, "    node [shape=Mrecord, color=black];\n\n");
    fprintf(dot_file, "    graph [nodesep=0.5, ranksep=1.0];\n");
    fprintf(dot_file, "    edge [arrowsize=0.8];\n\n");

    if (tree -> root == NULL)
        fprintf(dot_file, "    empty [label=\"Empty tree\"];\n");
    else
        create_dot_tree_recursive(tree, tree->root, dot_file, ZERO_RANK);

    fprintf(dot_file, "}\n");
    fclose(dot_file);

    return TREE_NO_ERROR;
}


tree_error_type execute_graphviz_command(const char* input_file, const char* output_file)
{
    assert(input_file  != NULL);
    assert(output_file != NULL);

    char command[MAX_LENGTH_OF_SYSTEM_COMMAND] = {};
    snprintf(command, sizeof(command), "dot -Tsvg \"%s\" -o \"%s\"", input_file, output_file);

    int result = system(command);
    if (result != 0)
        return TREE_ERROR_OPENING_FILE;

    return TREE_NO_ERROR;
}


tree_error_type create_graph_visualization_tree(tree_t* tree, FILE* htm_file, const char* folder_name, time_t now)
{
    assert(tree        != NULL);
    assert(htm_file    != NULL);
    assert(folder_name != NULL);

    static int number_of_pictures = 0;

    char temp_dot[MAX_LENGTH_OF_FILENAME] = {};
    char temp_svg[MAX_LENGTH_OF_FILENAME] = {};

    snprintf(temp_dot, sizeof(temp_dot), "%s/tree_temp_%d%ld.dot", folder_name, number_of_pictures, now);
    snprintf(temp_svg, sizeof(temp_svg), "%s/tree_temp_%d%ld.svg", folder_name, number_of_pictures, now);
    number_of_pictures++;

    tree_error_type dot_result = create_dot_file_tree(tree, temp_dot);
    if (dot_result != TREE_NO_ERROR)
        return dot_result;

    tree_error_type execution_result = execute_graphviz_command(temp_dot, temp_svg);

    if (execution_result == 0)
    {
        fprintf(htm_file, "<div style='text-align:center;'>\n");
        fprintf(htm_file, "<img src='%s' style='max-width:100%%; border:1px solid #ddd;'>\n", temp_svg);
        fprintf(htm_file, "</div>\n");
    }

    else
        fprintf(htm_file, "<p style='color:red;'>Error generating SVG graph</p>\n");

    remove(temp_dot);

    return TREE_NO_ERROR;
}


tree_error_type tree_dump_to_htm(tree_t* tree, FILE* htm_file, const char* folder_name)
{
    assert(tree != NULL);
    assert(htm_file != NULL);

    time_t now = time(NULL); // получаем текущее время

    write_dump_header(htm_file, now);
    write_information_about_tree(htm_file, tree);
    create_graph_visualization_tree(tree, htm_file, folder_name, now);
    write_tree_nodes_table(htm_file, tree);

    fprintf(htm_file, "</div>\n\n");

    return TREE_NO_ERROR;
}


tree_error_type make_folder_name(const char* base_name, char* folder_name, size_t folder_name_size)
{
    assert(base_name   != NULL);
    assert(folder_name != NULL);

    ssize_t written = snprintf(folder_name, folder_name_size, "%s_dump", base_name);
    if (written < 0 || (size_t)written >= folder_name_size)
        return TREE_ERROR_SIZE_MISMATCH;

    return TREE_NO_ERROR;
}


tree_error_type make_directory(const char* folder_name)
{
    if (folder_name == NULL)
        return TREE_ERROR_NULL_PTR;

    char command[MAX_LENGTH_OF_SYSTEM_COMMAND] = {};
    snprintf(command, sizeof(command), "mkdir -p \"%s\"", folder_name);

    int result = system(command);
    if (result != 0)
        return TREE_ERROR_OPENING_FILE;

    return TREE_NO_ERROR;
}


tree_error_type tree_dump(tree_t* tree, const char* filename)
{
    assert(tree     != NULL);
    assert(filename != NULL);

    char folder_name[MAX_LENGTH_OF_FILENAME] = {};

    make_folder_name(filename, folder_name, sizeof(folder_name));
    make_directory(folder_name);

    char htm_filename[MAX_LENGTH_OF_FILENAME] = {};
    snprintf(htm_filename, sizeof(htm_filename), "%s.htm", filename);

    FILE* htm_file = fopen(htm_filename, "a");
    if (htm_file == NULL)
        return TREE_ERROR_OPENING_FILE;

    tree_error_type result = tree_dump_to_htm(tree, htm_file, folder_name);

    fclose(htm_file);

    return result;
}


tree_error_type initialization_of_tree_log(const char* filename)
{
    assert(filename != NULL);

    char htm_filename[MAX_LENGTH_OF_FILENAME] = {};
    snprintf(htm_filename, sizeof(htm_filename), "%s.htm", filename);

    FILE* htm_file = fopen(htm_filename, "w");
    if (htm_file == NULL)
        return TREE_ERROR_OPENING_FILE;

    fprintf(htm_file, "<!DOCTYPE html>\n"
                      "<html>\n"
                      "<head>\n"
                      "<title>Tree Dump Log</title>\n"
                      "<style>\n"
                      "body { font-family: Arial, sans-serif; margin: 20px; }\n"
                      "table { border-collapse: collapse; width: 100%%; }\n"
                      "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
                      "th { background-color: #f2f2f2; }\n"
                      "</style>\n"
                      "</head>\n"
                      "<body>\n"
                      "<h1>Tree Dump Log</h1>\n");
    fclose(htm_file);

    return TREE_NO_ERROR;
}


tree_error_type close_tree_log(const char* filename)
{
    assert(filename != NULL);

    char htm_filename[MAX_LENGTH_OF_FILENAME] = {};
    snprintf(htm_filename, sizeof(htm_filename), "%s.htm", filename);

    FILE* htm_file = fopen(htm_filename, "a");
    if (htm_file == NULL)
        return TREE_ERROR_OPENING_FILE;

    fprintf(htm_file, "</body>\n");
    fprintf(htm_file, "</html>\n");
    fclose(htm_file);

    return TREE_NO_ERROR;
}

