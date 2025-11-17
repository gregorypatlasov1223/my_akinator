#include <TXLib.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "tree.h"
#include "speech.h"
#include "graphics.h"
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
        speak_print_with_variable_number_of_parameters("Tree is NULL");
        return TREE_ERROR_NULL_PTR;
    }

    speak_print_with_variable_number_of_parameters("TREE DUMP");
    speak_print_with_variable_number_of_parameters("Tree size = %zu", tree -> size);

    tree_error_type verify_result = tree_verify(tree);
    speak_print_with_variable_number_of_parameters("Tree verification: %s", tree_error_translator(verify_result));

    speak_print_with_variable_number_of_parameters("Tree structure:");
    if (tree -> root == NULL)
        speak_print_with_variable_number_of_parameters("EMPTY TREE");
    else
        print_tree_node(tree -> root);

    putchar('\n');
    return verify_result;
}


char* string_to_lower_copy(const char* str)
{
    if (str == NULL)
        return NULL;

    char* lower_string = strdup(str);
    if (lower_string == NULL)
        return NULL;

    // приводим всю строку к нижнему регистру
    for (char* letter = lower_string; *letter; letter++)
    {
        *letter = (char)tolower((unsigned char)*letter);
    }

    return lower_string;
}


int contains_negative_words(const char* str)
{
    if (str == NULL)
        return OPERATION_FAILED;

    const char* forbidden_phrases[] = {"do not", "is not", "does not", "did not",
        "don't", "isn't", "doesn't", "didn't"};

    size_t number_of_phrases = sizeof(forbidden_phrases) / sizeof(forbidden_phrases[0]);

    char* lower_input = string_to_lower_copy(str);

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
            speak_print_with_variable_number_of_parameters("%s", input_message);

        fgets(buffer, (int)buffer_size, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // убираем символ \n который fgets() добавляет в конец введенной строки

        if (!contains_negative_words(buffer))
            valid = true;
        else
            speak_print_with_variable_number_of_parameters("Please avoid negative phrases. Try again: ");
    }
}


void validate_yes_no_input(char* answer, size_t answer_size)
{
    while (strcmp(answer, "yes") != 0 && strcmp(answer, "no") != 0)
    {
        speak_print_with_variable_number_of_parameters("Please answer only 'yes' or 'no': ");
        get_input_without_negatives("", answer, answer_size);
    }
}


node_t* ask_questions_until_leaf(node_t* current, char* answer, size_t answer_size)
{
    assert(answer  != NULL);
    assert(current != NULL);

    while (current -> yes != NULL && current -> no != NULL)
    {
        animate_question(current -> question);

        speak_print_with_variable_number_of_parameters("%s? (yes/no): ", current -> question);
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

    animate_question("Who was it?: ");
    get_input_without_negatives("Who was it?: ", new_object, sizeof(new_object));

    animate_question("What is the distinguishing feature?");
    speak_print_with_variable_number_of_parameters("How is %s different? It...", new_object);

    get_input_without_negatives("Enter the distinguishing feature: ", feature, sizeof(feature));
    tree_split_node(tree, current_node, feature, new_object);

    speak_print_with_variable_number_of_parameters("Great! I'll remember that for next time!");

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
    animate_question("Akinator Game Menu");

    speak_print_with_variable_number_of_parameters("\nAKINATOR GAME\n");
    speak_print_with_variable_number_of_parameters("1. Play game\n");
    speak_print_with_variable_number_of_parameters("2. Save tree to file\n");
    speak_print_with_variable_number_of_parameters("3. Show tree structure\n");
    speak_print_with_variable_number_of_parameters("4. Give definition\n");
    speak_print_with_variable_number_of_parameters("5. Compare two objects\n");
    speak_print_with_variable_number_of_parameters("6. Exit\n");
    speak_print_with_variable_number_of_parameters("Choose option: ");
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
        speak_print_with_variable_number_of_parameters("Error: No tree or object specified.\n");
        return TREE_ERROR_NULL_PTR;
    }

    *found_node = find_leaf_by_phrase(tree -> root, object);
    if (*found_node == NULL)
    {
        speak_print_with_variable_number_of_parameters("Object \"%s\" not found in the database.\n", object);
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
            path[*step_count].answer = true;

        else if (parent -> no == current)
            path[*step_count].answer = false;

        else
        {
            speak_print_with_variable_number_of_parameters("Error: The tree structure is broken.\n");
            return TREE_ERROR_STRUCTURE;
        }

        path[*step_count].question_node = parent;

        (*step_count)++;
        current = parent;
    }

    return TREE_NO_ERROR;
}


