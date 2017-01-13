add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE s49305_test;

DROP TABLE IF EXISTS userData;

CREATE EXTERNAL TABLE userData (
	ip STRING,
	browser STRING,
	sex STRING,
	age STRING
)
ROW FORMAT SERDE 'org.apache.hadoop.hive.contrib.serde2.RegexSerDe'
WITH SERDEPROPERTIES (
	"input.regex" = '^(\\S*)\\t(\\S*)\\t(\\S*)\\t(\\S*).*$',
	"output.format.string" = "%1$s %2$s %3$s %4$s"
)
STORED AS TEXTFILE
LOCATION '/data/user_logs/user_data_S';

select * from userData limit 100;
