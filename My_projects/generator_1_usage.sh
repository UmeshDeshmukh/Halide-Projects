check_file_exists()
{
    FILE=$1
    if [ ! -f $FILE ]; then
        echo $FILE not found
        exit -1
    fi
}

check_symbol()
{
    FILE=$1
    SYM=$2
    if !(nm $FILE | grep $SYM > /dev/null); then
        echo "$SYM not found in $FILE"
    exit -1
    fi
}

export LD_LIBRARY_PATH=/mnt/g/Halide_Language/Halide-13.0.2-x86-64-linux/lib/
#export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:../lib

./generator_1 -g my_first_gen -o . target=host

check_file_exists my_first_gen.a
check_file_exists my_first_gen.h
check_symbol my_first_gen.a my_first_gen


echo "Success!"


