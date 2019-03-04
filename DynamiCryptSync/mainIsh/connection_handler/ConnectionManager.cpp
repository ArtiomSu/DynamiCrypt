#include "ConnectionManager.h"

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <system_error>
#include <cstdio>

//------------------------------------------------------------------------------
ConnectionManager::ConnectionManager(unsigned port, size_t numThreads)
    :
    m_io_service  ()
  , m_acceptor    (m_io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
  , m_listenSocket(m_io_service)
  , m_threads     (numThreads)
  , m_timer       (m_io_service)
{
}

//------------------------------------------------------------------------------
ConnectionManager::~ConnectionManager()
{
    Stop();
}

//------------------------------------------------------------------------------
void ConnectionManager::Start()
{
    if (m_io_service.stopped())
    {
        m_io_service.reset();
    }

    DoAccept();

    for (auto & thread : m_threads)
    {
        if (!thread.joinable())
        {
            thread.swap(std::thread(&ConnectionManager::IoServiceThreadProc, this));
        }
    }

    DoTimer();
}

//------------------------------------------------------------------------------
void ConnectionManager::Stop()
{
    {
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        m_connections.clear();
    }

    // TODO - Will the stopping of the io_service be enough to kill all the connections and ultimately have them get destroyed?
    //        Because remember they have outstanding ref count to thier shared_ptr in the async handlers
    m_io_service.stop();

    for (auto & thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

//------------------------------------------------------------------------------
void ConnectionManager::IoServiceThreadProc()
{
    try
    {
        // Log that we are starting the io_service thread
        {
            printf("io_service socket thread starting.");
        }

        // Run the asynchronous callbacks from the socket on this thread
        // Until the io_service is stopped from another thread
        m_io_service.run();
    }
    catch (std::system_error & e)
    {
        printf("System error caught in io_service socket thread. Error Code: %d", e.code().value());
    }
    catch (std::exception & e)
    {
        printf("Standard exception caught in io_service socket thread. Exception: %s", e.what());
    }
    catch (...)
    {
        printf("Unhandled exception caught in io_service socket thread.");
    }

    {
        printf("io_service socket thread exiting.");
    }
}

//------------------------------------------------------------------------------
void ConnectionManager::DoAccept()
{
    m_acceptor.async_accept(m_listenSocket,
        [this](const boost::system::error_code errorCode)
        {
            if (errorCode)
            {
                printf("An error occured while attemping to accept connections. Error Code: %s", Connection::ErrorCodeToString(errorCode).c_str());
                return;
            }

            // Create the connection from the connected socket
            std::lock_guard<std::mutex> lock(m_connectionsMutex);
            Connection::SharedPtr connection = Connection::Create(this, m_listenSocket);
            m_connections.push_back(connection);
            connection->Start();

            DoAccept();
        });
}

//------------------------------------------------------------------------------
void ConnectionManager::OnConnectionClosed(Connection::SharedPtr connection)
{
    std::lock_guard<std::mutex> lock(m_connectionsMutex);

    auto itConnection = std::find(m_connections.begin(), m_connections.end(), connection);
    if (itConnection != m_connections.end())
    {
        m_connections.erase(itConnection);
    }
}

//------------------------------------------------------------------------------
void ConnectionManager::DoTimer()
{
    if (!m_io_service.stopped())
    {
        // Send messages every second
        m_timer.expires_from_now(boost::posix_time::seconds(30));
        m_timer.async_wait(
            [this](const boost::system::error_code & errorCode)
            {
                std::lock_guard<std::mutex> lock(m_connectionsMutex);
                for (auto connection : m_connections)
                {
                    connection->Send(std::vector<char>{'b', 'e', 'e', 'p', '#'});
                }

                DoTimer();
            });
    }
}