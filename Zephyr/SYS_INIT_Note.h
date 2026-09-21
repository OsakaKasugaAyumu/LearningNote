/**
 * 
 * 2026/09/20
 * 
 * Precondition:
 * 
 * 
 * - STRINGIFY(s) 
 *      - Z_STRINGIFY(s)
 *      - #s
 * 
 * 
 * 
 * - _CONCAT(x, y)
 *      - _DO_CONCAT(x, y)
 *      - x ## y
 * 
 * 
 * - __used
 *      - __attribute__((__used__))
 * 
 * 
 * - Z_DECL_ALIGN(type)
 *      - __aligned(__alignof(type)) type
 *      - __attribute__((__aligned__(__alignof(type)))) type
 * 
 * 
 * 
 * 
 *      @brief 这段代码的目的 : 为了让zephyr系统按照 level / prio / sub_prio的顺序自动调用初始化函数
 *        其中level就是原样字符化 APPLICATION ==> #APPLICATION ==> "APPLICATION"
 *        但是prio和sub_prio必须要展开, 比如宏 MY_INIT_PRIO  ==> STRINGIFY(MY_INIT_PRIO) ==> #80 ==> "80"
 * - Z_INIT_ENTRY_SECTION(level, prio, sub_prio)
 *      - __attribute__((__section__(".z_init_" #level "_P_" STRINGIFY(prio) "_SUB_" STRINGIFY(sub_prio) "_")))
 * 
 * 
 * 
 * - Z_INIT_ENTRY_NAME(init_id)
 *      - _CONCAT(__init_, init_id)
 * 
 * 
 *  @param: init_fn : 要注册到初始化流程的初始化函数, 不检查返回值
 *  @param: level :  EARLY -> PRE_KERNEL_1 -> PRE_KERNEL_2 -> POST_KERNEL -> APPLICATION
 *  @param: priority : 无符号整数
 *  SYS_INIT(init_fn, level, priority)
 * 
 * 
 *  例 : 
 *      SYS_INIT(application_manager_init, POST_KERNEL, 90);
 * 
 *      - SYS_INIT_NAMED(application_manager_init, application_manager_init, POST_KERNEL, 90);
 * 
 *      - static const Z_DECL_ALIGN(struct init_entry) Z_INIT_ENTRY_SECTION(POST_KERNEL, 90, 0) __used Z_INIT_ENTRY_NAME(application_manager_init) = {.init_fn = (application_manager_init), .dev = NULL};
 * 
 *      - static const __attribute__((__aligned__(__alignof(struct init_entry)))) struct init_entry __attribute__((__section__(".z_init_POST_KERNEL_P_90_SUB_0_"))) __attribute__((__used__))
 *        __init_application_manager_init = {.init_fn = application_manager_init, .dev = NULL};
 * 
 * 
*/


/**
 *  PS: 字符化的例子 ：
 *      #define MY_INIT_PRIO 80
 *      STRINGIFY(MY_INIT_PRIO)
 *          - Z_STRINGIFY(80)
 *          - #80
 *          - "80"
*/