#! /bin/bash

echo "rename LogName shell run..."

count=0

function findFileName()
{
while [ true ]
do
	#echo "while $count"
	let count+=1
	val=file
	filename=file-$count
	if [ -f $filename ]; then
		echo $filename exist!
	else
		#echo $filename no exist!
		break;
	fi
	
done
}





if [ ! -f "file" ];then
	echo "no exit file"
	touch file
else
	findFileName
	echo "rename file => file-$count"
	mv file file-$count
	
fi






echo "rename LogName shell exit..."
