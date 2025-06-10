# zcontainer

### boost库位置

安装指南：https://www.zhangqiongjie.com/1596.html

头文件：/usr/local/include/boost

lib: /usr/local/lib/

### 重定向tty


```c
  if (run_params->tty) {

    int tty_fd = open("/dev/tty", O_RDWR);
    if (tty_fd == -1) {
      LOG(ERROR) << "open /dev/tty:" << strerror(errno);
      return EXIT_FAILURE;
    }

    // 重定向标准输入到 tty
    if (dup2(tty_fd, STDIN_FILENO) == -1) {
      LOG(ERROR) << "dup2 tty_fd to STDIN_FILENO:" << strerror(errno);
      return EXIT_FAILURE;
    }

    // 重定向标准输出到 tty
    if (dup2(tty_fd, STDOUT_FILENO) == -1) {
      LOG(ERROR) << "dup2 tty_fd to STDOUT_FILENO:" << strerror(errno);
      return EXIT_FAILURE;
    }

    // 关闭 tty 文件描述符
    close(tty_fd);
  }
```

### 检测当前机器是否支持cgroups v2

```
grep cgroup /proc/filesystems
```

### zcontainer支持功能

至少实现容器server(后台运行),client通信

| 功能                         | 是否支持 |
| :--------------------------- | :------: |
| namespace隔离                |   支持   |
| cgroups限制cpu,mem,cpuset    |   支持   |
| pivot_root文件系统隔离       |   支持   |
| 使用overlay2支持镜像读写隔离 |   支持   |
| -v数据卷挂载                 |   支持   |
| exec命令                     |   支持   |
| -d后台运行                   |  半支持  |
| ps查看运行中容器             |  不支持  |
| -e指定环境变量               |   支持   |
| 容器网络                     |   支持   |
|                              |          |

### zcontainer vscode cmake命令

```
/usr/local/cmake-3.25.0/bin/cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/local/clang16/bin/clang -DCMAKE_CXX_COMPILER:FILEPATH=/usr/local/clang16/bin/clang++ -S/home/flyzz/zcontainer -B/home/flyzz/zcontainer/build -G Ninja
```

