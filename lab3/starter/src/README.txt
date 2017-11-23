README for Lab 3

Krishn Ramesh - 20521942
Ramandeep Farmaha - 20516974

This src folder contains two folders: pc_process and pc_thread. Both of these folders contain a produce.c file that implement the producer-consumer problem using processes and threads respectively.

To build both files run the following commands:
gcc pc_process/produce.c -lrt -lm -o produce
gcc pc_thread/produce.c -pthread -lm -o produce

Both folders also contain the run_pc.sh script which is the provided shell script that tests the produce source code with varying inputs and produces output tables with the mean and std deviation execution times.

Run the shell script with the following commands:
./pc_process/run_pc.sh
./pc_thread/run_pc.sh