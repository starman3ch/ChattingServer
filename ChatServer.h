#ifndef __CHATSERVER_H__
#define __CHATSERVER_H__

#include <vector>
#include <deque>

#include "ServerSession.h"
#include "Protocol.h"


class Session;

class ChatServer{
private:
  bool m_bIsAccepting;

  boost::asio::ip::tcp::acceptor m_accepter;

  std::vector<Session*> m_SessionList;
  std::deque<int> m_ValidSessionIdxQueue; // 아직 사용하지 않는 Session의 인덱스 번호를 저장 (새로운 접속이 있을 때 할당하기 위한..)

  bool PostAccept();
  void handle_accept(Session* pSession, const boost::system::error_code& error);

public:
  ChatServer( boost::asio::io_service& io_service );
  ~ChatServer();
  void Init(const int nMaxSessionCount);
  void Start();
  void CloseSession(const int nSessionId);
  void ProcessPacket(const int nSessionId, const char* pData);
};

#endif
