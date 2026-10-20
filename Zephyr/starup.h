/**
 * 
 *  Zephyr 启动流程说明 :
 * 
 *  定义主栈 z_main_stack
 * 
 *  K_THREAD_STACK_DEFINE(z_main_stack, CONFIG_MAIN_STACK_SIZE);
 * 
 *      - 1. Z_KERNEL_STACK_DEFINE_IN(z_main_stack, 1024, __kstackmem);
 * 
 *      - 2. Z_KERNEL_STACK_DEFINE_IN(z_main_stack, 1024, __noinit);
 * 
 *      - 3. struct z_thread_stack_element __noinit __aligned()
 * 
*/



