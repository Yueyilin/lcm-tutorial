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
