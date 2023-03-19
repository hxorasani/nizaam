#ifndef _physics_h
#define _physics_h

#include "physics/tinyphysicsengine.h"


#define ACCELERATION (TPE_F / 45)
#define ROOM_SIZE (40 * TPE_F)
#define RES_X 480
#define RES_Y 590
#define ROOM_W (TPE_F * 10)
#define ROOM_H ((RES_Y * ROOM_W) / RES_X)

#define MAX_BODIES 128
#define MAX_JOINTS 1024
#define MAX_CONNECTIONS 2048

TPE_World tpe_world;
TPE_Body tpe_bodies[MAX_BODIES];
TPE_Joint tpe_joints[MAX_JOINTS];
TPE_Connection tpe_connections[MAX_CONNECTIONS];

int targetbody = 0;

#define physics_lastBody tpe_world.bodies[tpe_world.bodyCount - 1]
#define C(n,a,b) body->connections[n].joint1 = a; body->connections[n].joint2 = b;

unsigned int 
  physics_jointsUsed = 0,
  physics_connectionsUsed = 0;

/*typedef struct
{
  TPE_Joint *joints;
  uint8_t jointCount;
  TPE_Connection *connections;
  uint8_t connectionCount;
  TPE_UnitReduced jointMass;       ///< mass of a single joint
  TPE_UnitReduced friction;        ///< friction of each joint
  TPE_UnitReduced elasticity;      ///< elasticity of each joint
  uint8_t flags;
  uint8_t deactivateCount;
} TPE_Body;*/

// hoisting
int physics_remove(int i);
int physics_bodies();
int physics_ptr2body(TPE_Body *b) {
	for (int j = 0; j < physics_bodies(); ++j) {
		if (&tpe_bodies[j] == b) {
			return j;
		}
	}
	return -1;
}


/* this does the memory management for the physics engine
 * bodies, joints, conns all have preallocated static buffers
 * all this does is move data around and update body pointers and counts
 * joints & conns are calloc'd or statically allocated per TPE_Body
 * also makes manipulation safe
 * */
