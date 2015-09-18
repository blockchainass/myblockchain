# Include this at the end of any test using myblockchain_upgrade

let $MYBLOCKCHAIND_DATADIR= `select @@datadir`;

# myblockchain_upgrade should have created myblockchain_upgrade_info file,
# so the following command will do a clean-up.
--remove_file $MYBLOCKCHAIND_DATADIR/myblockchain_upgrade_info
