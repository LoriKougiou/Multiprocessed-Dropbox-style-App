#!/bin/bash
#cat log_file1 log_file2 ... log_filen | ./get_stats.sh
declare -a clients_in
declare -a clients_out
num_clients_in=0
sent_bytes=0
sent_files=0
received_bytes=0
received_files=0
num_clients_out=0
max_id=0
min_id=0

#flags pou markarontai 1 ean i leksi einai kapoia apo ta kew words tou logfile gia na elegxthei i epomeni
flag_nc=0
flag_br=0
flag_bw=0
flag_fr=0
flag_fw=0
flag_jl=0



#kai twra diavazw grammi grammi to dosmeno sinoliko logfile kai elegxodas kathe leksi , vasizomai sta key words
#kai kratwntas ta aparaithta flags vgazw ta statistika
while read line
do
    for word in $line
    do
        if [ "$word" = "NEW_CLIENT:" ] 
        then
        	flag_nc=1
        else
        	if [ $flag_nc -eq "1" ] 
        	then
				if [[ ! " ${clients_in[@]} " =~ " ${word} " ]]
				then
				    clients_in=("$word" "${clients_in[@]}")

				    if [ $num_clients_in -eq "0" ]
				    then
				    	if [ "$min_id" -eq "0" ]
					    then 
					    	min_id="$word"
					    fi
					    if [ "$max_id" -eq "0" ]
					    then 
					    	max_id="$word"
					    fi
					fi
				    
				    if [ $word -le "$min_id" ]
				    then 
				    	min_id="$word"
				    fi

				    if [ $word -ge "$max_id" ]
				    then 
				    	max_id="$word"
				    fi

				    ((num_clients_in++))

				fi
				flag_nc=0

			else
				if [ "$word" = "BYTES_READ:" ] 
		        then
		        	flag_br=1
		        else
		        	if [ $flag_br -eq "1" ] 
        			then
        				received_bytes=$((received_bytes+word))
        				flag_br=0
        			else
        				if [ "$word" = "BYTES_WRITTEN:" ]
        				then
        					flag_bw=1
				        else
				        	if [ $flag_bw -eq "1" ] 
		        			then
		        				sent_bytes=$((sent_bytes+word))
		        				flag_bw=0
		        			else
		        				if [ "$word" = "FILES_WRITTEN:" ]
		        				then
		        					flag_fw=1
						        else
						        	if [ $flag_fw -eq "1" ] 
				        			then
				        				sent_files=$((sent_files+word))
				        				flag_fw=0
				        			else
				        				if [ "$word" = "FILES_READ:" ]
				        				then
				        					flag_fr=1
								        else
								        	if [ $flag_fr -eq "1" ] 
						        			then
						        				received_files=$((received_files+word))
						        				flag_fr=0
						        			else
						        				if [ "$word" = "JUST_LEFT:" ]
						        				then
						        					flag_jl=1
										        else
										        	if [ $flag_jl -eq "1" ] 
											    	then
														if [[ ! " ${clients_out[@]} " =~ " ${word} " ]]
														then
														    clients_out=("$word" "${clients_out[@]}")
														    ((num_clients_out++))
														fi
														flag_jl=0
														   
								        			fi
								        		fi
								        	fi
								        fi
								    fi
								fi
							fi
						fi
        			fi
        		fi

			fi
        fi

    done
done

echo "STATISTICS for clients ${clients_in[*]}"
echo "CLIENTS IN:	$num_clients_in unique IDs"
echo ${clients_in[*]} "		with max ID being $max_id and min ID being $min_id"
echo "TOTAL FILES READ:   $received_files"
echo "TOTAL BYTES READ:   $received_bytes"
echo "TOTAL FILES WRITTEN:   $sent_files"
echo "TOTAL BYTES WRITTEN:   $sent_bytes"
echo "CLIENTS OUT:	$num_clients_out"
