#include "iostream"
#include "pthread.h"
#include <iomanip>
#include <limits>
#include <ctime>
#include "thread"

bool is_noticed = false;
bool is_ivanov_busy = true;
bool is_petrov_busy = false;
bool petrov_come_to_car = false;
int count = 0;
int pripas_on_point = 0;
int ivStart=7;
int ivFinish=8;
int petStart=3;
int petFinish=4;

pthread_t ivanov_thread;
pthread_t petrov_thread;
pthread_t nech_thread;
pthread_t notice_thread;

pthread_mutex_t mutex;
pthread_cond_t cond;


double generateTime(int min_seconds, int max_seconds) {
    double rnd_val = double(rand())/RAND_MAX;
    rnd_val *= (max_seconds-min_seconds);
    rnd_val += min_seconds;
    return rnd_val;
}

void* ivanovTask(void* params) {
    while (!is_noticed) {
        is_ivanov_busy = true;
        puts("Иванов пошел на склад за боеприпасами");
        std::this_thread::sleep_for(std::chrono::seconds(int(generateTime(ivStart, ivFinish))));
        pripas_on_point++;
        puts("Иванов принес боеприпасы на точку.");
        pthread_cond_signal(&cond);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        is_ivanov_busy = false;

    }
    return NULL;
}

void* petrovTask(void* params) {
    while (!is_noticed) {
        pthread_mutex_lock(&mutex);
        while (pripas_on_point <= 0) {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);

        std::this_thread::sleep_for(std::chrono::seconds(1));
        is_petrov_busy = true;
        puts("Петров понес боеприпасы в машину.");
        pripas_on_point--;
        std::this_thread::sleep_for(std::chrono::seconds(int(generateTime(petStart, petFinish))));
        petrov_come_to_car = true;
        pthread_cond_signal(&cond);
        puts("Петров вернулся на точку и ждет Иванова");
        is_petrov_busy = false;
    }

    return NULL;
}

void* nechTask(void* params) {
    while (!is_noticed) {

        pthread_mutex_lock(&mutex);
        while (!petrov_come_to_car) {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);

        petrov_come_to_car = false;
        ++count;
        puts("Боеприпасы были доставлены в машину");
    }

    return NULL;
}

void* noticeTask(void* params) {
    std::this_thread::sleep_for(std::chrono::seconds(int(generateTime(std::max(ivFinish, petFinish)+15, std::max(ivFinish, petFinish)+20))));
    is_noticed = true;
    return NULL;
}

void start() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&ivanov_thread, NULL, ivanovTask, NULL);
    pthread_create(&petrov_thread, NULL, petrovTask, NULL);
    pthread_create(&nech_thread, NULL, nechTask, NULL);
    pthread_create(&notice_thread, NULL, noticeTask, NULL);

    pthread_join(ivanov_thread, NULL);
    pthread_join(petrov_thread, NULL);
    pthread_join(nech_thread, NULL);
    pthread_join(notice_thread, NULL);
}

int main() {
    std::cout << "Введите диапазон времени для Иванова: ";
    std::cin >> ivStart >> ivFinish;
    std::cout << "Введите диапазон времени для Петрова: ";
    std::cin >> petStart >> petFinish;

    start();

    std::cout << "Всего было доставлено " << count << " боеприпасов в машину.";
}