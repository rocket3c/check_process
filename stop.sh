#!/bin/bash
APP_NAME=checkSpread

pid=`ps -ef|grep $APP_NAME | grep -v grep | awk '{print $2}'`
kill -9 $pid
echo $pid"进程终止成功"