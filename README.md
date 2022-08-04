# 介绍
LCM（Lightweight Communications and Marshalling）是一组用于消息传递和数据编组的库和工具，其基于UDP传输的属性，传输速度较快，其目标是高带宽和低延迟的实时系统。它提供了一种发布/订阅消息传递模型以及带有各种编程语言C++、Java、python等应用程序绑定的自动编组/解组代码生成，LCM通过将消息封装在不同的Channel中进行通信，这点类似于ROS中的Topic。

- LCM 是一个包，旨在允许多个进程以安全和高性能的方式交换消息。
- **消息（message）**是LCM通信的基本单元：它表示一条独立的信息。
   - 消息被定义为与编程语言无关的数据结构;
   - lcm-gen 工具将这些定义编译为特定于语言的代码。
- 每条消息都在一个**通道（channel）**上发送，该通道由人类可读的名称标识。例如，包含有关走廊温度信息的消息可能会发布在“HALLWAY_TEMPERATURE”通道上。按照约定，通道上的所有消息都具有相同的类型。
   - 任何应用程序都可以在任何通道上**发布（Publish）**，尽管单个应用程序通常充当通道上的唯一数据源。
   - 任何应用程序都可以在任何通道上**接收（Subscribe）**数据，例如，恒温器应用程序和数据记录器都可以订阅“HALLWAY_TEMPERATURE”通道。

本教程将引导您完成在两个应用程序之间交换消息的主要任务：

1. 创建类型定义
1. 在应用程序中初始化 LCM
1. 发布消息
1. 订阅并接收消息

