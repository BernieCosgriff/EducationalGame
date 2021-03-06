/**
 * @file playerstate.h
 * @brief Contains all the states for the player, or maybe lots of spaghetti. It's hard to tell. \
 *        init() exists since we don't actually want the states to do stuff on creation, only when the player\
 *        actually switches states.
 * @author Dalton Nell
 * @version 0.0.0
 * @date 2016-12-06
 */

#ifndef LQ_PLAYERSTATE_H_
#define LQ_PLAYERSTATE_H_

#include <tweeny.h>

#include "player.h"
#include "map.h"

class Player;

static const std::string StateString[] = { "Idle", "Dashing", "Running", "Walking", "JumpSquat", "Airborne", "SpecialFall", "Jumping", "AirDodge", "Turning", "Landing", "Shocked", "KnockBack", "KnockBackRecover", "Win", "None" };
enum PlayerState { Idle, Dashing, Running, Walking, JumpSquat, Airborne, SpecialFall, Jumping, AirDodge, Turning, Landing, Shocked, KnockBack, KnockBackRecover, Win, None };

class GenericPlayerState {
public:
    Player* player;
    GenericPlayerState( Player* player );
    GenericPlayerState();
    virtual void init();
    virtual ~GenericPlayerState();
    virtual PlayerState getType();
    virtual void update( Player* player, double dt );
};

class WalkingState : public GenericPlayerState {
public:
    WalkingState( Player* player );
    ~WalkingState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class IdleState : public GenericPlayerState {
public:
    double walkTimer;
    bool reset;
    IdleState( Player* player );
    ~IdleState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class DashingState : public GenericPlayerState {
public:
    float dashingDirection;
    double walkTimer;
    double dashTimer;
    DashingState( Player* player, float direction );
    ~DashingState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class JumpSquatState : public GenericPlayerState {
public:
    double jumpSquatTimer;
    JumpSquatState( Player* player );
    ~JumpSquatState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class JumpingState : public GenericPlayerState {
public:
    double walkTimer;
    float wallJumpDirection;
    JumpingState( Player* player, float wallJumpDirection = 0 );
    ~JumpingState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class AirborneState : public GenericPlayerState {
public:
    double walkTimer;
    AirborneState( Player* player );
    ~AirborneState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class RunningState : public GenericPlayerState {
public:
    bool crossedNeutral;
    double walkTimer;
    float dashingDirection;
    RunningState( Player* player, float direction );
    ~RunningState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class AirDodgeState : public GenericPlayerState {
    glm::vec2 airDirection;
    glm::vec2 vel;
    tweeny::tween<float> tweenX,tweenY;
public:
    AirDodgeState( Player* player, glm::vec2 airdirection );
    ~AirDodgeState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class SpecialFallState : public GenericPlayerState {
public:
    SpecialFallState( Player* player);
    ~SpecialFallState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class LandingState : public GenericPlayerState {
public:
    GenericPlayerState* bufferedState;
    float walkTimer;
    bool reset;
    float landTimer;
    LandingState( Player* player);
    ~LandingState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class TurningState : public GenericPlayerState {
    tweeny::tween<float> tweenX;
    tweeny::tween<float> tweenY;
    float turnDirection;
    float turnTimer;
public:
    TurningState( Player* player, float direction);
    ~TurningState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class ShockedState : public GenericPlayerState {
public:
    float duration;
    int type;
    float shockTimer;
    glm::vec2 impulse;
    ShockedState( Player* player, glm::vec2 impulse, int type=0, float duration=1);
    ~ShockedState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class KnockbackState : public GenericPlayerState {
public:
    glm::vec2 impulse;
    KnockbackState( Player* player, glm::vec2 impulse);
    ~KnockbackState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class KnockbackRecoverState : public GenericPlayerState {
public:
    bool teched;
    KnockbackRecoverState( Player* player, bool teched = false);
    ~KnockbackRecoverState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

class WinState : public GenericPlayerState {
public:
    float timer;
    tweeny::tween<float> tweenW;
    tweeny::tween<float> tweenH;
    tweeny::tween<int> tweenC;
    WinState( Player* player );
    ~WinState();
    void init();
    PlayerState getType();
    void update( Player* player, double dt );
};

#endif
