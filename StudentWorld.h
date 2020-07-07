#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
//#include "Actor.h"
#include <string>
#include <vector>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class NachenBlaster;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);

    virtual int init();
    virtual int move();
    virtual void cleanUp();
	std::vector<Actor*>& getActors() { return m_actors; }
	virtual ~StudentWorld() { cleanUp(); }
	NachenBlaster* foundNB() { return nb; } //to recheck with sanity checker
	void add(Actor* s) { m_actors.push_back(s); }
//	int shipsDest() { return d_ships; }
	void incShipsDest()  
	{ d_ships = d_ships + 1; n_ships--;
	}
	void decShips() { n_ships--; }  //On Screen Counter for Ships
private:

	std::vector<Actor*> m_actors;
	//std::vector<NachenBlaster*> nb;
	NachenBlaster* nb;
	int d_ships;   
	int n_ships;        //number of ships on screen
};

#endif // STUDENTWORLD_H_
