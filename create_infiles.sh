#!/bin/bash
# ./create_infiles.sh dir_name num_of_files num_of_dirs levels

if [ $# -lt 4 ]
then
  echo "Invalid number of arguments"
  exit
fi

dir_name=$1 
directory=$PWD/$dir_name
num_of_files=$2

if [ $num_of_files -le 0 ]
then
	echo "Cannot have <=0 number of files.Exiting"
	exit
fi
num_of_dirs=$3

if [ $num_of_dirs -le 0 ]
then
	echo "Cannot have <=0 number of dirs.Exiting"
	exit
fi

levels=$4


#synarthsh pou paragei random alpharithmhtiko gramma
function rand_letter(){
	s='abcdefghijklmnopqrstuvxwyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
	p=$(($RANDOM % 61)) 
	echo -n ${s:$p:1}
}

#synarthsh pou ftiaxnei random string, pairnei ws lenght tou string to prwto orisma pou tha tou dothei
function rand_string(){
	length=$1
	name=""
	letter_count=1
	while [ $letter_count -le $length ] 
	do
		c="$(rand_letter)"
		name+=$c
		((letter_count++))
	done
	echo $name

}

# elegxw ean iparxei to dosmeno directory
if [ ! -d $dir_name ] 
then
    echo "Creating directory $dir_name" 
    mkdir "$directory"
else 
	echo "Directory $dir_name already exist"    
	exit
fi

# kai twra tha dimiourgisw tin ierarxeia directories 
counter=1

while [ $counter -le $num_of_dirs ]
do
	level_count=1
	length=$(((RANDOM % 8)+ 1))
	#name=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $length | head -n 1)
	name="$(rand_string $length)"
	directory=$PWD/$dir_name/$name
	mkdir "$directory"
	((counter++))
	((level_count++))


	while [ $level_count -le $levels ]
	do 
		if [ $counter -le $num_of_dirs ]
		then
			length=$(((RANDOM % 8)+ 1))
			name="$(rand_string $length)"
			directory=$directory/$name
			mkdir "$directory"
			((counter++))
			((level_count++))
		else 
			break
		fi
	done	
done

#twra tha dimiourghse ta files me mia round robin diadikasia
counter=1
start_dir=$PWD/$dir_name

while [ $counter -le $num_of_files ]
do

	for dir in $(find $start_dir -maxdepth $levels -type d)
	do 
		if [ $counter -le $num_of_files ]
		then			
			length=$(((RANDOM % 8)+ 1))
			name="$(rand_string $length)"
			#name=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $length | head -n 1)
			bytes=$(((RANDOM % (128*1024)) + 1024))
			curr_path=$dir/$name
			touch "$curr_path"
			string=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $bytes | head -n 1)
			#string="$(rand_string $bytes)"
			echo "$string" > $curr_path
			((counter++))
		else
			break
		fi
	done
done



