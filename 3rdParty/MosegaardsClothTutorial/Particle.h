/**
 * @file Particle.h
 * @author Can Erdogan
 * @date Aug 14, 2012
 * @brief
 */

#pragma once

#include "Vec3.h"

/* Some physics constants */
#define DAMPING 0.01 // how much to damp the cloth simulation each frame
#define TIME_STEPSIZE2 0.5*0.5 // how large time step each particle takes each frame
#define CONSTRAINT_ITERATIONS 15 // how many iterations of constraint satisfaction each frame (more is rigid, less is soft)

/* The particle class represents a particle of mass that can move around in 3D space*/
class Particle
{
private:
	bool movable; // can the particle move or not ? used to pin parts of the cloth

	float mass; // the mass of the particle (is always 1 in this example)
	Vec3 pos; // the current position of the particle in 3D space
	Vec3 old_pos; // the position of the particle in the previous time step, used as part of the verlet numerical integration scheme
	Vec3 acceleration; // a vector representing the current acceleration of the particle
	Vec3 accumulated_normal; // an accumulated normal (i.e. non normalized), used for OpenGL soft shading

public:
	Particle(Vec3 pos) : pos(pos), old_pos(pos),acceleration(Vec3(0,0,0)), mass(1), movable(true), accumulated_normal(Vec3(0,0,0)){}
	Particle(){}

	void addForce(Vec3 f)
	{
		acceleration += f/mass;
	}

	/* This is one of the important methods, where the time is progressed a single step size (TIME_STEPSIZE)
	   The method is called by Cloth.time_step()
	   Given the equation "force = mass * acceleration" the next position is found through verlet integration*/
	void timeStep()
	{
		if(movable)
		{
			Vec3 temp = pos;
			pos = pos + (pos-old_pos)*(1.0-DAMPING) + acceleration*TIME_STEPSIZE2;
			old_pos = temp;
			acceleration = Vec3(0,0,0); // acceleration is reset since it HAS been translated into a change in position (and implicitely into velocity)
		}
	}

	Vec3& getPos() {return pos;}

	void resetAcceleration() {acceleration = Vec3(0,0,0);}

	void offsetPos(const Vec3 v) { if(movable) pos += v;}

	void makeUnmovable() {movable = false;}

	void addToNormal(Vec3 normal)
	{
		accumulated_normal += normal.normalized();
	}

	Vec3& getNormal() { return accumulated_normal;} // notice, the normal is not unit length

	void resetNormal() {accumulated_normal = Vec3(0,0,0);}

};


