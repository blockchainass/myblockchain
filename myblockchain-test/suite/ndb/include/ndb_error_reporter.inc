--perl
use strict;

use File::Basename;
use IO::File;
use lib "lib/";
use My::Find;

my $vardir = $ENV{MYBLOCKCHAINTEST_VARDIR} or die "Need MYBLOCKCHAINTEST_VARDIR";
my $myblockchain_test_dir = $ENV{MYBLOCKCHAIN_TEST_DIR} or die "Need MYBLOCKCHAIN_TEST_DIR";
my $basedir = dirname($myblockchain_test_dir);
my $source_dist = 0;
if( -d "$basedir/sql" )
{
  $source_dist = 1;
}

# In the RPM case, binaries and libraries are installed in the
# default system locations, instead of having our own private base
# directory. And we install "/usr/share/myblockchain-test". Moving up one
# more directory relative to "myblockchain-test" gives us a usable base
# directory for RPM installs.
if ( ! $source_dist and ! -d "$basedir/bin" )
{
  $basedir= dirname($basedir);
}

# Out of source set MTR_BINDIR
my $bindir = $ENV{MTR_BINDIR} || $basedir;
my $ndb_error_reporter = my_find_file($bindir,
                            ["storage/ndb/tools", "bin"],
                            "ndb_error_reporter");
my $F = IO::File->new("$vardir/tmp/ndb_error_reporter_result.inc", "w") or die;
if ($ndb_error_reporter) {
  print $F "--let \$NDB_ERROR_REPORTER=$ndb_error_reporter\n";
}
$F->close();

EOF

--source $MYBLOCKCHAINTEST_VARDIR/tmp/ndb_error_reporter_result.inc
