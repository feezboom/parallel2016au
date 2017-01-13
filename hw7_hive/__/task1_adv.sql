add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
USE velkerr;

set hive.exec.dynamic.partition.mode=nonstrict;
--set hive.exec.max.dynamic.partitions=116;
--set hive.exec.max.dynamic.partitions.pernode=100;

DROP TABLE IF EXISTS Logs;
DROP TABLE IF EXISTS LogsNonPart;
DROP TABLE IF EXISTS IPRegions;
DROP TABLE IF EXISTS Users;

--EXPLAIN
CREATE EXTERNAL TABLE LogsNonPart (
    ip STRING,
    time int,
    query STRING,
    pageSize smallint,
    statusCode smallint,
    browser STRING
)
CLUSTERED BY (query) INTO 100 BUCKETS
ROW FORMAT SERDE 'org.apache.hadoop.hive.serde2.RegexSerDe'
--Class without 'contrib' (only for deserialize):
--http://hive.apache.org/javadocs/r0.12.0/api/org/apache/hadoop/hive/serde2/RegexSerDe.html
WITH SERDEPROPERTIES (--\\S* after d{8} because real time.length > 8. So we got only days.
    "input.regex" = '^(\\S*)\\t\\t\\t(\\d{8})\\S*\\t(\\S*)\\t(\\d{1,})\\t(\\d{1,})\\t(\\S*).*$', --{,3} isn't works
    "output.format.string" = "%1$s %2$s %3$s %4$s %5$s %6$s"
    --"output.format.string" = "%1$s %2$s %3$s %4$s %5$s %6$s %2$s"
)
STORED AS TEXTFILE
LOCATION '/data/user_logs/user_logs_S';

CREATE EXTERNAL TABLE Logs(
    ip STRING,
    query STRING,
    pageSize smallint,
    statusCode smallint,
    browser STRING
)
PARTITIONED BY (time INT)
STORED AS TEXTFILE;

INSERT OVERWRITE TABLE Logs PARTITION (time)
SELECT ip, query, pageSize, statusCode, browser, time FROM LogsNonPart;

CREATE EXTERNAL TABLE IPRegions (
    ip STRING,
    region STRING
)
CLUSTERED BY (ip) INTO 10 BUCKETS
ROW FORMAT SERDE 'org.apache.hadoop.hive.contrib.serde2.RegexSerDe'
WITH SERDEPROPERTIES (
    "input.regex" =  '^(\\S*)\\t(\\S*).*$',
    "output.format.string" = "%1$s %2$s"
)
STORED AS TEXTFILE
--LOCATION '/data/user_logs/ipdata_folder';
LOCATION '/data/user_logs/ip_data_S';

CREATE EXTERNAL TABLE Users(
    ip STRING,
    browser STRING,
    gender STRING, --todo: convert string to boolean (write UDF).
    age tinyint
)
CLUSTERED BY (ip) INTO 10 BUCKETS
ROW FORMAT SERDE 'org.apache.hadoop.hive.serde2.RegexSerDe'
WITH SERDEPROPERTIES (
       "input.regex" =  '^(\\S*)\\t(\\S*)\\t(\\S*)\\t(\\d{1,}).*$',
       "output.format.string" = "%1$s %2$s %3$s %4$s"
)
STORED AS TEXTFILE
LOCATION '/data/user_logs/user_data_S';

--Checking correctness of creation tables.
select * from Logs limit 50;
select * from Users limit 10;
select * from IPRegions limit 10;

--amount of partitions
--select count(distinct time) from LogsNonPart;

--counting amount of records
--select count(1) from Logs;
--select count(1) from Users;
--select count(1) from IPRegions;