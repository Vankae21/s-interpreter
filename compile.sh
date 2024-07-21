#!/bin/bash

CC="gcc"
SRC_DIR="src"
INC_DIR="$SRC_DIR/include"
OBJ_DIR="obj"

mkdir -p $OBJ_DIR

for FILE in $SRC_DIR/*.c; do
	$CC -c $FILE -I $INC_DIR -o "$OBJ_DIR/$(basename $FILE .c).o" -Wall
done

$CC $OBJ_DIR/*.o -o main

rm -rf $OBJ_DIR