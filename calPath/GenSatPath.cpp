#include "GenSatPath.h"
#include <ctime>
#include "TrackPoint.h"
#include "TargetArea.h"
#include "DateTime.h"
#include "StripUnit.h"
#include "MyPoint.h"
#include "Sensor.h"
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

GenSatPath::GenSatPath(string SatDbFile, string OutputFolder)
{
	m_SatSqlite = new SqliteOperation(&SatDbFile[0]);
	m_OutputFolder = OutputFolder;
	Init();
}
GenSatPath::~GenSatPath()
{
	delete (m_SatSqlite);
	delete (m_TrackSqlite);
	delete (m_PathSqlite);
}

void GenSatPath::Init()
{
	m_TrackDbInitCmds.push_back("CREATE TABLE track_info (id INTEGER NOT NULL, sat_noard_id,\
	 sat_name, start_time, PRIMARY KEY(id AUTOINCREMENT));");
	m_TrackDbInitCmds.push_back("CREATE TABLE track (id INTEGER NOT NULL, time_offset INTEGER, x REAL,\
	 y REAL, z REAL, vx REAL, vy REAL, vz REAL, lon REAL, lat REAL, alt REAL, PRIMARY KEY(id AUTOINCREMENT));");
	m_TrackDbInitCmds.push_back("CREATE INDEX index_time on track(time_offset);");

	m_PathDbInitCmds.push_back("CREATE TABLE sen_path (id INTEGER NOT NULL, time_offset INTEGER,\
	  lon1 REAL, lat1 REAL, lon2 REAL, lat2 REAL, PRIMARY KEY(id AUTOINCREMENT));");
	m_PathDbInitCmds.push_back("CREATE TABLE path_info (id INTEGER NOT NULL, sat_noard_id ,\
sat_name , sen_name TEXT, start_time , PRIMARY KEY(id AUTOINCREMENT));");

	m_PathDbInitCmds.push_back("CREATE INDEX index_lat1 on sen_path(lat1)");
	m_PathDbInitCmds.push_back("CREATE INDEX index_lat2 on sen_path(lat2)");
	m_PathDbInitCmds.push_back("CREATE INDEX index_lon1 on sen_path(lon1)");
	m_PathDbInitCmds.push_back("CREATE INDEX index_lon2 on sen_path(lon2)");
	m_PathDbInitCmds.push_back("CREATE INDEX index_pathtime on sen_path(time_offset)");
}

string GenSatPath::GetBjtTimeString()
{
	return "bj time";
}

