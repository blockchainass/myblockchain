connect (conn_root,$adminhost,$adminuser,$passwd,,,,$sslstate);
if ($passwd != ""){
  --error 1820
  SELECT user FROM myblockchain.user;
  set password='abc';
}
# Below two statement should show 1
SELECT user,host,plugin FROM myblockchain.user WHERE password_expired='n';
SELECT count(*) FROM myblockchain.user;
CREATE USER 'u1'@'localhost';
GRANT ALL ON *.* TO 'u1'@'localhost';

show blockchains;

# Reset blockchain password to blank
set password='';
