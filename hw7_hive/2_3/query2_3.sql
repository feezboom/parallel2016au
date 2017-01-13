add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE s49603;

select browser, count(*) as usersPerBrowser
from userLogs
group by browser
order by usersPerBrowser DESC;
