## 如果要在特定的 network namespace 下执行网络操作
ip netns exec ${net_namespace_name} ${cmd}

##增加一个网桥 container0
brctl addbr container0
brctl stp container0 off
ifconfig container0 192.168.10.1/24 up #为网桥设置IP地址

## 创建一个 network namespace
ip netns add ns1

# 激活namespace中的loopback，即127.0.0.1
ip netns exec ns1 ip link set dev lo up
ip netns exec ns1 ip link show
ip netns exec ns1 ping 127.0.0.1
ip netns exec ns1 ping baidu.com

# 增加一个 peer 虚拟网卡，名字 inside, type 为 veth, 它有一个 peer，名字 outside
ip link add inside type veth peer name outside
ip link show

# 把 inside 放到 namespace ns1中
ip netns exec ns1 ip link show
ip link set inside netns ns1
ip netns exec ns1 ip link show

# 把 inside 改名为 eth0, 不同的 namespace 之间可以重名
ip netns exec ns1 ip link set dev inside name eth0
ip netns exec ns1 ip link show

# 分配一个IP地址，并激活它
ip netns exec ns1 ifconfig eth0 192.168.10.11/24 up
ip netns exec ns1 ip link show

# 把 outside 添加到 container0
brctl addif help
brctl addif container0 outside

# 为容器增加一个路由规则，让容器可以访问外面的网络
ip netns exec ns1 ip route
ip netns exec ns1 ip route add default via 192.168.10.1

# 在 /etc/netns 下创建 ns1 的目录，设置resolv.conf
mkdir -p /etc/netns/ns1
echo "nameserver 233.5.5.5" > /etc/netns/ns1/resolv.conf
ip netns exec ns1 ping baidu.com

# 给 docker 容器动态添加一张网卡
container_id=$(docker ps -q | awk 'NR==1{ print $1 }')
container_pid=$(docker inspect -f '{{.State.Pid}}' ${container_id})
mkdir -p /var/run/netns/
ln -sfT /proc/$container_pid/ns/net /var/run/netns/$container_id
ip netns exec "${container_id}" ip -s link show eth0
ip link add peerA type veth peer name peerB
brctl addif docker0 peerA
ip link set peerA up
ip link set peerB netns ${container_id}
ip netns exec ${container_id} ip link set dev peerB name eth1
ip netns exec ${container_id} ip link set eth1 up
ip netns exec ${container_id} ip addr add 172.17.0.250 dev eth1
mkdir -p /etc/netns/${container_id}
echo "nameserver 233.5.5.5" > /etc/netns/${container_id}/resolv.conf