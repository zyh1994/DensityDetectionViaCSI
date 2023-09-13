#include <stdio.h>
#include <string.h>
#include <uv.h>

static uv_udp_t client;       // libuv UDP 客户端结构
static uv_loop_t* loop;       // libuv 事件循环


// 分配回调：用于为接收数据分配内存
static void alloc_cb(uv_handle_t* handle,
                     size_t suggested_size,
                     uv_buf_t* buf)
{
    static char slab[65536];  // 静态的缓冲区
    buf->base = slab;
    buf->len = sizeof(slab);
}

// 关闭回调：在句柄关闭时调用
static void close_cb(uv_handle_t* handle)
{
    uv_is_closing(handle);
}

// 客户端接收回调：在接收到数据时调用
static void cl_recv_cb(uv_udp_t* handle,
                       ssize_t nread,
                       const uv_buf_t* buf,
                       const struct sockaddr* addr,
                       unsigned flags)
{
    if (nread < 0) 
    {
        // 错误处理可以在这里进行
    }

    if (nread == 0)
    {
        // 没有数据读取
        return;
    }

    printf("I receive the message from server:%s\n", buf->base);  // 打印接收到的数据

    uv_close((uv_handle_t*)handle, close_cb);  // 关闭UDP客户端
}

// 客户端发送回调：数据发送后调用
static void cl_send_cb(uv_udp_send_t* req, int status)
{
    // 开始接收数据
    uv_udp_recv_start(req->handle, alloc_cb, cl_recv_cb);
}

int main(void)
{
    struct sockaddr_in addr;
    uv_udp_send_t req;
    uv_buf_t buf;
    int r;

    loop = uv_default_loop();  // 获取默认事件循环

    r = uv_ip4_addr("0.0.0.0", 0, &addr);  // 使用IPv4地址和随机端口
    r = uv_udp_init(loop, &client);        // 初始化UDP客户端
    r = uv_udp_bind(&client, (const struct sockaddr*)&addr, 0);  // 绑定客户端
    r = uv_udp_set_broadcast(&client, 1);  // 启用广播

    uv_ip4_addr("255.255.255.255", 8080, &addr);  // 设置要发送到的广播地址和端口

    char msg[] = { 'P', 'I', 'N', 'G'};    // 要发送的消息
    buf = uv_buf_init(msg, 4);             // 初始化缓冲区

    printf("I am sending message to server:%s.\n", buf.base);  // 打印发送的消息
    uv_udp_send(&req,
                &client,
                &buf,
                1,
                (const struct sockaddr*)&addr,
                cl_send_cb);  // 发送数据

    uv_run(loop, UV_RUN_DEFAULT);  // 运行事件循环

    return 0;
}
