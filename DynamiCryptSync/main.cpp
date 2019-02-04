/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: mrarchinton
 *
 * Created on 31 January 2019, 13:56
 */

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>



using namespace std;

// timer 1 #########################################################################
void TimerSyncronisly(boost::asio::io_context *io){
    boost::asio::steady_timer t(*io, boost::asio::chrono::seconds(5));
    t.wait();
    std::cout << "TimerSyncronisly done" << std::endl;
}
// end of timer 1 ##################################################################


// timer 2 #########################################################################
void timerAsynchronouslyCallback(const boost::system::error_code& /*e*/){
  std::cout << "timerAsynchronouslyCallback" << std::endl;
}

void timerAsynchronously(boost::asio::io_context *io){
    boost::asio::steady_timer t(*io, boost::asio::chrono::seconds(5));
    cout << "timerAsynchronously: test 0" << endl;
    t.async_wait(&timerAsynchronouslyCallback);
    cout << "timerAsynchronously: test 1" << endl;
    io->run();
    cout << "timerAsynchronously: test 2" << endl; // this runs after callback
}
// end of timer 2 ##################################################################


// timer 3 #########################################################################
void timerAsynchronouslyBindingCallback(const boost::system::error_code& /*e*/, boost::asio::steady_timer* t, int* count){
    if (*count < 5) {
        std::cout << *count << std::endl;
        ++(*count);
        t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));
        t->async_wait(boost::bind(timerAsynchronouslyBindingCallback, boost::asio::placeholders::error, t, count));
    }
}

void timerAsynchronouslyBinding(boost::asio::io_context *io){
    int count = 0;
    boost::asio::steady_timer t(*io, boost::asio::chrono::seconds(1));
    t.async_wait(boost::bind(timerAsynchronouslyBindingCallback, boost::asio::placeholders::error, &t, &count));
    cout << "timerAsynchronouslyBinding: test 0" << endl;
    io->run();
    cout << "timerAsynchronouslyBinding: test 1" << endl;
    std::cout << "Final count is " << count << std::endl;
}
// end of timer 3 ###################################################################




// timer 4 ##########################################################################
class printer {
    public:
        printer(boost::asio::io_context& io) : timer_(io, boost::asio::chrono::seconds(1)), count_(0){
            timer_.async_wait(boost::bind(&printer::print, this));
        }    

        ~printer(){
            std::cout << "Final count is " << count_ << std::endl;
        }

        void print(){
            if (count_ < 5){
                std::cout << count_ << std::endl;
                ++count_;

                timer_.expires_at(timer_.expiry() + boost::asio::chrono::seconds(1));
                timer_.async_wait(boost::bind(&printer::print, this));
           }
         }

    private:
        boost::asio::steady_timer timer_;
        int count_;
};

void timerAsynchronouslyBindingClassMember(boost::asio::io_context *io){
    printer p(*io);
    io->run();

}
// end of timer 4 ###################################################################



// timer 5 ##########################################################################
/*
 The previous four timers avoided the issue of handler synchronisation by calling the io_context::run() 
 function from one thread only. asio library provides a guarantee that callback handlers will only be called 
 from threads that are currently calling io_context::run(). Consequently, calling io_context::run() from only 
 one thread ensures that callback handlers cannot run concurrently.
 * 
 an alternative approach is to have a pool of threads calling io_context::run(). 
 However, as this allows handlers to execute concurrently, we need a method of synchronisation when handlers 
 might be accessing a shared, thread-unsafe resource.
 */

// note compile with this if using boost threads -lboost_thread
class printerDouble{
    public:
        printerDouble(boost::asio::io_context& io) : strand_(io), timer1_(io, boost::asio::chrono::seconds(1)), timer2_(io, boost::asio::chrono::seconds(1)), count_(0) {
            
            timer1_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&printerDouble::print1, this)));

            timer2_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&printerDouble::print2, this)));
        }

        ~printerDouble()
        {
            std::cout << "Final count is " << count_ << std::endl;
        }
        
        
        void print1()
        {
            if (count_ < 10)
            {
                std::cout << "Timer 1: " << count_ << std::endl;
                ++count_;

                timer1_.expires_at(timer1_.expiry() + boost::asio::chrono::seconds(1));

                timer1_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&printerDouble::print1, this)));
            }
        }

        void print2()
        {
            if (count_ < 10)
            {
                std::cout << "Timer 2: " << count_ << std::endl;
                ++count_;

                timer2_.expires_at(timer2_.expiry() + boost::asio::chrono::seconds(1));

                timer2_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&printerDouble::print2, this)));
            }
        }

    private:
        boost::asio::io_context::strand strand_;
        boost::asio::steady_timer timer1_;
        boost::asio::steady_timer timer2_;
        int count_;
};
  
void timerAsynchronouslyBindingClassMemberDouble(boost::asio::io_context *io){
    printerDouble p(*io);
    boost::thread t(boost::bind(&boost::asio::io_context::run, io));
    io->run();
    t.join();

}
  
  
// end of timer 5 ###################################################################



int main(int argc, char** argv) {
    boost::asio::io_context io;
    //TimerSyncronisly(&io);
    //timerAsynchronously(&io);
    //timerAsynchronouslyBinding(&io);
    //timerAsynchronouslyBindingClassMember(&io);
    timerAsynchronouslyBindingClassMemberDouble(&io);
    
    return 0;
}

