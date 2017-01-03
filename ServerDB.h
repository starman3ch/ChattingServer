#ifndef __SERVERDB_H__
#define __SERVERDB_H__

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>


class ServerDB
{
private:
  sql::mysql::MySQL_Driver* mysql_driver;
  sql::Connection* conn;
  sql::Statement* stmt;
  sql::PreparedStatement* prep_stmt;
  sql::ResultSet* res;

public:
  ServerDB();
  ~ServerDB();
  void ConnectDB();
  void CreateUserDBIfNotExists();                                   // UserDB를 만든다.
  bool HaveUser(std::string userId);                                // UserDB에 해당 userId가 존재하는지 체크
  bool MatchUserIdAndPwd(std::string userId, std::string userPwd);  // UserDB를 보고, userId와 userPwd가 맞는지 체크
  void InsertUserToDB(std::string userId, std::string userPwd);     // 새 user 정보를 UserDB에 삽입
};
#endif
