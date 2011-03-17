/**
 *  FRAlgorithm.h
 *  Projekt 3DVisual
 */
#ifndef LAYOUT_FRALGORITHM_DEF
#define LAYOUT_FRALGORITHM_DEF 1

//#include <stdio.h>
//#include <iostream>
//#include <sstream>
//#include <string.h>
#include <math.h>
#include <ctime>

#include <osg/Vec3f>
#include <osg/Matrixd>

#include <QMap>
#include <QMutex>
#include <QThread>

namespace Util {
class Config;
}

namespace Model {
class Graph;
class Node;
class Edge;
/**
 *  \class FRAlgorithm
 *  \brief Object represents Fruchterman-Reingold layout algorithm
 *  \author Michal Pavlik
 *  \date 28. 4. 2010
 */
class FRAlgorithm: public QThread {

public:
	FRAlgorithm();
	~FRAlgorithm();

	void setGraph(Graph *newGraph);

	/**
	 *  \fn public  SetParameters(float sizeFactor,float flexibility,int animationSpeed,bool useMaxDistance)
	 *  \brief Sets parameters of layout algorithm
	 *  \param       sizeFactor
	 *  \param       flexibility
	 *  \param       animationSpeed
	 *  \param       useMaxDistance
	 */
	void setParameters(float sizeFactor, float flexibility,
			bool useMaxDistance);

	/**
	 *  \fn public  Randomize
	 *  \brief Sets random position of nodes
	 */
	void randomize();

	/**
	 *  \fn inline public  SetAlphaValue(float val)
	 *  \brief Sets multiplicity of forces
	 *  \param      val  multipliciter of forces
	 */
	void setAlphaValue(float val) {
		ALPHA = val;
	}

	/**
	 *  \fn public  IsRunning
	 *  \brief Returns if layout algorithm is running or not
	 *  \return bool true, if algorithm is running
	 */
	bool isPlaying();

	/**
	 *  \fn public  play
	 *  \brief Play paused layout algorithm
	 */
	void play();

	/**
	 *  \fn public  pause
	 *  \brief Pause layout algorithm
	 */
	void pause();

	/**
	 *  \fn public  wakeUp
	 *  \brief Wakes up frozen layout algorithm
	 */
	void wakeUp();

	/**
	 *  \fn public  stop
	 *  \brief Terminates algorithm
	 */
	void stop();

private:
	/**
	 *  \fn public  Run
	 *  \brief Starts layout algorithm process
	 */
	void run();

	void resetNodes();

	/**
	 *  Graph * graph
	 *  \brief data structure containing nodes, edges and types
	 */
	Graph *graph;

	/**
	 *  float ALPHA
	 *  \brief multipliciter of forces
	 */
	float ALPHA;

	/**
	 *  float MIN_MOVEMENT
	 *  \brief minimal distance between two nodes, when forces are aplified
	 */
	float MIN_MOVEMENT;

	/**
	 *  float MAX_MOVEMENT
	 *  \brief maximal distance between two nodes, when forces are aplified
	 */
	float MAX_MOVEMENT;

	/**
	 *  float MAX_DISTANCE
	 *  \brief maximal distance of nodes, when repulsive force is aplied
	 */
	float MAX_DISTANCE;

	/**
	 *  enum State
	 *  \brief maximal distance of nodes, when repulsive force is aplied
	 */
	enum State {
		RUNNING, PAUSED, NO_GRAPH
	};

	/**
	 *  float flexibility
	 *  \brief flexibility of graph layouting
	 */
	float flexibility;

	/**
	 *  float sizeFactor
	 *  \brief size of graph layout
	 */
	float sizeFactor;

	/**
	 *  double K
	 *  \brief normal length of edge, rest mass of chord
	 */
	float K;

	// margin
	float M;

	/**
	 *  osg::Vec3f center
	 *  \brief center of cetripetal force
	 */
	osg::Vec3f center;

	/**
	 *  FRAlgorithm::State state
	 *  \brief actual status of the algorithm
	 */
	FRAlgorithm::State state;
	/**
	 *  bool useMaxDistance
	 *  \brief constaint using maximal distance of nodes, when repulsive force is aplied
	 */
	bool useMaxDistance;
	/**
	 *  bool notEnd
	 *  \brief algorithm end flag
	 */
	bool notEnd;

	/**
	 *  volatile bool isIterating
	 *  \brief algorithm iterating flag
	 */
	volatile bool isIterating;

	/**
	 *  \fn private  computeCalm
	 *  \brief computes rest mass chord
	 *  \return double normal length of edge
	 */
	double computeCalm();

	/**
	 *  \fn private  getRandomDouble
	 *  \brief Returns random double number
	 *  \return double Random number
	 */
	double getRandomDouble();

	/**
	 *  \fn private  getRandomLocation
	 *  \brief Gets random vector
	 *  \return osg::Vec3f random vector
	 */
	osg::Vec3f getRandomLocation();

	/**
	 *  osg::Vec3f last
	 *  \brief origin position of node
	 */
	osg::Vec3f last;

	/**
	 *  \fn private  iterate
	 *  \brief performs one iteration of the algorithm
	 *  \return bool
	 */
	bool iterate();

	void addForces(Node* u, Node* v, Edge* e);

	osg::Vec3f getVector(Node* u, Node* v);
	osg::Vec3f getProjVector(Node* u, Node* v);
	float getMinProjDistance(Node* u, Node* v, osg::Vec3f pv);

	/**
	 *  \fn private  applyForces(Node* node)
	 *  \brief Applyies forces to node
	 *  \param   node node to which will be aplified forces
	 *  \return bool
	 */
	bool applyForces(Node* node);

	float proj(double distance, double ideal);
	float rep(double distance, double ideal);
	float attr(double distance, double ideal);
	float centr(double distance);

};
}

# endif
