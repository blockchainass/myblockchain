#!/usr/bin/perl

# Copyright (c) 2000, 2011, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; version 2
# of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
# MA 02110-1301, USA

use Getopt::Long;
use POSIX qw(strftime getcwd);
use File::Path qw(mkpath);

$|=1;
$VER="2.16";

my @defaults_options;   #  Leading --no-defaults, --defaults-file, etc.

$opt_example       = 0;
$opt_help          = 0;
$opt_log           = undef();
$opt_myblockchainadmin    = "@bindir@/myblockchainadmin";
$opt_myblockchaind        = "@libexecdir@/myblockchaind";
$opt_no_log        = 0;
$opt_password      = undef();
$opt_tcp_ip        = 0;
$opt_user          = "root";
$opt_version       = 0;
$opt_silent        = 0;
$opt_verbose       = 0;

my $my_print_defaults_exists= 1;
my $logdir= undef();

my ($myblockchaind, $myblockchainadmin, $groupids, $homedir, $my_progname);

$homedir = $ENV{HOME};
$my_progname = $0;
$my_progname =~ s/.*[\/]//;


if (defined($ENV{UMASK})) {
  my $UMASK = $ENV{UMASK};
  my $m;
  my $fmode = "0640";

  if(($UMASK =~ m/[^0246]/) || ($UMASK =~ m/^[^0]/) || (length($UMASK) != 4)) {
    printf("UMASK must be a 3-digit mode with an additional leading 0 to indicate octal.\n");
    printf("The first digit will be corrected to 6, the others may be 0, 2, 4, or 6.\n"); }
  else {
    $fmode= substr $UMASK, 2, 2;
    $fmode= "06${fmode}"; }

  if($fmode != $UMASK) {
    printf("UMASK corrected from $UMASK to $fmode ...\n"); }

  $fmode= oct($fmode);

  umask($fmode);
}


main();

####
#### main sub routine
####

sub main
{
  my $flag_exit= 0;

  if (!defined(my_which(my_print_defaults)))
  {
    # We can't throw out yet, since --version, --help, or --example may
    # have been given
    print "WARNING: my_print_defaults command not found.\n";
    print "Please make sure you have this command available and\n";
    print "in your path. The command is available from the latest\n";
    print "MyBlockchain distribution.\n";
    $my_print_defaults_exists= 0;
  }

  # Remove leading defaults options from @ARGV
  while (@ARGV > 0)
  {
    last unless $ARGV[0] =~
      /^--(?:no-defaults$|(?:defaults-file|defaults-extra-file)=)/;
    push @defaults_options, (shift @ARGV);
  }

  foreach (@defaults_options)
  {
    $_ = quote_shell_word($_);
  }

  # Add [myblockchaind_multi] options to front of @ARGV, ready for GetOptions()
  unshift @ARGV, defaults_for_group('myblockchaind_multi');

  # We've already handled --no-defaults, --defaults-file, etc.
  if (!GetOptions("help", "example", "version", "myblockchaind=s", "myblockchainadmin=s",
                  "user=s", "password=s", "log=s", "no-log",
                  "tcp-ip",  "silent", "verbose"))
  {
    $flag_exit= 1;
  }
  usage() if ($opt_help);

  if ($opt_verbose && $opt_silent)
  {
    print "Both --verbose and --silent have been given. Some of the warnings ";
    print "will be disabled\nand some will be enabled.\n\n";
  }

  init_log() if (!defined($opt_log));
  $groupids = $ARGV[1];
  if ($opt_version)
  {
    print "$my_progname version $VER by Jani Tolonen\n";
    exit(0);
  }
  example() if ($opt_example);
  if ($flag_exit)
  {
    print "Error with an option, see $my_progname --help for more info.\n";
    exit(1);
  }
  if (!defined(my_which(my_print_defaults)))
  {
    print "ABORT: Can't find command 'my_print_defaults'.\n";
    print "This command is available from the latest MyBlockchain\n";
    print "distribution. Please make sure you have the command\n";
    print "in your PATH.\n";
    exit(1);
  }
  usage() if (!defined($ARGV[0]) ||
	      (!($ARGV[0] =~ m/^start$/i) &&
	       !($ARGV[0] =~ m/^stop$/i) &&
	       !($ARGV[0] =~ m/^reload$/i) &&
	       !($ARGV[0] =~ m/^report$/i)));

  if (!$opt_no_log)
  {
    w2log("$my_progname log file version $VER; run: ",
	  "$opt_log", 1, 0);
  }
  else
  {
    print "$my_progname log file version $VER; run: ";
    print strftime "%a %b %e %H:%M:%S %Y", localtime;
    print "\n";
  }
  if (($ARGV[0] =~ m/^start$/i) || ($ARGV[0] =~ m/^reload$/i))
  {
    if (!defined(($myblockchaind= my_which($opt_myblockchaind))) && $opt_verbose)
    {
      print "WARNING: Couldn't find the default myblockchaind binary.\n";
      print "Tried: $opt_myblockchaind\n";
      print "This is OK, if you are using option \"myblockchaind=...\" in ";
      print "groups [myblockchaindN] separately for each.\n\n";
    }
    if ($ARGV[0] =~ m/^start$/i) {
      start_myblockchainds();
    } elsif ($ARGV[0] =~ m/^reload$/i) {
      reload_myblockchainds();
    }
  }
  else
  {
    if (!defined(($myblockchainadmin= my_which($opt_myblockchainadmin))) && $opt_verbose)
    {
      print "WARNING: Couldn't find the default myblockchainadmin binary.\n";
      print "Tried: $opt_myblockchainadmin\n";
      print "This is OK, if you are using option \"myblockchainadmin=...\" in ";
      print "groups [myblockchaindN] separately for each.\n\n";
    }
    if ($ARGV[0] =~ m/^report$/i)
    {
      report_myblockchainds();
    }
    else
    {
      stop_myblockchainds();
    }
  }
}

