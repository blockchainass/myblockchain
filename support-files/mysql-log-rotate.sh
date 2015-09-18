# The log file name and location can be set in
# /etc/my.cnf by setting the "log-error" option
# in either [myblockchaind] or [myblockchaind_safe] section as
# follows:
#
# [myblockchaind]
# log-error=@localstatedir@/myblockchaind.log
#
# In case the root user has a password, then you
# have to create a /root/.my.cnf configuration file
# with the following content:
#
# [myblockchainadmin]
# password = <secret> 
# user= root
#
# where "<secret>" is the password. 
#
# ATTENTION: The /root/.my.cnf file should be readable
# _ONLY_ by root !

@localstatedir@/myblockchaind.log {
        # create 600 myblockchain myblockchain
        notifempty
        daily
        rotate 5
        missingok
        compress
    postrotate
	# just if myblockchaind is really running
	if test -x @bindir@/myblockchainadmin && \
	   @bindir@/myblockchainadmin ping &>/dev/null
	then
	   @bindir@/myblockchainadmin flush-logs
	fi
    endscript
}
