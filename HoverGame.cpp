// Mohammad Mohajerani ID:G20656222 
//A racing game, including car movement, race stages and collision detection and resolution.
//In the  the game the player controls a hover - car which must be driven along a desert race.

// Includes
#include <TL-Engine.h>	
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <algorithm>
#include <math.h>
#include <time.h>

// Namespace
using namespace tle;
using namespace std;

// Enums State Defenition
enum gameState
{
	Waiting,
	PreStart,
	Playing,
	Paused,
	Over
};
enum stateHover
{
	HoverWaiting,
	HoverCollieds,
	HoverForward,
	HoverBackward,
	HoverDamaged
};
enum stageState
{
	first ,
	stage1,
	stage2,
	stage3,
	stage4,
	stage5,
	stage6,
	stage7,
	stage8,
	stage9,
	lastStage 
};
enum lapState
{
	firstLap ,
	lap1,
	lap2,
	lastLap 
};
enum boostState {
	Ready,
	Active,
	Warning,
	OverHit
};

// Public usefull Constants
const int     kSpeed             = 20;                                 
const int     kNChechpoints      = 10;                          
const int     kNSTrack           = 10;                               
const int     kNTTrack           = 7;                                
const int     kNSDummy           = kNSTrack;                         
const int     kNCDummy           = kNTTrack;                         
const int     kNWaypoint         = 2 * kNTTrack + 2 * kNSTrack;    
const int     kNIsles            = 4 * kNSTrack;                      
const int     kNWalls            = 2 * kNSTrack;                      
const int     kNWays             = 6 * kNSTrack;                       
const int     kNWaterTanks       = 4 * kNTTrack;                 
const int     kNHovers           = 3;                                
const int     maxHoverEffect     = 1000;                       
const int     kNParticles        = 200;                           
const float   hightSpeedLimit    = 0.5;                    
const float   lowSpeedLimit      = -0.2;                     
const float   kgx                = 0;                                     
const float   kgy                = 0.5*-9.81;                                 
const float   kgz                = 0;                                     
const float   xP                 = 1;                                      
const float   yP                 = 4;                                      
const float   zP                 = 1;                                      
const float   maxRandomSpeed     = 0.6;                        
const float   minRandomSpeed     = 0.2;                        
float         trackRotate[kNSTrack];
string        winner;                                      


// Return a random number in the range between rangeMin and rangeMax inclusive 
// range_min <= random number <= range_max
float random(float rangeMin, float rangeMax)
{
	float result = (float)rand() / (float)(RAND_MAX + 1);
	result *= (float)(rangeMax - rangeMin);
	result += rangeMin;
	return result;
}

// Cross class  
class Cross
{
public:
	IModel* model;
	float lifeTime = 0;
	bool enable = false;
};
// Road class to create whole race track
class road
{
private:
	IMesh*       checkpointM;            // A checkpoint and the finishing line
	IMesh*       isleStraightM;          // A wall end
	IMesh*       wallM;                  // A wall section
	IMesh*       waterTank1M;            // A water tank
	IMesh*       crossM;                 // A Cross
	IMesh*       walkwayM;               // A walk way
	IMesh*       dummyM;                 // A Tribune 3
	IMesh*       waypointM;              // a Waypoint

public:
	// creating the straight race track
	void straightRaceTrack(int nT, IModel* aWaypoint[], int nR, IModel* aDummy[], IModel* aCheckpoint[], IModel* aIsle[],
		IModel* aWall[], IModel* aWalkway[], Cross aCross[], float xStartP, float zStartP, float tR, I3DEngine* myEngine)
	{
		checkpointM     = myEngine->LoadMesh("checkpoint.x");      // A checkpoint and the finishing line
		isleStraightM   = myEngine->LoadMesh("IsleStraight.x");    // A wall end
		wallM           = myEngine->LoadMesh("wall.x");            // A wall section
		crossM          = myEngine->LoadMesh("Cross.x");           // A Cross
		walkwayM        = myEngine->LoadMesh("Walkway.x");         // A walk way
		dummyM          = myEngine->LoadMesh("Dummy.x");           // A dummy
		waypointM       = myEngine->LoadMesh("Dummy.x");           // A way point

		int      kNIslesPT      = 4;
		int      kNWallsPT      = 2;
		int      kNWaysPT       = 6;
		int      kNWaypointPT   = 2;

		trackRotate[nR] = tR;
		aDummy[nR] = dummyM->CreateModel(xStartP, 0, zStartP);
		aCheckpoint[nR] = checkpointM->CreateModel(0, 0, 100);
		aCheckpoint[nR]->AttachToParent(aDummy[nR]);
		// Create cross models
		aCross[nR].model = crossM->CreateModel(0, -20, 100);
		aCross[nR].model->Scale(0.2);
		aCross[nR].model->AttachToParent(aDummy[nR]);
		// Create way models
		for (int i = 0; i < kNWaysPT; i++)
		{
			aWalkway[nR*kNWaysPT + i] = walkwayM->CreateModel(0, -0.3, i * 25);
			aWalkway[nR*kNWaysPT + i]->Scale(6);
			aWalkway[nR*kNWaysPT + i]->AttachToParent(aDummy[nR]);
		}
		// Create way point models
		for (int i = 0; i < kNWaypointPT; i++)
		{
			aWaypoint[nT*kNWaypointPT + i] = waypointM->CreateModel(0, 0, 50 + (i * 60));
			aWaypoint[nT*kNWaypointPT + i]->AttachToParent(aDummy[nR]);
		}
		// Create isle models
		for (int i = 0; i < kNIslesPT; i++)
		{
			aIsle[nR*kNIslesPT + i] = isleStraightM->CreateModel((-10.5 + (i / 2) * 20), 0, 40 + ((i % 2) * 13));
			aIsle[nR*kNIslesPT + i]->AttachToParent(aDummy[nR]);
		}
		// Create wall models
		for (int i = 0; i < kNWallsPT; i++)
		{
			aWall[nR*kNWallsPT + i] = wallM->CreateModel(-10.5 + (i * 20), 0, 46);
			aWall[nR*kNWallsPT + i]->AttachToParent(aDummy[nR]);
		}
		// Setting rotate dummy model wich is the track attached toit
		aDummy[nR]->RotateY(trackRotate[nR]);
	}

	// creating the angle race track
	void turnRaceTrack(int nT, IModel* aWaypoint[], int nR, IModel* aCDummy[], IModel* aWaterTank[], float xStartP, float zStartP, float tR, I3DEngine* myEngine)
	{
		waterTank1M       = myEngine->LoadMesh("TankSmall1.x");          // A water tank
		dummyM            = myEngine->LoadMesh("Dummy.x");               // Track attached dummy
		waypointM         = myEngine->LoadMesh("Dummy.x");               // waypoint dummy

		int   kNWaterTankPT  = 4;
		int   kNWaypointPT   = 2;
		aCDummy[nR] = dummyM->CreateModel(xStartP, 0, zStartP);
		// Water tanks set up
		aWaterTank[nR*kNWaterTankPT + 0] = waterTank1M->CreateModel(-15, -1, 12);
		aWaterTank[nR*kNWaterTankPT + 0]->RotateZ(+10);
		aWaterTank[nR*kNWaterTankPT + 0]->RotateX(-10);
		aWaterTank[nR*kNWaterTankPT + 0]->AttachToParent(aCDummy[nR]);
		aWaterTank[nR*kNWaterTankPT + 1] = waterTank1M->CreateModel(6, -1, 19);
		aWaterTank[nR*kNWaterTankPT + 1]->RotateZ(-10);
		aWaterTank[nR*kNWaterTankPT + 1]->RotateX(+10);
		aWaterTank[nR*kNWaterTankPT + 1]->AttachToParent(aCDummy[nR]);
		aWaterTank[nR*kNWaterTankPT + 2] = waterTank1M->CreateModel(-3.5, -1, 30);
		aWaterTank[nR*kNWaterTankPT + 2]->RotateZ(-10);
		aWaterTank[nR*kNWaterTankPT + 2]->RotateX(+10);
		aWaterTank[nR*kNWaterTankPT + 2]->AttachToParent(aCDummy[nR]);
		aWaterTank[nR*kNWaterTankPT + 3] = waterTank1M->CreateModel(-13, -1, 34);
		aWaterTank[nR*kNWaterTankPT + 3]->RotateZ(-10);
		aWaterTank[nR*kNWaterTankPT + 3]->RotateX(+10);
		aWaterTank[nR*kNWaterTankPT + 3]->AttachToParent(aCDummy[nR]);

		aWaypoint[nT*kNWaypointPT + 0] = waypointM->CreateModel(-7.5, 0, 17.5);
		aWaypoint[nT*kNWaypointPT + 0]->AttachToParent(aCDummy[nR]);
		aWaypoint[nT*kNWaypointPT + 1] = waypointM->CreateModel(-7.5, 0, 17.5);
		aWaypoint[nT*kNWaypointPT + 1]->AttachToParent(aCDummy[nR]);


		aCDummy[nR]->RotateY(tR);
	}