void print_definition(const path_step* path, int step_count)
{
    for (int i = step_count - 1; i >= 0; i--)
    {
        if (!path[i].answer)
            speak_print_with_variable_number_of_parameters("not ");

        speak_print_with_variable_number_of_parameters("%s", path[i].question_node -> question);

        if (i > 0)
            speak_print_with_variable_number_of_parameters(", ");
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

    if (found == NULL)
    {
        speak_print_with_variable_number_of_parameters("Object \"%s\" not found in the database.\n", object);
        return TREE_NO_ERROR;
    }


    // TODO: stack
    path_step path[MAX_PATH_DEPTH] = {};
    int step_count = 0;

    tree_error_type path_result = build_path_from_leaf_to_root(found, path, &step_count);
    if (path_result != TREE_NO_ERROR)
        return path_result;

    if (step_count == 0)
    {
        speak_print_with_variable_number_of_parameters("This is the root object: %s\n", found -> question);
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
        speak_print_with_variable_number_of_parameters("The tree is not initialized!\n");
        return;
    }

    char object_name[MAX_LENGTH_OF_ANSWER] = {};

    animate_question("Which object would you like me to describe?");
    speak_print_with_variable_number_of_parameters("Which object would you like me to describe?");
    get_input_without_negatives(" Enter the name of the object to search for: ",
                                 object_name, sizeof(object_name));

    speak_print_with_variable_number_of_parameters("Here's what I know about %s \n", object_name);

    print_object_path(tree, object_name);
}


void move_position_until_get_not_space(const char** position)
{
    assert(position  != NULL);
    assert(*position != NULL);

    while (isspace((unsigned char)**position))
        (*position)++;
}


tree_error_type check_symbol(const char** position, char expected_symbol)
{
    assert(position  != NULL);
    assert(*position != NULL);

    if (**position != expected_symbol)
        return TREE_ERROR_SYNTAX;

    (*position)++;

    return TREE_NO_ERROR;
}


tree_error_type read_child_node(const char** position, node_t** parent, node_t** child)
{
    assert(parent    != NULL);
    assert(*parent   != NULL);
    assert(position  != NULL);
    assert(*position != NULL);

    tree_error_type result = read_node(position, child);
    if (result != TREE_NO_ERROR)
    {
        tree_destroy_recursive(*parent);
        *parent = NULL;
        return result;
    }

    if (*child != NULL)
        (*child) -> parent = *parent;

    move_position_until_get_not_space(position);
    return TREE_NO_ERROR;
}


tree_error_type read_nil_node(const char** position, node_t** node)
{
    move_position_until_get_not_space(position);

    if (strncmp(*position, "nil", 3) == 0)
    {
        *position += 3;
        *node = NULL;
        return TREE_NO_ERROR;
    }

    return TREE_ERROR_SYNTAX;
}


tree_error_type create_node_and_read_children(const char** position, node_t** node, const char* phrase)
{
    tree_error_type result = tree_create_node(node, phrase);
    if (result != TREE_NO_ERROR)
        return result;

    result = read_child_node(position, node, &((*node) -> yes));
    if (result != TREE_NO_ERROR)
        return result;

    result = read_child_node(position, node, &((*node) -> no));
    if (result != TREE_NO_ERROR)
        return result;

    return check_symbol(position, ')');
}


tree_error_type read_phrase_in_quote(const char** position, char* phrase_buffer)
{
    assert(position      != NULL);
    assert(*position     != NULL);
    assert(phrase_buffer != NULL);

    tree_error_type result = check_symbol(position, '"');
    if (result != TREE_NO_ERROR)
        return result;

    int chars_read = 0;

    if (sscanf(*position, "%[^\"]%n", phrase_buffer, &chars_read) != 1)
        return TREE_ERROR_SYNTAX;

    (*position) += chars_read;

    check_symbol(position, '"');

    move_position_until_get_not_space(position);

    return TREE_NO_ERROR;
}


tree_error_type read_node(const char** position, node_t** node)
{
    assert(position  != NULL);
    assert(*position != NULL);

    if (node == NULL)
        return TREE_ERROR_NULL_PTR;

    move_position_until_get_not_space(position);

    if (**position == '(')
    {
        (*position)++;

        move_position_until_get_not_space(position);

        char phrase[MAX_LENGTH_OF_ANSWER] = {};

        tree_error_type result = read_phrase_in_quote(position, phrase);
        if (result != TREE_NO_ERROR)
            return result;

        return create_node_and_read_children(position, node, phrase);
    }

    return read_nil_node(position, node);
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


tree_error_type read_file_to_buffer(const char* filename, char** buffer)
{
    assert(buffer    != NULL);
    assert(filename  != NULL);

    FILE* file = fopen(filename, "r");
    if (file == NULL)
        return TREE_ERROR_OPENING_FILE;

    size_t file_size = get_file_size(file);
    if (file_size == 0)
    {
        fclose(file);
        speak_print_with_variable_number_of_parameters("Error: File is empty or cannot get file size\n");
        return TREE_ERROR_OPENING_FILE;
    }

    char* local_buffer = (char*)calloc(file_size + 1, sizeof(char));
    if (local_buffer == NULL)
    {
        fclose(file);
        return TREE_ERROR_ALLOCATION;
    }

    size_t bytes_read = fread(local_buffer, sizeof(char), file_size, file);
    if (bytes_read != file_size)
        speak_print_with_variable_number_of_parameters("Warning: Read only %zu bytes out of %zu\n", bytes_read, file_size);

    local_buffer[bytes_read] = '\0';
    fclose(file);

    *buffer = local_buffer;

    return TREE_NO_ERROR;
}


tree_error_type validate_no_extra_chars(const char* position, node_t* tree_root)
{
    assert(position  != NULL);
    assert(tree_root != NULL);

    move_position_until_get_not_space(&position);
    if (*position != '\0')
    {
        tree_destroy_recursive(tree_root);
        speak_print_with_variable_number_of_parameters("Syntax error: extra characters after tree: '%s'\n", position);
        return TREE_ERROR_SYNTAX;
    }

    return TREE_NO_ERROR;
}


void replace_tree(tree_t* tree, node_t* new_root)
{
    assert(tree     != NULL);
    assert(new_root != NULL);

    tree_destructor(tree);
    tree -> root = new_root;
    tree -> size = count_nodes_recursive(new_root);
}


tree_error_type load_tree_from_file(tree_t* tree, const char* filename)
{
    assert(tree     != NULL);
    assert(filename != NULL);

    char* buffer = NULL;

    tree_error_type result = read_file_to_buffer(filename, &buffer);
    if (result != TREE_NO_ERROR)
        return result;

    const char* position = buffer;
    node_t* new_root = NULL;

    result = read_node(&position, &new_root);

    if (result == TREE_NO_ERROR)
        result = validate_no_extra_chars(position, new_root);

    free(buffer);

    if (result != TREE_NO_ERROR)
    {
        speak_print_with_variable_number_of_parameters("Error loading tree from file: %s\n", tree_error_translator(result));
        return result;
    }

    replace_tree(tree, new_root);
    return TREE_NO_ERROR;
}


void print_comparison_results(const char* object1, const char* object2,
                              path_step* path1, int steps1,
                              path_step* path2, int steps2,
                              int common_steps)
{
    assert(path1   != NULL);
    assert(path2   != NULL);
    assert(object1 != NULL);
    assert(object2 != NULL);

    speak_print_with_variable_number_of_parameters("Common features: ");
    if (common_steps == 0)
        speak_print_with_variable_number_of_parameters("none\n");
    else
        print_definition(&path1[steps1 - common_steps], common_steps);

    speak_print_with_variable_number_of_parameters("\n%s has unique features: ", object1);
    if (steps1 - common_steps == 0)
        speak_print_with_variable_number_of_parameters("none\n");
    else
        print_definition(path1, steps1 - common_steps);

    speak_print_with_variable_number_of_parameters("%s has unique features: ", object2);
    if (steps2 - common_steps == 0)
        speak_print_with_variable_number_of_parameters("none\n");
    else
        print_definition(path2, steps2 - common_steps);
}


int find_common_steps(path_step* path1, int steps1, path_step* path2, int steps2)
{
    int common_steps = 0;
    int min_steps = (steps1 < steps2) ? steps1 : steps2;

    for (int i = 0; i < min_steps; i++)
    {
        int index1 = steps1 - 1 - i;
        int index2 = steps2 - 1 - i;

        if (path1[index1].question_node == path2[index2].question_node &&
            path1[index1].answer == path2[index2].answer)
        {
            common_steps++;
        }
        else
        {
            break;
        }
    }

    return common_steps;
}


tree_error_type find_common_and_different_features(tree_t* tree, const char* object1, const char* object2)
{
    assert(object1 != NULL);
    assert(object2 != NULL);

    node_t* found1 = NULL, *found2 = NULL;
    find_and_validate_object(tree, object1, &found1);
    find_and_validate_object(tree, object2, &found2);

    if (found1 == NULL || found2 == NULL)
    {
        speak_print_with_variable_number_of_parameters("One or both objects not found.\n");
        return TREE_NO_ERROR;
    }

    path_step path1[MAX_PATH_DEPTH] = {};
    path_step path2[MAX_PATH_DEPTH] = {};
    int steps1 = 0, steps2 = 0;

    build_path_from_leaf_to_root(found1, path1, &steps1);
    build_path_from_leaf_to_root(found2, path2, &steps2);

    int common_steps = find_common_steps(path1, steps1, path2, steps2);

    print_comparison_results(object1, object2, path1, steps1, path2, steps2, common_steps);

    return TREE_NO_ERROR;
}


void compare_two_objects(tree_t* tree)
{
    if (tree == NULL)
    {
        speak_print_with_variable_number_of_parameters("Error: Tree is not initialized.\n");
        return;
    }

    char object1[MAX_LENGTH_OF_ANSWER] = {};
    char object2[MAX_LENGTH_OF_ANSWER] = {};

    animate_question("Enter first object");
    get_input_without_negatives("Enter first object: ",  object1, sizeof(object1));

    animate_question("Enter second object");
    get_input_without_negatives("Enter second object: ", object2, sizeof(object2));

    find_common_and_different_features(tree, object1, object2);
}


tree_error_type akinator_play(tree_t* tree)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    node_t* current = tree -> root;
    char answer[MAX_LENGTH_OF_ANSWER] = {};

    speak_print_with_variable_number_of_parameters("Let's play! I'll try to guess your object.");
    printf("\n");
    // проходим по дереву вопросов
    current = ask_questions_until_leaf(current, answer, sizeof(answer));

    speak_print_with_variable_number_of_parameters("Is it %s?\n", current -> question);

    get_input_without_negatives("", answer, sizeof(answer));

    validate_yes_no_input(answer, sizeof(answer));

    if (strcmp(answer, "yes") == 0)
    {
        speak_print_with_variable_number_of_parameters("AI wins!");
        speak_print_with_variable_number_of_parameters("Hooray! I won!");
        return TREE_NO_ERROR;
    }
    else
    {
        speak_print_with_variable_number_of_parameters("Okay, I was wrong. Let me learn!");
        return learn_new_object(tree, current);
    }

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


tree_error_type create_tree_dot_header(FILE* dot_file)
{
    if (dot_file == NULL)
        return TREE_ERROR_NULL_PTR;

    fprintf(dot_file, "digraph AkinatorTree {\n");
    fprintf(dot_file, "    rankdir=TB;\n");
    fprintf(dot_file, "    node [shape=Mrecord, color=black];\n\n");
    fprintf(dot_file, "    graph [nodesep=0.5, ranksep=1.0];\n");
    fprintf(dot_file, "    edge [arrowsize=0.8];\n\n");

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

    tree_error_type header_result = create_tree_dot_header(dot_file);
    if (header_result != TREE_NO_ERROR)
    {
        fclose(dot_file);
        return header_result;
    }

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

    snprintf(temp_dot, sizeof(temp_dot), "%s/tree_temp_%d%lld.dot", folder_name, number_of_pictures, (long long)now);
    snprintf(temp_svg, sizeof(temp_svg), "%s/tree_temp_%d%lld.svg", folder_name, number_of_pictures, (long long)now);
    number_of_pictures++;

    tree_error_type dot_result = create_dot_file_tree(tree, temp_dot);
    if (dot_result != TREE_NO_ERROR)
        return dot_result;

    execute_graphviz_command(temp_dot, temp_svg);

    fprintf(htm_file, "<div style='text-align:center;'>\n");
    fprintf(htm_file, "<img src='%s' style='max-width:100%%; border:1px solid #ddd;'>\n", temp_svg);
    fprintf(htm_file, "</div>\n");

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
    snprintf(command, sizeof(command), "mkdir \"%s\" 2>nul", folder_name); // для винды
    // snprintf(command, sizeof(command), "mkdir -p \"%s\"", folder_name); // для wsl

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
