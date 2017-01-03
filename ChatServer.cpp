#include "ChatServer.h"
#include "ServerDB.h"

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
    case REQ_SIGNUP:
    {
      PKT_REQ_SIGNUP* pPacket = (PKT_REQ_SIGNUP*)pData;

      std::cout << "Client require singup." << std::endl;

      std::string userId = pPacket->userId;
      std::string userPwd = pPacket->userPwd;

      if (userId.empty() || userPwd.empty())// 아이디, 비밀번호 데이터 잘 있는지 확인
      {
        std::cout << "id or pwd information of a user who signup is empty!?" << std::endl;
      }

      ServerDB serverDB;
      serverDB.ConnectDB();
      serverDB.CreateUserDBIfNotExists();
      bool haveUser = serverDB.HaveUser(userId);
      PKT_RES_SIGNUP SendPkt;
      SendPkt.Init();
      if (haveUser)
      {
        // 이미 있는 아이디임. 에러코드를 내려줌.
        SendPkt.resultCode = -2;
      }
      else
      {
        // db에 insert 하고 잘 되면, ok 내려줌.
        serverDB.InsertUserToDB(userId, userPwd);
        SendPkt.resultCode = 1;
        // TODO : 실패했을 때는 언제인가..? 어떻게 아나? (resultCode = 0 인 경우)
      }
      m_SessionList[nSessionID]->PostSend(false, SendPkt.nSize, (char*)&SendPkt);

      // disconnect 시켜버린다. TODO : 이렇게 바로 disconnected 패킷을 보내도 되나...?
      PKT_NET_DISCONNECTED DisconnPkt;
      DisconnPkt.Init();
      m_SessionList[nSessionID]->PostSend(false, DisconnPkt.nSize, (char*)&DisconnPkt);
    }
    break;
    case REQ_LOGIN:
    {
      PKT_REQ_LOGIN* pPacket = (PKT_REQ_LOGIN*)pData;

      std::cout << "Client require login." << std::endl;

      // 아이디 비밀번호 데이터를 db에 매칭해보고 맞으면 ok내려주고, 다른 요청에 대한 권한을 준다.
      // 실패시 에러코드 떨어뜨려주고, 다른 요청에 대한 권한을 일절 거부한다.
      // 실패시 disconnect 시켜버린다.
    }
    break;
  }
}
