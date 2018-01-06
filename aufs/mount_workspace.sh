#!/usr/bin/env bash
mkdir mnt
mkdir workspace
sudo mount -t aufs -o dirs=./workspace=rw:./aaa=ro:./bbb=ro none ./mnt
tree ./mnt
