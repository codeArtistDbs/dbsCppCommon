#pragma once
#include <string>
#include <iostream>
#include "FileFunction.h"
#include "Common.h"


void CLog::setOutPutOption(bool vTxt, bool vStand)
{
	m_TxtRequire = vTxt;
	m_StandRequire = vStand;
}

CLog::CLog() : m_TxtRequire(true), m_StandRequire(true), m_TimeRequire(true),m_multiThreadMutex(false)
{
	if (!isExist("./logFiles/"))
		system("mkdir ./logFiles/");
	m_logFileName = "./logFiles/" + m_SystemTime.outputDateHour()+"_"+std::to_string(getThreadID()) + ".txt";
}

void CLog::flush()
{
	m_TxtAppander.flush();
}

CLog::~CLog()
{
	m_TxtAppander.close();
}

void CLog::__outputCurrentTime()
{
	if (m_TimeRequire)
	{
		auto Time = m_SystemTime.outputTime();
		if (m_TxtRequire) m_TxtAppander << Time << std::endl;
		if (m_StandRequire) std::cout << Time << std::endl;;
	}
	if (m_TxtRequire && !m_TxtAppander.is_open())
	{
		setLogFileName(m_logFileName);
		if (m_TxtRequire && !m_TxtAppander.is_open())
			m_TxtRequire = false;
	}
}

void CLog::setLogFileName(const std::string& vFileName)
{
	if (vFileName.empty())
		return;
	m_logFileName = vFileName;
	if (m_TxtRequire)
	{
		if (m_TxtAppander.is_open())
			m_TxtAppander.close();
		openFileStream(m_logFileName, std::ios::app, m_TxtAppander, "Fail to  open log file of " + m_logFileName);
	}
}

const std::string& CConfiger::readValue(const std::string& vKey)
{
	auto iValue = m_ConfigDataMap.find(toUpperCopy(vKey));
	if (iValue != m_ConfigDataMap.end())
		return iValue->second;
	static std::string NoValue = "";
	return NoValue;
}

void CConfiger::__parseConfigInfor(std::ifstream& vIFS)
{
	std::string Line;
	std::vector < std::string > KeyValue;
	while (getline(vIFS, Line))
	{
		trim(Line);
		if (!Line.empty() && false == isStartWith(Line, "//"))
		{
			splitString(Line, KeyValue, ' ');
			if (KeyValue.size() > 1)
				m_ConfigDataMap[toUpperCopy(KeyValue.front())] = KeyValue[1];
		}
	}
}

void CConfiger::appandConfigInfor(const std::string& vConfigFileName)
{
	std::string ConfigFileName = toUpperCopy(vConfigFileName);
	std::string& Flag = m_ConfigDataMap[ConfigFileName];
	if (Flag != "") Flag = "Flag";
	else
	{
		std::ifstream IFS(vConfigFileName);
		if (IFS.fail())
		{
			CLog *pLog = CLog::getOrCreateInstance();
			pLog->setOutPutOption(true, true);
			pLog->setLogTime(true);
			pLog->output("Can't open configure file: " + vConfigFileName);
		}
		else
		{
			__parseConfigInfor(IFS);
		}
		IFS.close();
	}
}

CConfiger* CConfiger::getOrCreateConfiger(const std::string& vConfigFileName /*= "Configer.txt"*/)
{
	CConfiger* pConfig = getOrCreateInstance();
	pConfig->appandConfigInfor(vConfigFileName);
	return pConfig;
}

void SSystemTime::refresh()
{
	auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm* ptm = localtime(&tt);
	this->wYear = ptm->tm_year+1900;
	this->wMonth = ptm->tm_mon+1;
	this->wDay = ptm->tm_mday;
	this->wHour = ptm->tm_hour;
	this->wMinute = ptm->tm_min;
	this->wSecond = ptm->tm_sec;
	std::chrono::milliseconds now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	this->wMilliseconds = now_ms.count() % 1000;
}

const std::string SSystemTime::outputDate(char vSplit/*='_'*/)
{
	refresh();
	std::string Date;
	Date.resize(10);
	sprintf((char*)Date.c_str(), "%4d%c%2d%c%2d", wYear, vSplit, wMonth, vSplit, wDay);
	return Date;
}

const std::string SSystemTime::outputTime(char vSplit/*=':'*/)
{
	refresh();
	std::string Time;
	Time.resize(12);
	sprintf((char*)Time.c_str(), "%2d%c%2d%c%2d%c%3d", wHour, vSplit, wMinute, vSplit, wSecond, vSplit, wMilliseconds);
	return Time;
}


const std::string SSystemTime::outputDateHour(char vSplit /*= '_'*/)
{
	refresh();
	std::string DateHour;
	DateHour.resize(13);
	int Lenth = sprintf((char*)DateHour.c_str(), "%d%c%d%c%d%c%d", wYear, vSplit, wMonth, vSplit, wDay, vSplit, wHour);
	DateHour.resize(Lenth);
	return DateHour;
}

CFactoryDirectory::CFactoryDirectory(void) : m_pLog(CLog::getOrCreateInstance())
{

}

void* CFactoryDirectory::createProduct(std::string vSig)
{
	if (!existFactory(vSig))
		return NULL;
	trim(vSig);
	toUpper(vSig);
	auto iFactory = m_FactoryMap.find(vSig);

	if (m_FactoryMap.end() == iFactory)
	{
#ifdef _DEBUG
		vSig += "D.dll";
#else
#ifdef DEBUG
		vSig += "D.dll";
#endif // DEBUG
		vSig += ".dll";
#endif // DEBUG
		vSig = assembleFullFileName(m_DllPath, vSig);
// 		HINSTANCE hInstLibrary = ::LoadLibrary(vSig.c_str());
// 		if (NULL != hInstLibrary)
// 		{
// 			m_DllSet.push_back(hInstLibrary);
// 			iFactory = m_FactoryMap.find(vSig);
// 		}
// 		else
// 		{
// 			m_pLog->output("Can not find dll file : " + vSig);
// 		}
	}

	if (iFactory != m_FactoryMap.end())
		return iFactory->second->_createProductV();
	return  NULL;
}

void CFactoryDirectory::registerFactory(CBaseFactory *vpFactory, const std::string& vSig)
{
	m_FactoryMap[toUpperCopy(vSig)] = vpFactory;
}

void CFactoryDirectory::setDllSearchPath(const std::string& vDllPath /*= "./"*/)
{
	m_DllPath = vDllPath;
}

bool CFactoryDirectory::existFactory(const std::string& vSigName)
{
	std::string vSig = vSigName;
	trim(vSig);
	if (vSig.empty()) 
		return false;
	toUpper(vSig);

	auto iFactory = m_FactoryMap.find(vSig);
	return iFactory != m_FactoryMap.end();
}

CFactoryDirectory::~CFactoryDirectory(void)
{
	while (m_DllSet.size())
	{
// 		FreeLibrary(m_DllSet.back());
		m_DllSet.pop_back();
	}
}

unsigned int getThreadID()
{
	auto Tid = std::this_thread::get_id();
	return *(unsigned int*)&Tid;
}
