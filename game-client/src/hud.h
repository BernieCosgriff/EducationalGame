/**
 * @file hud.h
 * @brief Spawns a cute hud so the player knows how terrible he/she's doing.
 * @author Zach Frye
 * @version 0.0.0
 * @date 2016-12-06
 */

#ifndef LQ_HUD_H_
#define LQ_HUD_H_

#include "entity.h"
#include "playerstats.h"
#include "world.h"
#include "player.h"
#include <sstream>

/**
 * @brief Spawns a cute hud so the player knows how terrible he/she's doing.
 */
class HUD : public Entity {
public:
    HUD();
    sf::Text scoreText;
    void update(double dt);
    void onHit(Entity* collider, b2Contact* c, b2Vec2 hitnormal);
    void draw(sf::RenderTarget& window);
    Entity::Type getType();
};

#endif
