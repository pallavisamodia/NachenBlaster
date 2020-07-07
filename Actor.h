#ifndef ACTOR_H_
#define ACTOR_H_


#include "GraphObject.h"


#endif // ACTOR_H_
class StudentWorld;
using namespace std;
class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, double size, int dir, int deep, StudentWorld* sw) : GraphObject(imageID, startX, startY, dir, size, deep) {
		m_state = "Alive";
		n_sw = sw;
	}

	virtual void doSomething() = 0;      

	//We need this pointer to add new actors to our screen & animate them
	StudentWorld* getWorld() { return n_sw; }
	
	//To modify Status of all the Actors
	virtual void setState(string state) { m_state = state; }
	virtual bool alive()
	{
		if (m_state == "Alive")
			return true;
		else {

			return false;
		}
	}
	//Keeping track of hit points of Nachenblaster and Alien Ships
	virtual void change_hit_pts(int pts) { m_hit_pts = m_hit_pts + pts; }
	void set_hit_pts(int hit) { m_hit_pts = hit; }
	int hit_pts() { return m_hit_pts; }

	//Damage from Projectiles to Nachenblaster and alien ships
	virtual void damageNB() { return; }
	virtual void projResult() { return; }

	//To identify different types of actors
	virtual bool isAlien() { return false; }
	

	//Measure Euclidian Distance between various actors
	bool collision(Actor* first, Actor* second) {  
		if (sqrt(pow(first->getX() - second->getX(), 2) + pow(first->getY() - second->getY(), 2)) < .75*(first->getRadius() + second->getRadius()))
			return true;
		else
			return false;
	}



private:
	int m_hit_pts;
	string m_state;
	StudentWorld* n_sw;

};

///////////////////////////////////////////////////////
//          NACHENBLASTER & STARS           //////////
/////////////////////////////////////////////////////


class NachenBlaster :public Actor {
public:
	NachenBlaster(double x, double y, StudentWorld *sw) : Actor(IID_NACHENBLASTER, 0, 128, 1.0, 0, 0, sw)
	{
		set_hit_pts(50);
		cabbage_pts = 30;
	}
	virtual void doSomething();
	//Modifying and Tracking the various scores;
	void incFlatTorps(int n) { flat_torps += n; }
	int cabScore() { return cabbage_pts; }
	int flatScore() { return flat_torps; }
private:


	int cabbage_pts;
	int flat_torps;

};

class Star : public Actor {
public:
	Star(double loc_x, double loc_y, double size, StudentWorld* sw) : Actor(IID_STAR, loc_x, loc_y, size, 0, 3, sw) {}
	virtual void doSomething();


};



/////////////////////////////////////////////////////////
//     ALIEN SHIPS & EXPLOSION                  ////////
/////////////////////////////////////////////////////////

class Alien : public Actor {
public:
	Alien(double x, double y, int imageID, StudentWorld* sw,  double spe_ed) : Actor(imageID, x, y, 1.5, 0, 1, sw), m_speed(spe_ed), m_damaged(false) {}

	virtual bool isAlien() { return true; }
	virtual void doSomething();

	//Modifying and acessing the travel direction
	void setTravel(int n) { m_travel = n; }
	int get_travelDir() { return m_travel; }
	void travelDir();

	//Modifying and acessing the flight plan
	int flightPlan() { return m_flight_plan; }
	void set_flightPlan(int fp) { m_flight_plan = fp; }
	virtual void newPlan();

	//Modifying and acessing the speed
	double speed() { return m_speed; }
	void setSpeed(double n) { m_speed = n; }

	virtual int AlienType() = 0;
	//to identify different types of alienships
	//returns SMALLGON=1, SMOREGON=2, SNAGGLEGON=3 and 0 if neither of them

	//Production and after effects of projectiles & goodies
	virtual void projResult();
	virtual void damageNB(NachenBlaster* NB);
	bool toFire(NachenBlaster*nb);
	void droppingGoods();



private:
	int m_travel;
	bool m_damaged;
	int m_flight_plan;
	double m_speed;
	
};

