#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

int customers;
int chairs;

mutex waitroomMutex;
mutex barberMutex;
mutex activityMutex;

void custThread(int custNum) {

    //usinfg mutexes
    {
        lock_guard<mutex> lock(activityMutex);
        cout << "Customer " << custNum << " leaving for barber shop" << endl;
    }

    {
        lock_guard<mutex> lock(activityMutex);
        cout << "Customer " << custNum << " arrived at barber shop" << endl;
    }

    unique_lock<mutex> waitroomLock(waitroomMutex);
    {
        lock_guard<mutex> lock(activityMutex);
        cout << "Customer " << custNum << " entering waiting room" << endl;
    }

    if (chairs > 0) {
        chairs--;
        waitroomLock.unlock();
    } else {
        waitroomLock.unlock();
        {
            lock_guard<mutex> lock(activityMutex);
            cout << "Waiting room full. Customer " << custNum << " leaving" << endl;
        }
        return;
    }

    barberMutex.lock();
    {
        lock_guard<mutex> lock(activityMutex);
        cout << "Customer " << custNum << " waking the barber" << endl;
    }

    {
        lock_guard<mutex> lock(activityMutex);
        cout << "The barber is cutting hair" << endl;
    }

    // Simulating hair cutting process
    this_thread::sleep_for(chrono::seconds(2));

    {
        lock_guard<mutex> lock(activityMutex);
        cout << "The barber has finished cutting hair" << endl;
    }

    barberMutex.unlock();

    {
        lock_guard<mutex> lock(activityMutex);
        cout << "Customer " << custNum << " leaving the barber shop" << endl;
    }
}

int main() {
    cout << "Maximum number of customers can only be 25 ";
    cout << "Enter number of customers and chairs." << endl;
    cin >> customers >> chairs;

    while (cin.fail() || customers < 1 || customers > 25 || chairs < 1) {
        cout << "Iput must be two integers separated by a space, and the number of customers should be between 1 and 25." << endl;
        cin.clear();
        cin.ignore(256, '\n');
        cin >> customers >> chairs;
    }

    cout << "A solution to the sleeping barber problem ussing mutexes" << endl;

    vector<thread> threads;
    threads.reserve(customers);

    cout << "The barber is sleeping" << endl;

    for (int n = 0; n < customers; ++n) {
        threads.emplace_back(custThread, n);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    cout << "The barber is going home for the day." << endl;

    return 0;
}
