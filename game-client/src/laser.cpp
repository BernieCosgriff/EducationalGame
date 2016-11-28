#include "laser.h"

Laser::Laser(tmx::Object& obj){

    std::vector<tmx::Property> properties = obj.getProperties();
    for(int i = 0; i < properties.size(); i++) {
        tmx::Property temp = properties[i];
        if(temp.getName() == "canBePassed"){
            canBePassed = temp.getBoolValue();
        }
    }

    tmx::FloatRect size = obj.getAABB();
    pos = glm::vec2(obj.getPosition().x+size.width/2.f, obj.getPosition().y+size.height/2.f); 
    size.width = size.width / 64;
    size.height = size.height / 64;
    size.left = size.left / 64;
    size.top = size.top / 64;

    b2Vec2 topLeft(size.left, size.top);
    b2Vec2 topRight(size.left + size.width, size.top);
    b2Vec2 bottomRight(size.left + size.width, size.top + size.height);
    b2Vec2 bottomLeft(size.left, size.top + size.height);

    b2BodyDef boxDef;
    boxDef.type = b2_staticBody;
    boxDef.angle = 0;

    b2PolygonShape shape;
    b2Vec2 vertices[] = {
        topLeft,
        topRight,
        bottomRight,
        bottomLeft
    };
    shape.Set(vertices, 4);
    b2FixtureDef fixDef;
    fixDef.shape = &shape;
    fixDef.density = 0;
    fixDef.restitution = 0;
    boxDef.position.Set(0,0);
    body = physicalWorld->get().CreateBody( &boxDef );
    b2Fixture* fixture = body->CreateFixture( &fixDef );
    fixture->SetSensor(true);
    body->SetUserData( this );
}

void Laser::update( double dt ){
}

void Laser::onHit( Entity* collider, b2Contact* c, b2Vec2 hitnormal ){
    if(collider->getType() == Entity::Type::Player){
        ::Player* p = static_cast< ::Player*>( collider );
        if(!this->canBePassed && !p->isDamageBoosted()){
            world->addEntity( new ShockDust( p->position ) );
            world->stutter(p->shockLength/2.f,0.1);
            p->shake(10,p->shockLength,0.1);
            glm::vec2 impulse = p->position - pos;
            impulse.y = 0;
            impulse = glm::normalize( impulse );
            impulse *= 15.f;

            p->switchState( new ShockedState( p, impulse, 0, p->shockLength) );
        }
    }
}

void Laser::draw( sf::RenderWindow& window ){
}

Entity::Type Laser::getType(){
    return Entity::Type::Laser;
}
