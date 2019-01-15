#!/bin/bash

WORKDIR=`pwd`

runCombine(){
	SIGNAME=$1
	if [ "$SIGNAME" = "T2tt" ]; then
		declare -a MASSES=( 800 900 1000 1100 1200 )
	else
		declare -a MASSES=( 1700 1800 1900 2000 2100 )
	fi

	cd cards
	for CHAN in 1 2 3 4 5; do
		for MASS in ${MASSES[@]}; do
			CARDNAME=simpleCard_ch${CHAN}_${SIGNAME}_${MASS}
			combine -m ${MASS} -n _${CARDNAME} -M Asymptotic ${CARDNAME}.txt
		done
	done	
}

cd $WORKDIR
runCombine T2tt
cd $WORKDIR
runCombine T1tttt
