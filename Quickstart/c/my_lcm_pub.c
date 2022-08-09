// file: send_message.c
//
// LCM example program.
//
// compile with:
//  $ gcc -o send_message send_message.c -llcm
//
// On a system with pkg-config, you can also use:
//  $ gcc -o send_message send_message.c `pkg-config --cflags --libs lcm`

#include <lcm/lcm.h>
#include <stdio.h>

#include "mylcm/mylcm_my_lcm_message_date_struct_t.h"

int main(int argc, char **argv)
{
    lcm_t *lcm = lcm_create(NULL);
    if (!lcm)
        return 1;

    mylcm_my_lcm_message_date_struct_t my_data = {
        .timestamp = 0, .position = {1, 2, 3}, .orientation = {1, 0, 0, 0},
    };
    int16_t ranges[15];
    int i;
    for (i = 0; i < 15; i++)
        ranges[i] = i;

    my_data.num_ranges = 15;
    my_data.ranges = ranges;
    my_data.name = "example string";
    my_data.enabled = 1;

    mylcm_my_lcm_message_date_struct_t_publish(lcm, "mychannel", &my_data);

    lcm_destroy(lcm);
    return 0;
}
