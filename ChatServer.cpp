#include "ChatServer.h"

ChatServer::ChatServer(boost::asio::io_service& io_service)
  : m_accepter(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT_NUMBER))
{
  m_bIsAccepting = false;
}

ChatServer::~ChatServer()
{
  for (size_t i = 0; i < m_SessionList.size(); ++i)
  {
    if (m_SessionList[i]->Socket().is_open())
    {
      m_SessionList[i]->Socket().close();
    }
    delete m_SessionList[i];
  }
}

void ChatServer::Init(const int nMaxSessionCount)
{
  for (size_t i = 0; i < nMaxSessionCount; i++)
  {
    Session* pSession = new Session(i, m_accepter.get_io_service(), this);
    m_SessionList.push_back(pSession);
    m_ValidSessionIdxQueue.push_back(i);
  }
}

void ChatServer::Start()
{
  std::cout << "ChatServer Start..." << std::endl;

  PostAccept();
}

void ChatServer::CloseSession(const int nSessionId)
{
  std::cout << "A Client disconnected. sessionId: " << nSessionId << std::endl;

  m_SessionList[ nSessionId ]->Socket().close();

  m_ValidSessionIdxQueue.push_back(nSessionId);

  if (m_bIsAccepting == false)
  {
    PostAccept();
  }
}

bool ChatServer::PostAccept()
{
  if (m_ValidSessionIdxQueue.empty())
  {
    m_bIsAccepting = false;
    std::cout << "There is no available session list" << std::endl;
    return false;
  }

  m_bIsAccepting = true;
  int nSessionId = m_ValidSessionIdxQueue.front();
  m_ValidSessionIdxQueue.pop_front();

  m_accepter.async_accept(m_SessionList[nSessionId]->Socket(),
                boost::bind(&ChatServer::handle_accept,
                            this,
                            m_SessionList[nSessionId],
                            boost::asio::placeholders::error)
                          );
  return true;
}

void ChatServer::handle_accept(Session* pSession, const boost::system::error_code& error)
{
  if (!error)
  {
    std::cout << "A Client accepted. SessionId: " << pSession->SessionID() << std::endl;

    pSession->Init();
    pSession->PostReceive();

    PostAccept();
  }
  else
  {
    std::cout << "handle_accept ERROR occurred. errNo: " << error.value() << ", errMessage: " << error.message() << std::endl;
  }
}

void ChatServer::ProcessPacket(const int nSessionID, const char *pData)
{
  PACKET_HEADER* pheader = (PACKET_HEADER*)pData;

  switch (pheader->nCmd)
  {
    case REQ_CONNECT:
    {
      PKT_REQ_CONNECT* pPacket = (PKT_REQ_CONNECT*)pData;
      // m_SessionList[nSessionID]->SetName(pPacket->szName);

      std::cout << "Client connect success" << std::endl;

      PKT_RES_CONNECT SendPkt;
      SendPkt.Init();
      SendPkt.resultCode = 1;

      m_SessionList[nSessionID]->PostSend(false, SendPkt.nSize, (char*)&SendPkt);
    }
    break;
  }
}
