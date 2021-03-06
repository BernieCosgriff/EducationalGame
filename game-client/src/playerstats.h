/**
 * @file playerstats.h
 * @brief Links the game to the server, as well as tracks some statistical information about the player.
 * @author Dalton Nell
 * @version 0.0.0
 * @date 2016-12-06
 */

#ifndef LQ_PLAYERSTATS_H_
#define LQ_PLAYERSTATS_H_

#include <exception>
#include <iostream>

#ifdef WIN32
#include "../../shared/loqueclient.h"
#else
#include "../shared/loqueclient.h"
#endif

class PlayerStats {
private:
    int userId;
    double time;
    GameStats stats;
    LoqueClient client;
public:
    void startTime();
    PlayerStats(int userId, int mapId);
    void sendStats();
    void setScore(double s);
    void update(double dt);
    double getScore();
};

extern PlayerStats *playerStats;

#endif
