add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
USE velkerr;

select time, count(1) as cnt 
from Logs 
group by time
-- sorting starts 2nd job. So, with 'order by' clause the query will work much slowly
order by cnt desc;
