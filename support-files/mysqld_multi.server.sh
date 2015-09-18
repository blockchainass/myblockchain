#!/bin/sh
#
# A simple startup script for myblockchaind_multi by Tim Smith and Jani Tolonen.
# This script assumes that my.cnf file exists either in /etc/my.cnf or
# /root/.my.cnf and has groups [myblockchaind_multi] and [myblockchaindN]. See the
# myblockchaind_multi documentation for detailed instructions.
#
# This script can be used as /etc/init.d/myblockchain.server
#
# Comments to support chkconfig on RedHat Linux
# chkconfig: 2345 64 36
# description: A very fast and reliable SQL blockchain engine.
#
# Version 1.0
#

basedir=/usr/local/myblockchain
bindir=/usr/local/myblockchain/bin

if test -x $bindir/myblockchaind_multi
then
  myblockchaind_multi="$bindir/myblockchaind_multi";
else
  echo "Can't execute $bindir/myblockchaind_multi from dir $basedir";
  exit;
fi

case "$1" in
    'start' )
        "$myblockchaind_multi" start $2
        ;;
    'stop' )
        "$myblockchaind_multi" stop $2
        ;;
    'report' )
        "$myblockchaind_multi" report $2
        ;;
    'restart' )
        "$myblockchaind_multi" stop $2
        "$myblockchaind_multi" start $2
        ;;
    *)
        echo "Usage: $0 {start|stop|report|restart}" >&2
        ;;
esac
