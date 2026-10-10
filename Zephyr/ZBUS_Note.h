/**
 * 
 * 2026/09/20
 * 
 * 
 *  Zbus : [核心理念] 轻量级, 多对多, 旨在通过解耦彻底提升嵌入式软件的模块化和可维护性
 * 
 *          - 空间解耦 :
 *              通信双方(发布者与观察者)不需要知道彼此的存在.
 * 
 *          - 时间解耦 :
 *              发布者发送消息之后, 无需等待接收者处理, 支持异步操作
 * 
 *          - 多对多 :
 *              一个线程发布消息, 可以同时被多线程/回调接收
 * 
 *  双通信范式融合 : 
 *          - 原生支持消息传递
 *          - 原生支持发布/订阅
 * 
 *  Channel-based : 
 *          - 硬件和业务逻辑彻底解耦
 *          - 基于共享内存
 * 
 * 
 * 
 *  Puslisher Roles :
 *          Virturl Distributed Event Dispatcher
 *          - 直接调用(Listener)
 *          - 调度到工作队列(Async Listener)
 *          - 放入队列(Subscriber放入消息队列, Message Subscriber放入FIFO)
 * 
 *  Observers Roles :
 *      subscribers             (Threads)
 *      message subscribers     (Threads)
 *      listeners               (callbacks)
 *      async listeners         (callbacks defered to a work queue)
 * 
*/

/**
 *      通道(全局共享)
 *      Publisher : Locking -> memory copy -> unlocking
 *              - 获取该通道的互斥锁
 *              - 将发布者线程栈上的数据，拷贝到通道的全局共享内存（Message）中
 *              - 释放通道互斥锁
 *              - VDED
 * 
 *       VDED:
 *          - Listener : 发布者直接调用回调
 *          - Async Listener : 发布者将回调塞到系统工作队列(k_work), 由系统线程以后执行
 *          - Subscriber : 发布者把通道引用扔进订阅者内部的消息队列, 并唤醒subscriber thread
 *          - Message Subscriber : 发布者线程把“消息的副本”扔进订阅者内部的 FIFO，并唤醒订阅者线程
 *          
 * 
 * 
 * 
 *       观察关系 : Channel和Observer
 *              - 对于每一个观察关系, 都会创建一个通道/观察者 pair配对
*/


/**
 *  
 * 
*/



/**
 * Precondition:
 * 
 * - ZBUS_OBSERVERS(...)
 *      - __VA_ARGS__
 * 
 * 
 *      @note ##__VA_ARGS__这个##的作用是当__VA_ARGS__为空时, 消除前面的逗号
 * - ZBUS_MSG_INIT(_val, ...)
 *      - {_val, ##__VA_ARGS__}   
 * 
 * 
 * - _CONCAT(x, y)
 *      - _DO_CONCAT(x, y)
 *      - x ## y
 * 
 * 
 * 
 * - _ZBUS_MESSAGE_NAME(_name)
 *      - _CONCAT(_zbus_message_, _name)
 *      - _DO_CONCAT(_zbus_message_, _name)
 *      - _zbus_message__name
 * 
 * 
 * 
 * - _ZBUS_CHAN_DEFINE(_name, _id, _type, _validator, _user_data)
 *      - static struct zbus_channel_data _CONCAT(_zbus_chan_data_, _name) = {.observers_start_idx = -1, .observers_end_idx = -1};
 *        const STRUCT_SECTION_ITERABLE(zbus_channel, _name) = {.message = &_zbus_message__name, .message_len = sizeof(_type), .user_data = _user_data, .validator = _validator, .data = &_CONCAT(_zbus_chan_data_, _name)}
 * 
 * 
 * 
 * - ZBUS_OBS_DECLARE(...)  
 * 
 * 
 *  - ZBUS_CHAN_DEFINE(activity_state_data_chan, struct activity_state_event, NULL, NULL, ZBUS_OBSERVERS(watchface_activity_state_event), ZBUS_MSG_INIT());
 *      - static struct activity_state_event _zbus_message_activity_state_data_chan = ZBUS_MSG_INIT();
 *        _ZBUS_CHAN_DEFINE(activity_state_data_chan, 0xFFFFFFFF, struct activity_state_event, NULL, NULL);
 *        ZBUS_OBS_DECLARE(ZBUS_OBSERVERS(watchface_activity_state_event));
 *        FOR_EACH_FIXED_ARG_NONEMPTY_TERM(_ZBUS_CHAN_OBSERVATION, (;), activity_state_data_chan, watchface_activity_state_event)
 * 
 * 
 *      - static struct activity_state_event _zbus_message_activity_state_data_chan = {};
 *        static struct zbus_channel_data _zbus_chan_data_activity_state_data_chan = {.observers_start_idx = -1, .observers_end_idx = -1};
 *        const STRUCT_SECTION_ITERABLE(zbus_channel, activity_state_data_chan) = {.message = &_zbus_message__name, .message_len = sizeof(_type), .user_data = _user_data, .validator = _validator, .data = &_CONCAT(_zbus_chan_data_, _name)}
 *        ZBUS_OBS_DECLARE(watchface_activity_state_event);
 *        
 *        
 * 
*/