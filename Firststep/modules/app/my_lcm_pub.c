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
#include <pb_encode.h>
#include <pb_decode.h>
#include "data_struct.pb.h"

int main()
{
    uint8_t mysimplemessagebuf[MySimpleMessage_size];
    memset(mysimplemessagebuf, 0, MySimpleMessage_size);
    size_t message_length;
    char channel_name[] = "channel";
    lcm_t *lcm_pub = lcm_create(NULL);
    if (!lcm_pub) return 1;
    MySimpleMessage message;
    // while (1){
        message.my_number = 15;
        pb_ostream_t enc_stream = pb_ostream_from_buffer(mysimplemessagebuf, MySimpleMessage_size);
        if (!pb_encode(&enc_stream, MySimpleMessage_fields, &message)) {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&enc_stream));
        } else {
            message_length = enc_stream.bytes_written;
            // printf("Encoding succeed! \n");
        }
        lcm_publish(lcm_pub, channel_name, (mysimplemessagebuf), message_length);
    // }
    return 0;
}
