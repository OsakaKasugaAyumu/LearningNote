/**
 *  Threads :
 *      其存在的本质 : 
 *          - 处理那些对于ISR来说过于冗长或过于复杂的应用程序逻辑
 *      线程是一个内核对象(Kernel Object)
 *      定义线程数量仅仅受限可用的RAM
 * 
 *  线程的七大属性 : 
 *      - Stack
 *      - Thread Control Block  
 *      - Entry Point Function (Up to 3 argument values)
 *      - Scheduling Priority
 *      - Thread Options
 *      - Start Delay
 *      - Execution Mode (Supervisor mode or User mode) by CONFIG_USERSPACE
 * 
 * 
 * 
 *  Thread Creation : 
 *      - 内核允许线程开始执行之前取消延迟启动, 如果线程已经启动, 请求将无效. 成功被取消的线程必须重新创建
 * 
 *  Thread Starting :
 *      - 以F_FOREVER作为启动延迟的线程, 不会加入到调度器就绪队列, 直到显式k_thread_start()启动才开始执行, 启动一个已经启动的线程没有效果
 * 
 *  Thread Termination :
 *      - 线程通过其entry point function返回来同步结束其执行的, 这就是线程的终止
 * 
 *  Thread Aborting :
 *      - 手动k_thread_abort()强制中止一个线程(不推荐), 如果线程发生了致命错误(解引用空指针), 内核会自动中止该线程
 * 
 *  Thread Suspension : 
 *      - k_thread_suspend()无期限阻止线程执行, 只能由k_thread_resume()来解除挂起
 *      - k_sleep()有期限阻止线程执行, 与k_thread_suspend()不同, k_sleep()在睡眠时间限制到达时自动变为可执行状态
 * 
 *  Summary : 
 *      - 内核仅仅初始化TCB + Stack的一端, 栈的其余部分未初始化
 *      - 立即启动 (K_NO_WAIT)
 *        延时启动 (指定超时时间)
 *        挂起启动 (K_FOREVER + k_thread_start())
 *      - 线程终止时, 内核不回收资源(互斥锁, 动态内存), 必须线程自己释放
 *      - 终止同步 + k_thread_join()阻塞等待目标线程完全终止
 *      - 内存复用, k_thread这个TCB内存用于重新创建线程
 * 
 *  PS: 线程设计的最高原则是“永不终止，永不中止，避免挂起”
 * 
 *  线程的状态 : 
 *      二值状态 : Ready and Unready
 *          - 一个没有阻碍因素的线程被视为Ready
 *          - 一个或多个阻碍其执行因素的线程被视为Unready
 *              - 未启动
 *              - 等待内核对象完成操作(比如等一个信号量、互斥锁、消息队列)
 *              - 等待超时(sleep)  这是最安全的阻塞方式, 明确了时间边界, 线程一定会回到Ready state
 *              - 挂起(suspend)
 *              - 终止(terminated) / 中止(abort)
 * 
 *  PS: 内核角度, 线程就只有Ready和Unready状态
 * 
 * 
 *  Thread State :
 * 
 *      - 
 *      [New]  --- start ---> [Ready] --- dispatch ---> [Running] --- suspend --> [Suspended] ---|
 *                              |                                                                |
 *                              | < -------------------------- resume ---------------------------|
 * 
 * 
 * 
 *      -
 *      [New]  --- start ---> [Ready] --- dispatch ---> [Running] ---|
 *                              |                                    |
 *                              | < --------- interrupt -------------|
 * 
 * 
 *      -
 *      [New]  --- start ---> [Ready] --- dispatch ---> [Running] --- abort ---> [Terminated]
 * 
 * 
 * 
 *      - 
 *      [New]  --- start ---> [Ready] --- dispatch ---> [Running] --- IO or event wait ---> [Waiting] ---|
 *                              |                                                                        |
 *                              | < ------------------------ IO or event completion  --------------------|
 * 
 *  
 *      Running/Ready 属于Ready态
 *      Suspended/Terminated/Waiting 属于Unready态
 * 
 * 
 * 
 * 
 *  Thread Stack :
 *      - K_KERNEL_STACK
 *      - K_THREAD_STACK
 *      
*/

/**
 *  Thread Priority :
 *      - 数值越小, 优先级越高
 *      - 可正可负
 * 
 *  根据优先级, 线程分类 :
 *      - Cooperative Threads
 *          - 优先级负数
 *          - 一旦成为此线程, 绝不可以被抢占, 只有自己主动变为Unready状态(比如 k_sleep, 等待信号量)
 * 
 *      - Preemptible Threads
 *          - 优先级非负数
 *          - 该优先级的线程可以被处于Ready状态的Cooperative Threads或者更高级别的Preemptible Threads抢占
 * 
 *      PS: 只要Cooperative Threads就绪, Preemptible Threads必须让路
 * 
 *      IDLE Thread :
 *          - Idle 线程占据唯一的绝对最低优先级
 * 
 *      Cooperative Threads : -CONFIG_NUM_COOP_PRIORITIES to -1
 *      Preemptive Threads : 0 to (CONFIG_NUM_PREEMPT_PRIORITIES - 1)
 *      PS: Configuring 5 cooperative priorities and 10 preemptive priorities result in the ranges [-5 to -1] and [0 to 9].
 * 
 * 
 *      
*/