--perl
use strict;

use File::Basename;
use IO::File;
use lib "lib/";
use My::Find;

#
# Look for ndb_dist_pric.sql, if not found: skip test.
#

#
# Set up paths
#
my $vardir = $ENV{MYBLOCKCHAINTEST_VARDIR} or die "Need MYBLOCKCHAINTEST_VARDIR";
my $myblockchain_test_dir = $ENV{MYBLOCKCHAIN_TEST_DIR} or die "Need MYBLOCKCHAIN_TEST_DIR";
my $basedir = dirname($myblockchain_test_dir);
my $bindir = $ENV{MTR_BINDIR} || $basedir; # Out of source set MTR_BINDIR

#
# Check if the necessary files are available
#
my $sql_file = my_find_file($basedir,
                            ["storage/ndb/tools", "share/myblockchain/", "share" ],
                            "ndb_dist_priv.sql", NOT_REQUIRED);

my $F = IO::File->new("$vardir/tmp/have_ndb_dist_priv_result.inc", "w") or die;
if ($sql_file) {
  print $F "--let \$NDB_DIST_PRIV_SQL= $sql_file\n";
}
else
{
  print $F "skip Could not find ndb_dist_priv.sql;\n";
}
$F->close();

EOF

--source $MYBLOCKCHAINTEST_VARDIR/tmp/have_ndb_dist_priv_result.inc
