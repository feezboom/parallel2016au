add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
USE velkerr;

add jar Reverser/jar/Reverser.jar;
create temporary function reverse as 'com.lab4udf.ReverseUDF';

select reverse(ip)
from IPRegions;
