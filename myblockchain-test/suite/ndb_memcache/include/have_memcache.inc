--source include/not_embedded.inc
--source include/not_windows.inc

--perl

use strict;
use File::Basename;
use IO::File;

my $myblockchain_test_dir = $ENV{MYBLOCKCHAIN_TEST_DIR} or die "Need MYBLOCKCHAIN_TEST_DIR";
my $vardir = $ENV{MYBLOCKCHAINTEST_VARDIR} or die "Need MYBLOCKCHAINTEST_VARDIR";
my $memcached_started = $ENV{NDB_MEMCACHED_STARTED};

my $F = IO::File->new("$vardir/tmp/have_memcache_result.inc", "w") or die;
if (!$memcached_started) {
  print $F "--skip Could not find NDB Memcache API\n";
}
$F->close();

EOF

--source $MYBLOCKCHAINTEST_VARDIR/tmp/have_memcache_result.inc


