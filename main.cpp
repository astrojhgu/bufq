#include "bufq.hpp"
#include <memory>
#include <vector>
#include <thread>
#include <iostream>
using namespace std::chrono_literals;

int main(int argc,char* argv[]){
    BufQ<int> q{std::make_shared<int>(4), std::make_shared<int>(0)};

    int cnt=0;
    std::thread th([&](){
        while(1){
            q.write([&](auto x){
                std::this_thread::sleep_for(1s);
                *x=cnt++;
                //std::cout<<":"<<*x<<std::endl;
            });
            std::this_thread::yield();
            //std::this_thread::sleep_for(0.1s);
        }
    });

    while(1){
        //std::this_thread::sleep_for(2.5s);
        auto a=(*q.fetch());
        std::cout<<"main:"<<a<<std::endl;
    }


    th.join();
    return 0;
}