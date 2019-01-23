#ifndef BUF_Q_HPP
#define BUF_Q_HPP
#include <cassert>
#include <iostream>
#include <memory>
#include <deque>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <initializer_list>
#include <atomic>
template <typename T>
class BufQ{
public:
    std::deque<std::shared_ptr<T>> filled_q;
    std::deque<std::shared_ptr<T>> unfilled_q;
    std::shared_ptr<T> proc_buf=nullptr;
    std::mutex mx;
    std::condition_variable cv;
    std::atomic_bool waiting;
public:
    BufQ(const std::initializer_list<std::shared_ptr<T>>& data)
    :unfilled_q{data}, waiting(false)
    {
    }

public:
    std::shared_ptr<T> fetch(){
        using namespace std::chrono_literals;
        waiting.store(true);
        std::unique_lock<std::mutex> lk(mx);  
        cv.wait(lk, [&,this]{
            return !this->filled_q.empty();
        }    
        );
        waiting.store(false);
        if(proc_buf!=nullptr){
            unfilled_q.push_back(proc_buf);
        }
        proc_buf=filled_q.front();
        filled_q.pop_front();
        
        return proc_buf;
    }

    template<typename F>
    void write(F&& func){
        while(waiting.load()&&!filled_q.empty())
        {
            cv.notify_one();
        }
        using namespace std::chrono_literals;
        {
            std::lock_guard<std::mutex> lk(mx);
            
            std::cout<<"entering write"<<std::endl;

            if(unfilled_q.empty()){
                unfilled_q.push_back(filled_q.front());
                filled_q.pop_front();
            }
        }
        func(unfilled_q.front());
        {
            std::lock_guard<std::mutex> lock(mx);
            filled_q.push_back(unfilled_q.front());
            unfilled_q.pop_front();
        }
        cv.notify_one();
    }
};

#endif
