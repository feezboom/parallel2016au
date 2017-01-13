add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
USE velkerr;

set mapred.reduce.tasks=8;
drop table if exists HitCount;

create table HitCount as
SELECT region, COUNT(IPRegions.ip) as cnt
FROM IPRegions inner join Logs on IPRegions.ip = Logs.ip
GROUP BY region
distribute by (region);

--set mapred.reduce.tasks=8;
select region, cnt
from (
    select region, cnt, avg(cnt) over() as avgCnt
    from HitCount
) as CntWithAvg
where CntWithAvg.cnt > CntWithAvg.avgCnt
distribute by (region);