void _physics_body_init(TPE_Body *body, TPE_Unit mass) {
	TPE_bodyInit(
		body,
		body->joints, body->jointCount,
		body->connections, body->connectionCount,
		mass
	);
}
int physics_bodies() {
	return tpe_world.bodyCount;
}
TPE_Body * physics_get(int i) {
	if (physics_bodies() && i > -1 && i < physics_bodies()) {
		return &tpe_bodies[i];
	}
	return NULL; // error
}
int physics_remove_connection(TPE_Body *body, int i) { // returns true/false
	if (body) { // TODO check if connection exists
		for (int j = i; j < body->connectionCount-1; ++j) {
			body->connections[j] = body->connections[j+1];
		}
		if (body->connectionCount > 0) {
			body->connectionCount--;
			
			_physics_body_init(body, 1);
			return 1;
		}
	}
	return 0;
}
char physics_joint(TPE_Body *body, int joint) { // bool
	if (body) {
		if (joint > -1 && joint < body->jointCount)
			return 1;
	}
	return 0;
}
int physics_add_joint(TPE_Body *body, float size) {
	if (body) {
		body->joints[ body->jointCount ] = TPE_joint(TPE_vec3(0,0,0), TPE_F * size);
		
		body->jointCount++;
		return body->jointCount;
	}
	return 0;
}
int physics_remove_joint(TPE_Body *body, int joint) {
	if (body && physics_joint(body, joint)) {
		// TODO TEST find any connections with joint in them and disconnect them
		for (int j = 0; j < body->connectionCount; ++j) {
			if (body->connections[j].joint1 == joint ||
				body->connections[j].joint2 == joint
			) {
				if (physics_remove_connection(body, j)) {
					j--; // because count gets reduced
				}
			}
		}

		for (int j = joint; j < body->jointCount-1; ++j) {
			body->joints[j] = body->joints[j+1];
		}
		if (body->jointCount > 0)
			body->jointCount--;

		// if no joints are left, delete the body
		if (body->jointCount == 0) {
			physics_remove( physics_ptr2body(body) );
			return 0;
		} else
			return body->jointCount;
	}
	return 0;
}
int physics_connect(TPE_Body *body, int joint1, int joint2) {
	if (body && physics_joint(body, joint1) && physics_joint(body, joint2)
	&& joint1 != joint2) {
		char yes = 0;
		for (int j = 0; j < body->connectionCount; ++j) {
			if (
				(body->connections[j].joint1 == joint1 &&
				body->connections[j].joint2 == joint2)
				||
				(body->connections[j].joint2 == joint1 &&
				body->connections[j].joint1 == joint2)
			) {
				yes = 1; // dont add, already connected
				break;
			}
		}
		if (!yes) {
			body->connections[ body->connectionCount ].joint1 = joint1;
			body->connections[ body->connectionCount ].joint2 = joint2;
			body->connectionCount++;
			_physics_body_init(body, 1);
		}
		return body->connectionCount;
	}
	return 0;
}
int physics_disconnect(TPE_Body *body, int joint1, int joint2) {
	if (body && physics_joint(body, joint1) && physics_joint(body, joint2)) {
		for (int j = 0; j < body->connectionCount; ++j) {
			if (
				(body->connections[j].joint1 == joint1 &&
				body->connections[j].joint2 == joint2)
				||
				(body->connections[j].joint2 == joint1 &&
				body->connections[j].joint1 == joint2)
			) {
				physics_remove_connection(body, j);
				break; // limit to 1
			}
		}
		return body->connectionCount;
	}
	return 0;
}
int physics_add(TPE_Unit mass) {
	if (physics_bodies() < MAX_BODIES) {
		// dont use physics_get() here cuz that validates count
		TPE_Body *body = &tpe_bodies[ physics_bodies() ];
		if (body) {
			body->joints = calloc(MAX_JOINTS, sizeof(TPE_Joint));
			if (!body->joints) { perror("physics_add joints calloc"); exit(EXIT_FAILURE); }
			body->connections = calloc(MAX_CONNECTIONS, sizeof(TPE_Connection));
			if (!body->connections) { perror("physics_add connections calloc"); exit(EXIT_FAILURE); }

			body->jointCount = 0;
			body->connectionCount = 0;

			physics_add_joint(body, 1);
			
			body->friction = 4 * TPE_F / 5;
			body->elasticity = TPE_F / 5;

			_physics_body_init(body, mass);

			tpe_world.bodyCount++;
		}
	}
	return physics_bodies();
}
int physics_remove(int i) {
	if (physics_bodies() && i > -1 && i < physics_bodies()) {
		TPE_Body *body = &tpe_bodies[i];
		if (body) {
			if (body->joints) {
				free(body->joints);
				body->joints = NULL;
			}
			if (body->connections) {
				free(body->connections);
				body->connections = NULL;
			}
			body->jointCount = 0;
			body->connectionCount = 0;
		}

		for (int j = i; j < physics_bodies()-1; ++j) {
			tpe_bodies[j] = tpe_bodies[j+1];
		}
		
		if (tpe_world.bodyCount > 0)
			tpe_world.bodyCount--;
	}
	return physics_bodies();
}

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


void physics_tounits(float x, float y, float z, TPE_Vec3 *pos) {
}
void physics_moveto(int body, float x, float y, float z) {
	if (body > -1 && body < tpe_world.bodyCount) {
		TPE_Vec3 position = { 0 };
		physics_tounits(x, y, z, &position);
		TPE_bodyMoveTo(&tpe_world.bodies[body], position);
	}
}
void physics_topx(TPE_Vec3 pos, nuqtahf *nq) {
	nq->x = (float) RES_X/2 + (pos.x * RES_X) / (float) ROOM_W;
	nq->y = (float) RES_Y/2 - (pos.y * RES_Y) / (float) ROOM_H;
}
void physics_init() {
	for (int i = 0; i < physics_bodies(); ++i) {
		physics_remove(i);
	}
	
	TPE_worldInit(&tpe_world, tpe_bodies, 0, &environmentDistance);
//	tpe_world.environmentFunction = environmentDistance;

	/*for (int i = 0; i < 4; ++i) {
		if (i != 2) {
			physics_addCenterRectFull(TPE_F, TPE_F, TPE_F / 5, TPE_F / 5);
			TPE_bodyRotateByAxis(&physics_lastBody, TPE_vec3(TPE_F / 4, 0, 0));
			physics_lastBody.joints[4].sizeDivided *= 3; // make center point bigger
		} else
			physics_addBall(6 * TPE_F / 5, TPE_F / 5);
	
		physics_lastBody.friction = 4 * TPE_F / 5;
		physics_lastBody.elasticity = TPE_F / 5;
	
		TPE_bodyMoveBy( &physics_lastBody, TPE_vec3(-2 * TPE_F + i * 2 * TPE_F, 0, 0) );
	}*/
}


#endif