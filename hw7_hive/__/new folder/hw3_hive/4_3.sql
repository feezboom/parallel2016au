add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;


USE s49305_test;


select req, sex, count(*) as tf
from (
	select l.http_request as req, d.sex as sex
	from userLogs as l 
	join userData as d 
	on (l.ip = d.ip) 
) as t1
group by req, sex
order by req DESC;