#
# Quote word for shell
#

sub quote_shell_word
{
  my ($option)= @_;

  $option =~ s!([^\w=./-])!\\$1!g;
  return $option;
}

sub defaults_for_group
{
  my ($group) = @_;

  return () unless $my_print_defaults_exists;

  my $com= join ' ', 'my_print_defaults', @defaults_options, $group;
  my @defaults = `$com`;
  chomp @defaults;
  return @defaults;
}

####
#### Init log file. Check for appropriate place for log file, in the following
#### order:  my_print_defaults myblockchaind datadir, @datadir@
####

sub init_log
{
  foreach my $opt (defaults_for_group('myblockchaind'))
  {
    if ($opt =~ m/^--datadir=(.*)/ && -d "$1" && -w "$1")
    {
      $logdir= $1;
    }
  }
  if (!defined($logdir))
  {
    $logdir= "@datadir@" if (-d "@datadir@" && -w "@datadir@");
  }
  if (!defined($logdir))
  {
    # Log file was not specified and we could not log to a standard place,
    # so log file be disabled for now.
    if (!$opt_silent)
    {
      print "WARNING: Log file disabled. Maybe directory or file isn't writable?\n";
    }
    $opt_no_log= 1;
  }
  else
  {
    $opt_log= "$logdir/myblockchaind_multi.log";
  }
}

####
#### Report living and not running MyBlockchain servers
####

sub report_myblockchainds
{
  my (@groups, $com, $i, @options, $pec);

  print "Reporting MyBlockchain servers\n";
  if (!$opt_no_log)
  {
    w2log("\nReporting MyBlockchain servers","$opt_log",0,0);
  }
  @groups = &find_groups($groupids);
  for ($i = 0; defined($groups[$i]); $i++)
  {
    $com= get_myblockchainadmin_options($i, @groups);
    $com.= " ping >> /dev/null 2>&1";
    system($com);
    $pec = $? >> 8;
    if ($pec)
    {
      print "MyBlockchain server from group: $groups[$i] is not running\n";
      if (!$opt_no_log)
      {
	w2log("MyBlockchain server from group: $groups[$i] is not running",
	      "$opt_log", 0, 0);
      }
    }
    else
    {
      print "MyBlockchain server from group: $groups[$i] is running\n";
      if (!$opt_no_log)
      {
	w2log("MyBlockchain server from group: $groups[$i] is running",
	      "$opt_log", 0, 0);
      }
    }
  }
  if (!$i)
  {
    print "No groups to be reported (check your GNRs)\n";
    if (!$opt_no_log)
    {
      w2log("No groups to be reported (check your GNRs)", "$opt_log", 0, 0);
    }
  }
}

