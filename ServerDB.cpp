#include "ServerDB.h"


ServerDB::ServerDB()
{
  mysql_driver = sql::mysql::get_mysql_driver_instance(); // TODO : this is not thread safe.
}

ServerDB::~ServerDB()
{
  if(conn != NULL) // TODO : 이렇게 체크하는 게 맞나....
  {
    delete conn;
  }
  if(stmt != NULL)
  {
    delete stmt;
  }
  if(prep_stmt != NULL)
  {
    delete prep_stmt;
  }
  if(res != NULL)
  {
    delete res;
  }
}

void ServerDB::ConnectDB()
{
  if (sql::Connection::isValid())
  {
    // connection is alive.
    return;
  }

  conn = mysql_driver->connect("tcp://127.0.0.1:3306", "user", "password"); // TODO : these info should be received as parameter.

  // sql::Connection::reconnect() // reconnects if the connection has gone down :: 이런 게 있다..는 정도. 아직 딱히 쓸데는..?
}

void ServerDB::CreateUserDBIfNotExists()
{
  if (stmt == NULL) // TODO : 초기값이 NULL은 맞으려나...?
  {
    stmt = conn->createStatement();
  }
  stmt->execute("CREATE TABLE IF NOT EXISTS users (
                          id VARCHAR(20) NOT NULL,
                          pwd VARCHAR(20) NOT NULL,
                          PRIMARY KEY (id) )"); // TODO : Create 문 이렇게 맞나...?
}
/*
  Table : users
  id : TEXT (key) NOT NULL
  pwd : TEXT      NOT NULL
*/

bool ServerDB::HaveUser(std::string userId)
{
  // if (prep_stmt == NULL)
  // {
  //   prep_stmt = conn->createStatement();
  // }
  // TODO : prep_stmt가 이미 할당돼 있으면 어떻게 처리하지? delete 하고 해야 되나..?
  prep_stmt = conn->prepareStatement("SELECT * FROM users WHERE id = ?");
  prep_stmt->setString(1, userId);
  res = prep_stmt->execute();

  if (res->next())
    return TRUE;
  return FALSE;
}

bool ServerDB::MatchUserIdAndPwd(std::string userId, std::string userPwd)
{
  prep_stmt = conn->prepareStatement("SELECT * FROM users WHERE id = ? AND pwd = ?");
  prep_stmt->setString(1, userId);
  prep_stmt->setString(2, userPwd);
  res = prep_stmt->execute();

  if (res->next())
    return TRUE;
  return FALSE;
}

void ServerDB::InsertUserToDB(std::string userId, std::string userPwd)
{
  prep_stmt = conn->prepareStatement("INSERT INTO users(id, pwd) VALUES (?, ?)");
  prep_stmt->setString(1, userId);
  prep_stmt->setString(2, userPwd);
  prep_stmt->execute();
  // TODO : 잘 insert 됐는지 안 됐는지 어떻게 알지?
}
