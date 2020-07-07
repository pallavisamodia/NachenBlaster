

#include "StudentWorld.h"

#include "Actor.h"
#include <string>
#include<sstream>
#include<algorithm>
#include<iostream>
#include<iomanip>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}



StudentWorld::StudentWorld(string assetDir)
	: GameWorld(assetDir)
{

}

int StudentWorld::init()
{


	vector<Actor*>::iterator p;

	for (int i = 0; i < 30; i++)  //dispersing 30 random stars
	{
		Star *s;
		int x = randInt(0, VIEW_WIDTH-1);
		int y = randInt(0, VIEW_HEIGHT-1);
		int temp = randInt(5, 50);
		double sw_size = temp / 100.0;
		s=new Star(x, y,sw_size,this);
		m_actors.push_back(s);
	}
	nb=(new NachenBlaster(0, 0,this));   //adding the NachenBlaster
	d_ships = 0;
	n_ships = 0;

    return GWSTATUS_CONTINUE_GAME;

	
}

int StudentWorld::move()

{//Required integers to account for number of ships destructed
	int T = 6 + 4 * getLevel();
	int D = d_ships; int R = T - D;      //Destructed and remaining number of ships to be destroyed
	int M =(int)( 4 + (.5*getLevel()));        //maximum number of ships on screen allowed


	//Stringstreams for Game stats
	ostringstream oss; 
	oss.setf(ios::fixed);
	oss.precision(2);
	oss << "Lives:";
	oss<<  oss.fill(' ');
	oss <<  setw(2)<<std::left << getLives();
	oss << oss.fill(' ');
	oss << setw(3) << "Health:";
	oss << setw(4) << std::right<<foundNB()->hit_pts() * 2;oss <<std::left<< "%";
	
	oss << setw(9) << std::right<<"Score:";
	oss << setw(5) <<std::right<< getScore();
	oss << setw(8) << "Level";

	oss << setw(2) << getLevel();
	oss << setw(12) << "Cabbages:";
	oss << setw(5)<<std::right<< (nb->cabScore() * 100) / 30 << std::left<<"%";
	oss << setw(13) << std::right<<"Torpedoes:";
	oss << setw(3) << std::right<<nb->flatScore();
	string scoreText = oss.str();
	
	setGameStatText(scoreText);

	if (nb != NULL)                 //checking for bad access
		if (nb->alive() == false)       //if NB (Nachenblaster) is still alive
		{
			decLives();                    //reduce lives if it dies
			return  GWSTATUS_PLAYER_DIED;
		}

	if(randInt(1,15)==1)               //adding new stars by 1/15th probability
	{
		Star* s;
		int x = randInt(0, VIEW_WIDTH - 1);
		int y = randInt(0, VIEW_HEIGHT - 1);
		int temp = randInt(5, 50);
		double sw_size = temp / 100.0;
		s = new Star(x, y, sw_size, this);
		m_actors.push_back(s);
	}


	//////////////CREATING ALIEN SHIPS/////////////////////////////////
	int s1 = 60, s2 = 20 + 5 * getLevel(), s3 = 5 + 10 * getLevel();
	int s = s1 + s2 + s3;       //Determining various probabilities for adding new Ships

	if (n_ships < min(R, M))
	{
		int prob = randInt(1, s);
		if (prob<=s1)                      
		{
			int y = randInt(0, VIEW_HEIGHT - 1);
			Smallgon* slg = new Smallgon(VIEW_WIDTH - 1, y, this);
			add(slg);
			n_ships++;

		}
		else if (prob<=s2)        
		{
			int y = randInt(0, VIEW_HEIGHT - 1);
			Smoregon* smg = new Smoregon(VIEW_WIDTH - 1, y, this);
			add(smg);
			n_ships++;

		}
	else   
		{
			int y = randInt(0, VIEW_HEIGHT - 1);
			Snagglegon* sng = new Snagglegon(VIEW_WIDTH - 1, y, this);
			add(sng);
			n_ships++;
		}
	}



	vector<Actor*>::iterator r;
	r = m_actors.begin();
	int i = 0;
	if (nb != NULL)
	{
		nb->doSomething();  //moving NB after checking that it is alive
	
		while((nb->alive())&& i<m_actors.size()) 
		{
			if (m_actors[i]->alive() == false)
				i++;
			else
			{
				m_actors[i]->doSomething();  //making all the actors move after checking that they are alive
				if (nb->alive() == false)
				{
					decLives();
					
					return  GWSTATUS_PLAYER_DIED;
				}
				if(D==T)
					return GWSTATUS_FINISHED_LEVEL;
				i++;
		
				
			}
		

		}
	}           
	
	vector<Actor*>::iterator p;
	p = m_actors.begin();
	while (p != m_actors.end())            //Deleting all the actors that were marked dead
	{
		if ((*p)->alive() == false)
		{
			delete (*p);
			p = m_actors.erase(p);
		}
		else
			p++;
	}
		
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	vector<Actor*>::iterator p;

	p = m_actors.begin();

		while (!m_actors.empty())
		{
			if ((*p) != nullptr)  
			{
				delete (*p);
				(*p) = nullptr;     
				p = m_actors.erase(p);
			}
			else p++;             
		}

		if (nb != NULL)
		{
			delete nb; nb = NULL;
		}

}
