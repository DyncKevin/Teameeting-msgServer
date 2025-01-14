#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "LRTLogical.h"
#include "atomic.h"
#include "OS.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "RTUtils.hpp"
#include "LRTLogicalManager.h"
#include "LRTConnManager.h"
#include "StatusCode.h"
#include "RTZKClient.hpp"


static bool		g_inited = false;
static char*	g_pVersion = (char*)"0.01.20150810";
static LRTLogical*	g_pLogical = NULL;

void LRTLogical::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void LRTLogical::Initialize(int evTbSize)
{
	// Initialize utility classes
    LI("Hello server...\n");
	bool inDontFork = false;
	OS::Initialize();//initialize
	OSThread::Initialize();// initialize
    IdleTask::Initialize();// initialize IdleTaskThread

	// Initialize socket classes
	Socket::Initialize(evTbSize);// initialize EventThread
	SocketUtils::Initialize(!inDontFork);
#if !MACOSXEVENTQUEUE
	//Initialize the select() implementation of the event queue
	::select_startevents();
#endif

	{
		SInt32 numShortTaskThreads = 0;
		SInt32 numBlockingThreads = 0;
		SInt32 numThreads = 0;
		SInt32 numProcessors = 0;
		numProcessors = OS::GetNumProcessors();
		//////LI("Processer core Num:%lu\n",numProcessors);
		// Event thread need 1 position;
		numShortTaskThreads = numProcessors*2 - 1;
		if(numShortTaskThreads == 0)
			numShortTaskThreads = 2;
		numBlockingThreads = 1;
		numThreads = numShortTaskThreads + numBlockingThreads;
		// ////LI("Add threads shortask=%lu blocking=%lu\n",numShortTaskThreads, numBlockingThreads);
		TaskThreadPool::SetNumShortTaskThreads(numShortTaskThreads);
		TaskThreadPool::SetNumBlockingTaskThreads(numBlockingThreads);
		TaskThreadPool::AddThreads(numThreads);//initialize TaskThread & Start
	}

	TimeoutTask::Initialize();// initialize Task

	Socket::StartThread();// start EventThread
	OSThread::Sleep(100);
	g_pLogical = new LRTLogical();
	g_inited = true;
}

void LRTLogical::DeInitialize()
{
	delete g_pLogical;
	g_pLogical = NULL;
	g_inited = false;
	TaskThreadPool::RemoveThreads();

	//Now, make sure that the server can't do any work
	TimeoutTask::UnInitialize();

	SocketUtils::UnInitialize();
    sleep(1);
    IdleTask::UnInitialize();

#if !MACOSXEVENTQUEUE
	::select_stopevents();
#endif

	Socket::Uninitialize();// uninitialize EventThread
	OS::UnInitialize();
    LI("ByeBye server...\n");
}

LRTLogical* LRTLogical::Inst()
{
	Assert(g_pLogical != NULL);
	return g_pLogical;
}

void LRTLogical::NodeAddCb(const std::string& nodePath)
{
}

void LRTLogical::NodeDelCb(const std::string& nodePath)
{
}


LRTLogical::LRTLogical(void)
: m_pLogicalListener(NULL)
{

}

LRTLogical::~LRTLogical(void)
{
    if (m_pLogicalListener)
    {
        delete m_pLogicalListener;
        m_pLogicalListener = NULL;
    }
}

