#include <bits/stdc++.h>
using namespace std;

class AsyncExecuter{
    public:
        virtual void execute(function<void()> callback){
            thread([this, callback]() {
                
                // simulate asynchronous work
               this_thread::sleep_for(chrono::seconds(2));
               
               callback();
            }).detach();
        }
};

class SynchronousExecutor : public AsyncExecuter{
    private:
        mutex mu;
        condition_variable cv;
        bool taskCompleted;
        
    public:
        SynchronousExecutor() : taskCompleted(false) {}
        
        void execute(function<void()>callback) override {
            {
                unique_lock<mutex> lock(mu);
                
                AsyncExecuter:: execute([this, callback](){
                    callback();
                    
                    taskCompleted = true;
                    
                    cv.notify_one();
                });
                
                cv.wait(lock, [this]{return taskCompleted;});
                taskCompleted = false;
            }
        }
};

int main()
{
    SynchronousExecutor synchronousExecutor;
    vector<thread>threads;
    thread t([&](){
        synchronousExecutor.execute([](){
        cout<<"Async task 1 completed...\n"; 
        });
    });
    threads.push_back(move(t));
    thread t1([&](){
        synchronousExecutor.execute([](){
        cout<<"Async task 2 completed...\n"; 
        });
    });
    threads.push_back(move(t1));
    
    // synchronousExecutor.execute([](){
    //   cout<<"Async task 3 completed...\n"; 
    // });
    
    threads[0].join();
    threads[1].join();
    
    cout<<"Main thread continues......\n";
    
    return 0;
}
