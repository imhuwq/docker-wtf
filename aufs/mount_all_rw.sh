#!/usr/bin/env bash
mkdir mnt
sudo mount -t aufs -o dirs=./aaa=rw:./bbb=rw none ./mnt
tree ./mnt