#!/bin/bash
FINAL_DIR="../src-final"

## Standalone soubory
cp ./Makefile $FINAL_DIR
cp ./rozdeleni $FINAL_DIR
cp ./rozsireni $FINAL_DIR

## Složky
# Generátor
cp ./generator/* $FINAL_DIR -r
# Hlavní program
cp ./main/* $FINAL_DIR -r
# Parser
cp ./parser/* $FINAL_DIR -r
# Scanner
cp ./scanner/* $FINAL_DIR -r

## Komprimace
cd $FINAL_DIR
tar -czf xdolej08.tar.gz ./*