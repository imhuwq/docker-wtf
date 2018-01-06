#!/usr/bin/env bash
mkdir mnt
sudo mount -t aufs -o dirs=./aaa:./bbb none ./mnt
tree ./mnt
