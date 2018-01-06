# Linux Control Group
# Resource limitation: 限制资源使用，比如内存使用上限以及文件系统的缓存限制。
# Prioritization: 优先级控制，比如：CPU利用和磁盘IO吞吐。
# Accounting: 一些审计或一些统计，主要目的是为了计费。
# Control: 挂起进程，恢复执行进程。

mount -t cgroup
cd /sys/fs/cgroup
ls cpu
mkdir cpu/john
ls cpu/john

cat /sys/fs/cgroup/cpu/john/cpu.cfs_quota_us
echo 20000 > /sys/fs/cgroup/cpu/john/cpu.cfs_quota_us
echo 36323 >> /sys/fs/cgroup/cpu/john/tasks