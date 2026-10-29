## Thread State
~~~
Ready : 线程创建, 线程时间片用完, 线程被抢占, 线程解除等待, 资源可用, 在就绪队列中等待执行

Running : 就绪队列中的线程被调度器选中, 只有从Ready -> Running这一条路径, 正在CPU中执行

Suspended : 正在执行的线程等待资源, 只有Running -> Suspended这一条路径, 等待信号量, 互斥锁

Terminated : 线程执行完毕, 被abort
~~~



## Thread Create
**k_thread_create()**
~~~
#include <zephyr/kernel.h>

#define STACK_SIZE          1024
#define THREAD_PRIORITY     5

K_THREAD_STACK_DEFINE(my_stack_area, STACK_SIZE);
struct k_thread my_thread_handle;

void my_thread_entry(void *p1, void *p2, void *p3)
{
    printk("线程启动, 参数 : %d\n", (int)p1);

    while(1)
    {
        printk("线程运行中...\n");
        k_sleep(K_SECONDS(1))；
    }
}

void create_thread_example(void)
{
    k_tid_t my_tid = k_thread_create(
        &my_thread_handle,
        my_stack_area,
        K_THREAD_STACK_SIZEOF(my_stack_area),
        my_thread_entry,
        (void *)42, NULL, NULL,
        THREAD_PRIORITY,
        0,
        K_NO_WAIT
    );

    printk("线程创建成功, tid : %p\n", my_tid);
}
~~~

**K_THREAD_DEFINE()**
~~~
#include <zephyr/kernel.h>

void static_thread_entry(void *p1, void *p2, void *p3)
{
    printk("静态线程创建\n");

    while(1)
    {
        printk("静态线程运行...\n");
        k_sleep(K_SECONDS(2));
    }
}

K_THREAD_DEFINE(
    static_thread_id,
    1024,
    static_thread_entry,
    NULL, NULL, NULL,
    7,
    0,
    0
);
~~~


## Thread Priority
~~~
Cooperative Thread : (负数) 不可抢占, 主动让出CPU(k_sleep)
Preemptive Thread : (0 - 14) 抢占, 数值小的优先级线程抢占数值大的优先级线程
~~~
~~~
#define HIGH_PRIORITY       3
#define MEDIUM_PRIORITY     7
#define LOW_PRIORITY        12
#define COOP_PRIORITY       -1

void high_priority_task(void *p1, void *p2, void p3)
{
    while(1)
    {
        k_sleep(K_MSEC(10));
    }
}

void low_priority_task(void *p1, void *p2, void *p3)
{
    while(1)
    {
        k_sleep(K_SECONDS(5));
    }
}

K_THREAD_DEFINE(hign_thread, 512, high_priority_task, 
            NULL, NULL, NULL, HIGH_PRIORITY, 0, 0);

K_THREAD_DEFINE(low_thread, 1024, low_priority_task,
            NULL, NULL, NULL, LOW_PRIORITY, 0, 0);
~~~


## Thread Stack
~~~
#define BASE_STACK_SIZE     512 
#define LOCAL_VAR_SIZE      256
#define CALL_DEPTH_SIZE     256
#define SAFETY_MARGIN       256
#define TOTAL_STACK_SIZE    (BASE_STACK_SIZE + LOCAL_VAR_SIZE + CALL_DEPTH_SIZE + SAFETY_MARGIN)
K_THREAD_STACK_DEFINE(my_stack, TOTAL_STACK_SIZE);
~~~


## Mutex
~~~
#include <zephyr/kernel.h>

K_MUTEX_DEFINE(my_mutex);

static int shared_counter = 0;

void thread_with_mutex(void *p1, void *p2, void *p3)
{
    int thread_id = (int)p1;

    while(1)
    {
        k_mutex_lock(&my_mutex, K_FOREVER);

        int old_value = shared_counter;
        k_sleep(K_MSEC(10));
        shared_counter = old_value + 1;
        printk("线程 : %d, counter = %d\n", thread_id, shared_counter);

        k_mutex_unlock(&my_mutex);
        k_sleep(K_MSEC(100));
    }
}

