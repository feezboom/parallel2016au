add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE adoludenko;

DROP TABLE IF EXISTS example;
DROP TABLE IF EXISTS ip_example;

CREATE EXTERNAL TABLE example(
    ip STRING,
    date INT,
    url STRING,
    pagesize SMALLINT,
    statuscode SMALLINT,
    browser STRING
)
ROW FORMAT SERDE 'org.apache.hadoop.hive.serde2.RegexSerDe'
WITH SERDEPROPERTIES(
    "input.regex" = '^(\\S*)\\t{3}(\\d{8})\\S*\\t(\\S*)\\t(\\d{1,})\\t(\\d{1,})\\t(\\S*).*$',
    "output.format.string" = "%1$s %2$s %3$s"
)
STORED AS TEXTFILE
LOCATION '/data/user_logs/user_logs_S';

CREATE EXTERNAL TABLE ip_example(
    ip STRING,
    region STRING
)
ROW FORMAT SERDE 'org.apache.hadoop.hive.contrib.serde2.RegexSerDe'
WITH SERDEPROPERTIES(
    "input.regex" = '^(\\S*)\\t(\\S*).*$',
    "output.format.string" = "%1$s %2$s"
)
STORED AS TEXTFILE
LOCATION '/data/user_logs/ip_data_S';

SELECT * FROM example LIMIT 5;
SELECT * FROM ip_example LIMIT 5;
