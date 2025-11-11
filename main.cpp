#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "tree_tests.h"
#include "tree_error_type.h"

int main()
{
//     tree_t tree = {};
//     tree_error_type result = tree_constructor(&tree);
//
//     if (result != TREE_NO_ERROR)
//     {
//         printf("Error initializing tree: %s\n", tree_error_translator(result));
//         return result;
//     }
//
//     printf("=== Welcome to Akinator! ===\n");
//     printf("Think of an animal or object, and I will try to guess it!\n");
//
//     int choice = 0;
//     char filename[MAX_LENGTH_OF_FILENAME] = "akinator_tree.txt";
//
//
//     do
//     {
//         print_menu();
//         if (scanf("%d", &choice) != 1)
//         {
//             clear_input_buffer();
//             printf("Invalid input. Please try again.\n");
//             choice = 0;  //сбрасываем choice
//             continue;
//         }
//     clear_input_buffer();
//
//         switch (choice)
//         {
//             case 1:
//                 printf("\n=== Starting Game ===\n");
//                 printf("Think of something, and I'll try to guess it!\n");
//                 printf("Answer with 'yes' or 'no' to my questions.\n\n");
//
//                 result = akinator_play(&tree);
//                 if (result != TREE_NO_ERROR)
//                 {
//                     printf("Game error: %s\n", tree_error_translator(result));
//                 }
//                 break;
//
//             case 2:
//                 result = save_tree_to_file(&tree, filename);
//                 if (result == TREE_NO_ERROR)
//                 {
//                     printf("Tree successfully saved to %s\n", filename);
//                 }
//                 else
//                 {
//                     printf("Error saving tree: %s\n", tree_error_translator(result));
//                 }
//                 break;
//
//             case 3:
//                 printf("\n=== Tree Structure ===\n");
//                 tree_common_dump(&tree);
//                 break;
//
//             case 4:
//                 printf("Goodbye!\n");
//                 break;
//
//             default:
//                 printf("Invalid option. Please try again.\n");
//                 break;
//         }
//
//     } while (choice != 4);
//
//     tree_destructor(&tree);
//     return 0;

    all_tests();
}
