#include <bits/stdc++.h>
using namespace std;

class Client{
  public:
    virtual void init() = 0;
    virtual void request() = 0;
    virtual void close() = 0;
    virtual ~Client() = default;
};

class ClientWrapper: public Client{
    private:
        mutex initMutex;
        mutex requestMutex;
        mutex closeMutex;
        bool isInitInProgress = false;
        bool isConnectionOpen = false;
        condition_variable initCondition;
        int requestNumber = 0;
        
    public:
        void init() override{
            unique_lock<mutex>lock(initMutex);
            
            if(isInitInProgress){
                // Another thread is already using init. wait.
                cout<<"Initializing is in progress. Waiting ...\n";
                initCondition.wait(lock,[this]{return !isInitInProgress;});
            }
            //Start Initialization;
            isInitInProgress = true;
            
            // Simulate Intialization process
            this_thread::sleep_for(chrono::seconds(2));
            isConnectionOpen = true;
            
            cout<<"Intialization complete.\n";
            isInitInProgress = false;
            lock.unlock();
            initCondition.notify_all();
        }
        
        void request() override{
            unique_lock<mutex> lock(requestMutex);
            
            if(!isConnectionOpen){
                cout<<"Error: Connection is not open. Call init() first.\n";
                return;
            }
            
            //Simulate request processing.
            cout<<"Processing Request: "<<requestNumber<<"\n";
            this_thread::sleep_for(chrono::seconds(1));
            cout<<"Request Processed "<<requestNumber<<"\n";
            requestNumber++;
        }
        
        void close() override{
            unique_lock<mutex>lock(closeMutex);
            
            if(!isConnectionOpen){
                cout<<"Connection is already closed.\n";
                return;
            }
            
            //Simulating close Connection
            cout<<"Closing  Connection...\n";
            isConnectionOpen = false;
            cout<<"Connection Closed.\n";
        }
};

int main()
{
    ClientWrapper client;
    
    thread t1([&](){
       client.init();
       client.request();
       client.close();
    });
    
    thread t2([&]() {
        client.request();
        client.close();
    });

    thread t3([&]() {
        client.init();
        client.request();
        client.close();
    });
    
    t1.join();
    t2.join();
    t3.join();
    return 0;
}
