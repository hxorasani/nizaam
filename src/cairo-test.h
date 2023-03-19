#ifndef _cairo_test_h
#define _cairo_test_h

#include "amr.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/input.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include "matn.h"
#include "kaatib.h"
#include "alwaan.h"
#include <cairo.h>
#include "cairo_jpg.h"
#include "physics/tinyphysicsengine.h"


//{ physics
TPE_World tpe_world;
int targetbody = 0;

#define ACCELERATION (TPE_F / 45)
#define ROOM_SIZE (40 * TPE_F)
#define RES_X 480
#define RES_Y 590
#define ROOM_W (TPE_F * 10)
#define ROOM_H ((RES_Y * ROOM_W) / RES_X)

#define MAX_BODIES 128
#define MAX_JOINTS 1024
#define MAX_CONNECTIONS 2048

TPE_Body tpe_bodies[MAX_BODIES];
TPE_Joint tpe_joints[MAX_JOINTS];
TPE_Connection tpe_connections[MAX_CONNECTIONS];

#define physics_lastBody tpe_world.bodies[tpe_world.bodyCount - 1]

unsigned int 
  physics_jointsUsed = 0,
  physics_connectionsUsed = 0;



TPE_Vec3 environmentDistance(TPE_Vec3 point, TPE_Unit maxDistance) {
//	return TPE_envGround(point,0); // just an infinite flat plane
	return TPE_envAABoxInside(point,TPE_vec3(0,0,0),TPE_vec3(ROOM_W,ROOM_H,ROOM_W));
//	return TPE_envAABoxInside(point,TPE_vec3(ROOM_SIZE / 2,ROOM_SIZE / 2,0),
//		TPE_vec3(ROOM_SIZE,ROOM_SIZE,ROOM_SIZE));
//	return TPE_envAABoxInside(p,TPE_vec3(0,0,0),TPE_vec3(200,200,ROOM_W));
}
void _physics_bodyAdded(int joints, int conns, TPE_Unit mass) {
	TPE_bodyInit(&tpe_bodies[tpe_world.bodyCount],
		&tpe_joints[physics_jointsUsed],joints,
		&tpe_connections[physics_connectionsUsed],conns,mass);
	
	physics_jointsUsed += joints;
	physics_connectionsUsed += conns;
	
	tpe_world.bodyCount++;
}
void physics_addCenterRectFull(TPE_Unit w, TPE_Unit d, TPE_Unit jointSize, TPE_Unit mass) {
	TPE_makeCenterRectFull(
		tpe_joints + physics_jointsUsed,
		tpe_connections + physics_connectionsUsed,w,d,jointSize);
	
	_physics_bodyAdded(5,10,mass);
}
void physics_addBall(TPE_Unit s, TPE_Unit mass) {
	tpe_joints[physics_jointsUsed] = TPE_joint(TPE_vec3(0,0,0),s);
	
	_physics_bodyAdded(1,0,mass);
}

void physics_topx(TPE_Vec3 pos, nuqtahf *nq) {
	nq->x = (float) RES_X/2 + (pos.x * RES_X) / ROOM_W;
	nq->y = (float) RES_Y/2 - (pos.y * RES_Y) / ROOM_H;
}
//}


#endif