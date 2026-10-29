## Zephyr Device Model
**现代RTOS四大标准驱动**
~~~
- Interrupt Controller
    - ARM NVIC/GIC
    - RISC PLIC
    - 设备树对应带有interrupt-controller的节点

- Timer
    - System Clock和Hardware Clock
    - 设备树对应SoC节点中专门的Timer节点或者SysTick

- Serial/UART
    - printk/printf底层
    - 设备树对应/chosen节点指定, chosen={zephyr,console=&uart0;};

- Entropy
    - 现代TLS/SSL加密的基石
    - 设备树对应带有硬件真随机数发生器(TRNG)的节点
~~~

**zephyr的同步机制**
~~~
Example :
    i2c_transfer(my_i2c_dev, &msgs, 2);

    - 1. API层面, i2c_transfer调用k_sem_take(&driver_data->sync_sem, K_FOREVER);进入阻塞
    - 2. 驱动transfer函数拿着DTS解析出来的reg基地址, 配置I2C寄存器, 启动传输, 打开I2C中断使能位, 然后驱动返回
    - 3. ISR层面, I2C硬件把数据1 bit 1 bit发送完或者出错之后, 拉高中断线, CPU执行I2C的ISR
    - 4. 唤醒, ISR执行k_sem_give(&driver_data->sync_sem)
~~~

**Zephyr Device Tree**
~~~
/ {
    model="my_device";
    #address-cells=<1>;
    #size-cells=<1>;
    soc@0x40003000 {
        #address-cells=<1>;
        reg=<0x40003000 0x1000>;
        i2c@0x39 {
            compatible="APDS9960";
            reg=<0x39>;
        };
        i2c@0x43 {
            compatible="HDC";
            reg=<0x43>;
        };
        i2c@0x1d {
            compatible="MMA8652FC";
            reg=<0x1d>;
        };
    };
};
~~~


**DEVICE_DEFINE**
~~~
DEVICE_DEFINE(dev_id, name, init_fn, pm, data, config, level, prio, api)

    - DEVICE_DEINIT_DEFINE(dev_id, name, init_fn, NULL, pm, data, config, level, prio, api)

    - Z_DEVICE_STATE_DEFINE(dev_id);
      Z_DEVICE_DEFINE(DT_INVALID_NODE, dev_id, name, init_fn, NULL, 0U, pm, data, config, level, prio, api, &Z_DEVICE_STATE_NAME(dev_id))

    - 
~~~

