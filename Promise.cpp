#include <bits/stdc++.h>
using namespace std;

template <typename T>
class Promise{
  private:
    enum class State {PENDING,FULFILED,REJECTED};
    promise<T> promise_;
    State state;
    mutex mu;
    condition_variable cv;
  public:
    Promise(): state(State::PENDING){}
    shared_future<T> get_future(){
        return promise_.get_future();
    }
    
    void resolve(const T& result){
        {
            lock_guard<mutex>lock(mu);
            if(state == State::PENDING){
                promise_.set_value(result);
                state = State::FULFILED;
            }
        }
        cout<<"Resolved\n";
        cv.notify_all();
    }
    
    void reject(const exception& error){
        {
            lock_guard<mutex>lock(mu);
            if(state == State::PENDING){
                promise_.set_exception(make_exception_ptr(error));
                state = State::REJECTED;
            }
        }
        cv.notify_all();
    }
    
    template <typename Function>
    void then(Function&& onFulfiled){
        unique_lock<mutex>lock(mu);
        cv.wait(lock,[this]{return state != State::PENDING;});
        
        if(state == State::FULFILED){
            onFulfiled(promise_.get_future().get());
        }
        else{
            // Handle rejection
        }
    }
    
};

int main()
{
    Promise<int> promise_;
    thread([&promise_](){
        this_thread::sleep_for(chrono::seconds(2));
        promise_.resolve(42);
    }).detach();
    
    promise_.then([](int result){
       cout<<"Result: "<<result<<"\n"; 
    });
    
    this_thread::sleep_for(chrono::seconds(1));
    
    return 0;
}
