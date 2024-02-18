#include <bits/stdc++.h>
using namespace std;

class RateLimiter{
    private:
        int capacity;
        const int ratePerSecond;
        int tokens;
        chrono::steady_clock::time_point lastRefillTime;
        mutex mu;
        condition_variable cv;
        
        void refillTokens(){
            chrono::steady_clock::time_point now = chrono::steady_clock::now();
            int elapsedSeconds = chrono::duration_cast<chrono::seconds>(now-lastRefillTime).count();
            
            if(elapsedSeconds>0){
                int newTokens = min(capacity,tokens+elapsedSeconds*ratePerSecond);
                tokens = newTokens;
                lastRefillTime = now;
                cv.notify_all();
            }
        }
    public:
        RateLimiter(int capacity, int ratePerSecond): capacity(capacity), ratePerSecond(ratePerSecond), tokens(capacity), lastRefillTime(chrono::steady_clock::now()) {}
        
        void consume(int tokenRequested){
            unique_lock<mutex>lock(mu);
            refillTokens();
            if(tokens<tokenRequested){
                cout<<"Not Enough Tokens. Throttling...\n";
                return;
            }
            
            cv.wait(lock, [this, tokenRequested]{return tokens>=tokenRequested;});
            
            
            tokens -= tokenRequested;
            cout<<"Tokens Consumed: "<<tokenRequested<<", Remaining Tokens: "<<tokens<<"\n";
        }
};

int main()
{
    RateLimiter rateLimiter(10, 2); // Capacity: 10, Rate: 2 tokens/second

    // Simulating token consumption
    vector<thread>threads;
    for (int i = 0; i < 15; ++i) {
        this_thread::sleep_for(std::chrono::seconds(2));
        thread t([&] { rateLimiter.consume(5); });
        threads.push_back(move(t));
    }
    for(auto &t:threads){
        t.join();
    }
    
    return 0;
}
