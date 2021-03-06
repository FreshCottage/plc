#!/bin/sh

execname=plc
pid=`ps |grep "$execname"|grep -v grep|wc -l`
case $1 in
    start)
    if [ $pid -lt 1 ]; then
        $execname >/dev/null&
        echo "Starting $execname:[ OK ]"
    else
        echo "Starting $execname:[ Failed ]"
    fi
    ;;
    stop)
    if [ $pid -gt 0 ]; then
        ps |grep "$execname"|grep -v grep|awk '{print $1}'|xargs kill -9
        echo "Stoping $execname:[ OK ]"
    else
        echo "Stoping $execname:[ Failed ]"
    fi
    ;;
    restart)
    if [ $pid -gt 0 ]; then
        ps |grep "$execname"|grep -v grep|awk '{print $1}'|xargs kill -9
        echo "Stoping $execname:[ OK ]"
        sleep 1
        $execname >/dev/null&
        echo "Starting $execname:[ OK ]"
    elif [ $pid -eq 0 ]; then
        $execname >/dev/null&
        echo "Starting $execname:[ OK ]"
    else
        echo "Restarting $execname:[ Failed ]"
    fi
    ;;
    status)
    if [ $pid -eq 0 ]; then
        echo "$execname is not running"
    else
        echo "$execname is running"
    fi
    ;;
    *)
        echo "Usage: service $execname {start|stop|restart|status}"
        exit 1
    ;;
esac
