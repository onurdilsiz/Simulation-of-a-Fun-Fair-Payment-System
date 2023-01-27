# Vending Machine Simulation
This project simulates the behavior of vending machines and customers using multi-threading in C++. The program reads a CSV file that contains customer data, such as sleep time, vending machine number, company name, and amount. The program then simulates the customer's interaction with the vending machine using threads, and writes a log file of the simulation.

## Getting Started
To run the program, you will need to have a C++ compiler installed on your machine. The program also requires the following libraries:

- iostream
- cstdlib
- fstream
- sstream
- string
- vector
- thread
- pthread.h
- chrono
You can compile the program by running the following command in the terminal:

`g++ -std=c++11 -pthread vendingMachine.cpp -o vending`
The program takes one command line argument, which is the path to the txt file containing the customer data.


`./vending path/to/file.txt`
The program will create a log file in the same directory as the input file, with the same name and a "_log.txt" extension.

## Functionality
The program starts by reading the input file and initializing the necessary variables and data structures. It then creates 10 vending machine threads and as many customer threads as specified in the input file. Each customer thread simulates the customer's interaction with the vending machine by requesting a mutex lock on the vending machine and company, updating the company's balance and customer's amount, and then releasing the lock. The vending machine thread simply waits for customers to finish using it and then updates its own balance.

The program also uses a mutex lock for the log file to ensure that the log entries are written in the correct order.

## Conclusion
This program demonstrates the use of multi-threading in C++ for simulating a real-world scenario. It showcases the use of threading, mutex locks, and file input/output in C++. The program is a good starting point for understanding the basics of multi-threading in C++ and can be modified to implement more advanced functionality.