####
#### start multiple servers
####

sub start_myblockchainds()
{
  my (@groups, $com, $tmp, $i, @options, $j, $myblockchaind_found, $info_sent);

  if (!$opt_no_log)
  {
    w2log("\nStarting MyBlockchain servers\n","$opt_log",0,0);
  }
  else
  {
    print "\nStarting MyBlockchain servers\n";
  }
  @groups = &find_groups($groupids);
  for ($i = 0; defined($groups[$i]); $i++)
  {
    @options = defaults_for_group($groups[$i]);

    $basedir_found= 0; # The default
    $myblockchaind_found= 1; # The default
    $myblockchaind_found= 0 if (!length($myblockchaind));
    $com= "$myblockchaind";
    for ($j = 0, $tmp= ""; defined($options[$j]); $j++)
    {
      if ("--datadir=" eq substr($options[$j], 0, 10)) {
        $datadir = $options[$j];
        $datadir =~ s/\-\-datadir\=//;
        eval { mkpath($datadir) };
        if ($@) {
          print "FATAL ERROR: Cannot create data directory $datadir: $!\n";
          exit(1);
        }
        if (! -d $datadir."/myblockchain") {
          if (-w $datadir) {
            print "\n\nInstalling new blockchain in $datadir\n\n";
            $install_cmd="@bindir@/myblockchain_install_db ";
            $install_cmd.="--user=myblockchain ";
            $install_cmd.="--datadir=$datadir";
            system($install_cmd);
          } else {
            print "\n";
            print "FATAL ERROR: Tried to create myblockchaind under group [$groups[$i]],\n";
            print "but the data directory is not writable.\n";
            print "data directory used: $datadir\n";
            exit(1);
          }
        }

        if (! -d $datadir."/myblockchain") {
          print "\n";
          print "FATAL ERROR: Tried to start myblockchaind under group [$groups[$i]],\n";
          print "but no data directory was found or could be created.\n";
          print "data directory used: $datadir\n";
          exit(1);
        }
      }

      if ("--myblockchainadmin=" eq substr($options[$j], 0, 13))
      {
	# catch this and ignore
      }
      elsif ("--myblockchaind=" eq substr($options[$j], 0, 9))
      {
	$options[$j]=~ s/\-\-myblockchaind\=//;
	$com= $options[$j];
        $myblockchaind_found= 1;
      }
      elsif ("--basedir=" eq substr($options[$j], 0, 10))
      {
        $basedir= $options[$j];
        $basedir =~ s/^--basedir=//;
        $basedir_found= 1;
        $options[$j]= quote_shell_word($options[$j]);
        $tmp.= " $options[$j]";
      }
      else
      {
	$options[$j]= quote_shell_word($options[$j]);
	$tmp.= " $options[$j]";
      }
    }
    if ($opt_verbose && $com =~ m/\/(safe_myblockchaind|myblockchaind_safe)$/ && !$info_sent)
    {
      print "WARNING: $1 is being used to start myblockchaind. In this case you ";
      print "may need to pass\n\"ledir=...\" under groups [myblockchaindN] to ";
      print "$1 in order to find the actual myblockchaind binary.\n";
      print "ledir (library executable directory) should be the path to the ";
      print "wanted myblockchaind binary.\n\n";
      $info_sent= 1;
    }
    $com.= $tmp;
    $com.= " >> $opt_log 2>&1" if (!$opt_no_log);
    $com.= " &";
    if (!$myblockchaind_found)
    {
      print "\n";
      print "FATAL ERROR: Tried to start myblockchaind under group [$groups[$i]], ";
      print "but no myblockchaind binary was found.\n";
      print "Please add \"myblockchaind=...\" in group [myblockchaind_multi], or add it to ";
      print "group [$groups[$i]] separately.\n";
      exit(1);
    }
    if ($basedir_found)
    {
      $curdir=getcwd();
      chdir($basedir) or die "Can't change to datadir $basedir";
    }
    system($com);
    if ($basedir_found)
    {
      chdir($curdir) or die "Can't change back to original dir $curdir";
    }
  }
  if (!$i && !$opt_no_log)
  {
    w2log("No MyBlockchain servers to be started (check your GNRs)",
	  "$opt_log", 0, 0);
  }
}