# 下载
源代码版本可以从 [https://github.com/lcm-proj/lcm/releases](https://github.com/lcm-proj/lcm/releases) 获得。
也可以通过克隆 git 存储库来构建最新的开发版本，[https://github.com/lcm-proj/lcm.git](https://github.com/lcm-proj/lcm.git)。
# 构建
需求的包:

- build-essential
- libglib2.0-dev
- cmake

可选包 (e.g., for language-specific support or building documentation):

- default-jdk
- python-all-dev
- liblua5.1-dev
- golang
- doxygen
- python-epydoc

终端运行以下指令：
```c
sudo apt install build-essential
sudo apt install libglib2.0-dev
sudo apt-get install openjdk-8-jdk
sudo apt install cmake

sudo git clone https://github.com/lcm-proj/lcm.git

cd lcm
mkdir build
cd build
cmake ..
make
sudo make install
```
# 使用（基于C++）
源码在我的Github上：[链接](https://github.com/Yueyilin/lcm-tutorial)
用于快速入门的小例子。
## 创建消息类型定义
在两个应用程序之间交换消息时，您可能有许多不同类型的数据。
LCM 允许您将这些类型定义为与语言无关的数据结构。
您可以有多个字段，每个字段都有自己的类型和名称。
其中一些字段可能是结构本身或数组。LCM 支持多种语言，并且类型是在看起来与 C 非常相似的语言中性规范中定义的。
定义 my_lcm_message_date_struct_t.lcm  文件，包含以下内容：
```c
package mylcm;
struct my_lcm_message_date_struct_t
{
    int64_t  timestamp;
    double   position[3];
    double   orientation[4]; 
    int32_t  num_ranges;
    int16_t  ranges[num_ranges];
    string   name;
    boolean  enabled;
}
```
该文件相当简单，由两部分组成：

1. 包名称  package name
1. 结构定义 structure definition

该包为数据结构定义了一个命名空间mylcm，并映射到适当的语言（例如，C++中的命名空间 namespace）。
结构定义是数据字段的列表，每个字段都有一个类型和一个名称。
有许多基本类型可供使用，其中一些如上所示。[LCM 类型规范](https://lcm-proj.github.io/type_specification.html#type_specification_primitives)具有基本类型的完整说明。
尽管此示例中未显示，但您可以通过在结构定义中引用任何其他 LCM 类型来构建更复杂的类型。examples/LCM 源代码分发中的目录包含更多示例类型定义。此功能和其他功能都在[LCM 类型规范](https://lcm-proj.github.io/type_specification.html)中进行了更详细的描述。
## 在应用程序中初始化 LCM
使用下表中一行中列出的参数运行lcm-gen，为所选编程语言生成绑定。

| Language | lcm-gen usage |
| --- | --- |
| C | lcm-gen -c example_t.lcm |
| C++ | lcm-gen -x example_t.lcm |
| Java | lcm-gen -j example_t.lcm |
| Python | lcm-gen -p example_t.lcm |
| MATLAB | Generate Java code |

运行 lcm-gen -h 以获取其可用选项的完整列表。
之前已经定义好了my_lcm_message_date_struct_t.lcm
因为后面是C++使用，所以，打开终端运行
```c
lcm-gen -x my_lcm_message_date_struct_t.lcm
```
生成了一个mylcm的文件夹，其中有my_lcm_message_date_struct_t.hpp
之后创建my_lcm_sub.cpp、my_lcm_pub.cpp、my_lcm_readlog.cpp
```c
.
├── mylcm
│   └── my_lcm_message_date_struct_t.hpp
├── my_lcm_message_date_struct_t.lcm
├── my_lcm_pub.cpp
├── my_lcm_readlog.cpp
├── my_lcm_sub.cpp
└── CMakeLists.txt
```
## 发布消息
```cpp
#include <lcm/lcm-cpp.hpp>
#include "mylcm/my_lcm_message_date_struct_t.hpp"
int main(int argc, char ** argv)
{
    lcm::LCM lcm("udpm://239.255.76.67:7667?ttl=1");
    if(!lcm.good())
        return 1;
    mylcm::my_lcm_message_date_struct_t my_lcm_data;
    my_lcm_data.timestamp = 0;
    my_lcm_data.position[0] = 1;
    my_lcm_data.position[1] = 2;
    my_lcm_data.position[2] = 3;
    my_lcm_data.orientation[0] = 1;
    my_lcm_data.orientation[1] = 0;
    my_lcm_data.orientation[2] = 0;
    my_lcm_data.orientation[3] = 0;
    my_lcm_data.num_ranges = 15;
    my_lcm_data.ranges.resize(my_lcm_data.num_ranges);
    for(int i = 0; i < my_lcm_data.num_ranges; i++)
        my_lcm_data.ranges[i] = i;
    my_lcm_data.name = "example string from computer1";
    my_lcm_data.enabled = true;
    lcm.publish("mychannel", &my_lcm_data);
    return 0;
}
```
## 订阅并接收消息
```cpp
#include <stdio.h>
#include <lcm/lcm-cpp.hpp>
#include "mylcm/my_lcm_message_date_struct_t.hpp"
class Handler 
{
    public:
        ~Handler() {}
        void handleMessage(const lcm::ReceiveBuffer* rbuf,
                const std::string& chan, 
                const mylcm::my_lcm_message_date_struct_t* msg)
        {
            int i;
            printf("在 \"%s\" 通道上接收消息:\n", chan.c_str());
            printf("  timestamp   = %lld\n", (long long)msg->timestamp);
            printf("  position    = (%f, %f, %f)\n",
                    msg->position[0], msg->position[1], msg->position[2]);
            printf("  orientation = (%f, %f, %f, %f)\n",
                    msg->orientation[0], msg->orientation[1], 
                    msg->orientation[2], msg->orientation[3]);
            printf("  ranges:");
            for(i = 0; i < msg->num_ranges; i++)
                printf(" %d", msg->ranges[i]);
            printf("\n");
            printf("  name        = '%s'\n", msg->name.c_str());
            printf("  enabled     = %d\n", msg->enabled);
        }
};
int main(int argc, char** argv)
{
    lcm::LCM lcm("udpm://239.255.76.67:7667?ttl=1");
    if(!lcm.good())
        return 1;
    Handler handlerObject;
    lcm.subscribe("mychannel", &Handler::handleMessage, &handlerObject);
    while(0 == lcm.handle());
    return 0;
}

```
## 读取日志
```cpp
#include <stdio.h>
#include <lcm/lcm-cpp.hpp>
#include "mylcm/my_lcm_message_date_struct_t.hpp"

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: read_log <logfile>\n");
        return 1;
    }

    // Open the log file.
    lcm::LogFile log(argv[1], "r");
    if (!log.good()) {
        perror("LogFile");
        fprintf(stderr, "couldn't open log file %s\n", argv[1]);
        return 1;
    }

    while (1) {
        // Read a log event.
        const lcm::LogEvent *event = log.readNextEvent();
        if (!event)
            break;

        // Only process messages on the EXAMPLE channel.
        if (event->channel != "mychannel")
            continue;

        // Try to decode the message.
        mylcm::my_lcm_message_date_struct_t msg;
        if (msg.decode(event->data, 0, event->datalen) != event->datalen)
            continue;

        // Decode success!  Print out the message contents.
        printf("消息:\n");
        printf("  timestamp   = %lld\n", (long long) msg.timestamp);
        printf("  position    = (%f, %f, %f)\n", msg.position[0], msg.position[1], msg.position[2]);
        printf("  orientation = (%f, %f, %f, %f)\n", msg.orientation[0], msg.orientation[1],
               msg.orientation[2], msg.orientation[3]);
        printf("  ranges:");
        for (int i = 0; i < msg.num_ranges; i++)
            printf(" %d", msg.ranges[i]);
        printf("\n");
        printf("  name        = '%s'\n", msg.name.c_str());
        printf("  enabled     = %d\n", msg.enabled);
    }

    // Log file is closed automatically when the log variable goes out of
    // scope.

    printf("done\n");
    return 0;
}

```


```cpp
cmake_minimum_required(VERSION 3.1)

project(my_lcm_test)

find_package(lcm REQUIRED)

add_executable(my_lcm_sub my_lcm_sub.cpp)

add_executable(my_lcm_pub my_lcm_pub.cpp)

add_executable(my_lcm_readlog my_lcm_readlog.cpp)

target_link_libraries(my_lcm_sub lcm)

target_link_libraries(my_lcm_pub lcm)

target_link_libraries(my_lcm_readlog lcm)
```
## 编译
```cpp
mkdir build
cd build
cmake ..
make
```
## 测试
在不同的终端依次订阅和发布
```cpp
cd build
./my_lcm_sub
```
```cpp
./my_lcm_pub
./my_lcm_pub
./my_lcm_pub
```
```cpp
在 "mychannel" 通道上接收消息:
  timestamp   = 0
  position    = (1.000000, 2.000000, 3.000000)
  orientation = (1.000000, 0.000000, 0.000000, 0.000000)
  ranges: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14
  name        = 'example string from computer1'
  enabled     = 1

```
想要读取日志，应先在终端使用以下命令记录log，之后运行几次./my_lcm_pub发送消息
```cpp
lcm-logger
```
然后会报错
```cpp
lcm-logger: error while loading shared libraries: liblcm.so.1: cannot open shared object file: No such file or directory
```
我们需要为lcm创建一个ld.so.conf文件
```cpp
export LCM_INSTALL_DIR=/usr/local/lib 
sudo sh -c "echo $LCM_INSTALL_DIR > /etc/ld.so.conf.d/lcm.conf"
sudo ldconfig
```
配置pkgconfig
```cpp
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$LCM_INSTALL_DIR/pkgconfig
```
重新lcm-logger
```cpp
lcm-logger
```
然后发布订阅若干次，然后停止log
随后在当前目录下可以看到以当前时间命名的log数据包，如lcmlog-2022-07-15.00
打开一个终端，只需运行my_lcm_readlog来读取log数据包。
```cpp
./my_lcm_readlog lcmlog-2022-07-15.00
```
在终端可以显示读取的log包数据
```cpp
消息:
  timestamp   = 0
  position    = (1.000000, 2.000000, 3.000000)
  orientation = (1.000000, 0.000000, 0.000000, 0.000000)
  ranges: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14
  name        = 'example string from computer1'
  enabled     = 1
done
```

# 使用（基于C）
和上面的例子相似，不过多赘述
结构
```cpp
.
├── bin
├── build
├── CMakeLists.txt
├── mylcm
│   ├── CMakeLists.txt
│   ├── lcm_gen.sh
│   ├── my_lcm_message_date_struct_t.lcm
│   ├── mylcm_my_lcm_message_date_struct_t.c
│   └── mylcm_my_lcm_message_date_struct_t.h
├── my_lcm_pub.c
└── my_lcm_sub.c
```
```c
package mylcm;
struct my_lcm_message_date_struct_t
{
    int64_t  timestamp;
    double   position[3];
    double   orientation[4]; 
    int32_t  num_ranges;
    int16_t  ranges[num_ranges];
    string   name;
    boolean  enabled;
}
```
然后运行`lcm-gen -c my_lcm_message_date_struct_t.lcm`生成对应的.c和.h文件，略
```c
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
```
```c
#include <inttypes.h>
#include <lcm/lcm.h>
#include <stdio.h>
#include "mylcm/mylcm_my_lcm_message_date_struct_t.h"

static void my_handler(const lcm_recv_buf_t *rbuf, const char *channel, const mylcm_my_lcm_message_date_struct_t *msg,
                       void *user)
{
    int i;
    printf("Received message on channel \"%s\":\n", channel);
    printf("  timestamp   = %" PRId64 "\n", msg->timestamp);
    printf("  position    = (%f, %f, %f)\n", msg->position[0], msg->position[1], msg->position[2]);
    printf("  orientation = (%f, %f, %f, %f)\n", msg->orientation[0], msg->orientation[1],
           msg->orientation[2], msg->orientation[3]);
    printf("  ranges:");
    for (i = 0; i < msg->num_ranges; i++)
        printf(" %d", msg->ranges[i]);
    printf("\n");
    printf("  name        = '%s'\n", msg->name);
    printf("  enabled     = %d\n", msg->enabled);
}

int main(int argc, char **argv)
{
    lcm_t *lcm = lcm_create(NULL);
    if (!lcm)
        return 1;
    
    mylcm_my_lcm_message_date_struct_t_subscribe(lcm, "mychannel", &my_handler, NULL);
    
    while (1)
        lcm_handle(lcm);
    
    lcm_destroy(lcm);
    return 0;
}
```
两个CMakeLists.txt如下
```cpp
project(libmylcm) 

# Add a library with the above sources
add_library(libmylcm mylcm_my_lcm_message_date_struct_t.c)

target_include_directories( libmylcm
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    
)
```
```c
cmake_minimum_required(VERSION 3.5)
project(my_lcm_c_test)
find_package(lcm REQUIRED)
add_subdirectory(mylcm)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_executable(my_lcm_sub my_lcm_sub.c)
target_link_libraries(my_lcm_sub lcm libmylcm)
add_executable(my_lcm_pub my_lcm_pub.c)
target_link_libraries(my_lcm_pub lcm libmylcm)
```
## 编译
和上边的例子一样，略
## 测试
和上边的例子一样，略
# 进阶
## [lcm_t](https://lcm-proj.github.io/group__LcmC__lcm__t.html#gabb730c9e49442a4bcf400e0f2fef7576)
包含 LCM 内容的不透明数据结构。
## 接口
### [lcm_creat()](https://lcm-proj.github.io/group__LcmC__lcm__t.html#gaf29963ef43edadf45296d5ad82c18d4b)
### [lcm_publish()](https://lcm-proj.github.io/group__LcmC__lcm__t.html#gad5e45f05d1fcdf446901f20426b5a6cc)
### [lcm_subscribe()](https://lcm-proj.github.io/group__LcmC__lcm__t.html#ga1c8521ce6e87cbbeb7bfd935d5c0d724)
### [lcm_handle()](https://lcm-proj.github.io/group__LcmC__lcm__t.html#ga1c8521ce6e87cbbeb7bfd935d5c0d724)


# 参考文献
[官方文档](https://lcm-proj.github.io/tutorial_general.html)
[C++API](https://lcm-proj.github.io/group__LcmCpp.html)
[Github源码](https://github.com/lcm-proj/lcm)
[LCM通信库的安装与使用](https://blog.csdn.net/weixin_45467056/article/details/123569027?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_baidulandingword~default-0-123569027-blog-81222219.pc_relevant_multi_platform_whitelistv1&spm=1001.2101.3001.4242.1&utm_relevant_index=3)
[自动驾驶消息传输机制LCM的安装与使用](https://blog.csdn.net/snail_zcx/article/details/104108081)
