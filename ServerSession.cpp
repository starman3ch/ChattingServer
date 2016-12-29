#include "ServerSession.h"

Session::Session(int nSessionID, boost::asio::io_service& io_service, ChatServer* pServer)
            : m_Socket(io_service), m_nSessionID(nSessionID), m_pServer(pServer)
{  }


Session::~Session()
{
  while(m_SendDataQueue.empty() == false)
  {
    delete[] m_SendDataQueue.front();
    m_SendDataQueue.pop_front();
  }
}


void Session::Init()
{
  m_nPacketBufferMark = 0;
}


void Session::PostReceive()
{
  m_Socket.async_read_some (
                          boost::asio::buffer(m_ReceiveBuffer),
                          boost::bind(&Session::handle_receive, this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred)
                          );
}


void Session::PostSend(const bool bImmediately, const int nSize, char* pData)
{
  char* pSendData = nullptr;

  if(bImmediately == false)
  {
    pSendData = new char[nSize];
    memcpy(pSendData, pData, nSize);

    m_SendDataQueue.push_back(pSendData);
  }
  else
  {
    pSendData = pData;
  }

  if(bImmediately == false && m_SendDataQueue.size() > 1)
  {
    return;
  }

  boost::asio::async_write(m_Socket, boost::asio::buffer(pSendData, nSize),
                          boost::bind(&Session::handle_write, this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred)
                          );
}


void Session::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
  delete[] m_SendDataQueue.front();
  m_SendDataQueue.pop_front();

  if(m_SendDataQueue.empty() == false)
  {
    char* pData = m_SendDataQueue.front();
    PACKET_HEADER* pHeader = (PACKET_HEADER*)pData;
    PostSend(true, pHeader->nSize, pData);
  }
}


void Session::handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
{
  if(error)
  {
    if(error == boost::asio::error::eof)
    {
      std::cout << "Disconnected with Client" << std::endl;
    }
    else
    {
      std::cout << "error No: " << error.value() << ", error Message: " << error.message() << std::endl;
    }

    m_pServer->CloseSession(m_nSessionID);
  }
  else
  {
    // Save received Data into Packet buffer
    memcpy(&m_PacketBuffer[m_nPacketBufferMark], m_ReceiveBuffer.data(), bytes_transferred);

    int nPacketData = m_nPacketBufferMark + bytes_transferred;
    int nReadData = 0;

    while (nPacketData > 0) // Repeat until processing all data received.
    {
      if(nPacketData < sizeof(PACKET_HEADER)) // if remain Data'size is smaller than pacet header, then stop..
      {
        break;
      }

      PACKET_HEADER* pHeader = (PACKET_HEADER*)&m_PacketBuffer[nReadData];

      if(pHeader->nSize <= nPacketData) // Process packet if enough data
      {
        m_pServer->ProcessPacket(m_nSessionID, &m_PacketBuffer[nReadData]);

        nPacketData -= pHeader->nSize;
        nReadData += pHeader->nSize;
      }
      else
      {
        break; // stop if not enough to process packet.
      }
    }

    if(nPacketData > 0) // Save remain data to m_PacketBuffer
    {
      char TempBuffer[MAX_RECEIVE_BUFFER_LEN] = {0,};
      memcpy(&TempBuffer[0], &m_PacketBuffer[nReadData], nPacketData);
      memcpy(&m_PacketBuffer[0], &TempBuffer[0], nPacketData);
    }

    // Save remain data's size and request data receiving.
    m_nPacketBufferMark = nPacketData;

    PostReceive();
  }
}
