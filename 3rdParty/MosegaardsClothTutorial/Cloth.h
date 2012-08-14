/**
 * @file Cloth.h
 * @author Can Erdogan
 * @date Aug 14, 2012
 * @brief
 */

#pragma once

#include "Constraint.h"

#include <GL/gl.h>
#include <GL/glut.h>

class Cloth {
private:

	// total number of particles is num_particles_width*num_particles_height
	std::size_t num_particles_width;  ///< number of particles in "width" direction
	std::size_t num_particles_height;  ///< number of particles in "height" direction

	std::vector <Particle> particles;  ///< all particles that are part of this cloth
	std::vector <Constraint> constraints;  ///< alle constraints between particles as part of this cloth

	Particle* getParticle(int x, int y) {
		return &particles[y * num_particles_width + x];
	}
	void makeConstraint(Particle *p1, Particle *p2) {
		constraints.push_back(Constraint(p1, p2));
	}

	/** A private method used by drawShaded() and addWindForcesForTriangle() to retrieve the
	 normal vector of the triangle defined by the position of the particles p1, p2, and p3.
	 The magnitude of the normal vector is equal to the area of the parallelogram defined by p1, p2 and p3
	 */
	Vector3d calcTriangleNormal(Particle *p1, Particle *p2, Particle *p3) {
		Vector3d pos1 = p1->getPos();
		Vector3d pos2 = p2->getPos();
		Vector3d pos3 = p3->getPos();

		Vector3d v1 = pos2 - pos1;
		Vector3d v2 = pos3 - pos1;

		return v1.cross(v2);
	}

	/** A private method used by windForce() to calcualte the wind force for a single triangle
	 defined by p1,p2,p3*/
	void addWindForcesForTriangle(Particle *p1, Particle *p2, Particle *p3, const Vector3d direction) {
		Vector3d normal = calcTriangleNormal(p1, p2, p3);
		Vector3d d = normal.normalized();
		Vector3d force = normal * (d.dot(direction));
		p1->addForce(force);
		p2->addForce(force);
		p3->addForce(force);
	}

	/** A private method used by drawShaded(), that draws a single triangle p1,p2,p3 with a color*/
	void drawTriangle(Particle *p1, Particle *p2, Particle *p3, const Vector3d color) {
		glColor3fv((GLfloat*) &color);

		Vector3d* n1 = new Vector3d(p1->getNormal().normalized());
		glNormal3fv((GLfloat *) n1);
		glVertex3fv((GLfloat *) &(p1->getPos()));

		Vector3d* n2 = new Vector3d(p1->getNormal().normalized());
		glNormal3fv((GLfloat *) n2);
		glVertex3fv((GLfloat *) &(p2->getPos()));

		Vector3d* n3 = new Vector3d(p1->getNormal().normalized());
		glNormal3fv((GLfloat *) n3);
		glVertex3fv((GLfloat *) &(p3->getPos()));
	}

public:

	/** This is a important constructor for the entire system of particles and constraints*/
	Cloth(float width, float height, int num_particles_width, int num_particles_height) :
			num_particles_width(num_particles_width), num_particles_height(num_particles_height) {

		// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
		for(int x = 0; x < num_particles_width; x++) {
			for(int y = 0; y < num_particles_height; y++) {
				Vector3d pos = Vector3d(width * (x / (float) num_particles_width), -height * (y / (float) num_particles_height),
						0);
				particles[y * num_particles_width + x] = Particle(pos);  // insert particle in column x at y'th row
			}
		}

		// Connecting immediate neighbor particles with constraints (distance 1 and sqrt(2) in the grid)
		for(int x = 0; x < num_particles_width; x++) {
			for(int y = 0; y < num_particles_height; y++) {
				if(x < num_particles_width - 1) makeConstraint(getParticle(x, y), getParticle(x + 1, y));
				if(y < num_particles_height - 1) makeConstraint(getParticle(x, y), getParticle(x, y + 1));
				if(x < num_particles_width - 1 && y < num_particles_height - 1) makeConstraint(getParticle(x, y),
						getParticle(x + 1, y + 1));
				if(x < num_particles_width - 1 && y < num_particles_height - 1) makeConstraint(getParticle(x + 1, y),
						getParticle(x, y + 1));
			}
		}

		// Connecting secondary neighbors with constraints (distance 2 and sqrt(4) in the grid)
		for(int x = 0; x < num_particles_width; x++) {
			for(int y = 0; y < num_particles_height; y++) {
				if(x < num_particles_width - 2) makeConstraint(getParticle(x, y), getParticle(x + 2, y));
				if(y < num_particles_height - 2) makeConstraint(getParticle(x, y), getParticle(x, y + 2));
				if(x < num_particles_width - 2 && y < num_particles_height - 2) makeConstraint(getParticle(x, y),
						getParticle(x + 2, y + 2));
				if(x < num_particles_width - 2 && y < num_particles_height - 2) makeConstraint(getParticle(x + 2, y),
						getParticle(x, y + 2));
			}
		}

		// making the upper left most three and right most three particles unmovable
		for(int i = 0; i < 3; i++) {
			getParticle(0 + i, 0)->offsetPos(Vector3d(0.5, 0.0, 0.0));  // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			getParticle(0 + i, 0)->makeUnmovable();

			getParticle(0 + i, 0)->offsetPos(Vector3d(-0.5, 0.0, 0.0));  // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			getParticle(num_particles_width - 1 - i, 0)->makeUnmovable();
		}
	}

