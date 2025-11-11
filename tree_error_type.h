#ifndef TREE_ERROR_TYPE_H_
#define TREE_ERROR_TYPE_H_

enum tree_error_type
{
    TREE_NO_ERROR            = 1,
    TREE_ERROR_ALLOCATION    = 2,
    TREE_ERROR_NULL_PTR      = 3,
    TREE_ERROR_CONSTRUCTOR   = 4,
    TREE_ERROR_OPENING_FILE  = 5,
    TREE_ERROR_SIZE_MISMATCH = 6,
};

#endif // TREE_ERROR_TYPE_H_
