COMPILATION

Compile the files sender.c and receiver.c using

gcc sender.c -o sender
gcc receiver.c -o receiver


RUNNING INSTRUCTIONS

First run the receiver using the command 

./receiver output.txt

Where output.txt is the file in which the results are stored

Then run the sender using the command 

./sender <packet-size-in-bits> <ip-address> <inter-packet-pair-spacing> <number-of-packet-pairs>

where packet-size-in-bits must be set to 8000

In the report all simulations have been run using the following parameters

./sender 8000 <ip-address> 1000 100

For expiriment 1, the commands utilized in setting the rate limit where

sudo tc qdisc add dev lo root tbf rate 10mbit burst 9kbit latency 50ms

To verify you can use

sudo tc qdisc show dev lo

and to remove the changes once the experiment is over

sudo tc qdisc del dev lo root

REFERENCES

ChatGPT was utilized in building the tool.

