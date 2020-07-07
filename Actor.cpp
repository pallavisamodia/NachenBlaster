#include <iostream>
#include "Actor.h"
#include "StudentWorld.h"
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

///////////////////////////////////////////////////////
//          PROJECTILES                   //////////
/////////////////////////////////////////////////////

void Projectile::doSomething()
{
	if (alive() == false)
		return;
	if (getX() < 0 || getX() >= VIEW_WIDTH) {
		setState("Dead"); return;
	}
	//NachenBlaster-fired Projectile
	if (source() == 'N')      
	{
		int times = 0;
		while (times < 2)
		{


			vector<Actor*>temp;
			vector<Actor*>::iterator p;
			temp = getWorld()->getActors();
			p = temp.begin();
			int i = 0;
			
			while (i < temp.size())
			{
			           //interactions with an alien ship
				if (temp[i]->isAlien() == true)
					if (collision(this, temp[i]) == true)       //if the collision occurs
					{
						if (projtype() == 1)
							temp[i]->change_hit_pts(-2);
						else
							temp[i]->change_hit_pts(-8);
						setState("Dead");//so that it can be deleted
						temp[i]->projResult();
						//to deal with alien ships' hit points and other specs after being hit by cabbage
						return;

					}
				i++;
			
			}
			times++;
			if (times == 2) { return; }
			moveTo(getX() + 8, getY());
			if (projtype() == 1)
				setDirection(getDirection() + 20);
			if (alive() == false)
				return;
			if (getX() < 0 || getX() >= VIEW_WIDTH) {
				setState("Dead"); return;

			}

		}

	}
	//If source is an alienship
	//Special cases included for Snagglegon
	if (source() == 'S' || source() == 'A')
	{
		NachenBlaster* nb = getWorld()->foundNB();

		{
			int times = 0;
			while (times < 2)
			{
				if (collision(this, nb) == true)
				{
					nb->change_hit_pts(-2);
					setState("Dead"); return;
				}
				times++;
				if (times == 2) { return; }
				moveTo(getX() - 6, getY());
				if (projtype() != 3)
					setDirection(getDirection() + 20);
			}

			
		}
	}
}


///////////////////////////////////////////////////////
//          NACHENBLASTER & STARS           //////////
/////////////////////////////////////////////////////

void NachenBlaster::doSomething() {
	if (alive() == false)
		return;

	if (cabbage_pts < 30)
		cabbage_pts++;
	int value;
	if (getWorld()->getKey(value) == true)
	{
		switch (value)
		{
		//blast off cabbage
		case KEY_PRESS_SPACE:
			if (cabbage_pts >= 5)
			{

				Cabbage* cab = (new Cabbage(getX() + 12, getY(), 0, 'N', getWorld()));    
				cabbage_pts = cabbage_pts - 5;

				getWorld()->add(cab);
				getWorld()->playSound(SOUND_PLAYER_SHOOT);

			}
			break;
		//Firing flatulence torpedoes
		case KEY_PRESS_TAB:
			if (flat_torps > 0)
			{
				Flatulence* flat=new Flatulence(getX() + 12, getY(), 0, 'N', getWorld());
				getWorld()->add(flat);
				flat_torps--;
				getWorld()->playSound(SOUND_TORPEDO);

			}
			break;
		//Directional movements
		case KEY_PRESS_LEFT:
		{if (!(getX() - 6 < 0))
			moveTo(getX() - 6, getY()); 	 }
		break;

		case KEY_PRESS_RIGHT:
		{if (!(getX() + 6 >= VIEW_WIDTH))

			moveTo(getX() + 6, getY()); }
		break;
		case KEY_PRESS_DOWN:
			if (!(getY() - 6 < 0))

				moveTo(getX(), getY() - 6);
			break;
		case KEY_PRESS_UP:
			if (!(getY() + 6 >= VIEW_HEIGHT))
				moveTo(getX(), getY() + 6);
			break;
		}

	}

	if (hit_pts() <= 0)
	{
		setState("Dead");
	}

}

void Star::doSomething() {
	//Making the stars drift left
	if (getX() - 1 >= 0)
		moveTo(getX() - 1, getY());
	else //Declaring them dead as they reach the leftmost screen
		setState("Dead");
}


/////////////////////////////////////////////////////////
//     ALIEN SHIPS & EXPLOSION                  ////////
/////////////////////////////////////////////////////////
void Alien::newPlan()
{
	//Bouncing it to down and left
	if (getY() >= VIEW_HEIGHT - 1)
	{
		setTravel(224);
	}
	//Bouncing it to up and left
	if (getY() < 0)
	{
		setTravel(134);
	}
	else if (flightPlan() == 0 && AlienType() != 3)
	{
		int temp = randInt(1, 3);
		switch (temp)
		{
	
		case 1:setTravel(0); break;
		case 2:setTravel(134); break;
		case 3:setTravel(224); break;
		}

	}

	//Setting new flight plans
	if (AlienType() != 3)
		set_flightPlan(randInt(1, 32));   
}
Smallgon::Smallgon(double x, double y, StudentWorld *sw) : Alien(x, y, IID_SMALLGON, sw, 2.0)
{
	set_hit_pts((int)(5 * (1 + (getWorld()->getLevel() - 1)*.1)));
	set_flightPlan(0);
}

//When a projectile collides with an alien
void Alien::projResult()
{
	if (hit_pts() <= 0)
	{
		getWorld()->increaseScore(250);
		setState("Dead");
		getWorld()->incShipsDest();
		getWorld()->playSound(SOUND_DEATH);
		getWorld()->add(new Explosion(getX(), getY(), getWorld()));
		if (AlienType() == 2)   //Smoregon's case of dropping goodies
			droppingGoods();
	}
	else
		getWorld()->playSound(SOUND_DEATH);
	return;
}

