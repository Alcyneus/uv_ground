#!/bin/sh

while true

do

ps -ef | grep "test" | grep -v "grep"

if [ "$?" -eq 1 ]

then

./test #启动应用，修改成自己的启动应用脚本或命令

echo "process has been restarted!"

else

echo "process already started!"

fi

sleep 10

done