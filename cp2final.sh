#!/bin/bash
WORKING_DIR=`pwd`
DOCS_DIR="$WORKING_DIR/docs"
SOURCES_DIR="$WORKING_DIR/src"
FINAL_DIR="$WORKING_DIR/src-final"

## Odstranění existujícího obsahu
printf "Removing old contends of %s\n" $FINAL_DIR
rm $FINAL_DIR/* -r

## Standalone soubory
printf "Copying new files to %s\n" $FINAL_DIR
cp $SOURCES_DIR/Makefile $FINAL_DIR
cp $SOURCES_DIR/rozdeleni $FINAL_DIR
cp $SOURCES_DIR/rozsireni $FINAL_DIR
cp $DOCS_DIR/dokumentace.pdf $FINAL_DIR

## Složky
# Generátor
cp $SOURCES_DIR/generator/* $FINAL_DIR -r
# Hlavní program
cp $SOURCES_DIR/main/* $FINAL_DIR -r
# Parser
cp $SOURCES_DIR/parser/* $FINAL_DIR -r
# Scanner
cp $SOURCES_DIR/scanner/* $FINAL_DIR -r
# Support
cp $SOURCES_DIR/support/* $FINAL_DIR -r

## Komprimace
printf "Compressing files, creating archive\n"
cd $FINAL_DIR
tar -czf xdolej08.tar.gz ./*

## Spuštění kontroly
printf "Running official archive validation...\n"
mkdir $FINAL_DIR/validation
$DOCS_DIR/_official/is_it_ok.sh $FINAL_DIR/xdolej08.tar.gz $FINAL_DIR/validation

printf "Removing temporary validation directory\n"
rm $FINAL_DIR/validation -r

printf "Press enter to continue . . . "
read
