#include <bits/stdc++.h>
using namespace std;

class TaskScheduler{
    private:
        struct Task{
            function<void()> func;
            int clientId;
        };
        queue<Task> taskQueue;
        mutex mu;
        condition_variable cv;
        
    public:
        void scheduleTask(int clientId, function<void()> taskFunction){
            unique_lock<mutex> lock(mu);
            
            taskQueue.push({taskFunction,clientId});
            cv.notify_one();
        }
        
        void taskWorker(){
            unique_lock<mutex> lock(mu);
            cv.wait(lock, [this]{ return !taskQueue.empty();});
            
            Task task = taskQueue.front();
            taskQueue.pop();
            
            lock.unlock();
            
            cout<<"Executing the Task: "<<task.clientId<<"\n";
            task.func();
        }
};

int main()
{
    TaskScheduler scheduler;
    
    thread worker1(&TaskScheduler::taskWorker, &scheduler);
    thread worker2(&TaskScheduler::taskWorker, &scheduler);

    // Schedule tasks for clients
    scheduler.scheduleTask(1, []() { this_thread::sleep_for(chrono::seconds(2)); std::cout << "Task 1 executed\n"; });
    scheduler.scheduleTask(2, []() { this_thread::sleep_for(chrono::seconds(2)); std::cout << "Task 2 executed\n"; });

    // Wait for workers to finish (in a real application, this might be an infinite loop)
    worker1.join();
    worker2.join();
    
    return 0;
}
