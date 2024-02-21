#include <bits/stdc++.h>
using namespace std;

class BarberShop {
private:
    mutex mu;
    condition_variable cv;
    queue<int> waitingCustomers;
    int availableChairs;
    bool barberSleeping;

public:
    BarberShop(int n) : availableChairs(n), barberSleeping(true) {}

    void customerArrives(int id) {
        unique_lock<mutex> lock(mu);

        if (availableChairs > 0) {
            cout << "Customer " << id << " takes a seat.\n";
            waitingCustomers.push(id);
            availableChairs--;

            if (barberSleeping) {
                cout << "Barber wakes up by Customer " << id << ".\n";
                barberSleeping = false;
                cv.notify_one();
            }
        } else {
            cout << "Customer " << id << " leaves, no available chairs.\n";
        }
    }

    void barber() {
        unique_lock<mutex> lock(mu);

        while (true) {
            if (!waitingCustomers.empty()) {
                int customerId = waitingCustomers.front();
                waitingCustomers.pop();
                availableChairs++;
                lock.unlock();

                cout << "Barber serves Customer " << customerId << ".\n";

                this_thread::sleep_for(chrono::seconds(1));  // Simulating haircut

                lock.lock();
            } else {
                cout << "Barber sleeps.\n";
                barberSleeping = true;
                cv.wait(lock);
            }
        }
    }
};

int main() {
    BarberShop shop(5);  // Number of chairs

    thread barberThread(&BarberShop::barber, &shop);

    // Simulate customers arriving
    vector<thread> customerThreads;
    for (int i = 1; i <= 10; ++i) {
        customerThreads.emplace_back(&BarberShop::customerArrives, &shop, i);
        this_thread::sleep_for(chrono::seconds(1));  // Customers arriving at different times
    }

    for (auto& thread : customerThreads) {
        thread.join();
    }

    barberThread.join();

    return 0;
}
