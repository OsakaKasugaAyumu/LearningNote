## Zephyr设备树绑定

~~~
zephyr中的DTS和YAML文件的关系
    - DTS只是一堆文本, YAML才是"灵魂和规矩"的法律
~~~
**DTS侧**
~~~
bar-device {
    compatible = "foo-company,bar-device";
    num-foos = <3>;
}
~~~
**YAML侧**
~~~
compatible: "foo-company,bar-device"       # 和DTS侧对应了
properties:
  num-foos:
    type: int                               # 规则1 :必须是整数
    required: true                          # 规则2 :必填项
~~~


**YAML侧顶层11字段**
~~~
- title(简短标题)
- description(详细描述)
- example(示例)

- compatible(匹配暗号) DTS和YAML链接的唯一纽带
- include(复用规则)

- properites(属性约束)  规定DTS节点能写什么属性, 比如定义reg必须有, 类型int

- bus(我是总线) 声明该节点提供什么类型的总线
- on-bus(我挂在总线上) 声明该节点接在哪个总线上, Zephyr根据这个参与绑定匹配
- child-binding(子节点规则)

- class(设备类别)
- [foo]-cells(给单元格命名), 比如gpio-cells和interrupt-cells
~~~

**设备树中一个compatible类型的节点集对应一个YAML文件**
~~~
比如stm32g4有四个i2c控制器, i2c1, i2c2, i2c3
i2c1: i2c@40005400 { compatible = "st,stm32-i2c"; ... };
i2c2: i2c@40005800 { compatible = "st,stm32-i2c"; ... };
i2c3: i2c@40005c00 { compatible = "st,stm32-i2c"; ... };

三个i2c对应一个YAML文件


sensor@68 {
    compatible = "bosch,bme280", "bosch,bme680-generic";
};
zephyr系统先找bosch,bme280.yaml, 如果没有再找bosch,bme680-generic.yaml
~~~


**arm,v7m-nvic.yaml**
~~~

description: ARMv7-M NVIC (Nested Vectored Interrupt Controller)                # 注释, 给人看的
compatible: "arm,v7-nvic"                                                       # 匹配暗号, 唯一和DTS连接的属性
include: [interrupt-controller.yaml, base.yaml]                                 # 魔法继承

properties:
  reg:
    required: true                                                              # 强制要求

  arm,num-irq-priority-bits:
    required: true
    type: int
    description: number of bits of IRQ priorities                               # 强制要求, 中断优先级位数, nrf 3 bits,  stm32 4 bits   

  "#interrupt-cells":
    const: 2                                                                    # 如果DTS里面的#interrupt-cells=<3>, 那么直接报错

interrupt-cells:
  - irq
  - priority                                                                    # 规定了, interrupts=<irq priority> 中断号 中断优先级

~~~


**匹配机制**
~~~
假设DTS一个node, compatible="vnd,my-sensor", 并且挂载i2c总线上,
    - 优先匹配compatible: "vnd,my-sensor"且on-bus: i2c的YAML文件
    - 其次匹配compatible: "vnd,my-sensor"但是没有on-bus: i2c的YAML文件
~~~

**Example**
~~~
vnd,sensor传感器, 支持I2C和SPI
~~~
**vnd,sensor-i2c.yaml**
~~~
compatible: "vnd,sensor"
on-bus: i2c                     # 限制：只有在 I2C 总线下才匹配
properties:
  reg:
    required: true              # I2C 需要从机地址
~~~
**vnd,sensor-spi.yaml**
~~~
compatible: "vnd,my-sensor"
on-bus: spi                     # 限制：只有在 SPI 总线下才匹配
properties:
  reg:
    required: true              # SPI 这里是片选号 (CS)
  spi-max-frequency:
    required: true              # SPI 必须指定最大时钟频率
~~~



**YAML的类型规则**
~~~
C语言是强类型语言, DTS是弱类型, YAML是连接DTS和C语言的桥梁, 把DTS的数值转成C语言的类型

- string :
    - YAML : type: string
    - DTS : my-string = "hello";
    - C语言 : #define ..._MY_STRING     "hello"

- int(32位cell)
    - YAML : type: int
    - DTS : my-int = <100>;
    - C语言 : #define ..._MY_INT        100

- boolean
    - YAML : type: boolean
    - DTS : my-boolean; (没有等号, 没有值)
    - C语言 : 如果这个值在设备树存在, C宏就是1


