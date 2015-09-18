# Export Table and Import from saved files .cfg and .ibd
# Caller should create t1 table definition and populate table

let $MYBLOCKCHAIND_DATADIR = `SELECT @@datadir`;

FLUSH TABLES t1 FOR EXPORT;

--copy_file $MYBLOCKCHAIND_DATADIR/test/t1.cfg $MYBLOCKCHAIND_DATADIR/t1.cfg_back
--copy_file $MYBLOCKCHAIND_DATADIR/test/t1.ibd $MYBLOCKCHAIND_DATADIR/t1.ibd_back

UNLOCK TABLES;
ALTER TABLE t1 DISCARD TABLESPACE;

--move_file $MYBLOCKCHAIND_DATADIR/t1.cfg_back $MYBLOCKCHAIND_DATADIR/test/t1.cfg
--move_file $MYBLOCKCHAIND_DATADIR/t1.ibd_back $MYBLOCKCHAIND_DATADIR/test/t1.ibd

ALTER TABLE t1 IMPORT TABLESPACE;

CHECK TABLE t1;
SHOW CREATE TABLE t1;
SELECT * FROM t1;
