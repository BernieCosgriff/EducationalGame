/**
 * @file dust.h
 * @brief Little embarrassed by this, since all of these objects have mostly the same functionality, but use tons of different classes anyway! Each dust spawns an animation (and sound), then deletes itself. That's it!
 * @author Dalton Nell
 * @version 0.0.0
 * @date 2016-12-06
 */

#ifndef LQ_DASHDUST_H_
#define LQ_DASHDUST_H_

#include <SFML/Audio/Sound.hpp>

#include "AnimatedSprite.hpp"
#include "entity.h"
#include "world.h"
#include "random.h"

class DashDust : public Entity {
private:
    sf::Texture* texture;
    Animation animation;
    AnimatedSprite* sprite;
public:
    DashDust( glm::vec2 pos, bool flipped, float angle );
    ~DashDust();
    void update(double dt);
    void draw(sf::RenderTarget& window);
    Entity::Type getType();
};

class JumpDust : public Entity {
private:
    sf::Texture* texture;
    Animation animation;
    AnimatedSprite* sprite;
public:
    JumpDust( glm::vec2 pos, float angle, bool doub = false );
    ~JumpDust();
    void update(double dt);
    void draw(sf::RenderTarget& window);
    Entity::Type getType();
};

class LandingDust : public Entity {
private:
    sf::Texture* texture;
    Animation animation;
    AnimatedSprite* sprite;
public:
    LandingDust( glm::vec2 pos, float angle );
    ~LandingDust();
    void update(double dt);
    void draw(sf::RenderTarget& window);
    Entity::Type getType();
};

class WallJumpDust : public Entity {
private:
    sf::Texture* texture;
    Animation animation;
    AnimatedSprite* sprite;
public:
    WallJumpDust( glm::vec2 pos, bool flipped, float angle );
    ~WallJumpDust();
    void update(double dt);
    void draw(sf::RenderTarget& window);
    Entity::Type getType();
};

class ShockDust : public Entity {
private:
    sf::Sound sound;
    sf::Texture* texture;
    Animation animation;
    AnimatedSprite* sprite;
public:
    ShockDust( glm::vec2 pos );
    ~ShockDust();
    void update(double dt);
    void draw(sf::RenderTarget& window);
    Entity::Type getType();
};

class PokeDust : public Entity {
private:
    sf::Sound sound;
    sf::Texture* texture;
    Animation animation;
    AnimatedSprite* sprite;
public:
    PokeDust( glm::vec2 pos );
    ~PokeDust();
    void update(double dt);
    void draw(sf::RenderTarget& window);
    Entity::Type getType();
};

class LavaDust : public Entity {
private:
    sf::Sound sound;
    sf::Texture* texture;
    Animation animation;
    AnimatedSprite* sprite;
public:
    LavaDust( glm::vec2 pos );
    ~LavaDust();
    void update(double dt);
    void draw(sf::RenderTarget& window);
    Entity::Type getType();
};

class FireworkDust : public Entity {
private:
    sf::Sound sound;
    sf::Texture* texture;
    Animation animation;
    AnimatedSprite* sprite;
public:
    FireworkDust( glm::vec2 pos );
    ~FireworkDust();
    void update(double dt);
    void draw(sf::RenderTarget& window);
    Entity::Type getType();
};

#endif
