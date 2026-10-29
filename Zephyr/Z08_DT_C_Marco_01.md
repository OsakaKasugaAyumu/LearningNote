
## Zephyr设备树定调
**编译前把硬件关系展开成常量, 对象和指针**
~~~
和Linux的理念完全不一样

Liunx : 典型路径是内核运行时解析二进制设备树
Zephyr : 构建期生成C宏, 通过公共头文件进行访问. 原因 : 目标设备资源有限, 完整的DTB及其运行时解析代码过重
~~~

## C语言宏参数中, 设备树名称转换成 小写字母 + 数字 + 下划线

**DT_PATH(...)**
```c
/**
    @note 核心红能: 通过设备树中的绝对路径(从根节点/开始), 获取该节点唯一标识符(Node ID)
    @param 可变参数, 每一个参数表示路径上一层节点名, 不能用标签(label)
    @note 全小写 + 非法字符(@ - ,等)全部转换成下划线_ . 比如 DTS节点serial@40002000, 在C语言传入宏中写serial_40002000
*/

/**
    DTS: 
    / {
        soc {
            serial1: serial@40001000 {
                status = "okay";
                current-speed = <115200>;
            };
        };
    };
*/

// 获取路径节点
#define UART_NODE   DT_PATH(soc, serial_40001000)

// 获取速度属性
#define SPEED       DT_PROP(UART_NODE, current_speed)
```

**DT_NODELABEL(label)**
```c
/**
    @note 核心功能: 通过设备树的节点标签(Node Label), 获得Node ID
    @param label: 小写字母 + 数字 + 下划线
    @note DT_NODELABEL依赖DTS必须写label, DT_PATH不需要, 只需要完整DT路径
*/

/**
    DTS:
    serial1: serial@40001000 {
        status = "okay";
        current-speed = <115200>;
    };
*/

// Usage 1 : 提取硬件属性
#define BAUD_RATE   DT_PROP(DT_NODELABEL(serial1), current_speed)   // 结果 #define BAUD_RATE   115200

// Usage 2 : 获取设备对象指针
const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(serial1));

if(device_is_ready(uart_dev))
{
    // 调用标准API
}
```

**DT_ALIAS(alias)**
```c
/**
    @note 核心功能: 通过 /aliases节点定义的别名, 获取Node ID, 必须是在DTS里面的/aliases节点里面定义的
    @param alias: 必须转换的别名, 不能是Node Label, 也不能是路径
    @note 跨平台
*/

/**
    DTS:
    / {
        aliases {
            my-serial = &serial1;
        };

        soc {
            serial1: serial@40001000 {
                status = "okay";
                current-speed = <115200>;
            };
        };
    };
*/

// Usage 1 : 提取硬件属性
#define BUAD_RATE   DT_PROP(DT_ALIAS(my_serial), current_speed) // 115200


// Usage 2 : 获取设备对象指针
const struct device *uart_dev = DEVICE_DT_GET(DT_ALIAS(my_serial));

if(device_is_ready(uart_dev))
{
    // 发送数据
}



// DT_HAS_ALIAS(alias_name)

#if DT_HAS_ALIAS(my_led)
    
    #define MY_LED_NODE     DT_ALIAS(my_led)
    const struct device *led_dev = DEVICE_DT_GET(MY_LED_NODE);
    void init_my_led(void)
    {
        if(device_is_ready(led_dev))
        {

        }
    }

#else

    // 3. 如果没有，提供一个降级方案（比如打印警告，或者什么也不做）
    #pragma message("警告：当前板子未定义 my-led 别名，跳过 LED 初始化。")

    void init_my_led(void) {
        // 空实现，防止链接报错
    }

#endif

```

**DT_INST(inst, compat)**
```c
/**
    @note 核心功能: 通过指定的实例号(Instance Number)和compatible字符串, 获取Node ID
    @param inst : 实例号(从0开始)
    @param compat : comaptible字符串(全小写 + 下划线 + 不带双引号), 比如"vnd,soc-serial" -> vnd_soc_serial
    @note 每种compatible编号都是独立的, 从0开始, 就算是status = "disabled", 也会有一个实例号, status = "okay"排在前面, status = "disabled"排在后面
*/

/**
    DTS:
    serial1: serial@40001000 {
        compatible = "vnd,soc-serial";
        status = "disabled";
        current-speed = <9600>;
    };

    serial2: serial@40002000 {
        compatible = "vnd,soc-serial";
        status = "okay";
        current-speed = <57600>;
    };

    serial3: serial@40003000 {
        compatible = "vnd, soc-serial";
        current-speed = <115200>;
    };

    @note serial2和serial3占用INST 0和1, 但是谁先谁后看Python脚本的心情
    @note serial1是2
*/

#define BUAD_RATE DT_PROP(DT_INST(0, vnd_soc_serial), current_speed)        // 可能是57600, 也可能是115200
```

