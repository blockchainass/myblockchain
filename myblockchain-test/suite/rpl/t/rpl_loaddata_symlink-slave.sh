#! /bin/bash
#
rm -f $MYBLOCKCHAINTEST_VARDIR/std_data_slave_link
ln -s $MYBLOCKCHAINTEST_VARDIR/std_data $MYBLOCKCHAINTEST_VARDIR/std_data_slave_link
