add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE s49603;

--##########################################################################################
-- Drop all tables if they were created before.

drop table if exists userLogs;
drop table if exists ipData;
drop table if exists userData;


--###########################################################################################
--CREATE TABLE WITH IP ADDRESSES

create external table ipData ( 
	ip string,
	region string
) 
row format serde 'org.apache.hadoop.hive.contrib.serde2.RegexSerDe' 
with serdeproperties( 
	"input.regex" = '^(\\S*)\\t(\\S*).*$',
	"output.format.string" = "%1$s %2$s"
)
stored as textfile location '/data/user_logs/ip_data.ipData.txt'; 



--###########################################################################################
-- CREATE TABLE WITH DATA 


create external table userData ( 
	ip string, 
	browser string, 
	sex string, 
	age string
)
row format serde 'org.apache.hadoop.hive.contrib.serde2.RegexSerDe' 
with serdeproperties( 
	"input.regex" = '^(\\S*)\\t(\\S*)\\t(\\S*)\\t(\\d+).*$',
	"output.format.string" = "%1$s %2$s %3$s %4$s"
)
stored as textfile location '/data/user_logs/user_data/userData';



--##########################################################################################
-- CREATE LOGS TABLE 

create external table userLogs (
	ip string,
	date string,
	url string,
	pagesize string,
	statuscode string,
	browser string
)
row format serde 'org.apache.hadoop.hive.serde2.RegexSerDe'
with serdeproperties(
	"input.regex" = '^(\\S*)\\t{3}(\\d{8})\\S*\\t(\\S*)\\t(\\d{1,})\\t(\\d{1,})\\t(\\S*).*$',
	"output.format.string" = "%1$s %2$s %3$s %4$s %5$s %6$s"
)
stored as  textfile location '/data/user_logs/user_logs/logsL.txt';



