/**
 * @file main.c
 * @author Félix Desmaretz 
 * @brief 
 * @version 0.1
 * @date 2019-03-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <time.h>
#include <stdlib.h>

#include "msg.h"
#include "test.h"

int main(int argc, char const *argv[])
{
    srand(time(NULL));

    test_msg();

    return 0;
}
