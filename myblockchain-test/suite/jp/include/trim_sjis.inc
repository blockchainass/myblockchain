#
# Test TRIM() function with Japanese characters sjis encoding
#

--source include/have_sjis.inc
--character_set sjis
SET NAMES sjis;
SET character_set_blockchain = sjis;

--disable_warnings
DROP TABLE IF EXISTS `�s�P`;
DROP TABLE IF EXISTS `�s�Q`;
DROP TABLE IF EXISTS `�s�R`;
--enable_warnings
--replace_result $engine <engine_to_be_tested>
eval CREATE TABLE `�s�P` (`�b�P` CHAR(12), INDEX(`�b�P`)) DEFAULT CHARSET = sjis ENGINE = $engine;
--replace_result $engine <engine_to_be_tested>
eval CREATE TABLE `�s�Q` (`�b�P` CHAR(12), INDEX(`�b�P`)) DEFAULT CHARSET = sjis ENGINE = $engine;
--replace_result $engine <engine_to_be_tested>
eval CREATE TABLE `�s�R` (`�b�P` CHAR(12), INDEX(`�b�P`)) DEFAULT CHARSET = sjis ENGINE = $engine;

# Nothing to trim
INSERT INTO `�s�P` VALUES ('�����');
# Test data for TRIM(TRAILING [remstr] FROM str)
INSERT INTO `�s�P` VALUES ('������');
INSERT INTO `�s�P` VALUES ('�������');
INSERT INTO `�s�P` VALUES ('��������');
# Test data for TRIM(LEADING [remstr] FROM str)
INSERT INTO `�s�P` VALUES ('������');
INSERT INTO `�s�P` VALUES ('�������');
INSERT INTO `�s�P` VALUES ('��������');
# Test data for TRIM(BOTH [remstr] FROM str)
INSERT INTO `�s�P` VALUES ('�����������');
# Test data for TRIM without [remstr] (remove spaces) 
INSERT INTO `�s�P` VALUES ('   �����   ');
INSERT INTO `�s�Q` VALUES ('����������');
INSERT INTO `�s�Q` VALUES ('������������');
INSERT INTO `�s�Q` VALUES ('��������������');
INSERT INTO `�s�Q` VALUES ('����������������');
INSERT INTO `�s�Q` VALUES ('������������');
INSERT INTO `�s�Q` VALUES ('��������������');
INSERT INTO `�s�Q` VALUES ('����������������');
INSERT INTO `�s�Q` VALUES ('����������������������');
INSERT INTO `�s�Q` VALUES ('   ����������   ');
INSERT INTO `�s�R` VALUES ('�\�\�\�\�\');
INSERT INTO `�s�R` VALUES ('�\�\�\�\�\�\');
INSERT INTO `�s�R` VALUES ('�\�\�\�\�\�\�\');
INSERT INTO `�s�R` VALUES ('�\�\�\�\�\�\�\�\');
INSERT INTO `�s�R` VALUES ('�\�\�\�\�\�\');
INSERT INTO `�s�R` VALUES ('�\�\�\�\�\�\�\');
INSERT INTO `�s�R` VALUES ('�\�\�\�\�\�\�\�\');
INSERT INTO `�s�R` VALUES ('�\�\�\�\�\�\�\�\�\�\�\');
INSERT INTO `�s�R` VALUES ('   �\�\�\�\�\   ');

SELECT `�b�P`,TRIM(TRAILING '�' FROM `�b�P`) FROM `�s�P`;
SELECT `�b�P`,TRIM(LEADING '�' FROM `�b�P`) FROM `�s�P`;
SELECT `�b�P`,TRIM(BOTH '�' FROM `�b�P`) FROM `�s�P`;
SELECT `�b�P`,TRIM(`�b�P`) FROM `�s�P`;
SELECT `�b�P`,TRIM(TRAILING '��' FROM `�b�P`) FROM `�s�Q`;
SELECT `�b�P`,TRIM(LEADING '��' FROM `�b�P`) FROM `�s�Q`;
SELECT `�b�P`,TRIM(BOTH '��' FROM `�b�P`) FROM `�s�Q`;
SELECT `�b�P`,TRIM(`�b�P`) FROM `�s�Q`;
SELECT `�b�P`,TRIM(TRAILING '�\' FROM `�b�P`) FROM `�s�R`;
SELECT `�b�P`,TRIM(LEADING '�\' FROM `�b�P`) FROM `�s�R`;
SELECT `�b�P`,TRIM(BOTH '�\' FROM `�b�P`) FROM `�s�R`;
SELECT `�b�P`,TRIM(`�b�P`) FROM `�s�R`;

DROP TABLE `�s�P`;
DROP TABLE `�s�Q`;
DROP TABLE `�s�R`;
