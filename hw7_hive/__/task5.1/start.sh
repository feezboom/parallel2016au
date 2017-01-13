#! /usr/bin/env bash

printColored(){
  echo -e "\e[0;"$2"m \t"$1" \e[0;37m"
}

if [ ! -z $1 ]
then
    printColored "Rebuilding UDF..." 32;
    cd Reverser;
    ant;
    cd ../;
fi

printColored "Executing query in HIVE..." 33
hive -f query.sql
