#include "SqliteOperation.h"
#include "sqlite3.h"
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

SqliteOperation::SqliteOperation(string sqliteDbName)
{
    CreateSqliteDB(&sqliteDbName[0]);
}
SqliteOperation::~SqliteOperation()
{
    ShutDown();
    //sqlite3_finalize(m_db);
    //sqlite3_finalize(m_pStmt);
    m_rc = 0;
}

void SqliteOperation::CreateSqliteDB(string sqliteDbName)
{
    m_ConnectString = sqliteDbName;
    char *zErrMsg = 0;
    int rc;

    m_rc = sqlite3_open(&sqliteDbName[0], &m_db);

    if (m_rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(m_db));
    }
    else
    {
        //fprintf(stderr, "Opened database successfully\n");
    }
}
void SqliteOperation::ShutDown()
{
    sqlite3_close(m_db);
}

bool SqliteOperation::InsertData(string insertCmd)
{
    return ExecuteSql(insertCmd);
}

bool SqliteOperation::DeleteData(string deleteCmd)
{
    return ExecuteSql(deleteCmd);
}

bool SqliteOperation::ExecuteSql(string sql)
{
    char *zErrMsg = 0;
    /* Execute SQL statement */
    m_rc = sqlite3_exec(m_db, &sql[0], callback, 0, &zErrMsg);

    if (m_rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
    else
    {
        //fprintf(stdout, "Records created successfully\n");
        return true;
    }
}

bool SqliteOperation::BatProcess(vector<string> *BatCmdList)
{
    char *zErrMsg = 0;
    // 'db' is the pointer you got from sqlite3_open*
    sqlite3_exec(m_db, "BEGIN ;", NULL, NULL, NULL);
    for (size_t i = 0; i < BatCmdList->size(); i++)
    {
        m_rc = sqlite3_exec(m_db, &(*BatCmdList)[i][0], callback, 0, &zErrMsg);
    }

    // Any (modifying) SQL commands executed here are not committed until at the you call:
    sqlite3_exec(m_db, "COMMIT;", NULL, NULL, NULL);

    bool result = false;
    if (m_rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        //fprintf(stdout, "Records created successfully\n");
        result = true;
    }
    return result;
}

vector<vector<string> > SqliteOperation::Query(string queryCmd, int fieldCount)
{
    vector<vector<string> > data;
    m_pStmt = NULL;
    if (sqlite3_prepare_v2(m_db, &queryCmd[0], -1, &m_pStmt, NULL) != SQLITE_OK)
    {
        //printf("%4d Error:%sn", __LINE__, errmsg);
        return data;
    }

    int starttime;
    string satid, satname, timeinfo;
    while (sqlite3_step(m_pStmt) == SQLITE_ROW)
    {
        vector<string> row;
        for (int i = 0; i < fieldCount; i++)
        {
            const char *txt = (const char *)sqlite3_column_text(m_pStmt, i);
            row.push_back(txt);
        }

        data.push_back(row);
    }
    sqlite3_reset(m_pStmt);
    sqlite3_finalize(m_pStmt);

    return data;
}