**DT_CHOSEN(prop)**
```c
/**
    @note 通过/chosen节点里面的属性, 获得Node ID
    @param prop: 全小写 + 下划线,  比如DTS里面zephyr,console转换成C宏传参zephyr_console
    @note 仅在DT_HAS_CHOSEN(prop)为1时有效
*/

/**
    DTS:
    / {
        chosen {
            zephyr,console = &uart0;
            zephyr,entropy = &rng;
        };
    };
*/

//安全写法
#if DT_HAS_CHOSEN(zephyr_console)
    const struct device *console_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
#endif


#if DT_HAS_CHOSEN(zephyr_watchdog)
    #define WDT_NODE DT_CHOSEN(zephyr_watchdog)
    const struct device *wdt = DEVICE_DT_GET(WDT_NODE);
    // 调用看门狗 API
#else
    #pragma message("注意：当前板子没有钦点看门狗！")
#endif

```

**DT_PARENT(node_id)**
```c
/**
    @note 输入一个已知节点的标识符(Node ID),得到直接父节点的标识符
    @param 必须是一个有效的标识符, 通过DT_NODELABEL, DT_PATH等获得
*/

/**
    DTS:
    parent: parent-node {
        child: child-node {

        };
    };
*/

// 完全等价 DT_NODELABEL(parent)和DT_PARENT(DT_NODELABEL(child))
```


**DT_PROP(node_id, prop)**
```c
    /**
        DTS:
        / {
            foo: foo-node {
                compatible = "vnd,foo-device";
                status = "okay";

                a = <100 200 300>;

                b = [aa bb cc];

                c = "bar", "baz";
            };
        };
    */

    /**
        YAML:
        compatible: "vnd,foo-device"
        properites:
          a: 
            type: array
            description: "32 cells数组"

          b:
            type: uint8-array
            description: "字节数组"

          c:
            type: string-array
            description: "字符串数组"
    */


#include "zephyr/devicetree.h"

#define FOO     DT_NODELABEL(foo)

void dt_prop(void)
{
    int a[] = DT_PROP(FOO, a);
    unsigned char b[] = DT_PROP(FOO, b);
    const char *c[] = DT_PROP(FOO, c);

    int a_len = DT_PROP_LEN(FOO, a);
    int b_len = DT_PROP_LEN(FOO, b);
    int c_len = DT_PROP_LEN(FOO, c);
}
```


## 设备树特殊的reg属性和interrupts属性, 不能用DT_PROP直接获取, 这两个有专门的宏
```c

// 比如ARMv7-M NVIC

/**
    DTS:
    / {
        my-timer: timer@40000000 {
            compatible: "vnd,my-timer";
            reg = <0x40000000 0x100>, <0x40001000 0x1000>;

            interrupt-parent = <&nvic>;
            interrupts = <10 1>, <11, 2>;
        };
    };
*/

/**
    YAML:
    compatible: "vnd,my-timer"
    include [base.yaml]

    properties:
      reg:
        required: true
      interrupts:
        required: true
*/


#define TIMER_NODE          DT_NODELABEL(my_timer)

#define REGS_NUM            DT_NUM_REGS(TIMER_NODE)                 // 2
#define REG0_ADDR           DT_REG_ADDR(TIMER_NODE)                 // 0x40000000
#define REG0_SIZE           DT_REG_SIZE(TIMER_NODE)                 // 0x100
#define REG1_ADDR           DT_REG_ADDR_BY_IDX(TIMER_NODE, 1)       // 0x40001000
#define REG1_SIZE           DT_REG_SIZE_BY_IDX(TIMER_NODE, 1)       // 0x1000

#define IRQS_NUM            DT_NUM_IRQS(TIMER_NODE)                 // 2
#define IRQ0_NUM            DT_IRQ_BY_IDX(TIMER_NODE, 0, irq)       // 10, 中断号
#define IRQ0_PRIO           DT_IRQ_BY_IDX(TIMER_NODE, 0, priority)  // 1, 中断优先级
#define IRQ1_NUM            DT_IRQ_BY_IDX(TIMER_NODE, 1, irq)       // 11 中断号



/**
    DT_IRQN(node_id)        只要中断号
        - 等同于 DT_IRQ(node_id, irq) ==> DT_IRQ_BY_IDX(node_id, 0, irq)

    DT_IRQ(node_id, cell)
        - DT_IRQ_BY_IDX(node_id, 0, cell)
*/
```

