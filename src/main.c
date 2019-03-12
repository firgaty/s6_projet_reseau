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

int main(int argc, char const *argv[])
{
    srand(time(NULL));

    tlv_t *t = gen_tlv_padn(10);
    print_tlv(t);

    printf("%d: %c ; %d\n", PADN, (unsigned char)PADN, (int)((unsigned char)PADN));

    return 0;
}
