#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "speech.h"
#include "graphics.h"
#include "tree_tests.h"
#include "akinator_app.h"
#include "tree_error_type.h"

int main()
{
    if (!initialize_akinator_app())
        return OPERATION_FAILED;

    tree_t tree = {};
    if (!load_or_create_database(&tree))
    {
        close_graphics();
        return OPERATION_FAILED;
    }

    run_akinator_loop(&tree);

    cleanup_akinator_app(&tree);

    return 0;
}
