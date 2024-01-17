<!--
 * @Description: 
 * @Author: qianwan
 * @Date: 2023-12-19 17:41:51
 * @LastEditTime: 2024-01-18 03:18:27
 * @LastEditors: qianwan
-->
# Fish Chassis System
*深圳科创学院2024高中生机器人冬令营 底盘嵌入式系统*   
*InnoxSZ 2024 Robotics Winter Camp for High School Students.* 

Get this repository by:   
``` bash
git clone https://github.com/RM-camp-for-high-school-students/Shenzhen-innoX-2024wc-chassis.git --recursive
```

- [Fish Chassis System](#fish-chassis-system)
  - [Environment](#environment)
      - [Softwares](#softwares)
      - [Packages](#packages)
      - [Communication Module](#communication-module)
      - [Publisher-Subscriber Module](#publisher-subscriber-module)
  - [Construction](#construction)
  - [Chassis Details](#chassis-details)
      - [Coordinate](#coordinate)


## Environment
#### Softwares
- Code generator: [STM32CubeMX-6.10.0](https://www.st.com/zh/development-tools/stm32cubemx.html)
- Tools chains: [arm-none-eabi-gcc](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- Debuger: [OpenOCD](https://github.com/xpack-dev-tools/openocd-xpack/releases) + GDB(Windows need [mingw64](https://github.com/skeeto/w64devkit/releases))
- Code editor: [Vscode](https://code.visualstudio.com/)/[Clion](https://www.jetbrains.com/zh-cn/clion/)
- Refer tutorial:  [Vscode](https://gitee.com/hnuyuelurm/basic_framework/blob/master/.Doc/VSCode+Ozone%E4%BD%BF%E7%94%A8%E6%96%B9%E6%B3%95.md)/[Clion](https://zhuanlan.zhihu.com/p/145801160)

> Author recommende Clion as IDE for more intelligent. And in this project, both Clion and Vscode are ready to go.   
 
> One more thing: Use clangd need to edit ___.vscode/setting.json___ :    
>   _--query-driver = "your\\arm\\tool\\chain\\bin\\arm-none-eabi-g*"_

#### Packages
- STM32CubeF4 Firmware Package V1.28.0
- STM32CubeExpansion_AZRTOS-F4 V1.1.0

#### Communication Module
[FishChassis Protocl with Mavlink2](Module/Mavlink/README.md)

#### Publisher-Subscriber Module
[OneMessage](Module/OneMessage/README.md)

## Construction
![Layer](Doc/system_layer.svg)

## Chassis Details
#### Coordinate
*ENU* indicates Right-Forward-Upward would be the correct direction of the chassis.