K_THREAD_DEFINE(thread1, 1024, thread_with_mutex, 
            (void *)1, NULL, NULL, 7, 0, 0);

K_THREAD_DEFINE(thread2, 512, thread_with_mutex,
            (void *)2, NULL, NULL, 7, 0, 0);
~~~
**临界区代码尽可能短, 减少锁的持有时间**
**避免在持有锁时调用可能阻塞的函数**
**使用K_FOREVER的时候小心死锁**
**尽量使用K_MSEC之类的超时函数替代K_FOREVER**

## 死锁
~~~
K_MUTEX_DEFINE(mutex1);
K_MUTEX_DEFINE(mutex2);

void thread_a(void *p1, void *p2, void *p3)
{
    k_mutex_lock(&mutex1, K_FOREVER);
    k_sleep(K_MSEC(10));
    k_mutex_lock(&mutex2, K_FOREVER);
    // ...
    k_mutex_unlock(&mutex2);
    k_mutex_unlock(&mutex1);
}

void thread_b(void *p1, void *p2, void *p3)
{
    k_mutex_lock(&mutex2, K_FOREVER);
    k_sleep(K_MSEC(10));
    k_mutex_lock(&mutex1, K_FOREVER);
    // ...
    k_mutex_unlock(&mutex1);
    k_mutex_unlock(&mutex2);
}
~~~
**上面是错误的使用顺序, 在使用互斥锁, 统一锁的使用顺序**
~~~
K_MUTEX_DEFINE(mutex1);
K_MUTEX_DEFINE(mutex2);

void thread_a_fixed(void *p1, void *p2, void *p3)
{
    k_thread_lock(&mutex1, K_FOREVER);          // 总是先锁1
    k_thread_lock(&mutex2, K_FOREVER);          // 再锁2
    // ...
    k_thread_unlock(&mutex2);
    k_thread_unlock(&mutex1);
}

void thread_b_fixed(void *p1, void *p2, void *p3)
{
    k_thread_lock(&mutex1, K_FOREVER);          // 总是先锁1
    k_thread_lock(&mutex2, K_FOREVER);          // 再锁2
    // ...
    k_thread_unlock(&mutex2);
    k_thread_unlock(&mutex1);
}
~~~

## Semaphore
~~~
// 定义信号量, 初始值0, 最大值10
K_SEM_DEFINE(my_sem, 0, 10);

void producter_thread(void *p1, void *p2, void *p3)
{
    int count = 0;
    while(1)
    {
        // 生产数据
        k_sleep(K_MSEC(500));
        count++;
        printk("生产者生产数据 %d\n", count);

        // 发送信号量(增加计数)
        k_sem_give(&my_sem);
    }
}


void consumer_thread(void *p1, void *p2, void *p3)
{
    while(1)
    {
        // 等待信号量
        k_sem_take(&my_sem, K_FOREVER);

        // 消费数据
        printk("消费者消费数据\n");
        k_sleep(K_MSEC(100));
    }
}

K_THREAD_DEFINE(producter, 1024, producter_thread,
            NULL, NULL, NULL, 7, 0, 0);

K_THREAD_DEFINE(consumer, 1024, consumer_thread,
            NULL, NULL, NULL, 7, 0, 0);
~~~

## Events
~~~
#include <zephyr/kernel.h>

K_EVENT_DEFINE(my_event);

#define EVENT_SENSOR_READY      BIT(0)      // 传感器就绪
#define EVENT_DATA_RECEIVED     BIT(1)      // 数据接收完成
#define EVENT_PROCESSING_DONE   BIT(2)

void sensor_thread(void *p1, void *p2, void *p3)
{
    while(1)
    {
        k_sleep(K_SENCONDS(1));

        k_event_post(&my_event, EVENT_SENSOR_READY);
    }
}

void data_received_thread(void *p1, void *p2, void *p3)
{
    while(1)
    {
        k_sleep(K_SENCONDS(2));
        
        k_event_post(&my_event, EVENT_DATA_RECEIVED);
    }
}

