add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
USE velkerr;

select hitCount.region, SUM(men), SUM(women)
from (
    select region, IF(gender='male', count(1), 0) AS men, IF(gender='female', count(1), 0) AS women
    from logs inner join ipregions on logs.ip = ipregions.ip 
          inner join users on logs.ip = ipregions.ip
    group by region, gender
) as hitCount
group by hitCount.region;