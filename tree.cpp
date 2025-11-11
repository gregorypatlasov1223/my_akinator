#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

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
        default:                         return "Unknown error";
    }
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
// result_of_operation is_whole_word(const char* string, const char* word, size_t position, int word_length)
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


// result_of_operation contains_negative(const char* string)
// {
//     if (string == NULL)
//         return OPERATION_FAILED;
//
//     if (strstr(string, " no ")    != NULL ||
//         strstr(string, " not ")   != NULL ||
//         strstr(string, " never ") != NULL ||
//         strstr(string, "no ")     == string ||
//         strstr(string, "not ")    == string ||
//         strstr(string, "never ")  == string)
//     {
//         return COMPLETED_SUCCESSFULLY;
//     }
//
//     return OPERATION_FAILED;
// }


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
    fprintf(file, "\n");
    fclose(file);
    return TREE_NO_ERROR;
}


void print_menu()
{
    printf("\n=== AKINATOR GAME ===\n");
    printf("1. Play game\n");
    printf("2. Save tree to file\n");
    printf("3. Show tree structure\n");
    printf("4. Exit\n");
    printf("Choose option: ");
}


void clear_input_buffer()
{
    int symbol = 0;
    while ((symbol = getchar()) != '\n' && symbol != EOF) { }
}


tree_error_type akinator_play(tree_t* tree)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    node_t* current = tree -> root;
    char answer[MAX_LENGTH_OF_ANSWER] ={};

    while (current -> yes != NULL && current -> no != NULL)
    {
        printf("%s? (yes/no): ", current -> question);
        scanf("%s", answer);

        if (strcmp(answer, "yes") == 0)
            current = current -> yes;
        else
            current = current -> no;
    }

        printf("Is it %s? (yes/no): ", current -> question);
        scanf("%s", answer);

        if (strcmp(answer, "yes") == 0)
        {
            printf("AI wins!\n");
            return TREE_NO_ERROR;
        }
        else
        {
            char new_object[MAX_LENGTH_OF_ANSWER] = {};
            char feature[MAX_LENGTH_OF_ANSWER]    = {};

            printf("Who was it?: ");
            scanf(" %[^\n]", new_object);

            printf("How is %s different? It... ", new_object);
            scanf(" %[^\n]", feature);

            tree_split_node(tree, current, feature, new_object);

            printf("I'll remember that!\n");
            return TREE_NO_ERROR;
        }

        return TREE_NO_ERROR;

    }


// ============================GRAPHIC_DUMP===========================================

void write_dump_header(FILE* htm_file, time_t now)
{
    fprintf(htm_file, "<div style='border:2px solid #ccc; margin:10px; padding:15px; background:#f9f9f9;'>\n");
    fprintf(htm_file, "<h2 style='color:#333;'>Tree Dump at %s</h2>\n", ctime(&now));
}


void write_information_about_tree(FILE* htm_file, tree_t* tree)
{
    assert(tree     != NULL);
    assert(htm_file != NULL);

    fprintf(htm_file, "<div style='margin-bottom:15px;'>\n");
    fprintf(htm_file, "<p><b>Tree size:</b> %zu</p>\n", tree -> size);
    fprintf(htm_file, "<p><b>Root address:</b> %p</p>\n", (void*)tree -> root); // для чего тут void
    fprintf(htm_file, "</div>\n");
}


tree_error_type write_tree_nodes_table_recursive(node_t* node, FILE* htm_file)
{
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


tree_error_type create_dot_tree_recursive(tree_t* tree, node_t* node, FILE* dot_file, int level)
{
    if (node == NULL)
        return TREE_ERROR_NULL_PTR;

    const char* fill_color = is_root_node(tree, node) ? "lightblue" : "white";
    const char* shape = "Mrecord";

    char yes_part[MAX_LENGTH_OF_ADDRESS] = "O";
    char no_part[MAX_LENGTH_OF_ADDRESS]  = "O";

    if (node -> yes != NULL)
        snprintf(yes_part, sizeof(yes_part), "YES: %p", (void*)node -> yes);

    if (node -> no != NULL)
        snprintf(no_part, sizeof(no_part), "NO: %p", (void*)node -> no);

    fprintf(dot_file, "    node_%p [label=\"{%s | {<f0> %s | <f1> %s}}\", shape=%s, style=filled, fillcolor=%s, color=black];\n",
           (void*)node, node -> question, yes_part, no_part, shape, fill_color);

    double distance = BASE_EDGE_LENGTH + (level * DEPTH_SPREAD_FACTOR); // distance - min расстояние между узлом и листом

     if (node -> yes != NULL)
    {
        fprintf(dot_file, "    node_%p:<f0> -> node_%p [color=green, minlen=%.1f, label=\"YES\"];\n",
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

    char command[MAX_LENGTH_OF_SYSTEM_COMMAND] = {};
    snprintf(command, sizeof(command), "dot -Tsvg %s -o %s", temp_dot, temp_svg);
    int result = system(command);

    if (result == 0)
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


tree_error_type tree_dump(tree_t* tree, const char* filename)
{
    assert(tree     != NULL);
    assert(filename != NULL);

    char folder_name[MAX_LENGTH_OF_FILENAME] = {};
    snprintf(folder_name, sizeof(folder_name), "%s_dump", filename);

    char command[MAX_LENGTH_OF_SYSTEM_COMMAND] = {};
    // флаг -p гарантирует, что папка создастся даже если родительские директории не существуют
    snprintf(command, sizeof(command), "mkdir -p %s", folder_name); // mkdir - это команда для создания директорий в OC
    system(command);

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





