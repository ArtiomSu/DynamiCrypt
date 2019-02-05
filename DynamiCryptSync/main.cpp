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
#include <boost/array.hpp>
#include <ctime>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>



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


// synchronous TCP client and server ################################################
void tcpDayTimeClient(boost::asio::io_context *io, int argc, char** argv){
    try{
        /*
        if (argc != 2){
            std::cerr << "Usage: client <host>" << std::endl;
            //return 1;
        }
         */
        
        boost::asio::ip::tcp::resolver resolver(*io); // used to turn the server name into a TCP endpoint.
        // A resolver takes a query object and turns it into a list of endpoints. We construct a query using the name of the server and the name of the service, in this case "daytime".
        // The list of endpoints is returned using an iterator of type ip::tcp::resolver::iterator. (Note that a default constructed ip::tcp::resolver::iterator object can be used as an end iterator.)
        //Now we create and connect the socket. The list of endpoints obtained above may contain both IPv4 and IPv6 endpoints, so we need to try each of them until we find one that works. 
        //This keeps the client program independent of a specific IP version. The boost::asio::connect() function does this for us automatically.

        //The connection is open. All we need to do now is read the response from the daytime service.
        //We use a boost::array to hold the received data. The boost::asio::buffer() function automatically determines the size of the array to help prevent buffer overruns. Instead of a boost::array, we could have used a char [] or std::vector.
        //When the server closes the connection, the ip::tcp::socket::read_some() function will exit with the boost::asio::error::eof error, which is how we know to exit the loop.
        boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1:9080", "daytime");
        boost::asio::ip::tcp::socket socket(*io);
        boost::asio::connect(socket, endpoints);
        for (;;) {
              boost::array<char, 128> buf;
              boost::system::error_code error;

              size_t len = socket.read_some(boost::asio::buffer(buf), error);

              if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
              else if (error)
                throw boost::system::system_error(error); // Some other error.

              std::cout.write(buf.data(), len);
            }
    }    
    catch(std::exception& e){
        std::cerr << e.what() << std::endl;
    }    
}

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

void tcpDayTimeServer(boost::asio::io_context *io){
    // can use netcat 127.0.0.1 9080 as client
    // to brute force run this in a few terminals for (( ; ; )); do netcat 127.0.0.1 9080 -w 1; done
    try{
        cout << "tcpDayTimeServer test 0" << endl;                                                               //port is 9080
        boost::asio::ip::tcp::acceptor acceptor(*io, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), 9080)); // A ip::tcp::acceptor object needs to be created to listen for new connections. It is initialised to listen on TCP port 13, for IP version 4.
        cout << "tcpDayTimeServer test 1" << endl;  
        for (;;){
            cout << "tcpDayTimeServer test 2" << endl;  // test 0-2 run straight away with no client connected
            boost::asio::ip::tcp::socket socket(*io);
            acceptor.accept(socket);
            cout << "tcpDayTimeServer test 3" << endl;  
            std::string message = make_daytime_string();

            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
            cout << "tcpDayTimeServer test 4" << endl;  // after client connected test 3,4 and back to 2 are run
            cout << "stats: " <<  socket.remote_endpoint().address().to_string() << "\t" << socket.remote_endpoint().port() << "\t" << socket.remote_endpoint().size() << "\t" << socket.remote_endpoint().capacity() << endl;
            }
    }    
    catch(std::exception& e){
        std::cerr << e.what() << std::endl;
    }    
}
// end of synchronous TCP client and server ################################################


// Asynchronous TCP server #################################################################

class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
    public:
            typedef boost::shared_ptr<tcp_connection> pointer;

            static pointer create(boost::asio::io_context& io_context){
                return pointer(new tcp_connection(io_context));
            }

            boost::asio::ip::tcp::socket& socket(){
                return socket_;
            }

            void start(){
                message_ = make_daytime_string();
                boost::asio::async_write(socket_, boost::asio::buffer(message_), boost::bind(&tcp_connection::handle_write, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
                // if boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred are not used in this->handle_write then it is possible to only do this:
                // boost::asio::async_write(socket_, boost::asio::buffer(message_), boost::bind(&tcp_connection::handle_write, shared_from_this()));
        }

    private:
        tcp_connection(boost::asio::io_context& io_context) : socket_(io_context){

        }

        void handle_write(const boost::system::error_code& error, size_t bytes_transferred){
            cout << "Bytes transfered = " << bytes_transferred << endl;
            //socket_.close(); // doesn't really do anything
        }

        boost::asio::ip::tcp::socket socket_;
        std::string message_;
};

class tcp_server{
public:
    tcp_server(boost::asio::io_context& io_context) : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 9080)){
        start_accept();
    }

