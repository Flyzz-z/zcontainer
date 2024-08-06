# Linux Cgroups学习

本文介绍Linux Ggroups v1。

## 介绍

cgroups 的全称是control groups，cgroups为每种可以控制的资源定义了一个子系统。典型的子系统介绍如下：

1. cpu 子系统，主要限制进程的 cpu 使用率。
2. cpuacct 子系统，可以统计 cgroups 中的进程的 cpu 使用报告。
3. cpuset 子系统，可以为 cgroups 中的进程分配单独的 cpu 节点或者内存节点。
4. memory 子系统，可以限制进程的 memory 使用量。
5. blkio 子系统，可以限制进程的块设备 io。
6. devices 子系统，可以控制进程能够访问某些设备。
7. net_cls 子系统，可以标记 cgroups 中进程的网络数据包，然后可以使用 tc 模块（traffic control）对数据包进行控制。
8. freezer 子系统，可以挂起或者恢复 cgroups 中的进程。
9. ns 子系统，可以使不同 cgroups 下面的进程使用不同的 namespace。

## 使用cgroups

cgroups使用VFS暴露功能，cgroups是一个子文件系统，在/sys/fs/cgroup对应子系统下进行操作就可以操作cgroups.

/sys/fs/cgroup目录如下

![image-20240610193018690](.\assets\image-20240610193018690.png)

每个目录代表一个子系统，每个cgroup可以关联到多个子系统，如在cpu和memory目录下创建目录Cgroup1,即创建了一个cgroup,它和cpu子系统和memory子系统关联，可以设置cpu限制和内存限制。

进程，cgroup,子系统关系：

![image-20240610193935889](.\assets\image-20240610193935889.png)

在对应cgroup目录tasks文件中写入pid即可将对应进程加入cgroup。

cpu子系统目录下cgroup目录如下，其中文件在创建cgroup目录时由Cgroups文件系统自动创建：

![image-20240610194108325](.\assets\image-20240610194108325.png)

向cpu.cfs_period_us和cpu.cfs_quota_us写入数值即可控制cpu使用率=cpu.cfs_quota_us / cpu.cfs_period_us。如4核，利用率50%即最多使用两核。

## 参考

[Linux资源管理之cgroups简介-美团技术团队](https://tech.meituan.com/2015/03/31/cgroups.html)

