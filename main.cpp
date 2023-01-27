#include <iostream>
#include <cstdlib> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <pthread.h>
#include <chrono>
using namespace std;


// global variables for companies 
int kevin = 0;
int bob = 0;
int stuart = 0;
int otto = 0;
int dave = 0;
// counter for the last customer to finish vending machine thread
int unlock=0;
int noOfCustomers;

//necessary mutexes names are self explanatory
pthread_mutex_t mutexesForCustomers[10];
pthread_mutex_t mutexesForCompanies[5];
pthread_mutex_t mutexForlog;
// Stream for writing to a log file.
ofstream outputFile;

// struct for the customerdata
struct customerStruct{
    int customerId;
    int sleepTime;
    int vendingmachine;
    string companyName ;
    int amount;
   
};

// void * array in order to pass data from customer to vending machine
void * parameterForVTM[10];

//declarations for thread functions
void * customerThreadFunc(void * customerData);
void * vendingMachineThreadFunc(void* vmtData);


int main (int argc, char *argv[]){
    
    string filename=argv[1];
    // Open the file
    std::ifstream file(filename);

    // Declare a vector to store the lines
    std::vector<std::string> lines;

    // Read the file line by line
    std::string line;
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }
    
    file.close();

    
    string firstline=lines[0];  
    noOfCustomers= std::stoi(firstline);
    // assigning outputpath
    size_t lastindex = filename.find_last_of("."); 
    string rawname =filename.substr(0, lastindex);
    string outputPath= rawname+"_log.txt";
    // Create and open a text file
    outputFile.open(outputPath);



    std::vector<struct customerStruct> customers;

    
    pthread_t customerThreads[noOfCustomers];
    pthread_attr_t attr; /* set of attributes for the thread */
    pthread_attr_init(&attr); // initialize the thread attributes with default configuration
    pthread_t vendingMachineThreads[10];
    
    //initializations of mutexes
    for (int i=0; i<10;i++){
        pthread_mutex_init(&mutexesForCustomers[i], NULL);
    }
   for (int i=0; i<5;i++){
        pthread_mutex_init(&mutexesForCompanies[i], NULL);
    }
    pthread_mutex_init(&mutexForlog,NULL);


    int customerID[noOfCustomers];
    //create customer structs from the line data
    for(int i=1; i<= noOfCustomers;i++){

        std::vector<std::string> parts;
        // Create a stringstream from the string
        std::istringstream stream(lines[i]);

        // Split the string using getline and a comma as the delimiter
        std::string part;
        while (std::getline(stream, part, ','))
        {
            parts.push_back(part);
        }

        customerID[i]=i;
        int sleepTime=std::stoi(parts[0]);
        int vendingMachine=std::stoi(parts[1])-1 ;
        string companyName=parts[2];
        int amount=std::stoi(parts[3]);

        struct customerStruct customer={customerID[i],sleepTime,vendingMachine,companyName,amount};
        customers.push_back(customer);
        

    }

    int vmtID[10];

    for( int i = 0; i<10; i++){
        vmtID[i]=i;
        pthread_create(&vendingMachineThreads[i],&attr, vendingMachineThreadFunc, &vmtID[i]);
    }

    for (int i = 0 ;i< noOfCustomers;i++){
        pthread_create(&customerThreads[i],&attr, customerThreadFunc ,&customers[i]);
    }
   

     for (int i = 0 ;i< noOfCustomers;i++){
        pthread_join(customerThreads[i],NULL);
    }


    for( int i = 0; i<10; i++){

        pthread_join(vendingMachineThreads[i],NULL);
    }

    //Output part of main thread 

    outputFile<<"[Main]:"<<"All payments are completed"<<endl;
    outputFile<<"[Main]:"<<"Kevin: "<<kevin<<endl;
    outputFile<<"[Main]:"<<"Bob: "<<bob<<endl;
    outputFile<<"[Main]:"<<"Stuart: "<<stuart<<endl;
    outputFile<<"[Main]:"<<"Otto: "<<otto<<endl;
    outputFile<<"[Main]:"<<"Dave: "<<dave<<endl;

    // Close the file
    outputFile.close();
    return 0;

        
}

//gets customer data sleeps for the given amount, and  locks mutex related to chosen vending machine 
//passes the data to vending machine
void * customerThreadFunc(void * customerData){
    
    customerStruct *customer = (customerStruct *) customerData;
    int customerId = customer->customerId;
    int sleepTime = customer->sleepTime;
    int vendingMachine = customer->vendingmachine;
   
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));

    pthread_mutex_lock(&mutexesForCustomers[vendingMachine]);

    parameterForVTM[vendingMachine] = customerData;
 
    return 0;

}
//Gets vmt and checks the related part of parameterForVTM, If a customer comes ot it, 
// writes the necessary output to outputFile and then unlocks the mutex related to it.

void * vendingMachineThreadFunc(void* vmtData) {
    int*  vmtID = (int*) vmtData;
 
    while(true){
        
        if(parameterForVTM[*vmtID]!=NULL){

            pthread_mutex_lock(&mutexForlog);
            // outputFile<<"mutex"<<*vmtID<<"locked"<<endl;
            customerStruct *customer = (customerStruct *) parameterForVTM[*vmtID];
        
            int customerId = customer->customerId;
            string companyName  = customer->companyName;
            int amount = customer->amount;
            int machine = customer -> vendingmachine;
            outputFile<<"[vtm" <<*vmtID<<"]: "<<"Customer"<<customerId<< ","<<amount<<"TL,"<<companyName<<endl;
            parameterForVTM[*vmtID]=NULL;  
            
            unlock++;  
            
            pthread_mutex_unlock(&mutexForlog);
            
            pthread_mutex_unlock(&mutexesForCustomers[*vmtID]);
            
            //Calculation for companies
            if(companyName=="Bob"){
                pthread_mutex_lock(&mutexesForCompanies[0]);
                bob = bob + amount;
                pthread_mutex_unlock(&mutexesForCompanies[0]);
            }else if(companyName =="Stuart"){
                pthread_mutex_lock(&mutexesForCompanies[1]);
                stuart=stuart+amount;
                pthread_mutex_unlock(&mutexesForCompanies[1]);
            }
            else if(companyName =="Kevin"){
                pthread_mutex_lock(&mutexesForCompanies[2]);
                kevin=kevin+amount;
                pthread_mutex_unlock(&mutexesForCompanies[2]);
            }
            else if(companyName =="Dave"){
                pthread_mutex_lock(&mutexesForCompanies[3]);
                dave=dave+amount;
                pthread_mutex_unlock(&mutexesForCompanies[3]);

            }
            else if(companyName =="Otto"){
                pthread_mutex_lock(&mutexesForCompanies[4]);
                otto=otto+amount;
                pthread_mutex_unlock(&mutexesForCompanies[4]);
                
            }
        }   
     
        // counter for exit
        if(unlock>= noOfCustomers){
            break;
        }
       
     }

    return 0;

}
