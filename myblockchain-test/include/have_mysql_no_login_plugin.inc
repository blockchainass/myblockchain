#
# Check if server has support for loading plugins
#
if (`SELECT @@have_dynamic_loading != 'YES'`) {
  --skip myblockchain_no_login requires dynamic loading
}

#
# Check if the variable MYBLOCKCHAIN_NO_LOGIN is set
#
if (!$MYBLOCKCHAIN_NO_LOGIN) {
  --skip myblockchain_no_login requires the environment variable \$MYBLOCKCHAIN_NO_LOGIN to be set (normally done by mtr)
}

#
# Check if --plugin-dir was setup for myblockchain_no_login
#
if (`SELECT CONCAT('--plugin-dir=', REPLACE(@@plugin_dir, '\\\\', '/')) != '$MYBLOCKCHAIN_NO_LOGIN_OPT/'`) {
  --skip myblockchain_no_login requires that --plugin-dir is set to the myblockchain_no_login dir (either the .opt file does not contain \$MYBLOCKCHAIN_NO_LOGIN_OPT or another plugin is in use)
}