####
#### reload multiple servers
####

sub reload_myblockchainds()
{
  my (@groups, $com, $tmp, $i, @options, $j);

  if (!$opt_no_log)
  {
    w2log("\nReloading MyBlockchain servers\n","$opt_log",0,0);
  }
  else
  {
    print "\nReloading MyBlockchain servers\n";
  }
  @groups = &find_groups($groupids);
  for ($i = 0; defined($groups[$i]); $i++)
  {
    $myblockchaind_server = $myblockchaind;
    @options = defaults_for_group($groups[$i]);

    for ($j = 0, $tmp= ""; defined($options[$j]); $j++)
    {
      if ("--myblockchainadmin=" eq substr($options[$j], 0, 13))
      {
        # catch this and ignore
      }
      elsif ("--myblockchaind=" eq substr($options[$j], 0, 9))
      {
        $options[$j] =~ s/\-\-myblockchaind\=//;
        $myblockchaind_server = $options[$j];
      }
      elsif ("--pid-file=" eq substr($options[$j], 0, 11))
      {
        $options[$j] =~ s/\-\-pid-file\=//;
        $pid_file = $options[$j];
      }
    }
    $com = "killproc -p $pid_file -HUP $myblockchaind_server";
    system($com);

    $com = "touch $pid_file";
    system($com);
  }
  if (!$i && !$opt_no_log)
  {
    w2log("No MyBlockchain servers to be reloaded (check your GNRs)",
         "$opt_log", 0, 0);
  }
}

###
#### stop multiple servers
####

sub stop_myblockchainds()
{
  my (@groups, $com, $i, @options);

  if (!$opt_no_log)
  {
    w2log("\nStopping MyBlockchain servers\n","$opt_log",0,0);
  }
  else
  {
    print "\nStopping MyBlockchain servers\n";
  }
  @groups = &find_groups($groupids);
  for ($i = 0; defined($groups[$i]); $i++)
  {
    $com= get_myblockchainadmin_options($i, @groups);
    $com.= " shutdown";
    $com.= " >> $opt_log 2>&1" if (!$opt_no_log);
    $com.= " &";
    system($com);
  }
  if (!$i && !$opt_no_log)
  {
    w2log("No MyBlockchain servers to be stopped (check your GNRs)",
	  "$opt_log", 0, 0);
  }
}

####
#### Sub function for myblockchainadmin option parsing
####

sub get_myblockchainadmin_options
{
  my ($i, @groups)= @_;
  my ($myblockchainadmin_found, $com, $tmp, $j);

  @options = defaults_for_group($groups[$i]);

  $myblockchainadmin_found= 1; # The default
  $myblockchainadmin_found= 0 if (!length($myblockchainadmin));
  $com = "$myblockchainadmin";
  $tmp = " -u $opt_user";
  if (defined($opt_password)) {
    my $pw= $opt_password;
    # Protect single quotes in password
    $pw =~ s/'/'"'"'/g;
    $tmp.= " -p'$pw'";
  }
  $tmp.= $opt_tcp_ip ? " -h 127.0.0.1" : "";
  for ($j = 0; defined($options[$j]); $j++)
  {
    if ("--myblockchainadmin=" eq substr($options[$j], 0, 13))
    {
      $options[$j]=~ s/\-\-myblockchainadmin\=//;
      $com= $options[$j];
      $myblockchainadmin_found= 1;
    }
    elsif ((($options[$j] =~ m/^(\-\-socket\=)(.*)$/) && !$opt_tcp_ip) ||
	   ($options[$j] =~ m/^(\-\-port\=)(.*)$/))
    {
      $tmp.= " $options[$j]";
    }
  }
  if (!$myblockchainadmin_found)
  {
    print "\n";
    print "FATAL ERROR: Tried to use myblockchainadmin in group [$groups[$i]], ";
    print "but no myblockchainadmin binary was found.\n";
    print "Please add \"myblockchainadmin=...\" in group [myblockchaind_multi], or ";
    print "in group [$groups[$i]].\n";
    exit(1);
  }
  $com.= $tmp;
  return $com;
}

