#include "Connection.h"
#include "ConnectionManager.h"

#include <boost/bind.hpp>

#include <algorithm>
#include <cstdlib>

//--------------------------------------------------------------------
size_t Connection::m_nextClientId(0);

//--------------------------------------------------------------------
Connection::SharedPtr Connection::Create(ConnectionManager * connectionManager, boost::asio::ip::tcp::socket & socket)
{
    return Connection::SharedPtr(new Connection(connectionManager, std::move(socket)));
}

//--------------------------------------------------------------------------------------------------
std::string Connection::ErrorCodeToString(const boost::system::error_code & errorCode)
{
    std::ostringstream debugMsg;
    debugMsg << " Error Category: " << errorCode.category().name() << ". "
             << " Error Message: "  << errorCode.message() << ". ";

    // IMPORTANT - These comparisons only work if you dynamically link boost libraries
    //             Because boost chose to implement boost::system::error_category::operator == by comparing addresses
    //             The addresses are different in one library and the other when statically linking.
    //
    // We use make_error_code macro to make the correct category as well as error code value.
    // Error code value is not unique and can be duplicated in more than one category.
    if (errorCode == boost::asio::error::make_error_code(boost::asio::error::connection_refused))
    {
        debugMsg << " (Connection Refused)";
    }
    else if (errorCode == boost::asio::error::make_error_code(boost::asio::error::eof))
    {
        debugMsg << " (Remote host has disconnected)";
    }
    else
    {
        debugMsg << " (boost::system::error_code has not been mapped to a meaningful message)";
    }

    return debugMsg.str();
}

//--------------------------------------------------------------------
Connection::Connection(ConnectionManager * connectionManager, boost::asio::ip::tcp::socket socket)
    :
    m_clientId                          (m_nextClientId++)
  , m_owner                             (connectionManager)
  , m_socket                            (std::move(socket))
  , m_stopped                           (false)
  , m_receiveBuffer                     ()
  , m_sendMutex                         ()
  , m_sendBuffers                       ()
  , m_activeSendBufferIndex             (0)
  , m_sending                           (false)
  , m_allReadData                       ()
{
    printf("Client connection with id %zd has been created.", m_clientId);
}

//--------------------------------------------------------------------
Connection::~Connection()
{
    // Boost uses RAII, so we don't have anything to do. Let thier destructors take care of business
    printf("Client connection with id %zd has been destroyed.", m_clientId);
}

//--------------------------------------------------------------------
void Connection::Start()
{
    DoReceive();
}

//--------------------------------------------------------------------
void Connection::Stop()
{
    // The entire connection class is only kept alive, because it is a shared pointer and always has a ref count
    // as a consequence of the outstanding async receive call that gets posted every time we receive.
    // Once we stop posting another receive in the receive handler and once our owner release any references to
    // us, we will get destroyed.
    m_stopped = true;
    m_owner->OnConnectionClosed(shared_from_this());
}

//--------------------------------------------------------------------
void Connection::Send(const std::vector<char> & data)
{
    std::lock_guard<std::mutex> lock(m_sendMutex);

    // Append to the inactive buffer
    std::vector<char> & inactiveBuffer = m_sendBuffers[m_activeSendBufferIndex ^ 1];
    inactiveBuffer.insert(inactiveBuffer.end(), data.begin(), data.end());

    //
    DoSend();
}

//--------------------------------------------------------------------
void Connection::DoSend()
{
    // Check if there is an async send in progress
    // An empty active buffer indicates there is no outstanding send
    if (m_sendBuffers[m_activeSendBufferIndex].empty())
    {
        m_activeSendBufferIndex ^= 1;

        std::vector<char> & activeBuffer = m_sendBuffers[m_activeSendBufferIndex];
        auto self(shared_from_this());

        boost::asio::async_write(m_socket, boost::asio::buffer(activeBuffer),
            [self](const boost::system::error_code & errorCode, size_t bytesTransferred)
            {
                std::lock_guard<std::mutex> lock(self->m_sendMutex);

                self->m_sendBuffers[self->m_activeSendBufferIndex].clear();

                if (errorCode)
                {
                    printf("An error occured while attemping to send data to client id %zd. %s", self->m_clientId, ErrorCodeToString(errorCode).c_str());

                    // An error occurred
                    // We do not stop or close on sends, but instead let the receive error out and then close
                    return;
                }

                // Check if there is more to send that has been queued up on the inactive buffer,
                // while we were sending what was on the active buffer
                if (!self->m_sendBuffers[self->m_activeSendBufferIndex ^ 1].empty())
                {
                    self->DoSend();
                }
            });
    }
}

//--------------------------------------------------------------------
void Connection::DoReceive()
{
    auto self(shared_from_this());

    boost::asio::async_read_until(m_socket, m_receiveBuffer, '#',
        [self](const boost::system::error_code & errorCode, size_t bytesRead)
        {
            if (errorCode)
            {
                // Check if the other side hung up
                if (errorCode == boost::asio::error::make_error_code(boost::asio::error::eof))
                {
                    // This is not really an error. The client is free to hang up whenever they like
                    printf("Client %zd has disconnected.", self->m_clientId);
                }
                else
                {
                    printf("An error occured while attemping to receive data from client id %zd. Error Code: %s", self->m_clientId, ErrorCodeToString(errorCode).c_str());
                }

                // Notify our masters that we are ready to be destroyed
                self->m_owner->OnConnectionClosed(self);

                // An error occured
                return;
            }

            // Grab the read data
            std::istream stream(&self->m_receiveBuffer);
            std::string data;
            std::getline(stream, data, '#');
            data += "#";

            printf("Received data from client %zd: %s", self->m_clientId, data.c_str());

            // Issue the next receive
            if (!self->m_stopped)
            {
                self->DoReceive();
            }
        });
}