private:
    
    void start_accept(){
        tcp_connection::pointer new_connection = tcp_connection::create(acceptor_.get_executor().context());

        acceptor_.async_accept(new_connection->socket(), boost::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));
    }
    
    void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error) {
        if (!error){
            new_connection->start();
        }

        start_accept();
    }
    
    boost::asio::ip::tcp::acceptor acceptor_;
    
};




   /* void handle_write()
      {
      }

    boost::asio::async_write(socket_, boost::asio::buffer(message_),boost::bind(&tcp_connection::handle_write, shared_from_this()));
*/

void tcpDayTimeServerAsync(boost::asio::io_context *io){
    try{
        tcp_server server(*io);
        io->run();
      }
      catch (std::exception& e)
      {
        std::cerr << e.what() << std::endl;
      }
}

// End of Asynchronous TCP server ##########################################################




// #######################################################################################################################################################################################
// different totorial ####################################################################################################################################################################
// #######################################################################################################################################################################################
/* https://www.gamedev.net/blogs/entry/2249317-a-guide-to-getting-started-with-boostasio/ */

// simple thread ###########################################################################

void WorkerThread(boost::shared_ptr< boost::asio::io_service > io_service){
    std::cout << "Thread Start\n";
    io_service->run();
    std::cout << "Thread Finish\n";
}

void example_2e(){
    
        boost::shared_ptr< boost::asio::io_service > io_service( new boost::asio::io_service);
        boost::shared_ptr< boost::asio::io_service::work > work( new boost::asio::io_service::work( *io_service ));

	std::cout << "Press [return] to exit." << std::endl;

	boost::thread_group worker_threads;
	for( int x = 0; x < 4; ++x ){
            worker_threads.create_thread( boost::bind( &WorkerThread, io_service ) );
	}

	std::cin.get();

	io_service->stop();

	worker_threads.join_all();
}
// end of simple thread ####################################################################

// bind and execute ########################################################################

void bindedfunction(int i, float f){
    std::cout << "i: " << i << std::endl;
    std::cout << "f: " << f << std::endl;
}

void example_2c(){
    boost::bind( &bindedfunction, 42, 3.14f )(); // bind the parameters along with the function, match the signature exactly preventing a ton of hard to read errors
    // if you do this
    // boost::bind( &bindedfunction, 42, 3.14f ); // the function wont execute since this is a function invocation object.
}

// end of bind and execute #################################################################

// bind and execute class function #########################################################
class BindedClass {
public:
	void F3( int i, float f )
	{
		std::cout << "i: " << i << std::endl;
		std::cout << "f: " << f << std::endl;
	}
};

void example_2d(){
        BindedClass c; // must pass the address of the class object to invoke after the class function
	boost::bind( &BindedClass::F3, &c, 42, 3.14f )();
}



// end of bind and execute class function ##################################################


// threads with mutex ######################################################################
boost::mutex global_stream_lock;

void WorkerThread2( boost::shared_ptr< boost::asio::io_service > io_service )
{
	global_stream_lock.lock();
	std::cout << "[" << boost::this_thread::get_id() << 
		"] Thread Start" << std::endl;
	global_stream_lock.unlock();

	io_service->run();

	global_stream_lock.lock();
	std::cout << "[" << boost::this_thread::get_id() <<
		"] Thread Finish" << std::endl;
	global_stream_lock.unlock();
}

void example_2f(){
        boost::shared_ptr< boost::asio::io_service > io_service( new boost::asio::io_service );
	boost::shared_ptr< boost::asio::io_service::work > work( new boost::asio::io_service::work( *io_service ) );

	global_stream_lock.lock();
	std::cout << "[" << boost::this_thread::get_id() << "] Press [return] to exit." << std::endl;
	global_stream_lock.unlock();

	boost::thread_group worker_threads;
	for( int x = 0; x < 4; ++x ) {
            worker_threads.create_thread( boost::bind( &WorkerThread2, io_service ) );
	}

	std::cin.get();

	io_service->stop();

	worker_threads.join_all();
}

// end of threads with mutex ###############################################################


// fibonachi and threads ###################################################################
void WorkerThread3( boost::shared_ptr< boost::asio::io_service > io_service ) {
    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] Thread Start" << std::endl;
    global_stream_lock.unlock();

    io_service->run();

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] Thread Finish" << std::endl;
    global_stream_lock.unlock();
}

