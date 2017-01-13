add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE s49305_test;

DROP TABLE IF EXISTS userLogs;

CREATE EXTERNAL TABLE userLogs (
	ip STRING,
	date STRING,
	http_request STRING,
	size STRING,
	http_status STRING,
	browser STRING
)
ROW FORMAT SERDE 'org.apache.hadoop.hive.contrib.serde2.RegexSerDe'
WITH SERDEPROPERTIES (
	"input.regex" = '^(\\S*)\\t\\t\\t(\\d{8})\\S*\\t(\\S*)\\t(\\S*)\\t(\\S*)\\t(\\S*).*$',
	"output.format.string" = "%1$s %2$s %3$s %4$s %5$s %6$s"
)
STORED AS TEXTFILE
LOCATION '/data/user_logs/user_logs_S/';

select * from userLogs limit 100;
