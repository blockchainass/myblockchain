--exec echo "wait" > $MYBLOCKCHAINTEST_VARDIR/tmp/myblockchaind.1.expect
--shutdown_server 
--source include/wait_until_disconnected.inc

--perl
use File::Path 'rmtree';
$DDIR=$ENV{"MYBLOCKCHAIN_TMP_DIR"}."/installdb_test";
rmtree([ "$DDIR" ]);
EOF

--echo # Execution of myblockchain_install_db starts............
--exec $MYBLOCKCHAIN_INSTALL_DB --defaults-file=$MYBLOCKCHAIN_TEST_DIR/t/m_i_db_config.cnf --myblockchaind-file=$MYBLOCKCHAIND --loose-ssl-setup-file=$MYBLOCKCHAIN_SSL_RSA_SETUP --datadir=$DDIR --basedir=$BASEDIR --lc_messages_dir=$MYBLOCKCHAIN_SHAREDIR --extra-sql-file=$MYBLOCKCHAIN_TEST_DIR/t/m_i_db_createschema.sql $midparam > $MYBLOCKCHAINTEST_VARDIR/tmp/m_i_db.out 2>&1
--echo # Execution of myblockchain_install_db ends..............
--exec echo "restart:--datadir=$DDIR " > $MYBLOCKCHAINTEST_VARDIR/tmp/myblockchaind.1.expect
--enable_reconnect
--source include/wait_until_connected_again.inc
--remove_file $MYBLOCKCHAINTEST_VARDIR/tmp/m_i_db.out

--perl
$passwd="";
open(PWDPATH, "$ENV{MYBLOCKCHAIN_TMP_DIR}/readpwdfilepath.txt") or die("Could not open pwdpath file");
my $filename = <PWDPATH>;
chomp $filename;
if (-e $filename) {
  open(PWDFILE, $filename) or die("Could not open password file");
  my @pass = <PWDFILE>;
  chomp @pass;
  $passwd=@pass[1];
}
open(FILE, ">", "$ENV{MYBLOCKCHAIN_TMP_DIR}/readpwd.inc") or die $!;
print FILE "delimiter lessprobability;\n";
print FILE "let passwd=$passwd";
print FILE "lessprobability\n";
print FILE "--delimiter ;\n";
close FILE;
EOF
--source $MYBLOCKCHAIN_TMP_DIR/readpwd.inc