int	LRTLogical::Start(const RTConfigParser& conf)
{
	Assert(g_inited);
    int debug = conf.GetIntVal("global", "debug", 1);

    std::string strLocalIp("");
    std::string strSequenceIp("");
    std::string strStorageIp("");
    std::string strHttpIp("");
    if (debug==1)
    {
        strLocalIp = conf.GetValue("global", "logical_int_ip", "127.0.0.1");
        strSequenceIp = conf.GetValue("global", "sequence_int_ip", "127.0.0.1");
        strStorageIp = conf.GetValue("global", "storage_int_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_int_ip", "127.0.0.1");
    } else {
        strLocalIp = conf.GetValue("global", "logical_ext_ip", "127.0.0.1");
        strSequenceIp = conf.GetValue("global", "sequence_ext_ip", "127.0.0.1");
        strStorageIp = conf.GetValue("global", "storage_ext_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_ext_ip", "127.0.0.1");
    }
    if (strLocalIp.length()==0 || strSequenceIp.length()==0 || strStorageIp.length()==0 || strHttpIp.length()==0) {
        std::cout << "Error: Ip length is 0!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    int nSequencePort = conf.GetIntVal("global", "listen_sequence_port", 6650);
    int nStoragePort = conf.GetIntVal("global", "listen_storage_port", 6660);
    int nLogicalPort = conf.GetIntVal("global", "listen_logical_port", 6670);
    int nHttpPort = conf.GetIntVal("resetful", "listen_http_port", 8055);

    LRTLogicalManager::Instance().InitManager();

	if(nSequencePort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %d", strSequenceIp.c_str(), nSequencePort);
        LRTConnManager::Instance().GetSequenceAddrList()->push_front(addr);

        if (!(LRTConnManager::Instance().ConnectSequence())) {
            LE("Start to ConnectSequence failed\n");
            return -1;
        }
        LRTConnManager::Instance().SetSvrSequence(true);
	}
    LI("SequenceServer should already be connected!!!\n\n");

    if(nStoragePort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", strStorageIp.c_str(), nStoragePort);
        LRTConnManager::Instance().GetStorageAddrList()->push_front(addr);

        if (!(LRTConnManager::Instance().ConnectStorage())) {
            LE("Start to ConnectStorage failed\n");
            return -1;
        }
        LRTConnManager::Instance().SetSvrStorage(true);
	}
    LI("StorageServer should already be connected!!!\n\n");

    std::string ssid;
    if (nLogicalPort > 0) {
        m_pLogicalListener = new LRTLogicalListener();
        OS_Error err = m_pLogicalListener->Initialize(INADDR_ANY, nLogicalPort);
        if (err!=OS_NoErr) {
            LE("CreateLogicalListener error port:%d\n", nLogicalPort);
            delete m_pLogicalListener;
            m_pLogicalListener = NULL;
            return -1;
        }
        LI("Start Logical service:(%d) ok...,socketFD:%d\n", nLogicalPort, m_pLogicalListener->GetSocketFD());
        m_pLogicalListener->RequestEvent(EV_RE);
    }

    RTZKClient::Instance().SetNodeCallback(LRTLogical::NodeAddCb, LRTLogical::NodeDelCb);
    mServerSession.Init();
	return 0;
}

void LRTLogical::DoTick()
{
#if 1
    //LRTLogicalManager::Instance().GenerateLogical();
    LI("LRTLogical::DoTick IsSvrSequence ok:%d, IsSvrStorage ok:%d\n", LRTConnManager::Instance().IsSvrSequence(), LRTConnManager::Instance().IsSvrStorage());
    if (!LRTConnManager::Instance().IsSvrSequence())
    {
        // svr sequence is not ok
        if ((LRTConnManager::Instance().ConnectSequence())) {
            LRTConnManager::Instance().SetSvrSequence(true);
            LRTConnManager::Instance().ReportError(pms::ETransferModule::MSEQUENCE, "", ERR_STR_SVR_OK, ERR_CODE_SVR_OK);

        }
    }
    if (!LRTConnManager::Instance().IsSvrStorage())
    {
        // svr storage is not ok
        if ((LRTConnManager::Instance().ConnectStorage())) {
            LRTConnManager::Instance().SetSvrStorage(true);
            LRTConnManager::Instance().ReportError(pms::ETransferModule::MSTORAGE, "", ERR_STR_SVR_OK, ERR_CODE_SVR_OK);
        }
    }
#endif
}

void LRTLogical::Stop()
{
    mServerSession.Unin();
    LRTLogicalManager::Instance().SignalKill();
    LRTLogicalManager::Instance().ClearAll();
    LRTLogicalManager::Instance().UninManager();
    LRTConnManager::Instance().SignalKill();
    LRTConnManager::Instance().ClearAll();
}
