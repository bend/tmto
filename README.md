TMTO
====
Generates rainbow tables to crack integers.
The pass should be written in Big endian hexadecimal representation of the int

Genrate tables
=====
./rainbow_gen -n nb_entries -c chain_length -s init_val -o output_file

Run crack
=====
./rainbow_crack -c chain_length -p password_file -t table_file -o output_file -s nb_threads

