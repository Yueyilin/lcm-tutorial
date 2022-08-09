# 前言：
基于LCM通讯协议，使用`nanopb`工具编译`.proto`文件，生成`.pb.c`和`.pb.h`文件并使用他们进行消息收发
源码已上传Github：
假定已经具有以下方面的基础

- protobuf
- lcm
- nanopb

目录结构
```cmake
.
├── build
├── CMakeLists.txt
└── modules
    ├── app
    │   ├── my_lcm_pub.c
    │   └── my_lcm_sub.c
    └── common
        └── data_struct
            └── proto
                └── data_struct.proto

```
```protobuf
syntax = "proto2";

message MySimpleMessage {
  optional int32 my_number = 1;
}
```
```c
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
```
```c
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
```


