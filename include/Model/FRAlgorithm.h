/**
 *  FRAlgorithm.h
 *  Projekt 3DVisual
 */
#ifndef LAYOUT_FRALGORITHM_DEF
#define LAYOUT_FRALGORITHM_DEF 1

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <osg/Vec3f>
#include <osg/Matrixd>
#include <QMap>
#include <math.h>
#include <ctime>
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

	void setGraph(Graph *newGraph);

	/**
	 *  \fn public  SetParameters(float sizeFactor,float flexibility,int animationSpeed,bool useMaxDistance)
	 *  \brief Sets parameters of layout algorithm
	 *  \param       sizeFactor
	 *  \param       flexibility
	 *  \param       animationSpeed
	 *  \param       useMaxDistance
	 */
	void setParameters(float sizeFactor, float flexibility, int animationSpeed,
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

protected:
	/**
	 *  \fn public  Run
	 *  \brief Starts layout algorithm process
	 */
	void run();

private:
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
	double K;

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
	 *  osg::Vec3f fv
	 *  \brief force vector
	 */
	osg::Vec3f fv;

	/**
	 *  osg::Vec3f last
	 *  \brief origin position of node
	 */
	osg::Vec3f last;

	/**
	 *  osg::Vec3f up
	 *  \brief vector U
	 */
	osg::Vec3f up;

	/**
	 *  osg::Vec3f vp
	 *  \brief vector V
	 */
	osg::Vec3f vp;

	/**
	 *  osg::Vec3f barycenter
	 *  \brief barycenter position
	 */
	osg::Vec3f barycenter;

	/**
	 *  osg::Vec3f centripetal
	 *  \brief centripetal position
	 */
	osg::Vec3f centripetal;

	/**
	 *  double dist
	 *  \brief distance between two positions
	 */
	double dist;

	/**
	 *  \fn private  iterate
	 *  \brief performs one iteration of the algorithm
	 *  \return bool
	 */
	bool iterate();

	/**
	 *  \fn private  applyForces(Node* node)
	 *  \brief Applyies forces to node
	 *  \param   node node to which will be aplified forces
	 *  \return bool
	 */
	bool applyForces(Node* node);

	/**
	 *  \fn private  addAttractive(Edge* edge, float factor)
	 *  \brief Adds attractive force to node
	 *  \param  edge  which associate two nodes on which effect attractive force
	 *  \param  factor  multiplicer of attaractive force
	 */
	void addAttractive(Edge* edge, float factor);

	/**
	 *  \fn private  addRepulsive(Node* u, Node* v, float factor)
	 *  \brief Adds repulsive force between two nodes into node U
	 *  \param  u  node U
	 *  \param  v  node V
	 *  \param  factor  multiplicer of repulsive force
	 */
	void addRepulsive(Node* u, Node* v, float factor);
	void addRepulsiveProj(Node* u, Node* v, float factor);

	/**
	 *  \fn private  rep(double distance)
	 *  \brief Computes repulsive force
	 *  \param  distance  distance between two vectors
	 *  \return float size of repulsive force
	 */
	float rep(double distance);

	/**
	 *  \fn private  attr(double distance)
	 *  \brief Computes attractive force
	 *  \param     distance  distance between two vectors
	 *  \return float size of attractive force
	 */
	float attr(double distance);

	/**
	 *  \fn private  centr(double distance)
	 *  \brief Computes centripetal force
	 *  \param      distance   distance between two vectors
	 *  \return float size of centripetal force
	 */
	float centr(double distance);

	/**
	 *  \fn private  distance(osg::Vec3f u,osg::Vec3f v)
	 *  \brief Computes distance between vectors U and V.
	 *  \param       u  vector U
	 *  \param       v  vector V
	 *  \return double distance between two vectors
	 */
	double distance(osg::Vec3f u, osg::Vec3f v);
};
}

# endif