void GenSatPath::Do()
{
	// define time span
	DateTime today = DateTime::UtcToday();
	DateTime StartTime = DateTime(today.getYear(), today.getMonth(), today.getDay());
	DateTime StopTime = StartTime.AddDays(7);
	// define step
	int tsp = 20;
	// initialize satellites
	SatInit(&StartTime);
	for (map<string, Satellite>::iterator it = m_mapSat.begin(); it != m_mapSat.end(); it++)
	{
		std::cout << "satellite name: " << it->second.getSatName() << endl;
		string TrackFile = m_OutputFolder + "/" + it->second.getNoardID() + ".sqlite";
		ifstream file(TrackFile);
		if (file.is_open())
		{
			//file exists
			//std::cout << "clearing db file " << TrackFile << endl;
			m_TrackSqlite = new SqliteOperation(TrackFile);
			m_TrackSqlite->ExecuteSql("delete from track");
			m_TrackSqlite->ExecuteSql("delete from track_info");
		}
		else
		{
			//file not found
			//std::cout << "Creating db file " << TrackFile << endl;
			m_TrackSqlite = new SqliteOperation(TrackFile);
			m_TrackSqlite->BatProcess(&m_TrackDbInitCmds);
		}

		//std::cout << "ready to compute track" << endl;
		vector<TrackPoint> Points = it->second.ComputeTrack2(StartTime, StopTime, tsp);
		//std::cout << "ready to write to db" << endl;
		//write to db
		vector<string> cmds;

		cmds.push_back("insert into track_info(sat_noard_id,sat_name,start_time) values('" +
					   it->second.getNoardID() + "','" + it->second.getSatName() + "','" +
					   to_string(StartTime.ToTimestamp()) + "')");
		for (int i = 0; i < Points.size(); i++)
		{
			TrackPoint tp = Points[i];
			cmds.push_back("insert into track(time_offset,x,y,z,vx,vy,vz,lon,lat,alt) values('" +
						   to_string(tp.getTime().ToTimestamp() - StartTime.ToTimestamp()) + "','" +
						   to_string(tp.getEciPoint().getX()) + "','" +
						   to_string(tp.getEciPoint().getY()) + "','" + to_string(tp.getEciPoint().getZ()) +
						   "','" + to_string(tp.getVel().getX()) + "','" + to_string(tp.getVel().getY()) + "','" +
						   to_string(tp.getVel().getZ()) + "','" + to_string(tp.getBlhPoint().getX()) + "','" +
						   to_string(tp.getBlhPoint().getX()) + "','" + to_string(tp.getBlhPoint().getZ()) + "')");
		}

		m_TrackSqlite->BatProcess(&cmds);
		//compute path
		cout << "computing path..." << endl;
		for (size_t jIndex = 0; jIndex < it->second.getSensorList().size(); jIndex++)
		{
			Sensor sen = it->second.getSensorList()[jIndex];
			std::cout << sen.getSatName() << ", sen: " << sen.getSenName() << endl;
			string PathFolder = m_OutputFolder + "/" + it->second.getNoardID();
			if (!ExistDirOrFile(PathFolder))
			{
				mkdir(&PathFolder[0], 0777);
			}
			string PathFile = PathFolder + "/" + sen.getSenName() + ".sqlite";
			if (ExistDirOrFile(PathFile))
			{
				//cout << "Clearing db file " << PathFile << " ..." << endl;
				m_PathSqlite = new SqliteOperation(PathFile);
				m_PathSqlite->ExecuteSql("delete from sen_path");
				m_PathSqlite->ExecuteSql("delete from path_info");
			}
			else
			{
				//cout << "Creating db file " << PathFile << " ...";
				m_PathSqlite = new SqliteOperation(PathFile);
				m_PathSqlite->BatProcess(&m_PathDbInitCmds);
			}
			cmds.clear();
			cmds.push_back("insert into path_info(sat_noard_id,sat_name,sen_name,start_time) values('" +
						   it->second.getNoardID() + "','" + it->second.getSatName() + "','" + sen.getSenName() + "','" +
						   to_string(StartTime.ToTimestamp()) + "')");
			for (int i = 0; i < Points.size(); i++)
			{
				TrackPoint tp = Points[i];
				//compute sensor path point
				double r[] = {tp.getEciPoint().getX(), tp.getEciPoint().getY(), tp.getEciPoint().getZ()};
				double v[] = {tp.getVel().getX(), tp.getVel().getY(), tp.getVel().getZ()};
				double res[4] = {0};
				it->second.GetSensorPointsBLH(&sen, tp.getTime(), r, v, res);

				cmds.push_back("insert into sen_path(time_offset,lon1,lat1,lon2,lat2) values('" +
							   to_string(tp.getTime().ToTimestamp() - StartTime.ToTimestamp()) + "','" +
							   to_string(res[0]) + "','" + to_string(res[1]) + "','" + to_string(res[2]) +
							   "','" + to_string(res[3]) + "');");
			}

			//write the rest points
			m_PathSqlite->BatProcess(&cmds);
		}
	}
}

