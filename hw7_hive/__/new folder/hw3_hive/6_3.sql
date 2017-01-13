add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE s49305_test;

add FILE run.sh;

select TRANSFORM (browser)
	USING 'run.sh'
	as (info)
from userLogs;
