--perl
use strict; 
use lib "lib/";
use My::Find;
use File::Basename;

my $myblockchain_test_dir = $ENV{MYBLOCKCHAIN_TEST_DIR} or die "Need MYBLOCKCHAIN_TEST_DIR";
my $basedir = dirname($myblockchain_test_dir);

# In the RPM case, binaries and libraries are installed in the
# default system locations, instead of having our own private base
# directory. And we install "usr/share/myblockchain-test". Moving up two
# more directory relative to "myblockchain-test" gives us a usable base
# directory for RPM installs.
if (! -d "$basedir/sql" and ! -d "$basedir/bin" )
{
  $basedir= dirname($basedir);
}
my $bindir = $ENV{MTR_BINDIR} || $basedir; # Out of source set MTR_BINDIR

my $NDB_PRINT_FILE =  my_find_bin($bindir,
                           ["storage/ndb/src/kernel/blocks", "bin"],
                           "ndb_print_file");

my $vardir = $ENV{MYBLOCKCHAINTEST_VARDIR} or die "Need MYBLOCKCHAINTEST_VARDIR" ;
open ( OUTPUT, ">$vardir/tmp/ndb_find_print_file_tool_result.inc") ;
print OUTPUT "let \$NDB_PRINT_FILE=$NDB_PRINT_FILE;\n";
close (OUTPUT);
EOF

--source $MYBLOCKCHAINTEST_VARDIR/tmp/ndb_find_print_file_tool_result.inc
--remove_file $MYBLOCKCHAINTEST_VARDIR/tmp/ndb_find_print_file_tool_result.inc