	// creating the narrow race track
	void narrowRaceTrack(int nT, IModel* aWaypoint[], int nR, IModel* aDummy[], IModel* aCheckpoint[], IModel* aIsle[],
		IModel* aWall[], IModel* aWalkway[], Cross aCross[], float xStartP, float zStartP, float tR, I3DEngine* myEngine)
	{

		checkpointM     = myEngine->LoadMesh("checkpoint.x");          // A checkpoint and the finishing line
		isleStraightM   = myEngine->LoadMesh("IsleStraight.x");        // A wall end
		wallM           = myEngine->LoadMesh("wall.x");                // A wall section
		walkwayM        = myEngine->LoadMesh("Walkway.x");             // A walk way
		dummyM          = myEngine->LoadMesh("Dummy.x");               // Track attached dummy
		waypointM       = myEngine->LoadMesh("Dummy.x");			   // waypoint dummy

		int   kNIslesPT    = 4;
		int   kNWallsPT    = 2;
		int   kNWaysPT     = 6;
		int   kNWaypointPT = 2;

		trackRotate[nR] = tR;
		aDummy[nR] = dummyM->CreateModel(xStartP, 0, zStartP);
		aCheckpoint[nR] = checkpointM->CreateModel(0, 0, 100);
		aCheckpoint[nR]->AttachToParent(aDummy[nR]);
		aCross[nR].model = crossM->CreateModel(0, -20, 100);
		aCross[nR].model->Scale(0.2);
		aCross[nR].model->AttachToParent(aDummy[nR]);
		for (int i = 0; i < kNWaysPT; i++)
		{
			aWalkway[nR*kNWaysPT + i] = walkwayM->CreateModel(0, 0, i*25);
			aWalkway[nR*kNWaysPT + i]->Scale(6);
			aWalkway[nR*kNWaysPT + i]->AttachToParent(aDummy[nR]);
		}
		for (int i = 0; i < kNIslesPT; i++)
		{
			aIsle[nR*kNIslesPT + i] = isleStraightM->CreateModel((-7.5 + (i / 2) * 14), 0, 40 + ((i % 2) * 13));
			aIsle[nR*kNIslesPT + i]->AttachToParent(aDummy[nR]);
		}
		for (int i = 0; i < kNWallsPT; i++)
		{
			aWall[nR*kNWallsPT + i] = wallM->CreateModel(-5.5 + (i * 10), 0, 46);
			aWall[nR*kNWallsPT + i]->AttachToParent(aDummy[nR]);
		}
		for (int i = 0; i < kNWaypointPT; i++)
		{
			aWaypoint[nT*kNWaypointPT + i] = waypointM->CreateModel(0, 0, i * 105);
			aWaypoint[nT*kNWaypointPT + i]->AttachToParent(aDummy[nR]);
		}
		aDummy[nR]->RotateY(trackRotate[nR]);
	}
};

// Hover Details class
class Player
{
public:
	IModel*  hocerCroft;
	int      currentState             = stageState::first;
	int      currentLap               = lapState::firstLap;
	int      currentPos               = first;
	int      currentWaypoint          = first;
	int      totalStagePassed         = 0;
	int      Score                    = 0;
	bool     crossed[kNChechpoints];
	float    health                   = 100;
	float    acceleration             = 0.000000f;
	float    xRotate                  = 0;
	float    zRotate                  = 0;
	float    yRotate                  = 0;
	float    hoverEffect              = 0;
	float    colisionTimer            = 0;
	bool     collision                = false;
	string   Name;
};

// Temporary class to save the hover details to compare(get the position)
class Temp
{
public:
	int id;
	string playerName;
	int playerLap;
	int playerPosition;
	int totalStagePassed;
	int playerState;
	int health;
	IModel* model;
};
Temp tempPlayers[kNHovers];

// Particle struct
struct s_Particle
{
	IModel* model;
	IModel* dummy;
	float vx, vy, vz;
};
typedef struct s_Particle t_particle;

// checkpoint cross state initialize
void checkpointCrossStateInitialize(Player player[])
{
	for (int j = 0; j < kNHovers; j++)
	{
		for (int i = 0; i < kNChechpoints; i++)
		{
			player[j].crossed[i] = false;
		}
	}
}

// Hover collision check function
void checkHoversCollision(Player player[], IModel* camDummy)
{
	for (int j = 0; j < kNHovers; j++)
	{
		IModel* hover = player[j].hocerCroft;
		int tN = player[j].currentState;
		if (j == 0)
			IModel* hover = camDummy;
		for (int i = 0; i < kNHovers; i++)
		{
			if (i != j)
			{
				IModel* Model = player[i].hocerCroft;
				if (i == 0)
					IModel* Model = camDummy;
				float modelMinX = Model->GetX() - 3;
				float modelMaxX = Model->GetX() + 3;
				float modelMinZ = Model->GetZ() - 10;
				float modelMaxZ = Model->GetZ() + 10;

				if (trackRotate[tN] != 0 && trackRotate[tN] != 180)
				{
					modelMinX = Model->GetX() - 10,
						modelMaxX = Model->GetX() + 10,
						modelMinZ = Model->GetZ() - 3,
						modelMaxZ = Model->GetZ() + 3;
				}

				float hoverX = hover->GetX(), hoverZ = hover->GetZ();
				if ((hoverX > modelMinX) && (hoverX < modelMaxX) &&
					(hoverZ > modelMinZ) && (hoverZ < modelMaxZ))

				{
					if (player[j].currentPos > player[i].currentPos)
					{
						player[j].acceleration += 0.8*player[i].acceleration;
						player[i].acceleration *= -0.9;
						if (j!=0)
						{
							player[j].hocerCroft->SetZ(player[i].hocerCroft->GetZ() + 10);
						}
						if (player[i].health >= 0) player[i].health -= 0.1;
						player[i].collision = true;
					}
					else
					{
						player[i].acceleration += 0.8 * player[j].acceleration;
						player[j].acceleration *= -0.9;
	     				if (i != 0)
						{
							player[i].hocerCroft->SetZ(player[j].hocerCroft->GetZ() + 10);
						}
						if (player[j].health >= 0) player[j].health -= 0.1;
						player[j].collision = true;
					}
					if (player[i].health >= 0) player[i].health -= 0.1;
					player[i].collision = true;
					
				}
			}
		}
	}
}

// Isle collision check function
void checkIsleCollision(int nModel, IModel* aModel[], IModel* hover, float & acceleration, float & health,bool & Collision, float dt)
{
	for (int i = 0; i < nModel; i++)
	{
		float Dx, Dz;
		Dx = aModel[i]->GetX() - hover->GetX();
		Dz = aModel[i]->GetZ() - hover->GetZ();
		float collisionDist = sqrt(Dx*Dx + Dz*Dz);

		if (collisionDist < 5)	// Collision occurred…
		{
			acceleration *= -0.9;
			if(health >=0) health -= 0.1;
			Collision = true;
		}
	}
}

