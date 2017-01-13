add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;

drop database if exists adoludenko cascade;
create database adoludenko location '/user/adoludenko/metastore';
