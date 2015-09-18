# Check if platform is little endian

perl;
my $packed= pack("i", 0x676e6162);
my $is_le = ($packed =~ /bang/);
open (F, ">" . $ENV{'MYBLOCKCHAIN_TMP_DIR'} . "/isle.inc");
print F "let \$is_le= $is_le;\n";
close F;
EOF

--source $MYBLOCKCHAIN_TMP_DIR/isle.inc
--remove_file $MYBLOCKCHAIN_TMP_DIR/isle.inc

if (! $is_le) {
  --skip Works only on little-endian
}
