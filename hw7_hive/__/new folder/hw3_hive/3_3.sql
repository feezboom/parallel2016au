add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE s49305_test;


select table_for_age.age_ 
from
	(
		select d.age as age_, count(*)/count(DISTINCT l.ip) as avr_of_age 
		from userLogs as l
		join userData as d
		on l.ip = d.ip
		group by d.age
	) as table_for_age
	join
	(
	select count(*)/count(DISTINCT l.ip) as avr 
	from userLogs as l
	join userData as d
	on l.ip = d.ip
	) as table_for_avr
where table_for_avr.avr < table_for_age.avr_of_age
