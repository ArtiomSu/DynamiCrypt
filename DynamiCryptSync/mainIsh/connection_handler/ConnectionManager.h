#pragma once

#include "Connection.h"

// Boost Includes
#include <boost/asio.hpp>

// Standard Includes
#include <thread>
#include <vector>

//--------------------------------------------------------------------
class ConnectionManager
{
public:

    ConnectionManager(unsigned port, size_t numThreads);
    ConnectionManager(const ConnectionManager &) = delete;
    ConnectionManager(ConnectionManager &&) = delete;
    ConnectionManager & operator = (const ConnectionManager &) = delete;
    ConnectionManager & operator = (ConnectionManager &&) = delete;
    ~ConnectionManager();

    void Start();
    void Stop();

    void OnConnectionClosed(Connection::SharedPtr connection);

protected:

    boost::asio::io_service            m_io_service;
    boost::asio::ip::tcp::acceptor     m_acceptor;
    boost::asio::ip::tcp::socket       m_listenSocket;
    std::vector<std::thread>           m_threads;

    mutable std::mutex                 m_connectionsMutex;
    std::vector<Connection::SharedPtr> m_connections;

    boost::asio::deadline_timer        m_timer;

    void IoServiceThreadProc();

    void DoAccept();
    void DoTimer();
};