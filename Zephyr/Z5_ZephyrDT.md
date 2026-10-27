

## Zephyr中的设备树
~~~
node-name@unit-address :

    unit-address五种情况 :
        - MMIO, 寄存器块基地址
            - i2c@40003000

        - i2c外设
            - apds9960@39, 39是i2c的从机地址, 而且父节点i2c控制器的#size-cells=<0>

        - spi外设
            - 片选引脚号

        - RAM / 内存映射Flash, 存储区物理起始地址
            - memory@80000000

        - 固定Flash分区, 分区Flash的偏移量
            - flash@8000000, partition@20000, 最终地址 = 0x08020000
~~~

**label, aliases, chosen**
~~~
label : 标签
    - 可以在设备树源文件中使用&label引用该节点

aliases : 别名
    - 把硬件物理节点映射到固定的"角色名"上
    - blinky例程, 永远找的是led0, 而不关心板子

chosen : 选定
    - 系统级的钦定, 全局唯一配置

C Marco :
    aliases :
        - #define DT_N_ALIAS_my_uart DT_N_S_soc_S_serial_12340000

    chosen :
        - #define DT_CHOSEN_zephyr_console DT_N_S_soc_S_serial_12340000
~~~

**skeleton.dtsi**
~~~
/ {
    #address-cells = <1>;
    #size-cells = <1>;

    chosen {};

    aliases {};
};
~~~

**Armv7-m.dtsi**
~~~
#include "skeleton.dtsi"

/ {
    #address-cells = <1>;
    #size-cells = <1>;
    compatible = "simple-bus";
    interrupt-parent = <&nvic>;
    ranges;

    nvic: interrupt-controller@e000e100 {
        #address-cells = <1>;
        compatible = "arm,v7m-nvic";
        reg = <0xe000e100 0xc00>;
        interrupt-controller;
        #interrupt-cells = <2>;
    };

    systick: timer@e000e010 {
        compatible = "arm,armv7m-systick";
        reg = <0xe000e010 0x10>;
    };
};


compatible = "simple-bus"  一个简单的内存映射总线

interrupt-parent = <&nvic>; 核心一行, 在这个SoC内部, 如果外设自己没有写interrupt-parent, 那么中断线默认全部连接到NVIC上, 省去了后面成百上千外设节点重复写这行代码的麻烦

ranges; 空值, 1:1恒等变换, SoC总线内部局部地址, 就是CPU能看到的地址

0xe000e100 : ARMv7-M硬编码的NVIC寄存器的基地址, 任何Cortex-M都是这个地址

0xc00 : 寄存器块映射长度(3KB)

#interrupt-cells = <2>; 2个cells, <中断号 触发方式>
~~~

