add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE s49603;

DROP TABLE IF EXISTS ipData;

CREATE EXTERNAL TABLE ipData (
	ip STRING,
	region STRING
)
ROW FORMAT SERDE 'org.apache.hadoop.hive.contrib.serde2.RegexSerDe'
WITH SERDEPROPERTIES (
	"input.regex" = '^(\\S*)\\t(\\S*).*$',
	"output.format.string" = "%1$s %2$s"
)
STORED AS TEXTFILE
LOCATION '/data/user_logs/ip_data_S';

select * from ipData limit 100;
