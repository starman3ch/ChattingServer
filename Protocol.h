#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <string>

const unsigned short PORT_NUMBER = 31400;

const int MAX_RECEIVE_BUFFER_LEN = 512;

// const int MAX_NAME_LEN = 20;
// const int MAX_MESSAGE_LEN = 145;


struct PACKET_HEADER
{
  int nCmd;
  int nSize;
};

enum Command
{
  REQ_SIGNUP = 1,
  RES_SIGNUP,
  REQ_LOGIN,
  RES_LOGIN,
  REQ_ALL_USER_LIST,
  RES_ALL_USER_LIST,
  REQ_ACTIVE_USER_LIST,
  RES_ACTIVE_USER_LIST,
  REQ_INVITE_CHAT,
  RES_INVITE_CHAT,
  NET_INVITED_CHAT,
  REQ_SEND_CHAT,
  RES_SEND_CHAT,
  NET_RECEIVE_CHAT,
  REQ_LEAVE_CHAT,
  RES_LEAVE_CHAT,
  NET_CLOSE_CHAT,
  REQ_CONNECT,
  RES_CONNECT,
  REQ_DISCONNECT,
  RES_DISCONNECT,
  NET_HEARTBEAT,
  NET_DISCONNECTED,
};

struct PKT_REQ_SIGNUP : public PACKET_HEADER
{
  void Init()
  {
    nCmd = REQ_SIGNUP;
    nSize = sizeof(PKT_REQ_SIGNUP);
    // memset( szName, 0, MAX_NAME_LEN );
  }
  // char szName[MAX_NAME_LEN];
  // 이렇게 해야 Init()할 때 고정 size를 알 수 있을 것 같은데.. 일단 해보자. 아님 size를 다른 시점에 해야 되나..?
  std::string userId;
  std::string userPwd;
};

struct PKT_RES_SIGNUP : public PACKET_HEADER
{
  void Init()
  {
    nCmd = RES_SIGNUP;
    nSize = sizeof(PKT_RES_SIGNUP);
  }
  int resultCode; // -2 : 이미 존재하는 ID,     0 : 실패,    1 : 성공
};

struct PKT_REQ_LOGIN : public PACKET_HEADER
{
  void Init()
  {
    nCmd = REQ_LOGIN;
    nSize = sizeof(PKT_REQ_LOGIN);
  }
  std::string userId;
  std::string userPwd;
};

struct PKT_RES_LOGIN : public PACKET_HEADER
{
  void Init()
  {
    nCmd = RES_LOGIN;
    nSize = sizeof(PKT_RES_LOGIN);
  }
  int resultCode;
};

struct PKT_REQ_ALL_USER_LIST : public PACKET_HEADER
{
  void Init()
  {
    nCmd = REQ_ALL_USER_LIST;
    nSize = sizeof(PKT_REQ_ALL_USER_LIST);
  }
};

struct PKT_RES_ALL_USER_LIST : public PACKET_HEADER
{
  void Init()
  {
    nCmd = RES_ALL_USER_LIST;
    nSize = sizeof(PKT_RES_ALL_USER_LIST);
  }
  std::string allUserData;
};
/* allUserData 는 json string
   "list" : [
      {
        "memberId" : "sandman"
      },
      {
        "memberId" : "starman"
      }
   ]
*/

struct PKT_REQ_ACTIVE_USER_LIST : public PACKET_HEADER
{
  void Init()
  {
    nCmd = REQ_ACTIVE_USER_LIST;
    nSize = sizeof(PKT_REQ_ACTIVE_USER_LIST);
  }
};

struct PKT_RES_ACTIVE_USER_LIST : public PACKET_HEADER
{
  void Init()
  {
    nCmd = RES_ACTIVE_USER_LIST;
    nSize = sizeof(PKT_RES_ACTIVE_USER_LIST);
  }
  std::string actUserData;
};
/* allUserData 는 json string
   "list" : [
      {
        "memberId" : "sandman"
      },
      {
        "memberId" : "starman"
      }
   ]
*/

struct PKT_REQ_INVITE_CHAT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = REQ_INVITE_CHAT;
    nSize = sizeof(PKT_REQ_INVITE_CHAT);
  }
  std::string invitedUserId;
};

struct PKT_RES_INVITE_CHAT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = RES_INVITE_CHAT;
    nSize = sizeof(PKT_RES_INVITE_CHAT);
  }
  int resultCode;
  int chatroomId;
};

struct PKT_NET_INVITED_CHAT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = NET_INVITED_CHAT;
    nSize = sizeof(PKT_NET_INVITED_CHAT);
  }
  std::string invitingUserId;
  int chatroomId;
};

struct PKT_REQ_SEND_CHAT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = REQ_SEND_CHAT;
    nSize = sizeof(PKT_REQ_SEND_CHAT);
  }
  int chatroomId;
  std::string chatMsg;
};

struct PKT_RES_SEND_CHAT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = RES_SEND_CHAT;
    nSize = sizeof(PKT_RES_SEND_CHAT);
  }
  int resultCode;
  int chatroomId;
};

struct PKT_NET_RECEIVE_CHAT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = NET_RECEIVE_CHAT;
    nSize = sizeof(PKT_NET_RECEIVE_CHAT);
  }
  int chatroomId;
  std::string chatMsg;
};

struct PKT_REQ_LEAVE_CHAT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = REQ_LEAVE_CHAT;
    nSize = sizeof(PKT_REQ_LEAVE_CHAT);
  }
  int chatroomId;
};

struct PKT_RES_LEAVE_CHAT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = RES_LEAVE_CHAT;
    nSize = sizeof(PKT_RES_LEAVE_CHAT);
  }
  int resultCode;
  int chatroomId;
};

struct PKT_NET_CLOSE_CHAT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = NET_CLOSE_CHAT;
    nSize = sizeof(PKT_NET_CLOSE_CHAT);
  }
  int chatroomId;
};

struct PKT_REQ_CONNECT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = REQ_CONNECT;
    nSize = sizeof(PKT_REQ_CONNECT);
  }
};

struct PKT_RES_CONNECT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = RES_CONNECT;
    nSize = sizeof(PKT_RES_CONNECT);
  }
  int resultCode;
};

struct PKT_REQ_DISCONNECT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = REQ_DISCONNECT;
    nSize = sizeof(PKT_REQ_DISCONNECT);
  }
  std::string userId;
};

struct PKT_RES_DISCONNECT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = RES_DISCONNECT;
    nSize = sizeof(PKT_RES_DISCONNECT);
  }
  int resultCode;
};

struct PKT_NET_DISCONNECTED : public PACKET_HEADER
{
  void Init()
  {
    nCmd = NET_DISCONNECTED;
    nSize = sizeof(PKT_NET_DISCONNECTED);
  }
};

struct PKT_NET_HEARTBEAT : public PACKET_HEADER
{
  void Init()
  {
    nCmd = NET_HEARTBEAT;
    nSize = sizeof(PKT_NET_HEARTBEAT);
  }
};


#endif
