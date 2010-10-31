/**
 *  EdgeGroup.h
 *  Projekt 3DVisual
 */
#ifndef VIEWER_EDGEWRAPPER_DEF
#define VIEWER_EDGEWRAPPER_DEF 1

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/AutoTransform>

#include <QMap>

#include "Model/Edge.h"

namespace Vwr {
/**
 *  \class EdgeGroup
 *  \brief Edge group wrapper
 *  \author Michal Paprcka
 *  \date 29. 4. 2010
 */
class EdgeGroup {
public:

	/**
	 *  \fn public constructor  EdgeGroup(QMap<qlonglong, osg::ref_ptr<Model::Edge> > *edges, float scale)
	 *  \brief Creates edge group
	 *  \param edges     edges to wrap
	 *  \param scale     edges scale
	 */
	EdgeGroup(QMap<qlonglong, osg::ref_ptr<Model::Edge> > *edges, float scale);

	/**
	 *  \fn public destructor  ~EdgeGroup
	 *  \brief destructor
	 */
	~EdgeGroup();

	/**
	 *  \fn public  updateEdgeCoords
	 *  \brief Updates edges coordinates
	 */
	void updateEdgeCoords(osg::Vec3d viewVector);

	/**
	 *  \fn inline public  getGroup
	 *  \brief returns edges group
	 *  \return osg::ref_ptr edges group
	 */
	osg::ref_ptr<osg::Group> getGroup() {
		return edgeGroup;
	}

	static osg::ref_ptr<osg::StateSet> createStateSet(bool oriented); // XXX

private:

	/**
	 *  QMap<qlonglong,osg::ref_ptr<Model::Edge> > * edges
	 *  \brief Wrapped edges
	 */
	QMap<qlonglong, osg::ref_ptr<Model::Edge> > *edges;

	/**
	 *  float scale
	 *  \brief edge scale
	 */
	float scale;

	/**
	 *  osg::ref_ptr edgeGroup
	 *  \brief edges group
	 */
	osg::ref_ptr<osg::Group> edgeGroup;



};
}
#endif
