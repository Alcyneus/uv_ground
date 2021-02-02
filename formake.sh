#!/bin/sh
echo "=== rm -rf * ==="
# rm -rf *
echo "=== cmake .. ==="
cmake ..
echo "=== make ==="
make
echo "=== scp ./bin/exe root@192.168.2.1:/tmp ==="
scp ./bin/exe root@192.168.2.1:/tmp
echo "=== ok ==="