# exec命令实现

我们一般使用docker exec命令进入容器内部运行命令，这是如何实现的呢，其实就是创建一个进程加入容器命名空间，运行命令即可。

## 获取容器环境变量

即便可以加入容器进程命名空间，但不会自动设置环境变量，需要手动获取容器进程环境变量，可以从`/proc/{container pid}/environ`文件中读取。

## 加入容器命名空间

使用`setns(int fd,int nstype)`可以加入fd所表示的命名空间，如何获取命名空间的fd呢？都在`/proc/{container pid}/ns/`目录下。

![image-20240615185847559](assets\image-20240615185847559.png)

在对应目录下可以找到进程对应命名空间的文件，挑选需要加入的加入即可。

```c++
  std::vector<std::string> ns_list = {"ipc", "pid", "uts", "net", "mnt"};
  for (const auto &ns : ns_list) {
    std::string ns_path =
        "/proc/" + std::to_string(container_pid) + "/ns/" + ns;
    int fd = open(ns_path.c_str(), O_RDONLY);
    if (setns(fd, 0) == -1) {
      LOG(ERROR) << "setns error:" << strerror(errno);
      return false;
    }
    close(fd);
  }
```

## 运行命令

之后再运行命令即可。



