ATRT_SETUP_README.txt
Author: Serge Kozlov, MyBlockchain
Date: 03/23/2006

Contents

1. Overview
2. Setup
3. Preparing for testing
4. CPCD. Command line and configuration options
5. d.tmp. Examples
6. Test Case File format. Examples

Note: how to run ATRT tests please look ATRT_USAGE_README.txt


  1.Overview
  ==========

  Auto Test Run Test (ATRT) is a home made test frame work. This frame work can 
start and stop processes on different Linux hosts through another executable 
called Cluster Process Control Daemon (CPCD) running on the other hosts. 
It uses for testing cluster configurations located on different machines. 
ATRT isn't replacement for myblockchaintest. In fact, ATRT can invoke myblockchain-test as 
samples blow show. This framework has been designed to run most any test 
(myblockchaintest testcases, stress tests, any applications) in cluster and detects 
all errors and issues which happened in any node. In addition, ATRT starts 
applications that are defined as test with arguments and will analyze log files 
from all nodes that are produced.


  2.Setup
  =======

  Following steps described how to setup ATRT framework on a cluster:
* You need to have cloned source tree (e.g myblockchain-5.1-new) on machine where you 
  plan to use ATRT.
* Compile and install build (e.g. for Linux/x86 can use 
  BUILD/compile-pentium-max --prefix=/path/to/installation).
* Copy $MYBLOCKCHAIN_DIR on all machines and on same path which you plan to use as 
  nodes (you can use scp utility or ask JonathanMiller about distribution on 
  ndbXX servers).
* Compile and install ATRT:
  * Go to $TREE_ROOT/storage/ndb/test
  * do make
  * do make install
  * Make sure that $MYBLOCKCHAIN_DIR/bin now contains files like that: testBasic, 
    testBlobs, testDict and so on
  * Make sure that $MYBLOCKCHAIN_DIR/myblockchain-test/ndb now contains files like that: atrt,
    atrt-analyze-result.sh, atrt-setup.sh, make-config.sh and so on
  * Add $MYBLOCKCHAIN_DIR/myblockchain-test/ndb to PATH and make sure that you can call atrt 
    from any place on disk.
* Configure CPCD processes on all machines. Repeat following steps for each 
  node:
  * Create $VAR_DIR/run/ndb_cpcd directory
  * Create /etc/ndb_cpcd.cnf file. It will configuration file ndb_cpcd daemon. 
    It strongly recommended instead command line options because more simply. 
    More option for configuration can fe found in this document in section CPCD
  * Add to file following text:

	[ndb_cpcd]
	work-dir= $VAR_DIR/run/ndb_cpcd # e.g. /myblockchain/builds/5.1/var/run/ndb_cpcd
	logfile= $VAR_DIR/run/ndb_cpcd/log.txt # e.g. /myblockchain/builds/5.1/var/run/ndb_cpcd/log.txt
	debug= 1
	user= ndbdev
  
  * Register ndb_cpcd in etc/initab. Hint: you can use more simply way if use 
    Linux - add following line to /etc/rc.d/rc.local file

	$MYBLOCKCHAIN_DIR/libexec/ndb_cpcd > /dev/null 2>&1 &

  * Start ndb_cpcd

	$MYBLOCKCHAIN_DIR/libexec/ndb_cpcd > /dev/null 2>&1 &

  * Open log file and make sure that ndb_cpcd process started properly. Ususal mistakes are: wrong definition of paths and ndb_cpcd process already running. You can test ndb_cpcd from any ndb system by using the command line below. If nothing is returned the process is up and running. Otherwise you will see Failed to connect to node:1234:

	$MYBLOCKCHAIN_DIR/libexec/ndb_cpcc node

Now ATRT Setup done. After accomplishment all steps above you should be have 
following:
* Each node contains fresh build
* Each node has configured and started ndb_cpcd process.
* The node that will used for starting ATRT has installed atrt binaries/scripts
  and some tests.


  3.Preparing for testing
  =======================

* Create new empty directory where you plan to use for testing. Note: if you 
  plan to test different cluster configuration you need to create own directory 
  for each cluster configuration. Do not use same directory.
* Create d.tmp file for your cluster configuration. See details in this 
  document in section d.tmp.
* Run make-config.sh ./d.tmp. It will create necessary directories and files for 
  ATRT.
* Create ATRT testcase file(s) (e.g. test1.atrt). See details in this document 
  in section Test Case File format. Examples.
* Put your test application into $MYBLOCKCHAIN_DIR/bin directory. If your application 
  requires another directory (e.g. myblockchain-test-run.pl) create redirect script 
  such as example below:

	#!/bin/sh
       
	set  -x
	cd $MYBLOCKCHAIN_BASE_DIR/myblockchain-test
	./myblockchain-test-run.pl --with-ndbcluster --ndb-connectstring=$*
        
Now preparing for testing done.

  
  4.CPCD.
  =======

  The CPCD process needs to be running on each host contacts CPCD to tell it 
what process to execute. In other word for testing each node should have to 
running ndb_cpcd. Usually ndb_cpcd can be found in storage/ndb/src/cw/cpcd for 
source tree or in libexec/ndb_cpcd for binary distribution. ndb_cpcd uses 
configuration file /etc/ndb_cpcd.cnf but all options from one can be replaced 
by command line. Names of options in command line are same as from 
configuration file except some some commands have short notation (-X).

ndb_cpcd command line options

-w, --work-dir=name
    Work directory. Should be exist before starting ndb_cpcd. Usually it's 
    $VARDIR/run/ndb_cpcd
