//
// Created by Juscelino Tanaka on 21/05/17.
//

#ifndef QUEUELIB_SERVERLIST_H
#define QUEUELIB_SERVERLIST_H

#include "TimedQueue.h"

// server holds unnecessary informations for this scenario.
class Server {
public:
    int id;
    double nextAvailableTime;

    Server(int i) { this->id = i; }

    int totalExecuted;
};

class ServerList {
public:
    ServerList(int totalServers);
    void RunQueue(TimedQueue * queue);
    Server *GetFirstAvailable(double arrivalTime);
    Server *GetLessTired();

private:
    vector<Server> servers;
};


#endif //QUEUELIB_SERVERLIST_H
