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
- on-bus(我挂在总线上) 声明该节点接在哪个总线上, Zephyr根据这个
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