- array(一组32位cell)
    - YAML : type: array
    - DTS : my-array = <0x10 0x20 0x30>;
    - C语言 : 生成一个C语言数组宏{0x10, 0x20, 0x30},  DT_PROP_LEN()获取长度, DT_PROP_BY_IDX()获取索引的值

- uint8-array
    - YAML : type: uint8-array
    - DTS : my-bytes = [00 01 ab];   (方括号, 无0x)
    - C语言 : 生成mac数组宏, 常用于mac地址

- string-array
    - YAML : type: string-array
    - DTS : my-strings = "tx", "rx";
    - C语言 : 字符串指针数组, 比如compatible


- phandle
    - YAML : type: phandle
    - DTS : my-node = <&uart1>;
    - C语言 : 节点标识符(设备树中某一个节点的唯一身份证号)

- phandles
    - YAML : type: phandles
    - DTS : my-node = <&uart1 &uart0>;

- phandle-array
    - YAML : type: phandle-array
    - DTS : my-pwms = <&pwm0 1 2000>, <&pwm1 2 1000>;
~~~


**YAML基础约束**
~~~
- required 
    - required: true        # 必填项, 如果DTS里面没写, 那么直接报错

- default
    - 如果DTS偷懒没写, 系统自动填上默认值, 适用 : 整数, 字符串, 普通数组, 字节数组, 字符串数组

- deprecated
    - deprecated: true      # 表示这个属性已经过时


- enum (选择题)
    - enum: [8, 16, 24, 32]  必须是这四个值, 如果DTS写<12>就会报错

- min/max(数值范围)

- min-len/max-len(数组长度限制)

- const(死规定, DTS里面的值必须永远等于这个固定值)

~~~

~~~
properties:
  current-speed:
    type: int
    required: true
    description: 初始波特率

  resolution:
    type: int
    enum: [8, 16, 24, 32]       # DTS中 resolution = <12>会直接报错, 只能是enum的四个值

  brightness:
    type: int
    min: 0
    max: 100                    # DTS中 如果brghtness = <200>会直接报错, 0-100

  coordinates:
    type: array
    min-len: 3
    max-len: 3                  # DTS中 数组的长度只能是3个

  "#address-cells":
    type: int
    required: true
    const: 1                    # DTS中的#address-cells = <1> 只能是1
~~~


**specifier-space**
``````
指定资源描述空间

properties:
  mboxes:
    type: phandle-array
    specifier-space: mbox
``````

**dependency-mode**
~~~
极其复杂的一个属性

四个值可选
- normal
- reverse
- ignore
- child-ignore

properites:
  phy-phandle:
    type: phandle           #这个属性作用域phandle属性上
    dependency-mode: ignore

  optional-ref:
    type: phandle
    dependency-mode: child-ignore
~~~

**child-binding**
~~~
DTS侧 :
    pwd-leds {
        compatible = "pwm-leds";

        led_red {
            pwms = <&pwm0 1 2000>;             // 在pwm-leds.yaml, 规定了pwms这个属性是必须的
        };

        led_green {
            pwms = <&pwm0 2 2000>;
        };
    };


YAML侧(pwm-leds.yaml) :
    compatible: "pwm-leds"
    child-binding:                              // 规定了在DTS侧的"pwm-leds"节点下的子节点, 如果没有显式写compatible, 那么统一使用我下面这套规则
      description: 使用PWM控制的LED
      properites:
        pwms:
          type: phandle-array
          required: true
~~~


**include合并规则**
~~~
 - 如果include的文件中定义了properties: {reg: { type: int } }, 在当前文件中properties: {reg: {required: true}}, 自动合并
 - 如果include的文件中required: false, 当前文件中required: true, 那么加强为true, 如果include文件中共required: true, 当前文件required: false, 则报错
 - 如果include: [a.yaml, b.yaml], a.yaml是必填的, b.yaml是选填的, 合并之后, 必填的
~~~

~~~
include:
  - name: foo.yaml
    property-allowlist:                 # 白名单, 引入foo.yaml, 但只要i-want-this-one属性
      - i-want-this-one

  - name: bar.yaml
    property-blocklist:
      - do-not-include-this-one         # 黑名单, 引入bar.yaml, 但把do-not-include-this-one屏蔽
~~~
