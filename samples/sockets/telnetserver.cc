//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "telnetserver.h"
#include "telnetpkt_m.h"
#include <ctype.h>


Define_Module( TelnetServer );

simtime_t TelnetServer::startService(cMessage *msg)
{
    ev << "Starting service of " << msg->name() << endl;
    return par("serviceTime");
}

void TelnetServer::endService(cMessage *msg)
{
    ev << "Completed service of " << msg->name() << endl;

    TelnetPkt *telnetPkt = check_and_cast<TelnetPkt *>(msg);

    std::string reply = processChars(telnetPkt->getPayload());
    telnetPkt->setPayload(reply.c_str());
    telnetPkt->setName(reply.c_str());

    int clientAddr = telnetPkt->getSrcAddress();
    int srvAddr = telnetPkt->getDestAddress();
    telnetPkt->setDestAddress(clientAddr);
    telnetPkt->setSrcAddress(srvAddr);

    send(msg, "out");
}

std::string TelnetServer::processChars(const char *chars)
{
    std::string s = chars;
    for (int i=0; i<s.length(); i++)
        s.at(i) = toupper(s.at(i));
    return s;
}


