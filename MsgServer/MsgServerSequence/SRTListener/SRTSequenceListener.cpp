//
//  SRTSequenceListener.cpp
//  dyncRTSequence
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTSequenceListener.h"
#include "SRTTransferSession.h"
#include "SRTSequenceManager.h"
#include "StatusCode.h"

Task* SRTSequenceListener::GetSessionTask(int osSocket, struct sockaddr_in* addr)
{
    TCPSocket* theSocket = NULL;
    Assert(osSocket != EventContext::kInvalidFileDesc);

    // when the server is behing a round robin DNS, the client needs to knwo the IP address ot the server
    // so that it can direct the "POST" half of the connection to the same machine when tunnelling RTSP thru HTTP

    SRTTransferSession* theTask = new SRTTransferSession();
    if(NULL == theTask)
        return NULL;
    theSocket = theTask->GetSocket();  // out socket is not attached to a unix socket yet.

    //set options on the socket
    theSocket->Set(osSocket, addr);
    theSocket->InitNonBlocking(osSocket);
    //we are a server, always disable nagle algorithm
    theSocket->NoDelay();
    theSocket->KeepAlive();
    theSocket->SetSocketBufSize(MAX_SOCKET_BUF_32);
    theSocket->SetSocketRcvBufSize(MAX_SOCKET_BUF_64);

    //setup the socket. When there is data on the socket,
    //theTask will get an kReadEvent event
    theSocket->RequestEvent(EV_RE);
    theTask->SetTimer(120*1000);

    StrPtrLen* remoteStr = theSocket->GetRemoteAddrStr();
    //LI("SRTSequenceListener Get a connection,ip:%.*s port:%d \n",remoteStr->Len, remoteStr->Ptr, ntohs(addr->sin_port));

    theTask->InitConf();
    this->RunNormal();

    return theTask;
}