void main_thread(void *p1, void *p2, void *p3)
{
    // 默认 "AND", 必须等待EVENT_SENSOR_READY "AND" EVENT_DATA_RECEIVED同时就绪, 并且false表示不清除事件
    uint32_t events = k_event_wait(&my_event, EVENT_SENSOR_READY | EVENT_DATA_RECEIVED, false, K_FOREVER);

    if(events & EVENT_SENSOR_READY)
    {
        // .. 处理
    }

    if(events & EVENT_DATA_RECEIVED)
    {
        // .. 处理
    }

    // 清除事件
    k_event_clear(&my_event, EVENT_SENSOR_READY | EVENT_DATA_RECEIVED);

    // 处理事件
    k_sleep(K_MSEC(500));

    k_post_event(&my_event, EVENT_PROCESSING_DONE);
}

K_THREAD_DEFINE(sensor, 1024, sensor_thread, NULL, NULL, NULL, 7, 0, 0);

K_THREAD_DEFINE(data_received, 1024, data_received_thread, NULL, NULL, 7, 0, 0);

K_THREAD_DEFINE(main, 1024, main_thread, NULL, NULL, 5, 0, 0);
~~~
**事件有"AND"和"OR"模式**
**-"AND" : 必须所有的事件全部发生**
**-"OR" : 发生其中一个就可以**

## 总结 : 互斥锁, 信号量和事件
~~~
Mutex : 代码 K_MUTEX_DEFINE + k_mutex_lock/k_mutex_unlock
    - 自带优先级继承
    - 不能在中断中使用, 只能在线程上下文中使用

Semaphore : 代码 K_SEM_DEFINE + k_sem_take/k_sem_give
    - 可以在ISR中使用, 在中断中使用k_sem_give唤醒线程
    - 没有优先级继承

Events : 代码 K_EVENT_DEFINE + k_event_post/k_event_wait + k_event_clear
    - AND和OR机制组合, 比如手表亮屏, 必须同时满足 抬腕 + 光线很暗


Application :
    - 保护共享数据 : mutex
    - 资源池管理 : Semaphore   三个内存块, 线程拿一个用, 用完还回来  K_SEM_DEFINE(sem, 3, 3)
    - 简单事件通知 : Semaphore K_SEM_DEFINE(sem, 0, 1) 中断检测到按键按下, 通知主线程去处理
    - 复杂事件组合 : Events
    - ISR到线程通知: Semaphore/Events, 不能是Mutex

Mutex和Semaphore的核心区别 : 所有权
    Mutex是有主人的(一个线程), 必须 ** 谁拿的谁开 **
    Semaphore没有主人(谁都可以give/take), 因此中断可以调用
~~~


## 中断
**中断和线程通信一律使用信号量/事件**
~~~
#include <zephyr/kernel.h>
#include <zephyr/irq.h>

#define MY_IRQ_NUM      25
#define MY_IRQ_PRIORITY 2

void my_isr(const void *arg)
{
    // 清除中断标志位
    clear_interrupt_flag();

    // 通知线程处理数据
    k_sem_give(&data_ready_sem);
}

void setup_interrupt(void)
{
    IRQ_CONNECT(MY_IRQ_NUM, MY_IRQ_PRIORITY, my_isr, NULL, 0);
    irq_enable(MY_IRQ_NUM);
}
~~~


**中断嵌套**
~~~
#define HIGH_PRIORITY_IRQ       0
#define LOW_PRIORITY_IRQ        4

void high_priority_isr(const void *arg)
{

}

void low_priority_isr(const void *arg)
{

}

void setup_nested_interrupts(void)
{
    IRQ_CONNECT(10, HIGH_PRIORITY_IRQ, high_priority_isr, NULL, 0);
    IRQ_CONNECT(11, LOW_PRIORITY_IRQ, low_priority_isr, NULL, 0);

    irq_enable(10);
    irq_enable(11);
}
~~~
**可以使用CONFIG_IRQ_OFFLOAD**
**irq_offload()把原本在ISR中干的活, 打包成一个函数, 扔给高优先级线程执行(sysworkq)**
**使用Semaphore或者irq_offload把ISR精简到极致**

## 经典设计模式 : 信号量+volatile变量
**ISR尽可能短, 复杂处理逻辑延迟到线程上下文**
~~~
#include <zephyr/kernel.h>