// Wall collision check function
void checkWallCollision(float trackRotate[], int nModel, IModel* aModel[], IModel* hover, float & acceleration, float & health, bool & Collision, int rotate, float dt)
{
	for (int i = 0; i < nModel; i++)
	{
		float trackR = trackRotate[i / 2];
		float modelMinX = aModel[i]->GetX() - 3;
		float modelMaxX = aModel[i]->GetX() + 3;
		float modelMinZ = aModel[i]->GetZ() - 10;
		float modelMaxZ = aModel[i]->GetZ() + 10;

		if   (rotate != 0) trackR = rotate;
			           
		if (trackR != 0 && trackR != 180)
		{
			modelMinX = aModel[i]->GetX() - 10,
			modelMaxX = aModel[i]->GetX() + 10,
			modelMinZ = aModel[i]->GetZ() - 3,
			modelMaxZ = aModel[i]->GetZ() + 3;
		}

		float hoverX = hover->GetX(), hoverZ = hover->GetZ();
		if ((hoverX > modelMinX) && (hoverX < modelMaxX) &&
			(hoverZ > modelMinZ) && (hoverZ < modelMaxZ))
		{
			acceleration *= -0.9;
			if(health >=0) health -= 0.1;
			Collision = true;
		}
	}
}

// WaterTank collision check function
void checkSpareCollision(int nModel, IModel* aModel[], IModel* hover, float & acceleration, float & health,bool & collision, int MaxSpase, float dt)
{
	for (int i = 0; i < nModel; i++)
	{
		float Dx, Dz;
		Dx = aModel[i]->GetX() - hover->GetX();
		Dz = aModel[i]->GetZ() - hover->GetZ();
		float collisionDist = sqrt(Dx*Dx + Dz*Dz);

		if (collisionDist < MaxSpase)	// Collision occurred…
		{
			acceleration *= -0.9;
			if(health >=0) if(health >=0) health -= 0.1;
			collision = true;
		}
	}
}

// WaterTank collision check function
void checCheckpointCollision(int nModel, IModel* aModel[], Player player[], Cross aCross[])
{
	for (int j = 0; j < kNHovers; j++)
	{
		IModel* hover = player[j].hocerCroft;

		for (int i = 0; i < nModel; i++)
		{
			float minX = aModel[i]->GetX() - 8;
			float maxX = aModel[i]->GetX() + 8;
			float minDistX = minX - hover->GetX();
			float minDistZ = aModel[i]->GetZ() - hover->GetZ();
			float mincollisionDist = sqrt(minDistX*minDistX + minDistZ*minDistZ);
			float maxDistX = maxX - hover->GetX();
			float maxDistZ = aModel[i]->GetZ() - hover->GetZ();
			float maxcollisionDist = sqrt(maxDistX*maxDistX + maxDistZ*maxDistZ);
			float modelMinX = aModel[i]->GetX() - 10;
			float modelMaxX = aModel[i]->GetX() + 10;
			float modelMinZ = aModel[i]->GetZ() - 1;
			float modelMaxZ = aModel[i]->GetZ() + 1;
			// If track rotated
			if (trackRotate[i] != 0 && trackRotate[i] != 180)
			{
				float minZ = aModel[i]->GetZ() - 8;
				float maxZ = aModel[i]->GetZ() + 8;
				minDistZ = minZ - hover->GetZ();
				minDistX = aModel[i]->GetX() - hover->GetX();
				mincollisionDist = sqrt(minDistX*minDistX + minDistZ*minDistZ);
				maxDistZ = maxZ - hover->GetZ();
				maxDistX = aModel[i]->GetX() - hover->GetX();
				maxcollisionDist = sqrt(maxDistX*maxDistX + maxDistZ*maxDistZ);
				modelMinX = aModel[i]->GetX() - 1;
				modelMaxX = aModel[i]->GetX() + 1;
				modelMinZ = aModel[i]->GetZ() - 10;
				modelMaxZ = aModel[i]->GetZ() + 10;
			}
			// If hover is arund check point
			if (hover->GetX() > modelMinX && hover->GetX() < modelMaxX && hover->GetZ() > modelMinZ && hover->GetZ() < modelMaxZ)
			{
				// If collision happend whit calumns -> bounce
				if (maxcollisionDist < 2 || mincollisionDist < 2)
				{
					player[j].acceleration *= -0.9;
					if (player[j].health >= 0) player[j].health -= 0.1;
					player[j].collision = true;
				}
				// if hover passed in between of theme
				else
				{
					if (i <= 0 || player[j].crossed[i - 1])
					{
						player[j].crossed[i] = true;
						if (j == 0)
						{
							aCross[i].model->SetY(7);
							aCross[i].enable = true;
						}
					}
				}
			}
		}
	}
}

//None-Player hover Transportation
void handlTheNonePlayerHover(IModel* awaypoints[], Player player[], float dt, float gT, float time)
{
	for (int i = 1; i < kNHovers; i++)
	{
		//if none_payer health is More than 0 beable to move
		if (player[i].health > 0) {
			if (int(time) % 2 == 0) {
				player[i].acceleration = kSpeed*dt + random(minRandomSpeed, maxRandomSpeed);
			}
			int pos = player[i].currentWaypoint;
			float const Xawaypoints = awaypoints[pos]->GetX() + ((((i - 2) * 2) + 1) * 3),
				Yawaypoints = player[i].hocerCroft->GetLocalY(),
				Zawaypoints = awaypoints[pos]->GetZ();

			player[i].hocerCroft->LookAt(Xawaypoints, Yawaypoints, Zawaypoints);
			player[i].hocerCroft->MoveLocalZ(player[i].acceleration);

			float Dx, Dz;
			Dx = awaypoints[pos]->GetX() - player[i].hocerCroft->GetX();
			Dz = awaypoints[pos]->GetZ() - player[i].hocerCroft->GetZ();
			float collisionDist = sqrt(Dx*Dx + Dz*Dz);

			if (collisionDist < 7)	// Collision occurred…
			{
				if (pos + 1 < kNWaypoint)
				{
					player[i].currentWaypoint += 1;

				}
				else
				{
					player[i].currentWaypoint = 0;
				}
			}
		}
	}
	//for (int i = 1; i < kNHovers; i++)
	//{
	//	//if none_payer health is More than 0 beable to move
	//	if (player[i].health > 0)
	//	{
	//		int pos = player[i].currentWaypoint;
	//		float  Xawaypoints = awaypoints[pos]->GetX() + ((((i - 2) * 2) + 1) * 3),
	//			Yawaypoints = player[i].hocerCroft->GetLocalY(),
	//			Zawaypoints = awaypoints[pos]->GetZ();
	//		if (int(time) % 2 == 0 ) 
	//		{
	//			if (!player[i].collision)
	//			{
	//				player[i].acceleration = kSpeed*dt + random(minRandomSpeed, maxRandomSpeed);					
	//			}
	//			else
	//			{
	//				Zawaypoints *= -0.8;
	//				player[i].acceleration = -abs( kSpeed*dt + random(minRandomSpeed, maxRandomSpeed));
	//				player[i].collision = false;
	//			}
	//		}
	//		player[i].hocerCroft->LookAt(Xawaypoints, Yawaypoints, Zawaypoints);
	//		player[i].hocerCroft->MoveLocalZ(player[i].acceleration);

	//		float Dx, Dz;
	//		Dx = awaypoints[pos]->GetX() - player[i].hocerCroft->GetX();
	//		Dz = awaypoints[pos]->GetZ() - player[i].hocerCroft->GetZ();
	//		float collisionDist = sqrt(Dx*Dx + Dz*Dz);

	//		if (collisionDist < 7)	// Collision occurred…
	//		{
	//			if (pos + 1 < kNWaypoint)
	//			{
	//				player[i].currentWaypoint += 1;

	//			}
	//			else
	//			{
	//				player[i].currentWaypoint = 0;
	//			}
	//		}
	//	}
	//}
}