size_t fib( size_t n ) {
    if ( n <= 1 ){
        return n;
    }
    boost::this_thread::sleep( boost::posix_time::milliseconds( 1000 ) );
    return fib( n - 1 ) + fib( n - 2);
}

void CalculateFib( size_t n )
{
    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] Now calculating fib( " << n << " ) " << std::endl;
    global_stream_lock.unlock();

    size_t f = fib( n );

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] fib( " << n << " ) = " << f << std::endl;
    global_stream_lock.unlock();
}

void example_3a(){
    boost::shared_ptr< boost::asio::io_service > io_service( new boost::asio::io_service );
    
    boost::shared_ptr< boost::asio::io_service::work > work( new boost::asio::io_service::work( *io_service ) );

    global_stream_lock.lock();
    
    std::cout << "[" << boost::this_thread::get_id() << "] The program will exit when all work has finished." << std::endl;
    global_stream_lock.unlock();

    boost::thread_group worker_threads;
    for( int x = 0; x < 2; ++x ) {
        worker_threads.create_thread( 
            boost::bind( &WorkerThread3, io_service )
        );
    }
    //post 3 function objects to the io_service via the post function
    //since the current thread does not call the io_service run or poll function, dispatch() would also call the post function and not execute the code right away
    io_service->post( boost::bind( CalculateFib, 3 ) );
    io_service->post( boost::bind( CalculateFib, 4 ) );
    io_service->post( boost::bind( CalculateFib, 5 ) );

    //After we give the io_service work, through post, we reset the work object to signal once the work has been completed that we wish to exit. 
    work.reset();
    //Finally, we wait on all the threads to finish as we have with the join_all function
    worker_threads.join_all();
}

// note DISPATCH VS POST
// Dispatched events can execute from the current worker thread even if there are other pending events queued up. The posted events have to wait until the handler completes before being allowed to be executed


// end of fibonachi and threads ############################################################


// error handling 1 ######################################################################## 

void WorkerThread4( boost::shared_ptr< boost::asio::io_service > io_service )
{
    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] Thread Start" << std::endl;
    global_stream_lock.unlock();

    try{
        io_service->run();
    }
    catch( std::exception & ex )
    {
        global_stream_lock.lock();
        std::cout << "[" << boost::this_thread::get_id() << "] Exception: " << ex.what() << std::endl;
        global_stream_lock.unlock();
    }

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] Thread Finish" << std::endl;
    global_stream_lock.unlock();
}

void RaiseAnException( boost::shared_ptr< boost::asio::io_service > io_service )
{
    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] " << __FUNCTION__ << std::endl;
    global_stream_lock.unlock();

    io_service->post( boost::bind( &RaiseAnException, io_service ) );

    throw( std::runtime_error( "Oops!" ) );
}

void example_5a(){
    boost::shared_ptr< boost::asio::io_service > io_service( new boost::asio::io_service );
    boost::shared_ptr< boost::asio::io_service::work > work( new boost::asio::io_service::work( *io_service ) );

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] The program will exit when all work has finished." << std::endl;
    global_stream_lock.unlock();                                                                        // output looks like this
                                                                                                        // [7f433e794b80] The program will exit when all work has finished.
    boost::thread_group worker_threads;                                                                 // [7f433e791700] Thread Start
    for( int x = 0; x < 2; ++x )                                                                        // [7f433e791700] RaiseAnException
    {                                                                                                   // [7f433e791700] Exception: Oops!
        worker_threads.create_thread( boost::bind( &WorkerThread4, io_service ) );                      // [7f433e791700] Thread Finish
    }                                                                                                   // [7f433df90700] Thread Start
                                                                                                        // [7f433df90700] RaiseAnException
    io_service->post( boost::bind( &RaiseAnException, io_service ) );                                   // [7f433df90700] Exception: Oops!
    worker_threads.join_all();                                                                          // [7f433df90700] Thread Finish
    
}
// this way is not great since the program will exit pretty much straight away 
//because the exception propagated through the run function, so the worker threads exited. 
//Since all worker threads exited, the program is done since join_all returns. 
//Immediately we can see how this could lead to problems if we are not careful since worker threads could be taken out one by one until the system has none left.

// end of error handling 1 ################################################################# 


