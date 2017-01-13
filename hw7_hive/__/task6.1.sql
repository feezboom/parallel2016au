add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
USE velkerr;

SELECT TRANSFORM(ip, time, query, pageSize, statusCode, browser) 
USING '/bin/sed "s/.ru/.com/" ' 
AS ip, time, query, pageSize, statusCode, browser 
FROM Logs
LIMIT 50;
