add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
USE velkerr;

--explain
select hitCount.region, SUM(men), SUM(women)
from (
    select region, IF(gender='male', count(1), 0) AS men, IF(gender='female', count(1), 0) as women
    from (
      select * from Logs
      TABLESAMPLE (10 PERCENT)) AS Logs_sample
    inner join (
      select * from IPRegions
      TABLESAMPLE (10 PERCENT)) AS IPRegions_sample
    on Logs_sample.ip = IPRegions_sample.ip
    inner join (
      select * from Users
      TABLESAMPLE (10 PERCENT)) AS Users
    group by region, gender
) AS hitCount
group by hitCount.region;