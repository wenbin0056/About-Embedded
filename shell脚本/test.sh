#! /bin/bash
echo "123"




if [ ! -f "file" ];then
	echo "no exit file"
	#touch file
else
	echo "exit"
	#mv file 
fi


count=0
while [ true ]
do
	echo "while $count"
	count+=1
	sleep 1
done