void GenSatPath::SatInit(DateTime *startTime)
{
	m_mapSat.clear();
	vector<vector<string> > dtSensor = m_SatSqlite->Query("select sat_noard_id,sat_name,name,resolution,width,\
	right_side_angle, left_side_angle, observe_angle, hex_color, init_angle from sensor",
														  10);

	for (size_t iIndex = 0; iIndex < dtSensor.size(); iIndex++)
	{
		vector<string> row = dtSensor[iIndex];
		string satId = row[0];
		if (m_mapSat.find(satId) == m_mapSat.end())
		{
			//std::cout << "creating satellite" << endl;
			Satellite sat(row[0], row[1]);
			//std::cout << "satellite created " << row[0] << "  " << row[1] << endl;
			sat.setSatName(row[1]);
			SetCurTle(dynamic_cast<Satellite *>(&sat), startTime);
			//std::cout << "satellite tle set" << endl;
			pair<string, Satellite> pp(satId, sat);
			m_mapSat.insert(pp);
		}

		Sensor sen(satId, row[2]);
		sen.setSatName(row[1]);
		sen.setResolution(stod(row[3]));
		sen.setWidth(stod(row[4]));
		sen.setRightSideAngle(stod(row[5]));
		sen.setLeftSideAngle(stod(row[6]));
		sen.setObsAngle(stod(row[7]));
		sen.setInitAngle(stod(row[9]));

		map<string, Satellite>::iterator pos = m_mapSat.find(satId);
		pos->second.AddSensor(sen);
	}

	std::cout << "finished reading satellite, count: " << m_mapSat.size() << endl;
}

bool GenSatPath::string2bool(const string &v)
{
	return !v.empty() &&
		   (strcasecmp(v.c_str(), "true") == 0 ||
			atoi(v.c_str()) != 0);
}

bool GenSatPath::SetCurTle(Satellite *Sat, DateTime *Dt)
{
	//std::cout << "setting tle " << Sat->getSatName() << endl;
	//set current tle
	string tle[3] = {"", "", ""};
	//std::cout << "getting tle " << Sat->getNoardID() << endl;
	GetTLE(Sat->getNoardID(), Dt, tle);
	//std::cout << "got tle" << endl;

	if (tle[1].size() == 0)
	{
		std::cout << "can not set tle for " << Sat->getSatName() << endl;
		return false;
	}
	else
	{
		Sat->SetCurTLE(tle[0], tle[1], tle[2]);
		return true;
	}
}

void GenSatPath::GetTLE(string SatID, DateTime *Dt, string strTLE[])
{
	vector<vector<string> > datas;
	//find tle in one week
	//std::cout << "start time: " << to_string(Dt->AddDays(-1).ToTimestamp()) << endl;
	//std::cout << "stop time: " << to_string(Dt->AddDays(1).ToTimestamp()) << endl;
	string strDataCmd = "select line1, line2 from TLE where sat_noard_id= '" + SatID + "' and time > " +
						to_string(Dt->AddDays(-1).ToTimestamp()) + " and time < " +
						to_string(Dt->AddDays(1).ToTimestamp()) + " order by time desc";

	//std::cout << "getting tle data with cmd: " << strDataCmd << endl;
	datas = m_SatSqlite->Query(strDataCmd, 2);
	//std::cout << "got tle data, size: " << to_string(datas.size()) << endl;

	if (datas.size() == 0)
	{
		//get random one
		strDataCmd = "select line1, line2 from TLE where sat_noard_id= '" + SatID + "' order by time desc";
		datas = m_SatSqlite->Query(strDataCmd, 2);
		if (datas.size() != 0)
		{
			strTLE[0] = SatID;
			strTLE[1] = datas[0][0];
			strTLE[2] = datas[0][1];
		}
	}
	else
	{
		//get tle
		strTLE[0] = SatID;
		strTLE[1] = datas[0][0];
		strTLE[2] = datas[0][1];
	}
}

bool GenSatPath::ExistDirOrFile(string dirOrFile)
{
	return stat(&dirOrFile[0], &m_info) == 0;
}