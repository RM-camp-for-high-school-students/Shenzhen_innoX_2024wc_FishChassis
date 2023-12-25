/*Calculate can filter mask*/
#include "cstdio"
#include "iostream"
#include <cstdint> // Add missing include directive for cstdint header
using namespace std; // Fix typo in using statement

uint16_t std_id_list[] = {0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x208};
uint32_t ext_id_list[] = {0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x208};

int main() {
    //STDID with 11 bit data
    uint16_t mask = 0x7ff; // 下面开始计算屏蔽码
    uint16_t num = sizeof(std_id_list) / sizeof(std_id_list[0]);
    uint16_t tmp;
    for (uint16_t i = 0; i < num;
             i++) // 屏蔽码位StdIdArray[]数组中所有成员的同或结果
    {
        tmp = std_id_list[i] ^ (~std_id_list[0]); // 所有数组成员与第0个成员进行同或操作
        mask &= tmp;
    }
    cout << "std mask = " << hex << mask <<" << 5"<< endl;

    //STDID with 11 bit data
    uint32_t ext_mask = 0x1fffffff; // 下面开始计算屏蔽码
    uint32_t ext_num = sizeof(ext_id_list) / sizeof(ext_id_list[0]);
    uint32_t ext_tmp;
    for (uint16_t i = 0; i < num;
             i++) // 屏蔽码位StdIdArray[]数组中所有成员的同或结果
    {
        ext_tmp = ext_id_list[i] ^ (~ext_id_list[0]); // 所有数组成员与第0个成员进行同或操作
        ext_mask &= tmp;
    }
    cout << "ext mask = " << hex << ext_mask<<" << 3"<< endl;
}