class Smallgon : public Alien {
public:
	Smallgon(double x, double y, StudentWorld *sw);
	virtual int AlienType() { return 1; }

};

class Smoregon : public Alien {
public:
	Smoregon(double x, double y, StudentWorld *sw);
	virtual int AlienType() { return 2; }

private:


};

class Snagglegon : public Alien {
public:
	Snagglegon(double x, double y, StudentWorld *sw);
	virtual int AlienType() { return 3; }
	
private:

};




class Explosion : public Actor {
public:
	Explosion(double loc_x, double loc_y, StudentWorld *sw) :Actor(IID_EXPLOSION, loc_x, loc_y, 1, 0, 0, sw)
	{
		ex_lives = 4;
	}

	virtual void doSomething() {
		ex_lives--;
		//When given an opportunity to do something during a tic
		if (ex_lives == 0)
		{
			setState("Dead"); return;
		}
		setSize(1.5*getSize());

		//After exactly four tickss from the creation of the explosion, the explosion must set its state
		//to dead so it can be destroyed and removed from the level by your StudentWorld class.
	}
private:
	int ex_lives;
};
/////////////////////////////////////////////////////////
//           PROJECTILES                         ////////
/////////////////////////////////////////////////////////

class  Projectile : public Actor

{
public:
	Projectile(int imageID, double x, double y, int dir, char Source, StudentWorld *sw) :Actor(imageID, x, y, 0.5, dir, 1, sw) 
	{
		m_source = Source;
	}
	virtual void doSomething();

	virtual int projtype() = 0;
	char source() { return m_source; }  //source could be nachenblaster, alien or snagglegon(specifically)

private:
	char m_source;
};


class Cabbage : public Projectile {
public:
	Cabbage(double x, double y, int dir, char Source, StudentWorld *sw) :Projectile(IID_CABBAGE, x, y, 0, Source, sw) {

	}
	
	virtual int projtype() { return 1; }
private:


};

class Turnip :public Projectile {
public:
	Turnip(double x, double y, char Source, StudentWorld *sw) : Projectile(IID_TURNIP, x, y, 0, Source, sw) {}
	
	virtual int projtype() { return 2; }
};

class Flatulence : public Projectile {  
public:
	Flatulence(double x, double y, int dir, char Source,StudentWorld *sw) : Projectile(IID_TORPEDO, x, y, dir, Source, sw)
	{
		//'int Source' is so we can distinguish between who fired it 
			//'N' = Nachenblaster->0
			//'S'=Snagglegon->180
	}
	virtual int projtype() { return 3; }


private:

};
/////////////////////////////////////////////////////////
//           GOODIES                           ////////
/////////////////////////////////////////////////////////

class Goodies :public Actor {
public:
	Goodies(double x, double y, int imageID, StudentWorld* sw) :Actor(imageID, x, y, 0.5, 0, 1, sw) {}
	virtual void doSomething();
	virtual int goodieType() = 0; //to identify types of goodies
	void collisionOccurs(NachenBlaster* NB);
	
	

};

class ExtraLife : public Goodies {
public:
	ExtraLife(double x, double y, StudentWorld* sw) : Goodies(x, y, IID_LIFE_GOODIE, sw) {}
	virtual int goodieType() { return 1; }
private:

};

class Repair : public Goodies {
public:
	Repair(double x, double y, StudentWorld* sw) : Goodies(x, y, IID_REPAIR_GOODIE, sw) {}
	virtual int goodieType() { return 2; }
private:

};

class FlatulanceGoodie : public Goodies {
public:
	FlatulanceGoodie(double x, double y, StudentWorld* sw) : Goodies(x, y, IID_TORPEDO_GOODIE, sw) {}
	virtual int goodieType() { return 3; }
private:

};

