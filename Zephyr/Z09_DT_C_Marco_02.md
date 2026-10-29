## devicetree_generated.h


```
DT_N_<节点路径>_P_<属性名><可选后缀>

    - 把所有的 / 替换成_S_, 其他的替换成下划线, /foo@123/bar-BAZ转换成_S_foo_123_S_bar_baz 
    - _P_表示属性, zephyr,console转换成zephyr_console

    - DT_N_S_soc_S_uart_40000000_P_reg_IDX_0_VAL_ADDRESS
        - 路径 : /soc/uart@40000000
        - 属性reg, 索引0, 只是地址


cell 名字从哪来？ 
完全由目标控制器 YAML 文件里的 <specifier>-cells 列表决定(如 gpio-cells: [pin, flags])


idx 必须是字面常量：DT_PHA_BY_IDX(LED, gpios, i, pin) 这种带变量的写法是绝对禁止的。
因为底层是 DT_CAT7 字符串拼接，C 预处理器无法拼接变量。
如果非要遍历，必须用 Zephyr 提供的 DT_FOREACH_PROP_ELEM 等宏进行展开。

```

## DT_PHA和DT_PHANDLE的区别:
## DT_PHANDLE提取的时被引用的那个节点(Node ID)
## DT_PHA提取的是引用的那个数字参数(一个常量值)
~~~
~~~
### DT_PHANDLE_BY_IDX(node_id, prop, idx) / DT_PHANDLE(node_id, prop)
```c
/**
    DT_PHANDLE_BY_IDX(node_id, prop, idx)
        @note 核心功能: 从指定节点(node_id)属性(prop)中, 提取索引idx的引用, 并返回目标节点的标识符(Target Node ID)

    DT_PHANDLE(node_id, prop)
        @note 完全等价 DT_PHANDLE_BY_IDX(node_id, prop, 0)
*/

#define SENSOR  DT_NODELABEL(my_sensor)

#define INTC0   DT_PHANDLE(SERSOR, interrupt_parents)
#define INTC1   DT_PHANDLE_BY_IDX(SENSOR, interrupt-parents, 1)

#define PMIC    DT_PHANDLE_BY_IDX(SENSOR, power_supply, 0)

const struct device *pmic_dev = DEVICE_DT_GET(PMIC);
const struct device *intc0_dev = DEVICE_DT_GET(INTC0);
const struct device *intc1_dev = DEVICE_DT_GET(INTC1);
```
```d
/ {
    /*定义电源芯片*/
    pmic: pmic@1000 {
        compatible = "vnd,pmic";
        reg = <0x1000 0x100>;
    };

    /*定义两个中断控制器*/
    intc0: interrupt-controller@2000 {
        compatible = "vnd,intc";
    };

    intc1: interrupt-controller@3000 {
        compatible = "vnd,intc";
    };


    /**主传感器*/
    my-sensor: sensor@4000 {
        compatible = "vnd,sensor";
        power-supply = <&pmic>;
        interrupt-parents = <&intc0 &intc1>;
    };
};
```

```yml
# vnd,sensor.yml
compatible: "vnd,sensor"
properties:
  power-supply:
    type: phandle

  interrupt-parents:
    type: phandles

```

### DT_PHA_BY_IDX(node_id, pha, idx, cell) / DT_PHA(node_id, pha, cell)
```c
/**
    @note 核心功能: 从一个phandle-array中, 提取第idx个specifier, 名为cell的值
            node->phandle_array[index].cell

    DT_PHA_BY_IDX(node_id, pha, idx, cell)
        DT_CAT7(node_id, _P_, pha, _IDX_, idx, _VAL_, cell)
        DT_N_<节点>_P_<属性>_IDX_<索引>_VAL_<单元格名>
*/

#define LED0        DT_NODELABEL(led)

#define GPIO0_PIN   DT_PHA_BY_IDX(LED0, gpios, 0, pin)      // 17
#define GPIO1_PIN   DT_PHA_BY_IDX(LED0, gpios, 1, pin)      // 5
#define GPIO0_FLAG  DT_PHA_BY_IDX(LED0, gpios, 0, flags)    // 0x1
#define GPIO1_FLAG  DT_PHA_BY_IDX(LED0, gpios, 1, flags)    // 0x3

void led_init(void) {
    // 回忆上一问：用 DT_PHANDLE_BY_IDX 提取控制器节点并转成指针
    const struct device *gpio0_dev = DEVICE_DT_GET(DT_PHANDLE_BY_IDX(LED, gpios, 0));
    const struct device *gpio1_dev = DEVICE_DT_GET(DT_PHANDLE_BY_IDX(LED, gpios, 1));

    if (device_is_ready(gpio0_dev) && device_is_ready(gpio1_dev)) {
        // 配置 gpio0 的 17 号引脚
        gpio_pin_configure(gpio0_dev, LED0_PIN, GPIO_OUTPUT_ACTIVE | LED0_FLAGS);
        
        // 配置 gpio1 的 5 号引脚
        gpio_pin_configure(gpio1_dev, LED1_PIN, GPIO_OUTPUT_ACTIVE | LED1_FLAGS);
    }
}
```
```d
/ {
    gpio0: gpio@1234 {
        compatible = "vnd,gpio";
        #gpio-cells = <2>;
        reg = <0x1234 0x100>;
    };

    gpio1: gpio@5678 {
        compatible = "vnd,gpio";
        #gpio-cells = <2>;
        reg = <0x5678 0x200>;
    };

    led: led_0 {
        compatible = "vnd,led";
        gpios = <&gpio0 17 0x1>, <&gpio1 5 0x3> #这个由vnd,gpio.yaml文件里面定义
    };
};

```
```yml
# vnd,gpio.yaml
compatible: "vnd,gpio"
gpio-cells:
  - pin
  - flags


# vnd,led.yaml
compatible: "vnd,led"
properties:
  gpios:
    type: phandle-array

```


```c
GPIO三元组 : <&GPIO控制器标签 引脚号 配置标志>

DTS定个数
    gpio0: gpio@deadbeef {
        #gpio-cells = <2>; /* 规矩：引用我必须带 2 个数字 */
    };

YAML定名字
    # 在 gpio0 的 yaml 绑定中
    gpio-cells:
    - pin   # 第 1 个数字叫 pin
    - flags # 第 2 个数字叫 flags

```

```d
my-external-ic {
    irq-gpios = <&gpio1 7 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>;
};
```
