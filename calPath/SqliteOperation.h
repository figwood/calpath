#ifndef SQLITE_OPERATION_H_
#define SQLITE_OPERATION_H_
#include <string>
#include <vector>
#include <sqlite3.h>
#include <map> 
#include <iostream>
using namespace std;
class SqliteOperation
{
  private:
    sqlite3 *m_db;
    int m_rc;
    string m_ConnectString;
    string m_Passwd;
    string m_LastErrorMsg;
    sqlite3_stmt *m_pStmt;
    bool m_Busy;
    void CreateSqliteDB(string sqliteDbName);
    void ShutDown();
    map<string,string> m_data;

  public:
    string LastErrorMsg;
    SqliteOperation(string sqliteDbName);
    ~SqliteOperation();
    bool TestDB();
    vector<vector<string> > Query(string queryCmd,int fieldCount);
    bool BatProcess(vector<string> *BatCmdList);
    bool InsertData(string insertCmd);
    bool DeleteData(string deleteCmd);
    //bool UpdateData(string UpdateCmd);
    bool ExecuteSql(string sql);
    //void CompressDB();
};
#endif