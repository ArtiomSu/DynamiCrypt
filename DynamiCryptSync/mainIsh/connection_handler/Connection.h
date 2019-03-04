#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

//--------------------------------------------------------------------
class ConnectionManager;

//--------------------------------------------------------------------
class Connection : public std::enable_shared_from_this<Connection>
{
public:

    typedef std::shared_ptr<Connection> SharedPtr;

    // Ensure all instances are created as shared_ptr in order to fulfill requirements for shared_from_this
    static Connection::SharedPtr Create(ConnectionManager * connectionManager, boost::asio::ip::tcp::socket & socket);

    //
    static std::string ErrorCodeToString(const boost::system::error_code & errorCode);

    Connection(const Connection &) = delete;
    Connection(Connection &&) = delete;
    Connection & operator = (const Connection &) = delete;
    Connection & operator = (Connection &&) = delete;
    ~Connection();

    // We have to defer the start until we are fully constructed because we share_from_this()
    void Start();
    void Stop();

    void Send(const std::vector<char> & data);

private:

    static size_t                                           m_nextClientId;

    size_t                                                  m_clientId;
    ConnectionManager *                                     m_owner;
    boost::asio::ip::tcp::socket                            m_socket;
    std::atomic<bool>                                       m_stopped;
    boost::asio::streambuf                                  m_receiveBuffer;
    mutable std::mutex                                      m_sendMutex;
    std::vector<char>                                       m_sendBuffers[2];         // Double buffer
    int                                                     m_activeSendBufferIndex;
    bool                                                    m_sending;

    std::vector<char>                                       m_allReadData;            // Strictly for test purposes

    Connection(ConnectionManager * connectionManager, boost::asio::ip::tcp::socket socket);

    void DoReceive();
    void DoSend();
};
