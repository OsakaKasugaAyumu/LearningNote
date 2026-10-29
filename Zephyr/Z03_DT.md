## ZSWatch DT Usage
~~~
DT_ALIAS(alias)     
 - DT_CAT(DT_N_ALIAS_, alias)
 - DT_N_ALIAS_ ## alias

    ** Usage 1**
        DT_ALIAS(display_blk)
        - DT_N_ALIAS_display_blk


DT_PROP_OR(node_id, prop, default_value)
 - COND_CODE_1(DT_NODE_HAS_PROP(node_id, prop), (DT_PROP(node_id, prop)), (defult_value))
 - COND_CODE_1(IS_ENABLE(DT_CAT4(node_id, _P_, prop, _EXISTS)), (DT_CAT3(node_id, _P_, prop)), (default_value))
 - Z_CODE_CODE1(IS_ENABLE(DT_CAT4(node_id, _P_, prop, _EXISTS)), (DT_CAT3(node_id, _P_, prop)), (default_value))
 - __COND_CODE(__XXXX ## IS_ENABLE(DT_CAT4(node_id, _P_, prop, _EXISTS)), (DT_CAT3(node_id, _P_, prop)), (default_value))
 - __GET_ARG2_DEBRACKET(__XXXX ## IS_ENABLE(DT_CAT4(node_id, _P_, prop, _EXISTS)) (DT_CAT3(node_id, _P_, prop)), (default_value))
 - __DEBRACKET (DT_CAT3(node_id, _P_, prop))  or  __DEBRACKET (default_value)
 - DT_CAT3(node_id, _P_, prop)  or  default_value


PWM_DT_SPEC_GET_OR(node_id, default_value)
 - PWM_DT_SPEC_GET_BY_IDX_OR(node_id, 0, default_value)
 - COND_CODE_1(DT_NODE_HAS_PROP(node_id, pwms), (PWM_DT_SPEC_GET_BY_IDX(node_id, 0)), (default_value))
 - COND_CODE_1(IS_ENABLE(DT_CAT4(node_id, _P_, pwms, _EXISTS)), ({.dev = DEVICE_DT_GET(DT_PWMS_CTLR_BY_IDX(node_id, 0)), .channel = DT_PWMS_CHANNEL_BY_IDX(node_id, 0), .period = DT_PWMS_PERIOD_BY_IDX(node_id, 0), .flags = DT_PWMS_FLAGS_BY_IDX(node_id, 0),}), (default_value))
 - COND_CODE_1(Z_IS_ENABLE1(DT_CAT4(node_id, _P_, pwms, _EXISTS)), ({.dev = DEVICE_DT_GET(DT_PHANDLE_BY_IDX(node_id, pwms, 0)), .channel = DT_PWMS_CELL_BY_IDX(node_id, 0, channel), .period = DT_PWMS_CELL_BY_IDX(node_id, 0, period), .flags =    DT_PHA_BY_IDX_OR(node_id, pwms, 0, flags, 0)}), (default_value))
 - COND_CODE_1(Z_IS_ENABLE2(_XXXX ## node_id ## _P_ ## pwms ## _EXISTS), ({.dev = DEVICE_DT_GET(DT_CAT6(node_id, _P_, pwms, _IDX_, 0, _PH)), .channel = DT_PHA_BY_IDX(node_id, pwms, 0, channel), .period = DT_PHA_BY_IDX(node_id, pwms, 0, period), .flags = DT_PROP_OR(node_id, DT_CAT5(pwms, _IDX_, 0, _VAL_, flags), 0)}), (default_value))
 - COND_CODE_1(Z_IS_ENABLE3(_XXXX ## node_id ## _P_ ## pwms ## _EXISTS 1, 0), ({.dev = (&DEVICE_DT_NAME_GET(DT_CAT6(node_id, _P_, pwms, _IDX_, 0, _PH))), channel = DT_CAT7(node_id, _P_, pwms, _IDX_, 0, _VAL_, channel), .period = DT_CAT7(node_id, _P_, pwms, _IDX_, 0, _VAL_, period), .flags = __GET_ARG2_DEBRACKET(__XXXX ## IS_ENABLE(DT_CAT4(node_id, _P_, pwms_IDX_0_VAL_flags, _EXISTS)) (DT_CAT3(node_id, _P_, pwms_IDX_0_VAL_flags)), (0))}), (default_value))


    **Usage 1**
        PWM_DT_SPEC_OR(DT_ALIAS(display_blk), {})
         - PWM_DT_SPEC_OR(DT_N_ALIAS_display_blk, {})
         - COND_CODE_1(Z_IS_ENABLE3(_XXXX ## DT_N_ALIAS_display_blk_P_pwms_EXISTS 1, 0), ({.dev = &DEVCIE_DT_NAME_GET(DT_N_ALIAS_display_blk_P_pwms_IDX_0_PH), .channel = DT_N_ALIAS_display_blk_P_pwms_IDX_0_VAL_channel, .period = DT_N_ALIAS_display_blk_P_P_pwms_IDX_0_VAL_period, .flags = __GET_ARG2_DEBRACKET(__XXXX ## IS_ENABLE(DT_N_ALIAS_display_blk_P_pwms_IDX_0_VAL_flags_EXISTS) (DT_N_ALIAS_display_blk_P_pwms_IDX_0_VAL_flags), (0))}), {})
         - 
~~~


## DT
~~~
设备树用来描述系统硬件的结构

引导程序将设备树加载到客户程序的内存中, 并将指向该设备树的指针传递给客户程序
    核心目的 : 让客户程序(如操作系统内核)动态发现硬件信息, 避免对系统硬件描述进行硬编码

设备树是一种树形数据结构, 各节点用于描述系统中的设备。每个节点包含若干"属性/值"对, 用来描述设备的特征, 除了根节点没有父节点, 每个节点都恰好有一个父节点
设备树节点不必与物理硬件一一对应, 通常与物理硬件有某种关联
~~~
**节点名**
~~~
node-name@unit-address
    - node-name : 描述设备类别
        - 长度[1,31]个字符
        - 必须字母开头
        
    - unit-address : 用于在同一层级区分同类设备
        - 与reg属性强绑定, unit-address必须与reg属性中第一个地址一致
        - 如果节点没有reg, 那么@unit-address必须省略


/ {
    model="fsl,mpc8572ds";
    compatible="fsl,mpc8572ds";
    #address-cells=<1>;
    #size-cells=<1>;
    cpus {
        #address-cells=<1>;
        #size-cells=<0>;
        cpu@0 {
            device_type="cpu";
            reg=<0>;
            timebase-frequency=<825000000>;
            clock-frequency=<825000000>;
        };
        cpu@1 {
            device_type="cpu";
            reg=<1>;
            timebase-frequency=<825000000>;
            clock-frequency=<825000000>;
        };
    };
    memory@0 {
        device_type="memory";
        reg=<0 0x20000000>;
    };
    uart@fe001000 {
        compatible="ns16550";
        reg=<fe001000 0x100>;
    };
    chosen {
        bootargs="root=/dev/sda2";
    };
    aliases {
        serial0="/uart@fe001000";
    };
};


 - 根节点与全局属性
    / : 根节点, 没有父节点
    model : 方便日志识别
    compatible : 核心属性, Zephyr用它来匹配底层驱动
    #address-cells和#size-cells : 地址解析规则, 其子节点中的reg属性地址占用1个cell, 长度占用1个cell

 - CPU容器
    cpus : 容器节点, 容纳系统中所有CPU核心
    #size-cells=<0> : CPU没有长度概念, 不需要大小字段

 - cpu@0/cpu@1
    node-name@unit-address
    device_type="cpu" : 表明是一个CPU设备

 - memory@0
    device_type="memory" : 标准规定, 描述RAM节点必须加这个属性, 操作系统根据识别内存

 - chosen
    特殊节点, 不对应任何硬件, 让Bootloader向OS传递运行时的环境参数
    bootargs="root=/dev/sda2" : 告诉OS, 你的根文件系统位于/dev/sda2上

 - aliases 
    别名节点, 为设备树上特定节点起一个简短固定的名字
~~~

**节点路径**
~~~
// 从根节点开始
/node-name-1/node-name-2/node-name-3

比如:
    /cpus/cpu@0
~~~

**属性**

~~~
    在设备树中, #开头的属性常见且重要 : #address-cells和#size_cells

    属性分为标准属性和非标准属性
        - 非标准属性格式一般为 <厂商代码/组织名>,<属性名>
            fsl,channel-fifo-len            飞思卡尔公司
            ibm,ppc-interrupt-server#s      IBM公司

        -标准属性, DT官方规范

    属性值 : 0个或多个字节组成的数组, 在.dts无论写字符串, 数字还是引用的节点指针, 编译成.dtb之后都是一串连续的字节流
~~~

**标准属性**
~~~
DT官方规范
~~~
**compatible**
~~~
属性名 : compatible
属性值 : <stringList>
    - 建议格式 : "manufacturer,model"
    - 某些节点必须强制拥有

    硬件与驱动程序匹配的核心

    比如 compatible="vnd,my-device", zephyr会转成vnd_my_device, 非字符/字母/下划线全部转成_
~~~


**model**
~~~
属性名 : model
属性值 : <string>
    - 建议格式 : "manufacturer,model"
    - 建议在/根节点上加上model属性

    model="fsl,MPC8349EMITX";
~~~


**phandle**
~~~
属性名 : phandle
属性值 : <u32>
    - 全局唯一ID, 在整个DTB中, 给每个节点分配唯一数字身份证
    - compatible是向上面向操作系统标签, phandle是设备内部(节点与节点之间)的"指针"

    pic@0x10000000 {
        phandle = <1>;
        interrupt-controller;
        reg = <0x10000000 0x100>
    }

    another-device-node {
        interrupt-parent = <1>;     // 这个就是引用了pic节点
    }
~~~

**status**
~~~
属性名 : status
属性值 : <string>
    - "okay"
    - "disabled"
    - "reserved"
    - "fail"
    - "fail-sss"
~~~


**#address-cells和#size-cells**
~~~
这两个属性告诉Zephyr构建系统如何正确解读子节点中的reg属性

    cell : 在DTSpec中, 一个Cell就是一个32 bit(4字节)的无符号整数<u32>
    #address-cells : 规定了子节点中reg属性中, 地址(起始地址)需要占用几个32位整数
    #size-cells : 规定了子节点中reg属性, 长度(映射大小)需要占用几个32位整数

    每一个包含子节点的节点, 都必须显式定义这两个属性

    soc {
        #address-cells = <1>;
        #size-cells = <1>;                      // 子节点serial@4600中的 reg = <[1个cell的地址] [1个cell的长度]>
        serial@4600 {
            compatible = "ns16550";
            reg = <0x4600 0x100>;
            clock-frequency = <0>;
            interrupts = <0xA 0x8>;
            interrupt-parent = <&ipic>;
        };
    };
~~~


**reg**
~~~
    reg 是设备树中提供真实物理地址的硬核数据
    包含任意数量的 <address length> "地址 长度"对
    格式强依赖, 完全取决于父节点的 #address-cells和#size-cells

    如果父节点 #address-cells=<1>; #size-cells=<1>;
    子节点 reg=<0x3000 0x20 0xFE00 0x100>   // 表示该设备有两组独立的寄存器空间, 第一组 address=0x3000 size=0x20,  第二组 address=0xFE00 size=0x100
~~~

**ranges**
~~~
属性名 : ranges
属性值 : <empty> / <child-bus-address parent-bus-address length>
    核心作用 : 地址映射
    地址映射 : reg + ranges
    把child-bus-address映射到parent-bus-address地址上


    CPU最终物理地址 = parent-bus-address + (reg - child-bus-address)

    soc {
        compatible = "simple-bus";
        #address-cells = <1>;
        #size-cells = <1>;
        ranges = <0x0 0xe0000000 0x00100000>
        serial@4600 {
            device_type = "serial";
            comaptible = "ns16550";
            reg = <0x4600 0x100>;
            clock-frequency = <0>;
            interrupts = <0xA 0x8>;
            interrupt-parent = <&ipic>;
        };
    };
~~~

**device_type**
~~~
仅仅建议在CPU和memory节点中保留该属性
~~~


**Interrrupt Tree**
~~~
中断域 : 任何带有interrupt-controller属性且定义了#interrupt-cells节点, 都会新建一个独立的中断域

Nexus : 既是一个中断域的根, 又是一个另外中断域的普通节点

中断树的根 : 有interrupt-controller和#interrupt-cells, 没有interrupt-parent和interrupts
~~~

**interrupts, interrupt-parent, interrupt-extended**
~~~
interrupt-parent : (属性值 <phandle>)
    - 连接中断源和interrupt-controller的指路明灯
    - 中断树的层次结构可能和设备树不同, 使用interrupt-parent显式指定中断父节点
    - phandle是设备树内部节点的唯一数字ID(源码表现 &标签 ) 
    - interrupt-parent=<&intc>本质就是指针引用
    - 如果缺少该属性, 那么就找其设备树父节点的interrupt-parent, 一直往上找, 直到找到最近的一个interrupt-parent

interrupts-extended : (属性值 <phandle> <prop-encoded-array>)
    - 一个设备的输出中断连接到多个中断控制器上
    - 普通的interrupts只能配合唯一的interrupt-parent使用
    - interrupt-extended允许把父节点的phandle写入中断数组中, 从而精确的控制每一个中断走那条线
    - interrupt-extended=<&pic 0xA 8>, <&gic 0xda>;
~~~

**interrupt-controller, #interrupt-cells**
~~~
interrupt-controller : (属性值 <empty>)
    - 表明这是一个能接收中断信号的控制器
    - 如果顺着interrupt-parent一直网上找, 遇到interrupt-controller且没有interrupt-parent, 那么就是最终的中断树根了 (ARM GIC, RISC PLIC)
    - 如果一个节点既有interrupt-controller又有interrupt-parent, 那么说明是Nexus

#interrupt-cells : (属性值 <u32>)
    - 解码规则, 描述一个中断需要填几个cell
    - 任何interrupt-parent指向这个控制器的设备, 在写interrupts的时候, 必须是#interrupt-cells的整数倍
~~~

**interrupt nexus, interrupt-map, interrupt-map-mask**
~~~
interrupt-map : (属性值 5元组)
~~~

