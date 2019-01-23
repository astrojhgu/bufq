#include "bufq.hpp"
#include <memory>
#include <vector>
#include <thread>
#include <iostream>
using namespace std::chrono_literals;

int main(int argc,char* argv[]){

    //generate a buffer queue with two buffer blocks
    BufQ<int> q{std::make_shared<int>(4), std::make_shared<int>(0)};

    int cnt=0;
    //spawn a thread which is responsible to fill 
    //valid data into the buffers.
    std::thread th([&](){
        while(1){
            q.write([&](auto x){
                //sleep for a while to emulate
                //expensive computings.
                std::this_thread::sleep_for(1s);
                *x=cnt++;
            });
        }
    });

    //main thread that consumes the buffer
    while(1){
        auto a=(*q.fetch());
        std::cout<<"main:"<<a<<std::endl;
    }
    
    th.join();
    return 0;
}