// error handling that solves the previous issue ###########################################
void WorkerThread5 ( boost::shared_ptr< boost::asio::io_service > io_service )
{
	global_stream_lock.lock();
	std::cout << "[" << boost::this_thread::get_id() << "] Thread Start" << std::endl;
	global_stream_lock.unlock();

	while( true ) {
            try
            {
                boost::system::error_code ec;
                io_service->run( ec );
                if( ec ) {
                    global_stream_lock.lock();
                    std::cout << "[" << boost::this_thread::get_id() << "] Error: " << ec << std::endl;
                    global_stream_lock.unlock();
                }
                break;
            }
            catch( std::exception & ex ) {
                    global_stream_lock.lock();
                    std::cout << "[" << boost::this_thread::get_id() << "] Exception: " << ex.what() << std::endl;
                    global_stream_lock.unlock();
            }
	}

	global_stream_lock.lock();
	std::cout << "[" << boost::this_thread::get_id() << "] Thread Finish" << std::endl;
	global_stream_lock.unlock();
}

void example_5c(){
    boost::shared_ptr< boost::asio::io_service > io_service( new boost::asio::io_service );
    boost::shared_ptr< boost::asio::io_service::work > work( new boost::asio::io_service::work( *io_service ) );

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] The program will exit when all work has finished." << std::endl;
    global_stream_lock.unlock();

    boost::thread_group worker_threads;
    for( int x = 0; x < 2; ++x ){
            worker_threads.create_thread( boost::bind( &WorkerThread5, io_service ) );
    }

    io_service->post( boost::bind( &RaiseAnException, io_service ) );

    worker_threads.join_all();
}
// Now, when an exception occurs, it is outputted and the worker thread goes back to handling work. 
// When the stop member function is called or the work object is destroyed, the run function no longer blocks as we have seen before, so the loop exits and then the thread finishes up. 
// If we were to use this concept on the exception example, we would see an infinite output of the events since we are always posting new events to the queue.

// end of error handling that solves the previous issue ####################################


// timer more complex one than before

void TimerHandler( const boost::system::error_code & error )
{
    if( error ) {
        global_stream_lock.lock();
        std::cout << "[" << boost::this_thread::get_id() << "] Error: " << error << std::endl;
        global_stream_lock.unlock();
    }
    else {
        global_stream_lock.lock();
        std::cout << "[" << boost::this_thread::get_id() << "] TimerHandler " << std::endl;
        global_stream_lock.unlock();
    }
}

void example_6a(){
    boost::shared_ptr< boost::asio::io_service > io_service( new boost::asio::io_service );
    boost::shared_ptr< boost::asio::io_service::work > work( new boost::asio::io_service::work( *io_service ) );

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] Press [return] to exit." << std::endl;
    global_stream_lock.unlock();

    boost::thread_group worker_threads;
    for( int x = 0; x < 2; ++x ) {
        worker_threads.create_thread( boost::bind( &WorkerThread5, io_service ) ); // reuse last worker thread
    }

    boost::asio::deadline_timer timer( *io_service );
    timer.expires_from_now( boost::posix_time::seconds( 5 ) );
    timer.async_wait( TimerHandler );

    std::cin.get();

    io_service->stop();

    worker_threads.join_all();

}

// end of timer more complex one than before ###############################################



// recurring timer #########################################################################

void TimerHandler1( const boost::system::error_code & error, boost::shared_ptr< boost::asio::deadline_timer > timer ) {
	if( error ) {
            global_stream_lock.lock();
            std::cout << "[" << boost::this_thread::get_id() << "] Error: " << error << std::endl;
            global_stream_lock.unlock();
	}
	else{ // this yoke essentially goes off every 5 seconds
            global_stream_lock.lock();
            std::cout << "[" << boost::this_thread::get_id() << "] TimerHandler " << std::endl;
            global_stream_lock.unlock();

            timer->expires_from_now( boost::posix_time::seconds( 5 ) );
            timer->async_wait( boost::bind( &TimerHandler1, _1, timer ) ); //The _1 parameter is an argument place holder. Since the TimerHandler function requires one parameter for the callback, we must reference this in the bind call. All in all, the _1 means "the first parameter, which will be supplied later"
        }
}

void example_6b(){
    boost::shared_ptr< boost::asio::io_service > io_service( new boost::asio::io_service );
    
    boost::shared_ptr< boost::asio::io_service::work > work( new boost::asio::io_service::work( *io_service ) );

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] Press [return] to exit." << std::endl;
    global_stream_lock.unlock();

    boost::thread_group worker_threads;
    for( int x = 0; x < 2; ++x )
    {
        worker_threads.create_thread( boost::bind( &WorkerThread5, io_service ) );
    }

    boost::shared_ptr< boost::asio::deadline_timer > timer( new boost::asio::deadline_timer( *io_service ) );
    timer->expires_from_now( boost::posix_time::seconds( 5 ) );
    timer->async_wait( boost::bind( &TimerHandler1, _1, timer ) );

    std::cin.get();

    io_service->stop();

    worker_threads.join_all();
}

