/*
 * File:   test.c
 * Author: cthrash
 *
 * Created on September 4, 2016, 7:04 PM
 */


#include "esos.h"
#include "esos_pic24.h"

void user_init(void) {
    
}

ESOS_USER_TASK(hello_world) {
    ESOS_TASK_BEGIN();
    ESOS_TASK_END();
}