// Own-Player hover controls
void handlTheOwnHover(int & hoverStateDirection, IModel* hover,float Health, float & acceleration,bool & collision, float dt, float gT,float gametime)
{
	if (Health <= 0)
	{
		hoverStateDirection = stateHover::HoverDamaged;
	}
	switch (hoverStateDirection)
	{

	case stateHover::HoverWaiting:


		break;

	case stateHover::HoverForward:
		if (!collision)
		{
			if (acceleration <= hightSpeedLimit)
			{

				acceleration += kSpeed*dt;
			}
			hoverStateDirection = stateHover::HoverWaiting;
		}
		else
		{
			hoverStateDirection = stateHover::HoverCollieds;
		}

		break;

	case stateHover::HoverBackward:


		if (!collision)
		{
			if (acceleration >= lowSpeedLimit)
			{

				acceleration -= kSpeed*dt;
			}
			hoverStateDirection = stateHover::HoverWaiting;
		}
		else
		{
			hoverStateDirection = stateHover::HoverCollieds;
		}

		break;

	case stateHover::HoverDamaged:

		break;

	case stateHover::HoverCollieds:
		if (acceleration == 0)
		{
			acceleration += kSpeed*dt;
		}
		if (int(gametime) % 2 == 0)
		{
			collision = false;
			hoverStateDirection = stateHover::HoverWaiting;
		}
		break;
	}

	if (acceleration > 0)
	{
		acceleration += gT;
	}
	if (acceleration < 0)
	{
		acceleration -= gT;
	}

	hover->MoveLocalZ(acceleration);
}

// check the curent stage oh any player
int checkCrossedCheckpoint(int nCP, Player player)
{
	int counter = 0;
	for (int i = 0; i < nCP; i++)
	{
		if (player.crossed[i])
		{
			counter++;
		}
		else
		{
			return counter;
		}

	}
	return counter;
}

// Faceing Vector
void facingVector(IModel* currentHover, float &x, float &y, float &z)
{
	float matrix[16];
	currentHover->GetMatrix(matrix);
	x = matrix[8];
	y = matrix[9];
	z = matrix[10];
}

//Dot product Function
float dotProduct(float Vx, float Vy, float Vz, float Wx, float Wy, float Wz)
{
	return Vx*Wx + Vy*Wy + Vz*Wz;
}

// Player Position find out
void checkPosition(Player player[], int & gameState)
{
	// setup the temp details
	for (int i = 0; i < kNHovers; i++)
	{
		tempPlayers[i].id = i;
		tempPlayers[i].playerName = player[i].Name;
		tempPlayers[i].totalStagePassed = player[i].totalStagePassed;
		tempPlayers[i].playerLap = player[i].currentLap;
		tempPlayers[i].model = player[i].hocerCroft;
		tempPlayers[i].health = player[i].health;
		tempPlayers[i].playerState= player[i].currentState;
	}
	// If players are in the same stage
	if (tempPlayers[0].totalStagePassed == tempPlayers[1].totalStagePassed && tempPlayers[1].totalStagePassed == tempPlayers[2].totalStagePassed
		|| tempPlayers[0].totalStagePassed == tempPlayers[1].totalStagePassed
		|| tempPlayers[1].totalStagePassed == tempPlayers[2].totalStagePassed
		|| tempPlayers[2].totalStagePassed == tempPlayers[0].totalStagePassed)
	{
		//Sort the position base on the total stage
		std::sort(tempPlayers, tempPlayers + kNHovers, [](Temp const & a, Temp const & b) -> bool
		{return a.totalStagePassed > b.totalStagePassed; });
		//if hover are in the same stage check with the dot prudoct
		for (int i = 0; i < kNHovers; i++)
		{
			for (int j = 0; j < kNHovers; j++)
			{
				if (i == j || tempPlayers[i].totalStagePassed != tempPlayers[j].totalStagePassed)
					// Prevent os compare same player with their self
					continue;
				float xSub = tempPlayers[i].model->GetX() - tempPlayers[j].model->GetX(),
					ySub = tempPlayers[i].model->GetY() - tempPlayers[j].model->GetY(),
					zSub = tempPlayers[i].model->GetZ() - tempPlayers[j].model->GetZ();

				// Prossesed whit the dot product
				float xF, yF, zF;
				facingVector(tempPlayers[j].model, xF, yF, zF);

				//swap the player position if they are in the same total stage but dot product find the a head one
				if (dotProduct(xF, yF, zF, xSub, ySub, zSub)>0)
				{
					Temp tempPlayer = tempPlayers[i];
					tempPlayers[i] = tempPlayers[j];
					tempPlayers[j] = tempPlayer;
				}
			}
		}
	}
	else
	{
		//Different stage, sort by laps
		std::sort(tempPlayers, tempPlayers + kNHovers, [](Temp const & a, Temp const & b) -> bool
		{
			return a.totalStagePassed > b.totalStagePassed;
		}
		);
	}

	//if any player passed the last lap (game is over)
	for (int i = 0; i < kNHovers; i++)
	{
		if (tempPlayers[i].playerLap == lastLap)
		{
			gameState = gameState::Over;
			winner = tempPlayers[i].playerName;
		}
	}
	for (int i = 0; i < kNHovers; i++)
	{
		if (tempPlayers[i].id == 0)
		{
			player[0].currentPos = i + 1;
		}
		else if(tempPlayers[i].id == 1)
		{
			player[1].currentPos = i + 1;
		}
		else
		{
			player[2].currentPos = i + 1;
		}
	}
	
}

// Reset Particle variable by random number
void resetParticleState(t_particle& p)
{
	p.vx = p.vx*random(-0.1, 0.1);
	p.vy = p.vy*random(0.1, 1);
	p.vz = p.vz*random(-0.1, 0.1);
}

// Making the hover fire ready
void fire(t_particle aParticle[], IMesh* FireparticleM, IMesh* dummyM)
{
	for (int i = 0; i < kNParticles; i++)
	{
		aParticle[i].dummy = dummyM->CreateModel();
		aParticle[i].model = FireparticleM->CreateModel(0,-20,0);
		aParticle[i].model->Scale(0.1);
		aParticle[i].model->SetSkin("Fire.jpg");
		aParticle[i].vx = xP;
		aParticle[i].vy = yP;
		aParticle[i].vz = zP;
		resetParticleState(aParticle[i]);

	}
}

