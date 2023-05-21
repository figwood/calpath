#include "Sensor.h"
#include "Color.h"
Sensor::Sensor(string satId, string sensorName)
{
    Init();
    m_SatId = satId;
    m_SenName = sensorName;
}
Sensor::~Sensor()
{
}

void Sensor::Init()
{
    m_SatId = "";
    m_SenName = "";
    m_SatName = "";
    m_CountryName = "";
    m_Width = -1;
    m_ObsAngle = -1; //观测角
    m_CurSideAngle = 0; //当前的侧摆角
    m_Resolution = 0;
    m_RightSideAngle = 0; //向右侧摆的角度
    m_LeftSideAngle = 0;  //向左侧摆的角度
    m_InitAngle = 0;      //载荷安装角
}