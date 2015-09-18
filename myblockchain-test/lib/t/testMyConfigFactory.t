#!/usr/bin/perl
# -*- cperl -*-

# Copyright (c) 2007 MyBlockchain AB, 2009 Sun Microsystems, Inc.
# Use is subject to license terms.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

use strict;
use warnings;

use File::Temp qw / tempdir /;
my $dir = tempdir( CLEANUP => 1 );

use Test::More qw(no_plan);

BEGIN { use_ok ( "My::ConfigFactory" ) };

my $gen1_cnf= "$dir/gen1.cnf";
open(OUT, ">", $gen1_cnf) or die;

print OUT <<EOF
[myblockchaind.master]
# Comment
option1=value1
basedir=abasedir

[myblockchaind.1]
# Comment
option1=value1
option2=value2

[ENV]
MASTER_MY_PORT=\@myblockchaind.master.port

EOF
;
close OUT;

my $basedir= "../..";

my $config= My::ConfigFactory->new_config
(
 {
  basedir => $basedir,
  template_path => $gen1_cnf,
  vardir => "/path/to/var",
  baseport => 10987,
  #hosts => [ 'host1', 'host2' ],
 }
);

print $config;

ok ( $config->group("myblockchaind.master"), "group myblockchaind.master exists");
ok ( $config->group("myblockchaind.1"), "group myblockchaind.1 exists");
ok ( $config->group("client"), "group client exists");
ok ( !$config->group("myblockchaind.3"), "group myblockchaind.3 does not exist");

ok ( $config->first_like("myblockchaind"), "group like 'myblockchaind' exists");

is( $config->value('myblockchaind.1', '#host'), 'localhost',
    "myblockchaind.1.#host has been generated");

is( $config->value('client', 'host'), 'localhost',
    "client.host has been generated");

is( $config->value('client', 'host'),
    $config->value('myblockchaind.master', '#host'),
    "client.host is same as myblockchaind.master.host");

ok ( $config->value("myblockchaind.1", 'character-sets-dir') =~ /$basedir.*charsets$/,
     "'character-sets-dir' generated");

ok ( $config->value("myblockchaind.1", 'lc-messages-dir') =~ /$basedir.*share$/,
     "'lc-messages-dir' generated");

ok ( $config->value("ENV", 'MASTER_MY_PORT') =~ /\d/,
     "'lc-messages-dir' generated");

my $gen2_cnf= "$dir/gen2.cnf";
open(OUT, ">", $gen2_cnf) or die;

print OUT <<EOF
[myblockchaind.master]
EOF
;
close OUT;

my $config2= My::ConfigFactory->new_config
(
 {
  basedir => $basedir,
  template_path => $gen2_cnf,
  vardir => "/path/to/var",
  baseport => 10987,
  #hosts => [ 'host1', 'host2' ],
 }
);

print $config2;

ok ( $config2->first_like("myblockchaind"), "group like 'myblockchaind' exists");