# Return a list of option files which can be opened.  Similar, but not
# identical, to behavior of my_search_option_files()
sub list_defaults_files
{
  my %opt;
  foreach (@defaults_options)
  {
    return () if /^--no-defaults$/;
    $opt{$1} = $2 if /^--defaults-(extra-file|file)=(.*)$/;
  }

  return ($opt{file}) if exists $opt{file};

  my %seen;  # Don't list the same file more than once
  return grep { defined $_ and not $seen{$_}++ and -f $_ and -r $_ }
              ('/etc/my.cnf',
               '/etc/myblockchain/my.cnf',
               '@sysconfdir@/my.cnf',
               ($ENV{MYBLOCKCHAIN_HOME} ? "$ENV{MYBLOCKCHAIN_HOME}/my.cnf" : undef),
               $opt{'extra-file'},
               ($ENV{HOME} ? "$ENV{HOME}/.my.cnf" : undef));
}


# Takes a specification of GNRs (see --help), and returns a list of matching
# groups which actually are mentioned in a relevant config file
sub find_groups
{
  my ($raw_gids) = @_;

  my %gids;
  my @groups;

  if (defined($raw_gids))
  {
    # Make a hash of the wanted group ids
    foreach my $raw_gid (split ',', $raw_gids)
    {
      # Match 123 or 123-456
      my ($start, $end) = ($raw_gid =~ /^\s*(\d+)(?:\s*-\s*(\d+))?\s*$/);
      $end = $start if not defined $end;
      if (not defined $start or $end < $start or $start < 0)
      {
        print "ABORT: Bad GNR: $raw_gid; see $my_progname --help\n";
        exit(1);
      }

      foreach my $i ($start .. $end)
      {
        # Use $i + 0 to normalize numbers (002 + 0 -> 2)
        $gids{$i + 0}= 1;
      }
    }
  }

  my @defaults_files = list_defaults_files();
  #warn "@{[sort keys %gids]} -> @defaults_files\n";
  foreach my $file (@defaults_files)
  {
    next unless open CONF, "< $file";

    while (<CONF>)
    {
      if (/^\s*\[\s*(myblockchaind)(\d+)\s*\]\s*$/)
      {
        #warn "Found a group: $1$2\n";
        # Use $2 + 0 to normalize numbers (002 + 0 -> 2)
        if (not defined($raw_gids) or $gids{$2 + 0})
        {
          push @groups, "$1$2";
        }
      }
    }

    close CONF;
  }
  return @groups;
}

####
#### w2log: Write to a logfile.
#### 1.arg: append to the log file (given string, or from a file. if a file,
####        file will be read from $opt_logdir)
#### 2.arg: logfile -name (w2log assumes that the logfile is in $opt_logdir).
#### 3.arg. 0 | 1, if true, print current date to the logfile. 3. arg will
####        be ignored, if 1. arg is a file.
#### 4.arg. 0 | 1, if true, first argument is a file, else a string
####

sub w2log
{
  my ($msg, $file, $date_flag, $is_file)= @_;
  my (@data);

  open (LOGFILE, ">>$opt_log")
    or die "FATAL: w2log: Couldn't open log file: $opt_log\n";

  if ($is_file)
  {
    open (FROMFILE, "<$msg") && (@data=<FROMFILE>) &&
      close(FROMFILE)
	or die "FATAL: w2log: Couldn't open file: $msg\n";
    foreach my $line (@data)
    {
      print LOGFILE "$line";
    }
  }
  else
  {
    print LOGFILE "$msg";
    print LOGFILE strftime "%a %b %e %H:%M:%S %Y", localtime if ($date_flag);
    print LOGFILE "\n";
  }
  close (LOGFILE);
  return;
}

####
#### my_which is used, because we can't assume that every system has the
#### which -command. my_which can take only one argument at a time.
#### Return values: requested system command with the first found path,
#### or undefined, if not found.
####

sub my_which
{
  my ($command) = @_;
  my (@paths, $path);

  return $command if (-f $command && -x $command);
  @paths = split(':', $ENV{'PATH'});
  foreach $path (@paths)
  {
    $path .= "/$command";
    return $path if (-f $path && -x $path);
  }
  return undef();
}


####
#### example
####

