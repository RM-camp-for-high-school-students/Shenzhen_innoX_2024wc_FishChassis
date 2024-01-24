/*
 * @Description:
 * @Author: qianwan
 * @Date: 2023-12-25 16:13:17
 * @LastEditTime: 2024-01-24 17:02:51
 * @LastEditors: qianwan
 */
#include "../Module/Mavlink/FishCom/mavlink.h"
#include "iostream"
#include <string>

using namespace std;

int main() {
  mavlink_message_t msg_tx = {};
  mavlink_chs_ctrl_info_t chs_ctrl_info = {};
  mavlink_chs_servos_info_t chs_servos_info = {};
  mavlink_chs_manage_info_t chs_manage_info = {};
  uint8_t *rxbuf = (uint8_t *)malloc(512);

  cout << "Chassis Test v1.0 by qianwan" << endl;
  string input; // Declare the 'input' variable before the while loop

  cout << "--------------------------------------" << endl;
  while (input != "y" && input != "n" && input != "Y" && input != "N") {
    cout << "Enable wheel motor? (y/n)" << endl;
    cin >> input;
    cin.clear(); // 使流重新处于有效状态
    cin.sync();  // 清除缓存区的数据流
  }

  if (input == "y" || input == "Y") {
    chs_manage_info.enable_chassis = true;

    cin.clear(); // 使流重新处于有效状态
    cin.sync();  // 清除缓存区的数据流
    cout << "Input chassis speed x.(m/s)" << endl;
    cin >> input;
    chs_ctrl_info.vx = stof(input);

    cin.clear(); // 使流重新处于有效状态
    cin.sync();  // 清除缓存区的数据流
    cout << "Input chassis speed y.(m/s)" << endl;
    cin >> input;
    chs_ctrl_info.vy = stof(input);

    cin.clear(); // 使流重新处于有效状态
    cin.sync();  // 清除缓存区的数据流
    cout << "Input chassis speed w.(rad/s)" << endl;
    cin >> input;
    chs_ctrl_info.vw = stof(input);

    cout << "vx:" << to_string(chs_ctrl_info.vx) << " m/s" << endl;
    cout << "vy:" << to_string(chs_ctrl_info.vy) << " m/s:" << endl;
    cout << "vw:" << to_string(chs_ctrl_info.vw) << " rad/s:" << endl;
  } else {
    cout << "Disable wheel motor." << endl;
    chs_manage_info.enable_chassis = false;
  }

  cout << "--------------------------------------" << endl;
  input.clear();
  while (input != "y" && input != "n" && input != "Y" && input != "N") {
    cout << "Enable servos? (y/n)" << endl;
    cin >> input;
    cin.clear(); // 使流重新处于有效状态
    cin.sync();  // 清除缓存区的数据流
  }

  if (input == "y" || input == "Y") {
    chs_manage_info.enable_servos = true;

    for (uint8_t i = 0; i < 7; i++) {
      cin.clear(); // 使流重新处于有效状态
      cin.sync();  // 清除缓存区的数据流
      cout << "Input servo " << to_string(i) << " duty cycle.(499-2499)"
           << endl;
      cin >> input;
      chs_servos_info.servos[i] = stoi(input);
      cin.clear(); // 使流重新处于有效状态
      cin.sync();  // 清除缓存区的数据流
    }
    // 输出占空比
    for (uint8_t i = 0; i < 7; i++) {
      cout << "Servo " << to_string(i)
           << " duty cycle: " << to_string(chs_servos_info.servos[i]) << endl;
    }
  } else {
    cout << "Disable servos." << endl;
    chs_manage_info.enable_servos = false;
  }

  cout << "--------------------------------------" << endl;
  input.clear();
  while (input != "y" && input != "n" && input != "Y" && input != "N") {
    cout << "Reset quaternion? (y/n)" << endl;
    cin >> input;
    cin.clear(); // 使流重新处于有效状态
    cin.sync();  // 清除缓存区的数据流
  }

  if (input == "y" || input == "Y") {
    chs_manage_info.reset_quaternion = true;
    cout << "Reset quaternion." << endl;
  } else {
    chs_manage_info.reset_quaternion = false;
    cout << "Don't reset quaternion." << endl;
  }
  cout << "------------Serial Data(Hex)--------------" << endl;
  
  mavlink_msg_chs_ctrl_info_encode(0, 0, &msg_tx, &chs_ctrl_info);
  uint16_t len = mavlink_msg_to_send_buffer(rxbuf, &msg_tx);
  // 把len长度的数据发送出去, 使用0x12 0x13这种形式，以16进制发送
  for (int i = 0; i < len; i++) {
    printf("%02x ", rxbuf[i]);
  }
  // 发送其他的数据

  mavlink_msg_chs_servos_info_encode(0, 0, &msg_tx, &chs_servos_info);
  len = mavlink_msg_to_send_buffer(rxbuf, &msg_tx);
  for (int i = 0; i < len; i++) {
    printf("%02x ", rxbuf[i]);
  }

  mavlink_msg_chs_manage_info_encode(0, 0, &msg_tx, &chs_manage_info);
  len = mavlink_msg_to_send_buffer(rxbuf, &msg_tx);
  for (int i = 0; i < len; i++) {
    printf("%02x ", rxbuf[i]);
  }

  printf(" ");
  return 0;
}