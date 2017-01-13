add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;

drop database if exists s49603 cascade;
create database s49603 location '/user/s49603/metastore';