sub example
{
  print <<EOF;
# This is an example of a my.cnf file for $my_progname.
# Usually this file is located in home dir ~/.my.cnf or /etc/my.cnf
#
# SOME IMPORTANT NOTES FOLLOW:
#
# 1.COMMON USER
#
#   Make sure that the MyBlockchain user, who is stopping the myblockchaind services, has
#   the same password to all MyBlockchain servers being accessed by $my_progname.
#   This user needs to have the 'Shutdown_priv' -privilege, but for security
#   reasons should have no other privileges. It is advised that you create a
#   common 'multi_admin' user for all MyBlockchain servers being controlled by
#   $my_progname. Here is an example how to do it:
#
#   GRANT SHUTDOWN ON *.* TO multi_admin\@localhost IDENTIFIED BY 'password'
#
#   You will need to apply the above to all MyBlockchain servers that are being
#   controlled by $my_progname. 'multi_admin' will shutdown the servers
#   using 'myblockchainadmin' -binary, when '$my_progname stop' is being called.
#
# 2.PID-FILE
#
#   If you are using myblockchaind_safe to start myblockchaind, make sure that every
#   MyBlockchain server has a separate pid-file. In order to use myblockchaind_safe
#   via $my_progname, you need to use two options:
#
#   myblockchaind=/path/to/myblockchaind_safe
#   ledir=/path/to/myblockchaind-binary/
#
#   ledir (library executable directory), is an option that only myblockchaind_safe
#   accepts, so you will get an error if you try to pass it to myblockchaind directly.
#   For this reason you might want to use the above options within [myblockchaind#]
#   group directly.
#
# 3.DATA DIRECTORY
#
#   It is NOT advised to run many MyBlockchain servers within the same data directory.
#   You can do so, but please make sure to understand and deal with the
#   underlying caveats. In short they are:
#   - Speed penalty
#   - Risk of table/data corruption
#   - Data synchronising problems between the running servers
#   - Heavily media (disk) bound
#   - Relies on the system (external) file locking
#   - Is not applicable with all table types. (Such as InnoDB)
#     Trying so will end up with undesirable results.
#
# 4.TCP/IP Port
#
#   Every server requires one and it must be unique.
#
# 5.[myblockchaind#] Groups
#
#   In the example below the first and the fifth myblockchaind group was
#   intentionally left out. You may have 'gaps' in the config file. This
#   gives you more flexibility.
#
# 6.MyBlockchain Server User
#
#   You can pass the user=... option inside [myblockchaind#] groups. This
#   can be very handy in some cases, but then you need to run $my_progname
#   as UNIX root.
#
# 7.A Start-up Manage Script for $my_progname
#
#   In the recent MyBlockchain distributions you can find a file called
#   myblockchaind_multi.server.sh. It is a wrapper for $my_progname. This can
#   be used to start and stop multiple servers during boot and shutdown.
#
#   You can place the file in /etc/init.d/myblockchaind_multi.server.sh and
#   make the needed symbolic links to it from various run levels
#   (as per Linux/Unix standard). You may even replace the
#   /etc/init.d/myblockchain.server script with it.
#
#   Before using, you must create a my.cnf file either in @sysconfdir@/my.cnf
#   or /root/.my.cnf and add the [myblockchaind_multi] and [myblockchaind#] groups.
#
#   The script can be found from support-files/myblockchaind_multi.server.sh
#   in MyBlockchain distribution. (Verify the script before using)
#

[myblockchaind_multi]
myblockchaind     = @bindir@/myblockchaind_safe
myblockchainadmin = @bindir@/myblockchainadmin
user       = multi_admin
password   = my_password

[myblockchaind2]
socket     = /tmp/myblockchain.sock2
port       = 3307
pid-file   = @localstatedir@2/hostname.pid2
datadir    = @localstatedir@2
language   = @datadir@/myblockchain/english
user       = unix_user1

[myblockchaind3]
myblockchaind     = /path/to/myblockchaind_safe
ledir      = /path/to/myblockchaind-binary/
myblockchainadmin = /path/to/myblockchainadmin
socket     = /tmp/myblockchain.sock3
port       = 3308
pid-file   = @localstatedir@3/hostname.pid3
datadir    = @localstatedir@3
language   = @datadir@/myblockchain/swedish
user       = unix_user2

[myblockchaind4]
socket     = /tmp/myblockchain.sock4
port       = 3309
pid-file   = @localstatedir@4/hostname.pid4
datadir    = @localstatedir@4
language   = @datadir@/myblockchain/estonia
user       = unix_user3
 
[myblockchaind6]
socket     = /tmp/myblockchain.sock6
port       = 3311
pid-file   = @localstatedir@6/hostname.pid6
datadir    = @localstatedir@6
language   = @datadir@/myblockchain/japanese
user       = unix_user4
EOF
  exit(0);
}

