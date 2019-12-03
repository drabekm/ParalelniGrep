# Paralelní grep
Projekt do programovacího seminaře na VUT FIT ve druhém ročníku.

## Popis
Program založený na vláknech který bude paralelně provádět grep. 
Program načte řádek textu ze vstupu a několik regex výrazů, které mají uživatelem určenou váhu.
Každé vlákno zpracovává jeden regex a pokud řádek splňuje požadavky regexu tak přičte váhu regexu
k celkovému skore řádku. Pokud řádek má skóre stejné nebo vyšší než předem nastavené minimální skore
tak se vypíše na výstup.

## Použití
pgrep MIN_SCORE RE1 SC1 [ RE2 SC2 ] [ RE3 SC3 ] ...
MIN_SCORE: minimální score
RE: regulární výraz
SC: score získané, pokud řádka na vstupu splňuje regulární výraz RE

## Překlad
gcc pgrep.c -o pgrep -lpthread
nebo
g++ pgrep.c -o pgrep -lpthread
