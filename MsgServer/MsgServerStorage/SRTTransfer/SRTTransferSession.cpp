//
//  SRTTransferSession.cpp
//  dyncRTStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <list>
#include "SRTTransferSession.h"
#include "RTUtils.hpp"
#include "SRTConnManager.h"
#include "SRTStorageManager.h"
#include "StatusCode.h"

#define TIMEOUT_TS (60*1000)

SRTTransferSession::SRTTransferSession()
: RTJSBuffer()
, RTLstorage()
, m_lastUpdateTime(0)
, m_moduleId("")
, m_transferSessId("")
, m_addr("")
, m_port(0)
, m_connectingStatus(0)
, m_IsValid(true)
{
    AddObserver(this);
    GenericSessionId(m_transferSessId);
}

SRTTransferSession::~SRTTransferSession()
{
    m_IsValid = false;
    while(m_RecvMsgBuf.size()>0)
    {
         m_RecvMsgBuf.pop();
    }
    DelObserver(this);
    LI("SRTTransferSession::~SRTTransferSession ......\n");
    Unit();
}

void SRTTransferSession::Init()
{
    TCPSocket* socket = this->GetSocket();

    socket->Open();

    socket->InitNonBlocking(socket->GetSocketFD());
    socket->NoDelay();
    socket->KeepAlive();
    socket->SetSocketBufSize(MAX_SOCKET_BUF_32);
    socket->SetSocketRcvBufSize(MAX_SOCKET_BUF_64);


    socket->SetTask(this);
    this->SetTimer(120*1000);

    m_RedisManager.Init(this);
}

void SRTTransferSession::InitConf()
{
    m_RedisManager.Init(this);
}
void SRTTransferSession::Unit()
{
    m_RedisManager.Unin();
    Disconn();
    m_connectingStatus = 0;
}

bool SRTTransferSession::Connect(const std::string addr, int port)
{
    m_addr = addr;
    m_port = port;
    OS_Error err = GetSocket()->Connect(SocketUtils::ConvertStringToAddr(m_addr.c_str()), m_port);
    if (err == OS_NoErr || err == EISCONN) {
        m_connectingStatus = 1;
        return true;
    } else {
        LE("%s ERR:%d\n", __FUNCTION__, err);
        return false;
    }
}

bool SRTTransferSession::Connect()
{
    if (m_addr.empty() || m_port < 2048) {
        LE("%s invalid params addr:%s, port:%d\n", __FUNCTION__, m_addr.c_str(), m_port);
        return false;
    }
    OS_Error err = GetSocket()->Connect(SocketUtils::ConvertStringToAddr(m_addr.c_str()), m_port);
    if (err == OS_NoErr || err == EISCONN) {
        m_connectingStatus = 1;
        return true;
    } else {
        LE("%s ERR:%d\n", __FUNCTION__, err);
        return false;
    }
}

void SRTTransferSession::Disconn()
{
    GetSocket()->Cleanup();
}

bool SRTTransferSession::RefreshTime()
{
    UInt64 now = OS::Milliseconds();
    if (m_lastUpdateTime <= now) {
        m_lastUpdateTime = now  + TIMEOUT_TS;
        RTTcpNoTimeout::UpdateTimer();
        return true;
    } else {
        return false;
    }
}

void SRTTransferSession::KeepAlive()
{
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MSTORAGE);
    c_msg.set_conn_tag(pms::EConnTag::TKEEPALIVE);

    t_msg.set_type(pms::ETransferType::TCONN);
    t_msg.set_flag(pms::ETransferFlag::FNOACK);
    t_msg.set_priority(pms::ETransferPriority::PNORMAL);
    t_msg.set_content(c_msg.SerializeAsString());

    std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
}

void SRTTransferSession::EstablishConnection()
{
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MSTORAGE);
    c_msg.set_conn_tag(pms::EConnTag::THI);

    t_msg.set_type(pms::ETransferType::TCONN);
    t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
    t_msg.set_priority(pms::ETransferPriority::PHIGH);
    t_msg.set_content(c_msg.SerializeAsString());

    std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
}

void SRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    SRTStorageManager::Instance().SendResponseCounter();
    RTTcpNoTimeout::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void SRTTransferSession::SendTransferData(const std::string& data)
{
    SendTransferData(data.c_str(), data.length());
}

// from RTTcpNoTimeout
void SRTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void SRTTransferSession::OnRedisEvent(const char*pData, int nLen)
{
    int64_t c=0;
    if (m_RecvMsgBuf.size()>0)
    {
        std::string v = m_RecvMsgBuf.front();
        RTLstorage::DoProcessData(v.c_str(), v.length());
        m_RecvMsgBuf.pop();
    }

    if (m_IsValid && m_RecvMsgBuf.size()>0)
    {
        this->NotifyRedis();
    }
}

void SRTTransferSession::OnRecvMessage(const char*message, int nLen)
{
#if USE_QUEUE_TO_CACHE
    //write redis to store msg
    std::string s(message, nLen);
    m_RecvMsgBuf.push(s);
    if (m_IsValid)
        this->NotifyRedis();
#else
	RTLstorage::DoProcessData(message, nLen);
#endif
}


// from RTTransfer

