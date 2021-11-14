#!/bin/bash

# Be sure that this file has execution permissions:
# Use the nautilus explorer or chmod +x optimization.sh

# Write here the name and path of your program and database
DIR_P2=$HOME/PAV/P2
DB=$DIR_P2/db.v4
CMD=$DIR_P2/bin/vad

#arrays for threshold and time values
A1=($(seq 5 1 7))
A2=($(seq 3 1 5))
VS=($(seq 11 1 13))
SV=($(seq 1 1 3))

for a1 in "${A1[@]}"; do
    for a2 in "${A2[@]}"; do
        for vs in "${VS[@]}"; do
            for sv in "${SV[@]}"; do
                echo "A1 = $a1 , A2 = $a2 , VS = $vs , SV = $sv"
                for filewav in $DB/*/*wav; do
                #    echo
                #    echo "**************** $filewav ****************"
                #    if [[ ! -f $filewav ]]; then 
                #	    echo "Wav file not found: $filewav" >&2
                #	    exit 1
                #    fi

                    filevad=${filewav/.wav/.vad}

                    $CMD -i $filewav -o $filevad -k $a1 -l $a2 -m $sv -n $vs || exit 1

                # Alternatively, uncomment to create output wave files
                    #filewavOut=${filewav/.wav/.wav}
                    #$CMD -i $filewav -o $filevad -w $filewavOut || exit 1

                done

                scripts/vad_evaluation.pl $DB/*/*lab
            done
        done
    done
done


for zcr in "${ZCR[@]}"; do
    echo "ZCR threshold = $zcr"
    for filewav in $DB/*/*wav; do
    #    echo
    #    echo "**************** $filewav ****************"
    #    if [[ ! -f $filewav ]]; then 
    #	    echo "Wav file not found: $filewav" >&2
    #	    exit 1
    #    fi

        filevad=${filewav/.wav/.vad}

        $CMD -i $filewav -o $filevad -l $zcr || exit 1

    # Alternatively, uncomment to create output wave files
        #filewavOut=${filewav/.wav/.wav}
        #$CMD -i $filewav -o $filevad -w $filewavOut || exit 1

    done

    scripts/vad_evaluation.pl $DB/*/*lab
done


exit 0
