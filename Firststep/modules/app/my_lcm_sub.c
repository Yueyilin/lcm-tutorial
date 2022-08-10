// file: listener.c
//
// LCM example program.
//
// compile with:
//  $ gcc -o listener listener.c -llcm
//
// On a system with pkg-config, you can also use:
//  $ gcc -o listener listener.c `pkg-config --cflags --libs lcm`

#include <inttypes.h>
#include <lcm/lcm.h>
#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "data_struct.pb.h"


static MySimpleMessage message;

void sub_handler(const lcm_recv_buf_t* rbuf, const char* channel, void* user_data)
{
    pb_istream_t dec_stream;
    int rxlen = 0;
    dec_stream = pb_istream_from_buffer((uint8_t*)rbuf->data, rbuf->data_size);
    if (!pb_decode(&dec_stream, MySimpleMessage_fields, &message)) {
        printf("chassis pb decode error in %s\n", __func__);
    } else {
        printf("Your lucky number was %d!\n", (int)message.my_number);
    }
}

int main(int argc, char **argv)
{
    lcm_t *lcm_sub = lcm_create(NULL);
    char channel_name[] = "channel";
    if (!lcm_sub) return 1;
    lcm_subscribe(lcm_sub, channel_name, &sub_handler, NULL);
    while (1) {
        lcm_handle_timeout(lcm_sub, 0);
    }
    // lcm_destroy(lcm);
    return 0;
}