void SRTTransferSession::OnTransfer(const std::string& str)
{
    RTTcpNoTimeout::SendTransferData(str.c_str(), (int)str.length());
}

void SRTTransferSession::OnMsgAck(pms::TransferMsg& tmsg)
{
    pms::TransferMsg ack_msg;
    ack_msg.set_type(tmsg.type());
    ack_msg.set_flag(pms::ETransferFlag::FACK);
    ack_msg.set_priority(tmsg.priority());

    OnTransfer(ack_msg.SerializeAsString());
}

void SRTTransferSession::OnTypeConn(const std::string& str)
{
    pms::ConnMsg c_msg;
    if (!c_msg.ParseFromString(str)) {
        LE("OnTypeConn c_msg ParseFromString error\n");
        return;
    }

    if ((c_msg.conn_tag() == pms::EConnTag::THI)) {
        // when other connect to ME:
        // send the transfersessionid and StorageId to other
        pms::TransferMsg t_msg;

        c_msg.set_tr_module(pms::ETransferModule::MSTORAGE);
        c_msg.set_conn_tag(pms::EConnTag::THELLO);
        c_msg.set_transferid(m_transferSessId);
        //send self Storage id to other
        c_msg.set_moduleid(SRTConnManager::Instance().StorageId());

        t_msg.set_type(pms::ETransferType::TCONN);
        //this is for transfer
        t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
        t_msg.set_priority(pms::ETransferPriority::PHIGH);
        t_msg.set_content(c_msg.SerializeAsString());

        std::string s = t_msg.SerializeAsString();
        SendTransferData(s.c_str(), (int)s.length());
    } else if ((c_msg.conn_tag() == pms::EConnTag::THELLO)) {
        // when ME connector to other:
        // store other's transfersessionid and other's moduleId
        if (c_msg.transferid().length()>0) {
            m_transferSessId = c_msg.transferid();
            {
                SRTConnManager::ModuleInfo* pmi = new SRTConnManager::ModuleInfo();
                if (pmi) {
                    pmi->flag = 1;
                    pmi->othModuleType = c_msg.tr_module();
                    pmi->othModuleId = m_transferSessId;
                    pmi->pModule = this;
                    //bind session and transfer id
                    SRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                    //store which moudle connect to this connector
                    //c_msg._moduleid:store other's module id
                    LI("store other connector moduleid:%s, transfersessionid:%s\n", c_msg.moduleid().c_str(), m_transferSessId.c_str());
                    SRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
                } else {
                    LE("new ModuleInfo error!!!\n");
                }
            }

            pms::TransferMsg t_msg;

            c_msg.set_tr_module(pms::ETransferModule::MSTORAGE);
            c_msg.set_conn_tag(pms::EConnTag::THELLOHI);
            c_msg.set_transferid(m_transferSessId);
            //send self Storage id to other
            c_msg.set_moduleid(SRTConnManager::Instance().StorageId());

            t_msg.set_type(pms::ETransferType::TCONN);
            //this is for transfer
            t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
            t_msg.set_priority(pms::ETransferPriority::PHIGH);
            t_msg.set_content(c_msg.SerializeAsString());
            this->SetTestName(m_transferSessId);

            std::string s = t_msg.SerializeAsString();
            SendTransferData(s.c_str(), (int)s.length());
        } else {
            LE("Connection id:%s error!!!\n", c_msg.transferid().c_str());
        }
    } else if ((c_msg.conn_tag() == pms::EConnTag::THELLOHI)) {
        // when other connect to ME:
        if (m_transferSessId.compare(c_msg.transferid()) == 0) {
            SRTConnManager::ModuleInfo* pmi = new SRTConnManager::ModuleInfo();
            if (pmi) {
                pmi->flag = 1;
                pmi->othModuleType = c_msg.tr_module();
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                SRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                //store other module id
                LI("store module type:%d, moduleid:%s, transfersessid:%s\n", (int)c_msg.tr_module(), c_msg.moduleid().c_str(), m_transferSessId.c_str());
                SRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
            } else {
                LE("new ModuleInfo error!!!!\n");
            }
             this->SetTestName(m_transferSessId);
       }
    }  else if (c_msg.conn_tag() == pms::EConnTag::TKEEPALIVE) {
        RTTcpNoTimeout::UpdateTimer();
    } else {
        LE("%s invalid msg tag\n", __FUNCTION__);
    }
}

void SRTTransferSession::OnTypeTrans(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void SRTTransferSession::OnTypeQueue(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void SRTTransferSession::OnTypeDispatch(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void SRTTransferSession::OnTypePush(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void SRTTransferSession::OnTypeError(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void SRTTransferSession::OnTypeWriteRequest(const std::string& str)
{
    SRTStorageManager::Instance().RecvRequestCounter();
    m_RedisManager.PushRedisRequest(str);
}

void SRTTransferSession::OnTypeWriteResponse(const std::string& str)
{

}

void SRTTransferSession::OnTypeReadRequest(const std::string& str)
{
    SRTStorageManager::Instance().RecvRequestCounter();
    m_RedisManager.PostRedisRequest(str);
}

void SRTTransferSession::OnTypeReadResponse(const std::string& str)
{

}


void SRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        m_connectingStatus = 0;
        m_IsValid = false;
        SRTConnManager::Instance().TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

