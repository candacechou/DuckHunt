# DuckHunt 

This is the assignment of HMM in the course of DD2380 Artificial Intelligence @ KTH Royal Insitute of Technology (FALL2019)

## Introduction
The player has to observe the flight patterns of the birds and predict their nextmove in order to shoot them down. If the prediction is correct, the player will hit the bird and gain one point. If the prediction is wrong the player will miss the bird and lose one point. The game is over when all birds are shot down or when the time runs out.

    The players may also bet on the species of each bird after each round finishes. The players will get one point for each correct guess and lose one point for each incorrect guess.
    

## Compile

 please run
 
```
g++ -std=c++11 *pp 
```

## Run

The agent can be run in two different modes:

    1. Server - act as the judge by sending predefined observations one at a time
   and asking the client to respond.
    2. Client - get observations from standard input and output actions to
standard output (this is the default mode).

The server and client can be run in separate terminals and communicate
through pipes. Create the pipes first (we recommend Cygwin for Windows users).

```
mkfifo player2server server2player
```

### Terminal 1:
```
./a.out verbose server < player2server > server2player
```
### Terminal 2:
```
./a.out verbose > player2server < server2player
```

Where "Skeleton" is the compiled skeleton program. Or you may run both instances in the same terminal:

```
./a.out server < player2server | ./a.out verbose > player2server
```

You can test a different environment like this (if you do not want to make changes inside the program).
```
./a.out server load ../Data/ParadiseEmissions.in < player2server | ./a.out verbose > player2server
```