####
#### usage
####

sub usage
{
  print <<EOF;
$my_progname version $VER by Jani Tolonen

Description:
$my_progname can be used to start, reload, or stop any number of separate
myblockchaind processes running in different TCP/IP ports and UNIX sockets.

$my_progname can read group [myblockchaind_multi] from my.cnf file. You may
want to put options myblockchaind=... and myblockchainadmin=... there.  Since
version 2.10 these options can also be given under groups [myblockchaind#],
which gives more control over different versions.  One can have the
default myblockchaind and myblockchainadmin under group [myblockchaind_multi], but this is
not mandatory. Please note that if myblockchaind or myblockchainadmin is missing
from both [myblockchaind_multi] and [myblockchaind#], a group that is tried to be
used, $my_progname will abort with an error.

$my_progname will search for groups named [myblockchaind#] from my.cnf (or
the given --defaults-extra-file=...), where '#' can be any positive 
integer starting from 1. These groups should be the same as the regular
[myblockchaind] group, but with those port, socket and any other options
that are to be used with each separate myblockchaind process. The number
in the group name has another function; it can be used for starting,
reloading, stopping, or reporting any specific myblockchaind server.

Usage: $my_progname [OPTIONS] {start|reload|stop|report} [GNR,GNR,GNR...]
or     $my_progname [OPTIONS] {start|reload|stop|report} [GNR-GNR,GNR,GNR-GNR,...]

The GNR means the group number. You can start, reload, stop or report any GNR,
or several of them at the same time. (See --example) The GNRs list can
be comma separated or a dash combined. The latter means that all the
GNRs between GNR1-GNR2 will be affected. Without GNR argument all the
groups found will either be started, reloaded, stopped, or reported. Note that
syntax for specifying GNRs must appear without spaces.

Options:

These options must be given before any others:
--no-defaults      Do not read any defaults file
--defaults-file=...  Read only this configuration file, do not read the
                   standard system-wide and user-specific files
--defaults-extra-file=...  Read this configuration file in addition to the
                   standard system-wide and user-specific files
Using:  @{[join ' ', @defaults_options]}

--example          Give an example of a config file with extra information.
--help             Print this help and exit.
--log=...          Log file. Full path to and the name for the log file. NOTE:
                   If the file exists, everything will be appended.
                   Using: $opt_log
--myblockchainadmin=...   myblockchainadmin binary to be used for a server shutdown.
                   Since version 2.10 this can be given within groups [myblockchaind#]
                   Using: $myblockchainadmin
--myblockchaind=...       myblockchaind binary to be used. Note that you can give myblockchaind_safe
                   to this option also. The options are passed to myblockchaind. Just
                   make sure you have myblockchaind in your PATH or fix myblockchaind_safe.
                   Using: $myblockchaind
                   Please note: Since myblockchaind_multi version 2.3 you can also
                   give this option inside groups [myblockchaind#] in ~/.my.cnf,
                   where '#' stands for an integer (number) of the group in
                   question. This will be recognised as a special option and
                   will not be passed to the myblockchaind. This will allow one to
                   start different myblockchaind versions with myblockchaind_multi.
--no-log           Print to stdout instead of the log file. By default the log
                   file is turned on.
--password=...     Password for myblockchainadmin user.
--silent           Disable warnings.
--tcp-ip           Connect to the MyBlockchain server(s) via the TCP/IP port instead
                   of the UNIX socket. This affects stopping and reporting.
                   If a socket file is missing, the server may still be
                   running, but can be accessed only via the TCP/IP port.
                   By default connecting is done via the UNIX socket.
--user=...         myblockchainadmin user. Using: $opt_user
--verbose          Be more verbose.
--version          Print the version number and exit.
EOF
  exit(0);
}
