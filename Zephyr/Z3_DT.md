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


    /
    model="fsl,mpc8572ds"
    compatible="fsl,mpc8572ds"
    #address-cells=<1>
    #size-cells=<1>


    cpus
    #address-cells=<1>
    #size-cells=<0>

    cpu@0
    device_type="cpu"
    reg=<0>
    timebase-frequency=<825000000>
    clock-frequency=<825000000>

    cpu@1
    devcie_type="cpu"
    reg=<1>
    timebase-frequency=<825000000>
    clock-frequency=<825000000>


    memory@0
    device_type="memory"
    reg=<0 0x20000000>

    uart@fe001000
    compatible="ns16550"
    reg=<fe001000 0x100>

    chosen
    bootargs="root=/dev/sda2"

    aliases
    serial0="/uart@fe001000"
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

