add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;
add jar ./demotivator.jar;

USE s49305_test;


create temporary function demotivator as 'com.example.hive.udf.demotivator';

select ip, demotivator(age)
from userData

