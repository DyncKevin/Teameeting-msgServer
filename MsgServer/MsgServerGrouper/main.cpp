//
//  main.cpp
//  MsgServerGrouper
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include "rtklog.h"
#include "GRTGrouper.h"
#include "RTZKClient.hpp"
#include <google/protobuf/message.h>

#ifndef _TEST_
#define _TEST_ 0
#endif

int main(int argc, const char * argv[]) {
    printf("Hello, Grouper!!!\n");
    GRTGrouper::PrintVersion();

    if (argc <= 1) {
        std::cout << "Error: Please usage:$0 {conf_path} " << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

#if _TEST_
    if (RTZKClient::Instance().InitOnly(argv[1])!=0) {
#else
    if (RTZKClient::Instance().InitZKClient(argv[1])!=0) {
#endif
        std::cout << "Please check the config file ..." << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    RTConfigParser conf;
    conf.LoadFromFile(argv[2]);

    int nLogLevel = conf.GetIntVal("log", "level", 0);
    std::string strLogPath = conf.GetValue("log", "path", "./loggrouper.log");
    if (nLogLevel < 0 || nLogLevel > 5)
    {
        std::cout << "Error: Log level=" << nLogLevel << " extend range(0 - 5)!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }
    L_Init(nLogLevel, strLogPath.c_str());

    GRTGrouper::Initialize(1024);
    GRTGrouper* pGrouper = GRTGrouper::Inst();
    //int test = 0;
    int res = pGrouper->Start(conf);
    if (res != 0) {
        LI("GRTGrouper start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }
    //while (test++ < 30) {
    while (1) {
        pGrouper->DoTick();
        sleep(1);
        //break;
    }
        sleep(1);
EXIT:
    pGrouper->Stop();
    GRTGrouper::DeInitialize();
    L_Deinit();
    RTZKClient::Instance().Unin();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