	/** drawing the cloth as a smooth shaded (and colored according to column) OpenGL triangular mesh
	 Called from the display() method
	 The cloth is seen as consisting of triangles for four particles in the grid as follows:

	 (x,y)   *--* (x+1,y)
	 | /|
	 |/ |
	 (x,y+1) *--* (x+1,y+1)

	 */
	void drawShaded() {
		// reset normals (which where written to last frame)
		std::vector <Particle>::iterator particle;
		for(particle = particles.begin(); particle != particles.end(); particle++) {
			(*particle).resetNormal();
		}

		//create smooth per particle normals by adding up all the (hard) triangle normals that each particle is part of
		for(int x = 0; x < num_particles_width - 1; x++) {
			for(int y = 0; y < num_particles_height - 1; y++) {
				Vector3d normal = calcTriangleNormal(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1));
				getParticle(x + 1, y)->addToNormal(normal);
				getParticle(x, y)->addToNormal(normal);
				getParticle(x, y + 1)->addToNormal(normal);

				normal = calcTriangleNormal(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1));
				getParticle(x + 1, y + 1)->addToNormal(normal);
				getParticle(x + 1, y)->addToNormal(normal);
				getParticle(x, y + 1)->addToNormal(normal);
			}
		}

		glBegin (GL_TRIANGLES);
		for(int x = 0; x < num_particles_width - 1; x++) {
			for(int y = 0; y < num_particles_height - 1; y++) {
				Vector3d color(0, 0, 0);
				if(x % 2)  // red and white color is interleaved according to which column number
				color = Vector3d(0.6f, 0.2f, 0.2f);
				else color = Vector3d(1.0f, 1.0f, 1.0f);

				drawTriangle(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1), color);
				drawTriangle(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1), color);
			}
		}
		glEnd();
	}

	/** this is an important methods where the time is progressed one time step for the entire cloth.
	 This includes calling satisfyConstraint() for every constraint, and calling timeStep() for all particles
	 */
	void timeStep() {
		std::vector <Constraint>::iterator constraint;
		for(int i = 0; i < CONSTRAINT_ITERATIONS; i++)  // iterate over all constraints several times
				{
			for(constraint = constraints.begin(); constraint != constraints.end(); constraint++) {
				(*constraint).satisfyConstraint();  // satisfy constraint.
			}
		}

		std::vector <Particle>::iterator particle;
		for(particle = particles.begin(); particle != particles.end(); particle++) {
			(*particle).timeStep();  // calculate the position of each particle at the next time step.
		}
	}

	/** used to add gravity (or any other arbitrary vector) to all particles*/
	void addForce(const Vector3d direction) {
		std::vector <Particle>::iterator particle;
		for(particle = particles.begin(); particle != particles.end(); particle++) {
			(*particle).addForce(direction);  // add the forces to each particle
		}

	}

	/** used to add wind forces to all particles, is added for each triangle since the final force is proportional to the triangle area as seen from the wind direction*/
	void windForce(const Vector3d direction) {
		for(int x = 0; x < num_particles_width - 1; x++) {
			for(int y = 0; y < num_particles_height - 1; y++) {
				addWindForcesForTriangle(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1), direction);
				addWindForcesForTriangle(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1), direction);
			}
		}
	}

	/** used to detect and resolve the collision of the cloth with the ball.
	 This is based on a very simples scheme where the position of each particle is simply compared to the sphere and corrected.
	 This also means that the sphere can "slip through" if the ball is small enough compared to the distance in the grid bewteen particles
	 */
	void ballCollision(const Vector3d center, const float radius) {
		std::vector <Particle>::iterator particle;
		for(particle = particles.begin(); particle != particles.end(); particle++) {
			Vector3d v = (*particle).getPos() - center;
			float l = v.norm();
			if(v.norm() < radius)  // if the particle is inside the ball
					{
				(*particle).offsetPos(v.normalized() * (radius - l));  // project the particle to the surface of the ball
			}
		}
	}

	void doFrame() {

	}
};

