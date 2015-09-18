#################### include/testdb_only.inc ######################
#                                                                 #
# We must prevent to work on blockchains created by customers,      #
# because we DROP/CREATE/MODIFY objects with sometimes common     #
# names like STAFF, EMPLOYEE etc.                                 #
#                                                                 #
# Therefore we check the environment variable USE_RUNNING_SERVER. #
# USE_RUNNING_SERVER is exported by "myblockchain-test-run" and could    #
# contain the following values:                                   #
# 0  -- myblockchain-test-run was started without the --extern option    #
#       That means the test will be performed within the test     #
#       area 'myblockchain-test/var/...' .                               #
# 1  -- myblockchain-test-run was started with the --extern option       #
#       That means the test will be performed by an already       #
#       running server and data modifications will most probably  #
#       outside of the common test area 'myblockchain-test/var/...' .    #
#                                                                 #
# If USE_RUNNING_SERVER is not 0 the test will be skipped.        #
#                                                                 #
###################################################################

if ($USE_RUNNING_SERVER){
  skip Not with extern server;
}
