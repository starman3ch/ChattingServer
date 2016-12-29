#ifndef __SERVER_SESSION_H__
#define __SERVER_SESSION_H__

#include <deque>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>

#include "Protocol.h"
#include "ChatServer.h"

class ChatServer;

class Session
{
private:
  int m_nSessionID;
  boost::asio::ip::tcp::socket m_Socket;

  boost::array<char, MAX_RECEIVE_BUFFER_LEN> m_ReceiveBuffer;

  int m_nPacketBufferMark;
  char m_PacketBuffer[MAX_RECEIVE_BUFFER_LEN*2];

  std::deque<char*> m_SendDataQueue;
  std::string m_Name;

  ChatServer* m_pServer;

  void handle_write(const boost::system::error_code& error, size_t bytes_transferred);
  void handle_receive(const boost::system::error_code& error, size_t bytes_transferred);

public:
  Session(int nSessionID, boost::asio::io_service& io_service, ChatServer* pServer);
  ~Session();

  int SessionID() { return m_nSessionID; }
  boost::asio::ip::tcp::socket& Socket() { return m_Socket; }

  void Init();
  void PostReceive();
  void PostSend(const bool bImmediately, const int nSize, char* pData);

  void SetName(const char* pszName) { m_Name = pszName; }
  const char* GetName() { return m_Name.c_str(); }

};
#endif
