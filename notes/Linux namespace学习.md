# Linux Namespace 学习

## 介绍

Linux Namespace 是一种轻量级的进程隔离技术，它允许系统在同一个操作系统实例中创建多个隔离的环境。每个 Namespace 提供一套独立的资源视图，使得进程在其中可以如同在独立的系统环境中运行。这种技术在容器技术（如 Docker）中被广泛使用。

**PID Namespace**：

- 隔离进程 ID 空间。不同 PID Namespace 中的进程可以有相同的进程 ID，子 Namespace 的进程无法看到父 Namespace 中的进程。
- 例如，容器内的进程可能认为自己是 PID 1，而在主机上它可能是一个高于 1 的进程 ID。

**Mount Namespace**：

- 隔离文件系统挂载点。每个 Namespace 可以有独立的挂载点和文件系统视图。
- 这允许在一个容器内挂载文件系统，而不会影响其他容器或主机系统。

**UTS (UNIX Timesharing System) Namespace**：

- 隔离主机名和域名。每个 Namespace 可以有独立的主机名和域名。
- 这在容器中尤为重要，允许每个容器有自己的网络标识。

**IPC (Inter-Process Communication) Namespace**：

- 隔离进程间通信资源，如信号量、消息队列和共享内存。
- 使得一个容器内的进程间通信不会影响其他容器。

**Network Namespace**：

- 隔离网络接口和网络栈。每个 Namespace 可以有独立的网络设备、IP 地址、防火墙规则等。
- 这使得容器可以有独立的网络配置，甚至可以连接不同的虚拟网络。

**User Namespace**：

- 隔离用户和用户组 ID。每个 Namespace 可以有独立的用户和组 ID 映射。
- 这提高了安全性，例如在容器内运行的进程可以被认为是以root用户身份运行，而在主机上实际运行时可能具有更低的权限。

**Cgroup Namespace**：

- 隔离 cgroup 视图，使进程只能看到属于自己 cgroup 的资源限制和使用情况。
- 这增强了资源管理和限制的安全性和灵活性。

## 创建namespace

### unshare()

```
 int unshare(int flags);
 
 flags：
  CLONE_FS
  CLONE_NEWIPC
  CLONE_NEWNS
  ...
```

调用unshare后，进程创建的子进程会进入新的命名空间，但当前进程不会。

### clone()

会创建一个子进程,可以指定子进程的namespace。

```C
 int flags = CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET |
              CLONE_NEWUTS | SIGCHLD;
  int pid =
      clone(InitAndStartContainer, stack + stack_size, flags, (void *)&params);

// 指定SIGCHLD,子进程终止时会发送SIGCHLD信号给父进程
```

## 总结

日后可以增加更多实践