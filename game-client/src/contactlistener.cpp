#include "contactlistener.h"

ContactListener::ContactListener()
{
    //ctor
}

ContactListener::~ContactListener()
{
    //dtor
}

void ContactListener::BeginContact(b2Contact* contact){
    void* bodyA = contact->GetFixtureA()->GetBody()->GetUserData();
    void* bodyB = contact->GetFixtureB()->GetBody()->GetUserData();

    b2WorldManifold worldManifold;
    contact->GetWorldManifold( &worldManifold );
    
    if(bodyA && bodyB){
        Entity* entityA = static_cast<Entity*>( bodyA );
        Entity* entityB = static_cast<Entity*>( bodyB );
        entityA->onHit(entityB, contact, worldManifold.normal);
        entityB->onHit(entityA, contact, worldManifold.normal);
    }
}

MapQueryCallback::MapQueryCallback() {
    foundMap = false;
    hitFixtures.clear();
}

bool MapQueryCallback::ReportFixture(b2Fixture* fixture) {
    if ( ((Entity*)fixture->GetBody()->GetUserData())->getType() == Entity::Type::Map ) {
        hitFixtures.push_back(fixture);
        foundMap = true;
    }
    return true;
}
