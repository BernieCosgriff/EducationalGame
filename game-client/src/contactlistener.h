#ifndef CONTACTLISTENER_H
#define CONTACTLISTENER_H

#include <Box2D/Box2D.h>
#include "entity.h"

class ContactListener : public b2ContactListener
{
    public:
        ContactListener();
        virtual ~ContactListener();

        void BeginContact(b2Contact* contact);

    protected:

    private:
};

class MapQueryCallback : public b2QueryCallback {
public:
    MapQueryCallback();
    bool foundMap;
    bool ReportFixture(b2Fixture* fixture);
};

#endif // CONTACTLISTENER_H