-p, --port=#
    TCP port to listen on. By default 1234
-S, --syslog
    Log events to syslog
-L, --logfile=name
    File to log events to. Usually it's $VARDIR/run/ndb_cpcd/logfile.txt
-D, --debug
    Enable debug mode.
-u, --user=name
    Run as user


  5.d.tmp
  =======

  The d.tmp file is used to create the d.txt file (configuration file for ATRT) 
and the config.ini file (configuration file for MyBlockchain Cluster). This file does 
not have to be called d.tmp, it can be called by any name. The file will be feed 
to make-config.sh. All options in the file separated by two parts:
* Original options. They are located before '-- cluster config'. Description of 
all these options are below.
* Options for config.ini. They are located after '-- cluster config'. All 
options, sections and values completely coincide with config.ini.

d.tmp options

baseport
    Port used for communicating to the cluster on.
basedir
    basedir has to point to the root of the myblockchain install. Note that ATRT will 
    create a run directory under the base directory. All test directories and 
    files created will be copied to all hosts in the test under the basedir/run directory.
mgm
    Host to NDB Cluster manager on. Put hostnames separated by blanks.
ndb
    Host(s) to run NDB Data Nodes on. Put hostnames separated by blanks.
api
    Host(s) that NDB API should be ran on. Put hostnames separated by blanks.
myblockchaind
    Host(s) that myblockchaind processes should be started on. Put hostnames separated by blanks.
myblockchain
    Host(s) that myblockchain processes should be started on. Put hostnames separated 
    by blanks.

Example d.tmp for cluster configuration: 1 ndb node, 1 replica, 1 mgm, 1 api, 
1 myblockchain, 1 myblockchaind. Available hosts for nodes: ndb16, ndb17

baseport: 14000
basedir: /home/ndbdev/skozlov/builds
mgm: ndb16
ndb: ndb17
api: ndb16
myblockchaind ndb16
myblockchain ndb16
-- cluster config
[DB DEFAULT]
NoOfReplicas: 1

[MGM DEFAULT]
PortNumber: 14000
ArbitrationRank: 1

Example d.tmp for cluster configuration: 2 ndb nodes, 2 replicas, 1 mgm, 1 api, 
1 myblockchain, 1 myblockchaind. Available hosts for nodes: ndb16, ndb17, ndb18

baseport: 14000
basedir: /home/ndbdev/skozlov/builds
mgm: ndb16
ndb: ndb17 ndb18
api: ndb16
myblockchaind ndb16
myblockchain ndb16
-- cluster config
[DB DEFAULT]
NoOfReplicas: 2

[MGM DEFAULT]
PortNumber: 14000
ArbitrationRank: 1
               
Example d.tmp for cluster configuration: 4 ndb nodes, 4 replicas, 1 mgm, 3 api, 
2 myblockchain, 1 myblockchaind. Available hosts for nodes: ndb14, ndb15, ndb16, ndb17, ndb18

baseport: 14000
basedir: /home/ndbdev/skozlov/builds
mgm: ndb16
ndb: ndb17 ndb18 ndb15 ndb14
api: ndb16 ndb17 ndb18
myblockchaind ndb16
myblockchain ndb16 ndb17
-- cluster config
[DB DEFAULT]
NoOfReplicas: 4

[MGM DEFAULT]
PortNumber: 14000
ArbitrationRank: 1
               

  6.Test Case File Format. Examples.
  ==================================

  A test file consists of a list of test cases. Each test case is ended by an 
empty/blank line. Each test case is described by a set of name value pairs. 
ATRT looks for tests executables, shell and Perl scripts in the basedir/bin 
directory.

Test Case File options:

max-time
    This value is in seconds. Test ATRT how long to allow the test to run before 
    marking the test a failure and moving on to the next test. If the test 
   finishes before max-time, it will move on to the next test.
cmd
    Tells ATRT which test or script to run from the bin directory
args
    These are the command-line arguments to be passed to the test or script 
    that is being executed.
type
    Currently only bench. See ATRT Command-line parameters for details.
run-all
    will start the same command for each defined api/myblockchain (normally it only 
    started in 1 instance)

Example 1.
ATRT starts test $MYBLOCKCHAIN_DIR/bin/testBlobs without arguments and sets time for 
execution as 10 min. testBlobs is binary application

max-time: 600
cmd: testBlobs
args:

Example 2.
ATRT starts test $MYBLOCKCHAIN_DIR/bin/testRead -n PkRead and sets time for execution 
as 20 min. testRead is binary application

max-time: 1200
cmd: testRead
args: -n PkRead

Example 3.
ATRT starts test $MYBLOCKCHAIN_DIR/bin/atrt-myblockchain-test-run -force and sets time for 
execution as one hour. atrt-myblockchain-test-run is bash script and it points to 
$MYBLOCKCHAIN_DIR/myblockchain-test/myblockchain-test-run. In fact this test will start 
myblockchain-test-run --force that means the execution all myblockchaintest testcases in 
myblockchain-test/t directory.

max-time: 3600
cmd: atrt-myblockchain-test-run 
args: --force

Example 4.
ATRT starts test $MYBLOCKCHAIN_DIR/bin/MyTest1 -n and sets time for execution as 2 min.
Then starts $MYBLOCKCHAIN_DIR/bin/MyTest2 for each defined myblockchain/api node and set 
timeout 3 min.

max-time: 120
cmd: MyTest1 
args: -n

max-time: 180
cmd: MyTest2 
run-all: yes

