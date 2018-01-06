#!/usr/bin/env bash
echo common > ./mnt/common
echo mnt $(cat ./mnt/common)
echo aaa $(cat ./aaa/common)
echo bbb $(cat ./bbb/common)
