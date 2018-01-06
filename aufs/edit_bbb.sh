#!/usr/bin/env bash
echo b1 > ./mnt/b1
echo mnt $(cat ./mnt/b1)
echo aaa $(cat ./aaa/b1)
echo bbb $(cat ./bbb/b1)
