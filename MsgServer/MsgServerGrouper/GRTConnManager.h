//
//  GRTConnManager.h
//  MsgServerGrouper
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerGrouper__GRTConnManager__
#define __MsgServerGrouper__GRTConnManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include "OSMutex.h"
#include "RTSingleton.h"
#include "RTType.h"
#include "RTEventTimer.h"

#include "ProtoCommon.h"

class GRTTransferSession;

class GRTConnManager : public RTSingleton< GRTConnManager >{
    friend class RTSingleton< GRTConnManager >;
public:
    typedef struct _ModuleInfo{
        int             flag;
        pms::ETransferModule  othModuleType;
        std::string     othModuleId;
        GRTTransferSession*     pModule;
        _ModuleInfo() {
            flag = 0;
            othModuleType = pms::ETransferModule::MGRPNOTIFY;
            othModuleId = "";
            pModule = NULL;
        }
    }ModuleInfo;

    //store  module usage information
    //meet meet123456  session123456
    //msgqueue queue123456  session234567
    typedef struct _TypeModuleSessionInfo{
        pms::ETransferModule moduleType;
        std::string moduleId;
        std::set<std::string> sessionIds;
        _TypeModuleSessionInfo() {
            moduleType = pms::ETransferModule::MGRPNOTIFY;
            moduleId = "";
            sessionIds.clear();
        }
    }TypeModuleSessionInfo;

    typedef std::unordered_map< std::string, ModuleInfo* >      ModuleInfoMaps;
    typedef ModuleInfoMaps::iterator                            ModuleInfoMapsIt;
    //<user_id, UserModuleTypeInfo>
    typedef std::list<TypeModuleSessionInfo*>                   TypeModuleSessionInfoLists;
    typedef std::unordered_multimap<std::string, std::string>   UserConnectorMaps;
    typedef UserConnectorMaps::iterator                         UserConnectorMapsIt;
    typedef std::list< GRTTransferSession* >                    ConnectingSessList;

    ///////////////////////////////////////////////////////////////////////////////////

    ModuleInfo* findConnectorInfo(const std::string& userid);
    ModuleInfo* findModuleInfo(const std::string& userid, pms::ETransferModule module);
    ModuleInfo* findModuleInfoBySid(const std::string& sid);
    ModuleInfo* findConnectorInfoById(const std::string& userid, const std::string& connector);

    bool AddModuleInfo(ModuleInfo* pmi, const std::string& sid);
    bool DelModuleInfo(const std::string& sid, EventData& data);
    bool AddTypeModuleSession(pms::ETransferModule module, const std::string& mid, const std::string& sid);
    bool DelTypeModuleSession(const std::string& sid);

    void TransferSessionLostNotify(const std::string& sid);

    ///////////////////////////////////////////////////////////////////////////////////

    bool    ConnectGroupMgr();
    void    RefreshConnection();

    void OnTLogin(const std::string& uid, const std::string& token, const std::string& connector);
    void OnTLogout(const std::string& uid, const std::string& token, const std::string& connector);

    bool    SignalKill();
    bool    ClearAll();

    std::list<std::string>* GetGroupMgrAddrList() { return &m_groupMgrAddrList; }
    void SetGrouperId(const std::string& mid) { m_grouperId = mid; }
    std::string& GrouperId() { return m_grouperId; }
    void SetSvrRTLive(bool ok) { m_isSvrRTLiveOk = ok; }
    bool IsSvrRTLive() { return m_isSvrRTLiveOk; }

protected:
    GRTConnManager() { }
    ~GRTConnManager() { }

private:
    bool DoConnectGroupMgr(const std::string ip, unsigned short port);

private:
    bool                      m_isSvrRTLiveOk;
    std::list<std::string>    m_groupMgrAddrList;
    std::string               m_grouperId;
    OSMutex                   m_mutexMembers;
    UserConnectorMaps         m_userConnectors;
};

#endif /* defined(__MsgServerGrouper__GRTConnManager__) */