// However, our timers will execute asynchronously so if we have more than one worker thread, it is possible that we execute a timer in one thread while we execute another event in another. 
// Let us assume the timer handler and the work handler use the same shared object so we now have a non-thread safe design. How can we ensure a timer does not execute concurrently with a work handler?

// By using a strand object, we can post work through the strand as well as wrap the timer handler to be dispatched through it. 
// As a result, we will get our serialized output and will not have to explicitly synchronize access to our shared object.

// end of recurring timer ##################################################################


// solution to problem stated above using strand ###########################################

void TimerHandler2( const boost::system::error_code & error, boost::shared_ptr< boost::asio::deadline_timer > timer, boost::shared_ptr< boost::asio::io_service::strand > strand){
    if( error ){
        global_stream_lock.lock();
        std::cout << "[" << boost::this_thread::get_id() << "] Error: " << error << std::endl;
        global_stream_lock.unlock();
    }
    else{
        std::cout << "[" << boost::this_thread::get_id() << "] TimerHandler " << std::endl;

        timer->expires_from_now( boost::posix_time::seconds( 1 ) );
        timer->async_wait( strand->wrap( boost::bind( &TimerHandler2, _1, timer, strand ) ) );
    }
}

void PrintNum( int x ) {
    std::cout << "[" << boost::this_thread::get_id() << "] x: " << x << std::endl;
    boost::this_thread::sleep( boost::posix_time::milliseconds( 1000 ) );
}

void example_6c(){
    boost::shared_ptr< boost::asio::io_service > io_service( new boost::asio::io_service );
    boost::shared_ptr< boost::asio::io_service::work > work( new boost::asio::io_service::work( *io_service ) );
    boost::shared_ptr< boost::asio::io_service::strand > strand( new boost::asio::io_service::strand( *io_service ) );

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] Press [return] to exit." << std::endl;
    global_stream_lock.unlock();

    boost::thread_group worker_threads;
    for( int x = 0; x < 2; ++x ) {
        worker_threads.create_thread( boost::bind( &WorkerThread5, io_service ) );
    }

    boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );

    strand->post( boost::bind( &PrintNum, 1 ) );
    strand->post( boost::bind( &PrintNum, 2 ) );
    strand->post( boost::bind( &PrintNum, 3 ) );
    strand->post( boost::bind( &PrintNum, 4 ) );
    strand->post( boost::bind( &PrintNum, 5 ) );

    boost::shared_ptr< boost::asio::deadline_timer > timer( new boost::asio::deadline_timer( *io_service ) );
    timer->expires_from_now( boost::posix_time::seconds( 1 ) );
    timer->async_wait( strand->wrap( boost::bind( &TimerHandler2, _1, timer, strand ) ) );

    std::cin.get();

    io_service->stop();

    worker_threads.join_all();
}
// this will make sure the workers are taken care of first and then the timer handlers start
// output like so
/*
[7f68d695db80] Press [return] to exit.
[7f68d695a700] Thread Start
[7f68d6159700] Thread Start
[7f68d695a700] x: 1
[7f68d695a700] x: 2
[7f68d695a700] x: 3
[7f68d695a700] x: 4
[7f68d695a700] x: 5
[7f68d695a700] TimerHandler 
[7f68d6159700] TimerHandler 
[7f68d695a700] TimerHandler 
[7f68d695a700] TimerHandler 
[7f68d695a700] TimerHandler 
[7f68d6159700] TimerHandler 
[7f68d6159700] TimerHandler 
*/
// end of solution to problem stated above using strand ####################################


int main(int argc, char** argv) {
    //boost::asio::io_context io; // uncomment for timer and tcpdaytime
    //TimerSyncronisly(&io);
    //timerAsynchronously(&io);
    //timerAsynchronouslyBinding(&io);
    //timerAsynchronouslyBindingClassMember(&io);
    //timerAsynchronouslyBindingClassMemberDouble(&io);
    
    //tcpDayTimeClient(&io, argc, argv);
    //tcpDayTimeServer(&io);
    //tcpDayTimeServerAsync(&io);
    
    //example_1f();
    //example_2c();
    //example_2e();
    //example_2f();
    //example_3a();
    //example_5a();
    //example_5c();
    //example_6a();
    //example_6b();
    example_6c();
    
    
    
    
    return 0;
}

