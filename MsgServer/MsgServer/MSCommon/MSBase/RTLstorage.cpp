//
//  RTLstorage.cpp
//
//  Created by hp on 12/1/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <sstream>
#include "RTLstorage.h"
#include "http_common.h"

RTLstorage::RTLstorage()
{

}

RTLstorage::~RTLstorage()
{

}

int RTLstorage::DoProcessData(const char *pData, int nLen)
{
    pms::TransferMsg m_msg;
    const std::string msg(pData, nLen);
    if (!m_msg.ParseFromString(msg)) {
        LE("ParseFromString error\n");
        return nLen;
    }

    {
        if (m_msg.flag() == pms::ETransferFlag::FNEEDACK) {
            OnMsgAck(m_msg);
        } else if (m_msg.flag() == pms::ETransferFlag::FACK) {
            // handle ack
            return nLen;
        } else {
            //return nLen;
        }
    }

    {
        if (m_msg.type() == pms::ETransferType::TCONN) {
            OnTypeConn(m_msg.content());
        } else if (m_msg.type() == pms::ETransferType::TWRITE_REQUEST) {
            OnTypeWriteRequest(m_msg.content());
        } else if (m_msg.type() == pms::ETransferType::TWRITE_RESPONSE) {
            OnTypeWriteResponse(m_msg.content());
        } else if (m_msg.type() == pms::ETransferType::TREAD_REQUEST) {
            OnTypeReadRequest(m_msg.content());
        } else if (m_msg.type() == pms::ETransferType::TREAD_RESPONSE) {
            OnTypeReadResponse(m_msg.content());
        } else if (m_msg.type() == pms::ETransferType::TTRANS) {
            OnTypeTrans(m_msg.content());
        } else if (m_msg.type() == pms::ETransferType::TQUEUE) {
            OnTypeQueue(m_msg.content());
        } else if (m_msg.type() == pms::ETransferType::TDISPATCH) {
            OnTypeDispatch(m_msg.content());
        } else if (m_msg.type() == pms::ETransferType::TPUSH) {
            OnTypePush(m_msg.content());
        } else if (m_msg.type() == pms::ETransferType::TERROR) {
            OnTypeError(m_msg.content());
        } else {
            LE("invalid type::%d", m_msg.type());
        }
    }
    return nLen;
}