//When Nachenblaster collides with an alien
void Alien::damageNB(NachenBlaster* NB) {
	if (AlienType() == 3)
		NB->change_hit_pts(-15);
	else
		NB->change_hit_pts(-5);
	setState("Dead");
	getWorld()->incShipsDest();   
	if (AlienType() == 3)                
		getWorld()->increaseScore(1000);    //Snagglegon's case
	else
		getWorld()->increaseScore(250);		//Smoregon's case
	getWorld()->playSound(SOUND_DEATH);
	getWorld()->add(new Explosion(getX(), getY(), getWorld()));

	if (AlienType() != 1)   //Smoregon's & snagglegons' dropping goodies
	{
		droppingGoods();
	}
}

void Alien::droppingGoods() {

	if (AlienType() == 2)      //Smoregon drops a goodie
	{
		

		{
			if (randInt(1, 2) == 1)   //50% chance of dropping either goodies
			{
			
				Repair* rep = new Repair(getX(), getY(), getWorld());
				getWorld()->add(rep);
			}
			else
			{
				getWorld()->add(new FlatulanceGoodie(getX(), getY(), getWorld()));
			}
		}
	}
	else {                 //Snagglegon drops an extra life goodie

		if (randInt(1, 6) == 1)
			getWorld()->add(new ExtraLife(getX(), getY(), getWorld()));
	}
}



void Alien::doSomething()
{
	//if already dead
	if (alive() == false)
		return;
	//out of screen
	if (getX() < 0) { getWorld()->decShips();  setState("Dead"); return; }

	//In case of collision
	NachenBlaster* nb = getWorld()->foundNB();

	if (collision(this, nb)) { damageNB(nb); }

	//Change of Flight Plan
	if (getY() < 0 || getY() >= VIEW_HEIGHT - 1)
		newPlan();
	else if (flightPlan() == 0 && AlienType() != 3)
		newPlan();
	//Firing of Projectiles at the NachenBlaster
	if (toFire(nb) == true) { return; }
	//(toFire(nb));

	//moving in its current direction and reduction of flight plan
	travelDir();

	//checking again for collision
	if (collision(this, nb)) { damageNB(nb); }
	return;
}

void Alien::travelDir()
{
	double N = speed();
	if (get_travelDir() == 0) { moveTo(getX() - N, getY()); }
	else if (get_travelDir() == 134) { moveTo(getX() - N, getY() + N); }
	else if (get_travelDir() == 224) { moveTo(getX() - N, getY() - N); }
	if(AlienType()!=3)
		set_flightPlan(flightPlan() - 1);
}

//when alien gets in line of sight with a nachenblaster
bool Alien::toFire(NachenBlaster*nb)
{
	if (nb->getX() < getX() && (nb->getY() <= getY() + 4 && nb->getY() >= getY() - 4))
	{
		if (AlienType() == 3)
		{
			int chance = (15 / getWorld()->getLevel()) + 10;
			if (randInt(1, chance) == 1)
			{
				getWorld()->add(new Flatulence(getX() - 14, getY(), 180, 'S', getWorld()));
				getWorld()->playSound(SOUND_TORPEDO);
				return true;
			}
		}
		else
		{
			int chance = (20 / getWorld()->getLevel()) + 5;
			if (randInt(1, chance) == 1)
			{
				getWorld()->add(new Turnip(getX() - 14, getY(), 'A', getWorld()));
				getWorld()->playSound(SOUND_ALIEN_SHOOT);
				return true;
			}
			if (AlienType() == 2) 
			{
				if (randInt(1, chance) == 1)
				{
					setDirection(0);
					set_flightPlan(VIEW_WIDTH);
					setSpeed(5);
					return false;
				}
			}
		}

	}
	return false;

}

Smoregon::Smoregon(double x, double y, StudentWorld *sw) : Alien(x, y, IID_SMOREGON, sw, 2.0)
{
	set_hit_pts((int)(5 * (1 + (getWorld()->getLevel() - 1)*.1)));
	set_flightPlan(0);
}

Snagglegon::Snagglegon(double x, double y, StudentWorld *sw) : Alien(x, y, IID_SNAGGLEGON, sw, 1.75)
{
	set_hit_pts((int)(10 * (1 + (getWorld()->getLevel() - 1)*.1)));
	setTravel(224);
}

///////////////////////////////////////////////////////
//          GOODIES                  //////////
/////////////////////////////////////////////////////

void Goodies::doSomething()
{
	int times = 0;
	while (times < 2)
	{
		if (alive() == false)
			return;
		if (getY() < 0 || getY() >= VIEW_HEIGHT || getX() < 0 || getX() >= VIEW_WIDTH)
		{
			setState("Dead"); return;
		}

		NachenBlaster* nb = getWorld()->foundNB();
		if (collision(this, nb) == true)
		{

			collisionOccurs(nb);

			return;
		}
		times++;
		if (times == 2) { return; }
		moveTo(getX() - 0.75, getY() - .75);

	}
	return;
}

//When a goodie collides with a Nachenblaster
void Goodies::collisionOccurs(NachenBlaster* NB)
{

	getWorld()->increaseScore(100);
	setState("Dead");
	getWorld()->playSound(SOUND_GOODIE);
	int temp = goodieType();
	switch (temp)
	{

	case 1:	getWorld()->incLives(); break;
	case 2:
	{for (int i = 0; i < 10; i++)
		if (NB->hit_pts() < 50)
			change_hit_pts(1);
	} break;
	case 3:
		NB->incFlatTorps(5);
	}
	return;
}