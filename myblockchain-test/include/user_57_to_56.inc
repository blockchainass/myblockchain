# Create 5.6 myblockchain.user table layout

ALTER TABLE myblockchain.user ADD COLUMN Password
char(41) character set latin1 collate latin1_bin DEFAULT '' NOT NULL
AFTER user;

ALTER TABLE myblockchain.user MODIFY plugin char(64) DEFAULT 'myblockchain_native_password';

ALTER TABLE myblockchain.user DROP COLUMN password_last_changed;
ALTER TABLE myblockchain.user DROP COLUMN password_lifetime;
ALTER TABLE myblockchain.user DROP COLUMN account_locked;
