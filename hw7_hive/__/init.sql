add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;

drop database if exists velkerr cascade;
create database velkerr location '/user/velkerr/metastore';
