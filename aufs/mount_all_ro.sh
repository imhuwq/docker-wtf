#!/usr/bin/env bash
mkdir mnt
sudo mount -t aufs -o dirs=./aaa=ro:./bbb=ro none ./mnt
tree ./mnt