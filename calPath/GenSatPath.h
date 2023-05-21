#ifndef GENSATPATH_H_
#define GENSATPATH_H_
#include <vector>
#include <strings.h>
#include "SqliteOperation.h"
#include "Satellite.h"
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
// class GenSatPath
// This class generate satellite paths
class GenSatPath
{
  public:
    GenSatPath(string SatDbFile, string OutputFolder);
    virtual ~GenSatPath();

    string GetBjtTimeString();
    void Do();
    string getOutputFolder(){return m_OutputFolder;};

  private:
    vector<string> m_TrackDbInitCmds;
    vector<string> m_PathDbInitCmds;
    void Init();
    SqliteOperation *m_SatSqlite;
    SqliteOperation *m_TrackSqlite;
    SqliteOperation *m_PathSqlite;
    void SatInit(DateTime *startTime);
    map<string,Satellite> m_mapSat;
    bool string2bool(const string & v);
    string m_OutputFolder;
    void GetTLE(string SatID, DateTime *Dt, string strTLE[]);
    bool SetCurTle(Satellite *Sat, DateTime *Dt);
    struct stat m_info;
    bool ExistDirOrFile(string dirOrFile);
};
#endif