// Includes
#include "iostream"
#include "pthread.h"
#include <iomanip>
#include <limits>
#include <ctime>
#include "thread"

// Variables
bool is_noticed = false;
bool is_ivanov_busy = true;
bool is_petrov_busy = false;
bool petrov_come_to_car = false;
int count = 0;
int pripas_on_point = 0;
int ivStart=7;                           // Start values
int ivFinish=8;
int petStart=3;
int petFinish=4;

// Posix threads
pthread_t ivanov_thread;
pthread_t petrov_thread;
pthread_t nech_thread;
pthread_t notice_thread;

// General mutex and conditional
pthread_mutex_t mutex;
pthread_cond_t cond;

// Special method to generate time in range
double generateTime(int min_seconds, int max_seconds) {
    double rnd_val = double(rand())/RAND_MAX;
    rnd_val *= (max_seconds-min_seconds);
    rnd_val += min_seconds;
    return rnd_val;
}

// Function for Ivanov thread
void* ivanovTask(void* params) {
    while (!is_noticed) {           // While not noticed
        is_ivanov_busy = true;
        puts("Иванов пошел на склад за боеприпасами");
        // Action simulation
        std::this_thread::sleep_for(std::chrono::seconds(int(generateTime(ivStart, ivFinish))));
        pripas_on_point++;
        puts("Иванов принес боеприпасы на точку.");
        pthread_cond_signal(&cond);                 // Signal for Petrov thread function
        std::this_thread::sleep_for(std::chrono::seconds(1));
        is_ivanov_busy = false;

    }
    return NULL;
}

// Function for Petrov thread
void* petrovTask(void* params) {
    while (!is_noticed) {           // While not noticed
        // If does't ammunitions if point - wait for signal
        pthread_mutex_lock(&mutex);
        while (pripas_on_point <= 0) {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);

        // Carry ammunition
        std::this_thread::sleep_for(std::chrono::seconds(1));
        is_petrov_busy = true;
        puts("Петров понес боеприпасы в машину.");
        pripas_on_point--;
        // Action simulation
        std::this_thread::sleep_for(std::chrono::seconds(int(generateTime(petStart, petFinish))));
        petrov_come_to_car = true;
        pthread_cond_signal(&cond);
        puts("Петров вернулся на точку и ждет Иванова");
        is_petrov_busy = false;
    }

    return NULL;
}

// Function for Nicheporook thread
void* nechTask(void* params) {
    while (!is_noticed) {   // While not noticed

        // Wait for signal, that Petrov here
        pthread_mutex_lock(&mutex);
        while (!petrov_come_to_car) {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);

        // If signal - add count
        petrov_come_to_car = false;
        ++count;
        puts("Боеприпасы были доставлены в машину");
    }

    return NULL;
}

// Special thread for notice
void* noticeTask(void* params) {
    std::this_thread::sleep_for(std::chrono::seconds(int(generateTime(std::max(ivFinish, petFinish)+15, std::max(ivFinish, petFinish)+20))));
    is_noticed = true;
    return NULL;
}

// Do all
void start() {
    // Init mutex/conditional
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Start threads
    pthread_create(&ivanov_thread, NULL, ivanovTask, NULL);
    pthread_create(&petrov_thread, NULL, petrovTask, NULL);
    pthread_create(&nech_thread, NULL, nechTask, NULL);
    pthread_create(&notice_thread, NULL, noticeTask, NULL);

    // Wait threads
    pthread_join(ivanov_thread, NULL);
    pthread_join(petrov_thread, NULL);
    pthread_join(nech_thread, NULL);
    pthread_join(notice_thread, NULL);
}

int main() {
    // Enter user data
    std::cout << "Введите диапазон времени для Иванова: ";
    std::cin >> ivStart >> ivFinish;
    std::cout << "Введите диапазон времени для Петрова: ";
    std::cin >> petStart >> petFinish;

    // Do all
    start();

    // Result
    std::cout << "Всего было доставлено " << count << " боеприпасов в машину.";
}