// Run the fire
void runFire(t_particle aParticle[], float dt, IModel* hover)
{
	for (int i = 0; i < kNParticles; i++)
	{
		//1) Dynamic due the gravity:
		//-------------------------------------
		//1a) Update the velocity:
		aParticle[i].vx = aParticle[i].vx + kgx * dt;
		aParticle[i].vy = aParticle[i].vy + kgy * dt;
		aParticle[i].vz = aParticle[i].vz + kgz * dt;

		//2b)With new velocity calculate new position:
		float xOld = aParticle[i].model->GetX();
		float yOld = aParticle[i].model->GetY();
		float zOld = aParticle[i].model->GetZ();

		// New Position =  Old Position + V * dt    
		float xNew = xOld + aParticle[i].vx * dt;
		float yNew = yOld + aParticle[i].vy * dt;
		float zNew = zOld + aParticle[i].vz * dt;

		//Bounce
		if (yNew < 0.1)
		{
			yNew = aParticle[i].dummy->GetY() + 1;

		}

		if (yNew > yP)
		{
			yNew = aParticle[i].dummy->GetY() + 1;
		}

		if (yNew < 2)
		{
			if (abs(xNew) > 0.25)
			{
				xNew = aParticle[i].dummy->GetX();
			}

			if (abs(zNew) > 0.25)
			{
				zNew = aParticle[i].dummy->GetZ();

			}
		}

		if (abs(xNew) > 0.5)
		{
			xNew = aParticle[i].dummy->GetX();
		}

		if (abs(zNew) > 0.5)
		{
			zNew = aParticle[i].dummy->GetZ();
			
		}

		if ((xNew*xNew + yNew*yNew) > 3 * 3)
		{
			xNew = aParticle[i].dummy->GetX();
		}

		if ((zNew*zNew + yNew*yNew) > 3 * 3)
		{
			zNew = aParticle[i].dummy->GetZ();
		}

		if (aParticle[i].vy < -20)
		{
			aParticle[i].vy = kgy;
		}


		// attach to the parent
		aParticle[i].model->AttachToParent(aParticle[i].dummy);
		aParticle[i].dummy->AttachToParent(hover);
		//3) Update particale position
		aParticle[i].model->SetPosition(xNew, yNew, zNew);

	}
}

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("C:\\ProgramData\\TL-Engine\\Media");
	myEngine->AddMediaFolder("Media");
	myEngine->AddMediaFolder("Extra Models");
	myEngine->AddMediaFolder("Extra");
	myEngine->AddMediaFolder("Defult");


	/**** Set up your scene here ****/

	//-------------------------------------------------//
	//              Useful Variable                    //
	//-------------------------------------------------//

	int            gameState                 = gameState::Waiting;
	int            hoverStateDirection       = stateHover::HoverWaiting;
	int            boosterState              = boostState::Ready;
	int            x[kNHovers]               = { 0,-10,10 };
	float          cameraStepSpeed           = 0.5f;
	float          boosterTimer              = 0;
	float          time                      = 6;
	float          gameTimer                 = 0;
	float          finalHoverSpeed           = 0;
	const float    gT                        = -0.001;
	const int      kNTribunes                = 5;
	const int      kNNarrowModel             = 4;

	stringstream   timeText;
	stringstream   speedInfo;
	stringstream   stageInfo;
	stringstream   boosterInfo;
	stringstream   healthInfo;
	stringstream   lapInfo;
	stringstream   curentPosition;
	stringstream   winerAnnouncement;
    stringstream   ranking;
	stringstream   Skin;
	
	boosterInfo    << "Boost: READY";
	healthInfo     << "Health: 100";
	healthInfo     << "Lap: 0/" << lapState::lastLap;
	curentPosition << "curentPosition: " << kNHovers / 2 + 1;
	boosterInfo    << "Boost: READY";

	/***************************************************/

	//-------------------------------------------------//
	// Camera,Meshes,Models,Sprite,Fonts, State Set up //
	//-------------------------------------------------//
	// Camera
	ICamera* camera    = myEngine->CreateCamera(kManual);
	// Fonts		   
	IFont* font        = myEngine->LoadFont("Adobe Garamond Pro", 50);
	IFont* message     = myEngine->LoadFont("Monotype Corsiva", 200);
	IFont* myFont      = myEngine->LoadFont("Comic Sans MS", 32);
	IFont* Ranking     = myEngine->LoadFont("stencil std", 16);

	// Meshes		   
	IMesh* skyboxM     = myEngine->LoadMesh("skybox 07.x");
	IMesh* groundM     = myEngine->LoadMesh("ground.x");	
	IMesh* hoverM      = myEngine->LoadMesh("race2.x");
	IMesh* dummyM      = myEngine->LoadMesh("Dummy.x");
	IMesh* tribuneM    = myEngine->LoadMesh("Tribune1.x");
	IMesh* fParticleM  = myEngine->LoadMesh("Cube.x");
	IMesh* wallM       = myEngine->LoadMesh("wall.x");
	IMesh* isleCrossM  = myEngine->LoadMesh("IsleCross.x");
	IMesh* lampM       = myEngine->LoadMesh("Lamp.x");
	// Sprite
	ISprite* backdrop  = myEngine->CreateSprite("Moha_backdrop.jpg",-150, 570);
	// Models 
	IModel* skybox     = skyboxM->CreateModel(0.0, -960.0, 0.0);
	IModel* ground     = groundM->CreateModel();
	IModel* hoverModel   = dummyM->CreateModel(0,0,0);
	// Arrays Models
	IModel* aCheckpoint[kNChechpoints];
	IModel* aStraightTrackDummy[kNSDummy];
	IModel* aCornerDummy[kNCDummy];
	IModel* aWaypoint[kNWaypoint];
	IModel* aIsle[kNIsles];
	IModel* aWall[kNWalls];
	IModel* aNarrowaIsle[kNNarrowModel];
	IModel* aNarrowaLamp[kNNarrowModel];
	IModel* aNarrowaWall[kNNarrowModel];
	IModel* aTribune[kNTribunes];
	IModel* aWaterTank[kNWaterTanks];
	Player  aPlayerobject[kNHovers];
	IModel* aWalkway[kNWays];
	Cross aCross[kNChechpoints];
	// Array Classes
	t_particle aMFParticle[kNHovers][kNParticles];
	//*************************************************//

	//-------------------------------------------------//
	//              Some initializeing                 //
	//-------------------------------------------------//
	//Hover skin array


	//Player name , skin setup
	for (int i = 0; i < kNHovers; i++)
	{
		Skin << "sp0" << i+1 << ".jpg";
		aPlayerobject[i].hocerCroft = hoverM->CreateModel(x[i],0,0);
		aPlayerobject[i].hocerCroft->SetSkin(Skin.str());
		Skin.str("");
		if (i != 0)
		{
			aPlayerobject[i].Name = "Player " + std::to_string(i);		
		}
		else
		{
			aPlayerobject[i].Name = "You      ";
		}
	}

	// Tribunes models
	for (int i = 0; i < kNTribunes; i++)
	{
		float const xtribune = -20 + ((i % 2) * 40),
		         	ytribune = 0,
			        ztribune = 20 + (i / 2) * 40;
		aTribune[i] = tribuneM->CreateModel(xtribune, ytribune, ztribune);
	}
	
	// Extra narrowed Walls, Isle, and Lamp model
	for (int i = 0; i < kNNarrowModel; i++)
	{
		// Walls
		float const xNarrowaWall = -57 - (i / 2) * 28,
		            yNarrowaWall = 0,
		            zNarrowaWall = 118 + (i % 2) * 13,
		            RNarrowaWall = -90;
		aNarrowaWall[i] = wallM->CreateModel(xNarrowaWall, yNarrowaWall, zNarrowaWall);
		aNarrowaWall[i]->RotateY(RNarrowaWall);

		// Isle
		float const xNarrowaIsle = -49 - (i / 2) * 43, 
		            yNarrowaIsle = 0,
		            zNarrowaIsle = 114 + (i % 2) * +22,
		            RNarrowaIsle = -45;
		aNarrowaIsle[i] = isleCrossM->CreateModel(xNarrowaIsle, yNarrowaIsle, zNarrowaIsle);
		aNarrowaIsle[i]->RotateY(RNarrowaIsle);

		// Lamp
		float const xNarrowaLamp = xNarrowaIsle,
		            yNarrowaLamp = 9,
		            zNarrowaLamp = 116 + (i % 2) * +18, 
		            RNarrowaLamp = (((i - 2) * 2) + 1) * 90-90;
		aNarrowaLamp[i] = lampM->CreateModel(xNarrowaLamp, yNarrowaLamp, zNarrowaLamp);
		aNarrowaLamp[i]->RotateY(RNarrowaLamp);
	}

	// Stage achivment array initialize
	checkpointCrossStateInitialize(aPlayerobject);

	// Race track
	road objectroad;
	objectroad.straightRaceTrack(0, aWaypoint, 0, aStraightTrackDummy, aCheckpoint, aIsle, aWall, aWalkway, aCross, 0, 0, 0, myEngine);
	objectroad.turnRaceTrack(1, aWaypoint, 0, aCornerDummy, aWaterTank, 0, 100, 0, myEngine);
	objectroad.narrowRaceTrack(2, aWaypoint, 1, aStraightTrackDummy, aCheckpoint, aIsle, aWall, aWalkway, aCross, -25, 125, -90, myEngine);
	objectroad.turnRaceTrack(3, aWaypoint, 1, aCornerDummy, aWaterTank, -150, 150, 180, myEngine);
	objectroad.straightRaceTrack(4, aWaypoint, 2, aStraightTrackDummy, aCheckpoint, aIsle, aWall, aWalkway, aCross, -150, 150, 0, myEngine);
	objectroad.turnRaceTrack(5, aWaypoint, 2, aCornerDummy, aWaterTank, -125, 275, -90, myEngine);
	objectroad.straightRaceTrack(6, aWaypoint, 3, aStraightTrackDummy, aCheckpoint, aIsle, aWall, aWalkway, aCross, -125, 275, 90, myEngine);
	objectroad.straightRaceTrack(7, aWaypoint, 4, aStraightTrackDummy, aCheckpoint, aIsle, aWall, aWalkway, aCross, -25, 275, 90, myEngine);
	objectroad.turnRaceTrack(8, aWaypoint, 3, aCornerDummy, aWaterTank, 100, 250, 0, myEngine);
	objectroad.straightRaceTrack(9, aWaypoint, 5, aStraightTrackDummy, aCheckpoint, aIsle, aWall, aWalkway, aCross, 100, 250, 180, myEngine);
	objectroad.turnRaceTrack(10, aWaypoint, 4, aCornerDummy, aWaterTank, 100, 150, 180, myEngine);
	objectroad.narrowRaceTrack(11, aWaypoint, 6, aStraightTrackDummy, aCheckpoint, aIsle, aWall, aWalkway, aCross, 125, 125, 90, myEngine);
	objectroad.turnRaceTrack(12, aWaypoint, 5, aCornerDummy, aWaterTank, 250, 100, 0, myEngine);
	objectroad.straightRaceTrack(13, aWaypoint, 7, aStraightTrackDummy, aCheckpoint, aIsle, aWall, aWalkway, aCross, 250, 100, 180, myEngine);
	objectroad.turnRaceTrack(14, aWaypoint, 6, aCornerDummy, aWaterTank, 225, -25, 90, myEngine);
	objectroad.straightRaceTrack(15, aWaypoint, 8, aStraightTrackDummy, aCheckpoint, aIsle, aWall, aWalkway, aCross, 225, -25, -90, myEngine);
	objectroad.straightRaceTrack(16, aWaypoint, 9, aStraightTrackDummy, aCheckpoint, aIsle, aWall, aWalkway, aCross, 125, -25, -90, myEngine);

	// Basic hover fire set up
	for (int i = 0; i < kNHovers; i++)
	{
		fire(aMFParticle[i], fParticleM, dummyM);
	}

	// Some Setting
	camera->SetPosition(0, 20, -40);
	camera->RotateX(20);
	aPlayerobject[0].hocerCroft->AttachToParent(hoverModel);
	camera->AttachToParent(hoverModel);
	//*************************************************//

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

		//-------------------------------------------------//
		//              usefue calculations                //
		//-------------------------------------------------//
		float dt = myEngine->Timer();
		float speedGame = kSpeed*dt;
		camera->SetRotationSpeed(100);
		// Get access to the mouse Movement:
		float mouseMovementX = myEngine->GetMouseMovementX();
		//**************************************************//

		//-------------------------------------------------//
		//                   Game State                    //
		//-------------------------------------------------//
		switch (gameState)
		{
		case gameState::Waiting:
			//Start up dialuge
			font->Draw("Hit Space to Start", 660, 480, kWhite, kCentre, kTop);
			if (myEngine->KeyHit(Key_Space))
			{
				gameState = gameState::PreStart;
				aPlayerobject[0].hocerCroft->ResetOrientation();
				aPlayerobject[0].zRotate = 0;
			}

			if (myEngine->KeyHeld(Key_W)/* W button is held*/)
			{
				if (aPlayerobject[0].xRotate < 5)
				{
					aPlayerobject[0].xRotate += 0.2;
					aPlayerobject[0].hocerCroft->RotateLocalX(+2 * speedGame);
				}
			}
			else
			{
				if (aPlayerobject[0].xRotate > 0)
				{
					aPlayerobject[0].xRotate -= 0.2;
					aPlayerobject[0].hocerCroft->RotateLocalX(-2 * speedGame);
				}
			}

			// Backward setting
			if (myEngine->KeyHeld(Key_S)/* S button is held*/)
			{
				if (aPlayerobject[0].xRotate > -5)
				{
					aPlayerobject[0].xRotate -= 0.2;
					aPlayerobject[0].hocerCroft->RotateLocalX(-2 * speedGame);
				}
			}
			else
			{
				if (aPlayerobject[0].xRotate < 0)
				{
					aPlayerobject[0].xRotate += 0.2;
					aPlayerobject[0].hocerCroft->RotateLocalX(+2 * speedGame);
				}
			}

			// Turn to Right Setting 

					aPlayerobject[0].hocerCroft->RotateY(5 * speedGame);
					if (aPlayerobject[0].zRotate > -25)
					{
						aPlayerobject[0].zRotate -= 0.5;
						aPlayerobject[0].hocerCroft->RotateLocalZ(-0.5);
					}

			break;

			// Countdown
		case gameState::PreStart:

			if (int(time) >= 0)
				time -= dt;
			timeText.str("");
			if (int(time) > 2)  	timeText << int(time) - 2;
			if (int(time) == 2)  	timeText << "Ready!?";
			if (int(time) == 1)  	timeText << "Go!";
			
			message->Draw(timeText.str(), 650, 250, kRed, kCentre, kTop);

			if (int(time) <= 0)		gameState = gameState::Playing;
			break;

			// Play state 
		case gameState::Playing:
			//Game timer
			gameTimer += dt;
			
			//----------------------------------------------//
			//               collision check                //
			//----------------------------------------------//

			// Checkpoin
			checCheckpointCollision(kNChechpoints, aCheckpoint, aPlayerobject, aCross);
			// Hovers
			checkHoversCollision(aPlayerobject,hoverModel);

			for (int i = 1; i < kNHovers; i++)
			{			
					// Isles
					checkIsleCollision(kNIsles, aIsle, aPlayerobject[i].hocerCroft, aPlayerobject[i].acceleration, aPlayerobject[i].health, aPlayerobject[i].collision, dt);
					// Walls
					checkWallCollision(trackRotate, kNWalls, aWall, aPlayerobject[i].hocerCroft, aPlayerobject[i].acceleration, aPlayerobject[i].health, aPlayerobject[i].collision, 0, dt);
					//Extra narrow walls
					checkWallCollision(trackRotate, kNNarrowModel, aNarrowaWall, aPlayerobject[i].hocerCroft, aPlayerobject[i].acceleration, aPlayerobject[i].health, aPlayerobject[i].collision, 0, dt);
					// Water tanks
					checkSpareCollision(kNWaterTanks, aWaterTank, aPlayerobject[i].hocerCroft, aPlayerobject[i].acceleration, aPlayerobject[i].health, aPlayerobject[i].collision, 5, dt);
					// Tribunes
					checkSpareCollision(kNTribunes, aTribune, aPlayerobject[i].hocerCroft, aPlayerobject[i].acceleration, aPlayerobject[i].health, aPlayerobject[i].collision, 5, dt);
					// Cross isle
					checkSpareCollision(kNNarrowModel, aNarrowaIsle, aPlayerobject[i].hocerCroft, aPlayerobject[i].acceleration, aPlayerobject[i].health, aPlayerobject[i].collision, 8, dt);
			}
			// Isles
			checkIsleCollision(kNIsles, aIsle, hoverModel, aPlayerobject[0].acceleration, aPlayerobject[0].health, aPlayerobject[0].collision, dt);
			// Walls
			checkWallCollision(trackRotate, kNWalls, aWall, hoverModel, aPlayerobject[0].acceleration, aPlayerobject[0].health, aPlayerobject[0].collision, 0, dt);
			//Extra narrow walls
			checkWallCollision(trackRotate, kNNarrowModel, aNarrowaWall, hoverModel, aPlayerobject[0].acceleration, aPlayerobject[0].health, aPlayerobject[0].collision, 90, dt);
			// Water tanks
			checkSpareCollision(kNWaterTanks, aWaterTank, hoverModel, aPlayerobject[0].acceleration, aPlayerobject[0].health, aPlayerobject[0].collision, 5, dt);
			// Tribunes
			checkSpareCollision(kNTribunes, aTribune, hoverModel, aPlayerobject[0].acceleration, aPlayerobject[0].health, aPlayerobject[0].collision, 5, dt);
			// Cross isle
			checkSpareCollision(kNNarrowModel, aNarrowaIsle, hoverModel, aPlayerobject[0].acceleration, aPlayerobject[0].health, aPlayerobject[0].collision, 8, dt);

			/************************************************/

			//----------------------------------------------//
			//              Transportation                  //
			//----------------------------------------------//
			// Hover fire particle
			for (int i = 0; i < kNHovers; i++)
			{
				if (aPlayerobject[i].collision)
				{
					aPlayerobject[i].colisionTimer += dt;
				}
				if(aPlayerobject[i].colisionTimer>=3)
				{
					aPlayerobject[i].collision = false;
					aPlayerobject[i].colisionTimer = 0;
					if (i == 0) 
					{
						float newX = hoverModel->GetX();
						float newZ = hoverModel->GetZ();
						newX += 5 * newX / abs(newX);
						newZ += 5 * newZ / abs(newZ);
						hoverModel->SetX(newX);
						hoverModel->SetZ(newZ);
					}
					else
					{
						float newX = aPlayerobject[i].hocerCroft->GetLocalX();
						float newZ = aPlayerobject[i].hocerCroft->GetLocalZ();
						newX += 5 * newX / abs(newX);
						newZ += 5 * newZ / abs(newZ);
						aPlayerobject[i].hocerCroft->SetLocalX(newX);
						aPlayerobject[i].hocerCroft->SetLocalZ(newZ);
					}
				}
				runFire(aMFParticle[i], dt, aPlayerobject[i].hocerCroft);
			}
			// Own hover
			handlTheOwnHover(hoverStateDirection, hoverModel, aPlayerobject[0].health, aPlayerobject[0].acceleration, aPlayerobject[0].collision, dt, gT,gameTimer);
			// None-player 
			handlTheNonePlayerHover(aWaypoint, aPlayerobject, dt, gT, gameTimer);
			/************************************************/

			//-----------------------------------------------//
			//                  Handle the game              //
			//-----------------------------------------------//

			for (int i = 0; i < kNHovers; i++)
			{
				//if hover truly passed the chechpoints lap++
				if (aPlayerobject[i].currentState == lastStage)
				{
					for (int j = 0; j < kNChechpoints; j++)
					{
						aPlayerobject[i].crossed[j] = false;
					}
					if (aPlayerobject[i].currentLap < lapState::lastLap)
					{
						aPlayerobject[i].currentState = 0;
					}
					aPlayerobject[i].currentLap++;
				}
			}

			// Cross Show
			for (int i = 0; i < kNChechpoints; i++)
			{
				//if Croos is enable
				if (aCross[i].enable)
				{
					//Cross time check
					aCross[i].lifeTime += dt;
					if (aCross[i].lifeTime >= 10)
					{
						aCross[i].lifeTime = 0;
						aCross[i].enable = false;
						aCross[i].model->SetY(-20);
					}
				}
			}


				// Hower state is waiting
				if (hoverStateDirection == stateHover::HoverWaiting)
				{
					// Foreward Setiing
					if (myEngine->KeyHeld(Key_W)/* W button is held*/)
					{
						if (aPlayerobject[0].xRotate < 10)
						{
							aPlayerobject[0].xRotate += 0.1;
							aPlayerobject[0].hocerCroft->RotateX(0.1);
						}
						hoverStateDirection = stateHover::HoverForward;
					}
					else
					{
						if (aPlayerobject[0].xRotate > 0)
						{
							aPlayerobject[0].xRotate -= 0.1;
							aPlayerobject[0].hocerCroft->RotateX(-0.1);
						}
					}

					// Backward setting
					if (myEngine->KeyHeld(Key_S)/* S button is held*/)
					{
						if (aPlayerobject[0].xRotate > -10)
						{
							aPlayerobject[0].xRotate -= 0.1;
							aPlayerobject[0].hocerCroft->RotateX(-0.1);
						}

						hoverStateDirection = stateHover::HoverBackward;
					}
					else
					{
						if (aPlayerobject[0].xRotate < 0)
						{
							aPlayerobject[0].xRotate += 0.1;
							aPlayerobject[0].hocerCroft->RotateX(0.1);
						}
					}
				}

				if (!aPlayerobject[0].collision)
				{
					// Turn to Right Setting 
					if (myEngine->KeyHeld(Key_D)/* D button is held*/)
					{
						if (aPlayerobject[0].acceleration >= 0)
						{
							hoverModel->RotateY(5 * speedGame);
							if (aPlayerobject[0].zRotate > -25)
							{
								aPlayerobject[0].zRotate -= 0.5;
								hoverModel->RotateLocalZ(-0.5);
							}
						}
						else
						{
							hoverModel->RotateY(-3 * speedGame);
							if (aPlayerobject[0].zRotate < 25)
							{
								aPlayerobject[0].zRotate += 0.5;
								hoverModel->RotateLocalZ(0.5);
							}
						}
					}
					else
					{
						if (aPlayerobject[0].zRotate < 0)
						{
							aPlayerobject[0].zRotate += 0.5;
							hoverModel->RotateLocalZ(0.5);
						}
					}

					// Turn to Left Setting
					if (myEngine->KeyHeld(Key_A)/* A button is held*/)
					{
						if (aPlayerobject[0].acceleration >= 0)
						{
							hoverModel->RotateY(-5 * speedGame);
							if (aPlayerobject[0].zRotate < 25)
							{
								aPlayerobject[0].zRotate += 0.5;
								hoverModel->RotateLocalZ(0.5);
							}
						}
						else
						{
							hoverModel->RotateY(3 * speedGame);
							if (aPlayerobject[0].zRotate > -25)
							{
								aPlayerobject[0].zRotate -= 0.5;
								hoverModel->RotateLocalZ(-0.5);
							}
						}
					}
					else
					{
						if (aPlayerobject[0].zRotate > 0)
						{
							aPlayerobject[0].zRotate -= 0.5;
							hoverModel->RotateLocalZ(-0.5);
						}
					}
				}

				//booster functionality
				if (boosterState == OverHit)
				{
					boosterTimer -= dt;
					boosterInfo.str("");
					boosterInfo << "Boost: OVERHIT";
				}

				if (boosterTimer <= 0)
				{
					boosterInfo << "Boost: READY";
					boosterState = Active;
					boosterTimer = 0;
				}

				if (boosterState == Active)
				{
					if (myEngine->KeyHeld(Key_Space))
					{
						boosterInfo << "Boost: ACTIVATED";
						boosterTimer += dt;
						aPlayerobject[0].acceleration = hightSpeedLimit * 2;

						if (2 < boosterTimer && boosterTimer < 3)
						{
							boosterInfo.str("");
							boosterInfo << "Boost: WARNING";
						}
						if (boosterTimer > 3)
						{
							boosterInfo.str("");
							boosterInfo << "Boost: OVERHIT";
							boosterTimer = OverHit;
							boosterTimer = 5;
							aPlayerobject[0].acceleration = 0.01;
						}
					}
					else
					{
						if (boosterTimer > 0)
						{
							boosterTimer -= dt;
							boosterInfo << "Boost: LOADING";
						}
					}
				}

				else
				{
					boosterInfo << "Boost: LOADING";
				}
			
			// Pause the game
			if (myEngine->KeyHit(Key_P)/* P button is hit*/)
			{
				gameState = gameState::Paused;
			}


			break;

		case gameState::Paused:

			//Resume the game
			if (myEngine->KeyHit(Key_P)/* P button is hit*/)
			{
				gameState = gameState::Playing;
			}
			break;

		case gameState::Over:
			// show the Game Over massagee , winner and reset keys
			winerAnnouncement << "The Winner is: " << winner << "  race time: " << int(gameTimer) << " S" << endl;
			font->Draw(winerAnnouncement.str(), 660, 380, kWhite, kCentre, kTop);
			font->Draw("Press 'R' key to reset the game", 660, 450, kWhite, kCentre, kTop);
			message->Draw("Game Over", 650, 150, kWhite, kCentre, kTop);
			winerAnnouncement.str("");

			 //ReStart
			if (myEngine->KeyHit(Key_R))
			{
				// Reset the model position
				for (int i = 1; i < kNHovers; i++)
				{
					aPlayerobject[i].hocerCroft->SetLocalPosition(x[i], 0, 0);
					aPlayerobject[i].hocerCroft->ResetOrientation();
					aPlayerobject[i].currentState = stageState::first;
					aPlayerobject[i].currentLap = lapState::firstLap;
					aPlayerobject[i].currentPos = first;
					aPlayerobject[i].currentWaypoint = first;
					aPlayerobject[i].totalStagePassed = 0;
					aPlayerobject[i].Score = 0;
					aPlayerobject[i].health = 100;
					aPlayerobject[i].acceleration = 0.000000f;
					aPlayerobject[i].xRotate = 0;
					aPlayerobject[i].zRotate = 0;
					aPlayerobject[i].hoverEffect = 0;
				}
				aPlayerobject[0].hocerCroft->ResetOrientation();
				hoverModel->SetLocalPosition(0, 0, 0);
				hoverModel->ResetOrientation();
				// Reset the variables used in the game
				checkpointCrossStateInitialize(aPlayerobject);
				speedGame = kSpeed*dt;
				hoverStateDirection = stateHover::HoverWaiting;
				time = 6;
				gameTimer = 0;
				boosterTimer = 0;
				boosterState = boostState::Ready;
				boosterInfo << "Boost: READY";
				healthInfo << "Health: 100";
				healthInfo << "Lap: 0/" << lapState::lastLap;
				curentPosition << "curentPosition: " << kNHovers / 2 + 1;
				camera->ResetOrientation();
				camera->DetachFromParent();
				camera->SetPosition(0, 20, -40);
				camera->RotateX(20);
				camera->AttachToParent(hoverModel);
				aPlayerobject[0].hocerCroft->AttachToParent(hoverModel);

				gameState = gameState::Waiting;
			}
			break;
		}
	
		for (int i = 0; i < kNHovers; i++)
		{

			// Cheack the chechpoints passed by Any Player and dialuge it
			aPlayerobject[i].currentState = checkCrossedCheckpoint(kNChechpoints, aPlayerobject[i]);
			aPlayerobject[i].totalStagePassed = aPlayerobject[i].currentLap*stageState::lastStage + aPlayerobject[i].currentState;

            // hover wave along Y axes and effects
			if (aPlayerobject[i].health > 0)
			{
				if (aPlayerobject[i].hoverEffect < maxHoverEffect)
				{
					aPlayerobject[i].hoverEffect += random(speedGame, speedGame / 2);
					if (aPlayerobject[i].hoverEffect > maxHoverEffect)
						aPlayerobject[i].hoverEffect = 0;
				}
				if (i != 0)
				{
					aPlayerobject[i].hocerCroft->SetY(sin(aPlayerobject[i].hoverEffect / 10) + 1);
				}
				else
				{
					hoverModel->SetY(sin(aPlayerobject[i].hoverEffect / 10) + 1);
				}
			}
			else
			{
				aPlayerobject[i].hocerCroft->SetY(-1);
			}
		}

		//current state dialuge setup
		if (aPlayerobject[0].currentState == stageState::first)
		{
			stageInfo << "Start";
		}
		else if (aPlayerobject[0].currentState == stageState::lastStage)
		{
			stageInfo << "Race complete";
		}
		else
		{
			stageInfo << "Stage " << aPlayerobject[0].currentState << " complete ";
		}
		//Position dialuge setup
		checkPosition(aPlayerobject, gameState);

		// Dialugs setup
		speedInfo          << "Speed: "      << int(aPlayerobject[0].acceleration * 300);
		healthInfo         << "Health: "     << int(aPlayerobject[0].health)     << "%"  << endl;
		lapInfo            << "Lap: "        << aPlayerobject[0].currentLap      <<"/"   << lastLap << endl;
		curentPosition     << "POS: "        << aPlayerobject[0].currentPos      <<"/"   <<kNHovers<< endl;
		ranking            << "1. "          << tempPlayers[0].playerName
		                   << "  St: "       << tempPlayers[0].playerState   
		                   << "  Lp:  "      << tempPlayers[0].playerLap 
		                   << "  Hp:  "      << int(tempPlayers[0].health)
		                   << "%\n2. "       << tempPlayers[1].playerName
		                   << "  St: "       << tempPlayers[1].playerState 
		                   << "  Lp:  "      << tempPlayers[1].playerLap  
			               << "  Hp:  "      << int(tempPlayers[1].health)
		                   << "%\n3. "       << tempPlayers[2].playerName
		                   << "  St: "       << tempPlayers[2].playerState
		                   << "  Lp:  "      << tempPlayers[2].playerLap         
		                   << "  Hp:  "      << int(tempPlayers[2].health)        <<"%"   << endl;
		// The dialuge show 
		Ranking->Draw(ranking.str()       , 15 , 660, kWhite,kLeft,kVCentre);
		myFont->Draw(speedInfo.str()      , 310, 625, kRed);
		myFont->Draw(stageInfo.str()      , 580, 625, kRed);
		myFont->Draw(curentPosition.str() , 850, 625, kRed);
		myFont->Draw(healthInfo.str()     , 310, 680, kRed);
		myFont->Draw(boosterInfo.str()    , 580, 680, kRed);
		myFont->Draw(lapInfo.str()        , 850, 680, kRed);
		
		// Reset the dialuge
		ranking.str("");
		speedInfo.str("");
		stageInfo.str("");
		healthInfo.str("");
		boosterInfo.str("");
		lapInfo.str("");
		curentPosition.str("");

		//******************************************************//
	
		// camera Movement;
		// moves the camera forward
		if (myEngine->KeyHeld(Key_Up)/* Up button is held*/)
		{
			if (myEngine->KeyHeld(Key_Shift) && camera->GetY() <1000)
			{
				camera->ResetOrientation();
				if (myEngine->KeyHeld(Key_Control))
				{
					camera->MoveY(cameraStepSpeed);
				}
				else
				{
					camera->MoveZ(cameraStepSpeed);
				}

			}
			if (camera->GetY() > 5)
			{
				camera->ResetOrientation();
				camera->RotateX(20);
				camera->MoveLocalZ(+cameraStepSpeed);
			}
		}

		// moves the camera backward
		if (myEngine->KeyHeld(Key_Down)/* Down button is held*/)
		{
			if (myEngine->KeyHeld(Key_Shift) && camera->GetY() >0.2)
			{
				camera->ResetOrientation();
				if (myEngine->KeyHeld(Key_Control))
				{
					camera->MoveY(-cameraStepSpeed);
				}
				else
				{
					camera->MoveZ(-cameraStepSpeed);
				}
			}
			if (camera->GetY() < 50)
			{
				camera->ResetOrientation();
				camera->RotateX(20);
				camera->MoveLocalZ(-cameraStepSpeed);
			}
		}

		// rotates the camera right.
		if (myEngine->KeyHeld(Key_Right)/* Right button is held*/)
		{
			if (myEngine->KeyHeld(Key_Shift))
			{
				camera->ResetOrientation();
				camera->MoveX(cameraStepSpeed);
			}
			/*camera->MoveLocalX(+cameraStep);*/
			camera->RotateY(cameraStepSpeed);
		}

		// rotates the camera left.
		if (myEngine->KeyHeld(Key_Left)/* Left button is held*/)
		{
			if (myEngine->KeyHeld(Key_Shift))
			{
				camera->ResetOrientation();
				camera->MoveX(-cameraStepSpeed);
			}
			/*camera->MoveLocalX(-cameraStep);*/
			camera->RotateY(-cameraStepSpeed);
		}

		//Mouse movement rotates the camera.
		camera->RotateY(mouseMovementX * cameraStepSpeed);

		// Set first person Camera Position
		if (myEngine->KeyHit(Key_2)/* 2 button is held*/)
		{
			camera->ResetOrientation();
			camera->DetachFromParent();
			camera->SetLocalY(5);
			camera->SetLocalZ(0.9);
			camera->AttachToParent(hoverModel);
		}

		// Reset Camera Position
		if (myEngine->KeyHit(Key_1)/* 1 button is held*/)
		{
			camera->ResetOrientation();
			camera->DetachFromParent();
			camera->SetPosition(0, 20, -40);
			camera->RotateX(20);
			camera->AttachToParent(hoverModel);
		}

		
		if (myEngine->KeyHit(Key_R))
		{
			// Reset the model position
			for (int i = 0; i < kNHovers; i++)
			{
				int temp = random(1, 6.99);
				Skin << "sp0" << temp << ".jpg";
				aPlayerobject[i].hocerCroft->SetSkin(Skin.str());
				Skin.str("");
			}
		}

		//Exit	
		if (myEngine->KeyHit(Key_Escape))
		{
			myEngine->Stop();
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
