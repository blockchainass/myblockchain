# Allow anonymous users to connect 
disable_warnings;
disable_query_log;
INSERT IGNORE INTO myblockchain.user (host, user) VALUES ('localhost','');
FLUSH PRIVILEGES;
enable_query_log;
enable_warnings;