K_SEM_DEFINE(isr_sem, 0, 1);

static volatile uint32_t isr_data = 0;

void data_ready_isr(const void *arg)
{
    // 读数据
    isr_data = read_hardware_register();

    // 通知处理线程
    k_sem_give(&isr_sem);
}

void data_process_thread(void *p1, void *p2, void *p3)
{
    while(1)
    {
        // 等待信号量
        k_sem_take(&isr_sem, K_FOREVER);

        // 处理数据
        uint32_t data = isr_data;       // 这是一个极其优秀的习惯, 如果直接使用isr_data这个全局变量, 如果在发生中断, 这个变量就冲突了
        process_data(&data);
    }
}


K_THREAD_DEFINE(data_thread, 1024, data_process_thread,
            NULL, NULL, NULL, 7, 0, 0);

void setup_isr_thread_interaction(void)
{
    IRQ_CONNECT(MY_IRQ_NUM, 2, data_ready_isr, NULL, 0);
    irq_enable(MY_IRQ_NUM);
}
~~~

**中断ISR应该做的核心四件事**
**-1. 读取硬件寄存器**
**-2. 清除中断标志位**
**-3. 更新简单的全局变量**
**-4. 使用k_sem_give/k_event_post和线程通信, 把复杂的处理任务交给线程上下文**

~~~
void good_isr(const void *arg)
{
    uint32_t reg_data = read_hardware();
    buffer[buffer_index++] = reg_data;
    k_sem_give(&processing_sem);
    clear_interrupt_flag();
}
~~~

## 工作队列
**工作队列 VS 信号量+线程**
**- 工作队列适合一次性/偶发性的延迟处理, 无需创建专用线程**
**- 信号量+线程适合持续性, 需要特定优先级的处理任务**
~~~
#include <zephyr/kernel.h>

static struct k_work my_work;


// 标准的线程上下文
void work_handler(struct k_work *work)
{
    printk("工作队列线程, 处理延迟任务");

    k_sleep(K_MSEC(100));
    process_complex_data();
}

// 中断服务例程
void isr_with_work(const void *arg)
{
    // 见上面中断ISR应该做的核心四件事
    // ...
    k_work_submit(&my_work);
}


void setup_work_queue(void)
{
    // 初始化work
    k_work_init(&my_work, work_handler);

    // 注册终端
    IRQ_CONNECT(10, 2, isr_with_work, NULL, 0);
    irq_enable(10);
}
~~~

## ISR和Thread通信方式 : Semaphore, Events和work三种方式
~~~
~~~

## 消息队列
**用来传递固定大小的消息**
~~~
#include <zephyr/kernel.h>

struct data_msg
{
    uint32_t timestamp;
    uint16_t sensor_id;
    uint16_t value;
};

K_MSGQ_DEFINE(my_msgq, sizeof(struct data_msg), 10, 4);

void sender_thread(void *p1, void *p2, void *p3)
{
    struct data_msg msg;
    uint32_t count = 0;

    while(1)
    {
        msg.timestamp = k_uptime_get_32();
        msg.sensor_id = 1;
        msg.value = count++;

        int ret = k_msgq_put(&my_msgq, &msg, K_NO_WAIT);
        if(ret == 0)
        {
            printk("发送消息 : value = %d\n", msg.value);
        }
        else 
        {
            printk("消息队列已满\n");
        }

        k_sleep(K_MSEC(500));
    }
}


void receiver_thread(void *p1, void *p2, void *p3)
{
    struct data_msg msg;

    while(1)
    {
        k_msgq_get(&my_msgq, &msg, K_NO_WAIT);

        printk("接收消息 : sensor = %d, value = %d, time = %u\n", msg.sensor_id, msg.value, msg.timestamp);

        k_sleep(K_MSEC(100));
    }
}

K_THREAD_DEFINE(sender, 1024, sender_thread, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(receiver, 1024, receiver_thread, NULL, NULL, NULL, 7, 0, 0);
~~~

**消息队列**
**- 固定消息大小**
**- FIFO顺序**
**- 支持阻塞非阻塞**
**- 可以在ISR中使用(K_NO_WATI)**





