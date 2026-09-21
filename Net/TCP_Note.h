
/**
 * TCP的可靠性 :
 *      数据无丢失
 *      数据无重复
 *      数据无失序
 *      数据无错误
 * 
 *  全双工可靠按序无重复的字节流数据
 * 
 * 
 *  src port (16 bit)  dst port (16 bit)
 *  seq (32 bit)
 *  ack (32 bit)
 * 
 * 
 *  A - ACK : TCP规定, 连接建立后, ACK必须为1
 *  S - SYN : 只有在前两次握手中SYN为1
 *  F - FIN : 若FIN=1, 表示数据已经发送完成, 可以释放连接
 * 
 * 
 *  三次握手的主要作用 : 确认双方接收和发送能力是否正常, 指定自己的初始化序列号为后面可靠性传送做准备
 *                      实质就是连接服务器指定端口, 建立TCP连接, 同步双方的序列号和确认号, 交换TCP窗口的大小信息
 * 
 * 
 *  Client      --- SYN = 1, ACK = 0, seq = x                   --->  Server
 *              <--- SYN = 1, ACK = 1, ack = x + 1, seq = y     ---     
 *              --- SYN = 0, ACK = 1, seq = x + 1, ack = y + 1  --->
 * 
 * 
 *  ESTABLISHED                                                     ESTABLISHED
 *              ------------- FIN = 1 --------------------------->
 *  FIN_WAIT_1                                                      ESTABLISHED 
 *              <------------ ACK = 1 ----------------------------
 *  FIN_WAIT_2                                                      CLOSED_WAIT
 *              <------------ FIN = 1 ----------------------------
 *  FIN_WAIT_2                                                      LAST_ACK
 *              ------------- ACK = 1 ---------------------------->  
 *  TIME_WAIT                                                       CLOSE
 * 
 *                                                             
 * 
 * 
 *  - 客户端打算关闭连接, 发送一个FIN标志为1的报文, 客户端进入FIN_WAIT_1状态
 *  - 服务端收到该报文后, 向客户端发送ACK响应, 服务端进入CLOSED_WATI状态
 *  - 客户端接收到ACK之后, 进入FIN_WAIT_2状态, 等待服务端处理完数据, 也想客户端发送FIN, 服务端进入LAST_ACK状态
 *  - 客户端收到服务端发来的FIN之后, 回应一个ACK, 然后客户端进入TIME_WAIT状态, 服务端接收到ACK之后, 进入CLOSE状态, 服务端连接关闭, 等待2MSL之后, 客户端也进入CLOSED, 客户端连接关闭
 * 
 * 
*/


/**
 * 
 *  UDP 不保证数据的可靠性, 面向报文的
 *      使用场景 : 
 *          - DNS   
 *          - 广播 组播
 * 
 *  8 bytes
 *  Src Port (16 bit)           Dst Port (16 bit)
 *  UDP Data Len (16 bit)       Checksum (16 bit)
*/