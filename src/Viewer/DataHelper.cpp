#include "Viewer/DataHelper.h"
#include <QLinkedList>

using namespace Vwr;
using namespace Model;

osg::ref_ptr<osg::Geode> DataHelper::getSphereGeode(osg::Vec3 center, float radius)
{
	osg::ref_ptr<osg::Geode> sphereGeode = new osg::Geode;

	osg::ref_ptr<osg::Sphere> unitSphere = new osg::Sphere(center, radius);
	osg::ref_ptr<osg::ShapeDrawable> unitSphereDrawable = new osg::ShapeDrawable(unitSphere);

	sphereGeode->addDrawable(unitSphereDrawable);

	return sphereGeode.get();
}

osg::ref_ptr<osg::Vec3Array> DataHelper::getInitialVectors(int count)
{
	int x, y, z;
	int lowest = 1;
	int highest = 100;
	int range = (highest - lowest) + 1;

	osg::ref_ptr<osg::Vec3Array> positions = new osg::Vec3Array();

        srand((unsigned)time(0));

                for(int index = 0; index < count; index++)
	{
                x = lowest + int(range * rand() / (RAND_MAX + 1.0));
                y = lowest + int(range * rand() / (RAND_MAX + 1.0));
                z = lowest + int(range * rand() / (RAND_MAX + 1.0));

		positions->push_back(osg::Vec3(x, y, z));
	}

	return positions;
}

int DataHelper::getRandomNumber(int lowest, int highest)
{
	int range = (highest - lowest) + 1;

	return lowest + int(range * rand() / (RAND_MAX + 1.0));
}

osg::ref_ptr<osg::Vec3Array> DataHelper::getEdgeVectors(osg::ref_ptr<Node> inNode, osg::ref_ptr<Node> outNode)
{
	osg::ref_ptr<osg::Vec3Array> edgeVectors = new osg::Vec3Array;

	edgeVectors->push_back(osg::Vec3(inNode->getTargetPosition()));
	edgeVectors->push_back(osg::Vec3(inNode->getTargetPosition().x() - 0.5, inNode->getTargetPosition().y(), inNode->getTargetPosition().z()));
	edgeVectors->push_back(osg::Vec3(outNode->getTargetPosition().x() - 0.5, outNode->getTargetPosition().y(), outNode->getTargetPosition().z()));
	edgeVectors->push_back(osg::Vec3(outNode->getTargetPosition()));

	return edgeVectors;
}
/*
void DataHelper::generateTestingGraph(std::vector<Node*> *nodes, std::vector<Edge*> *edges,std::vector<Type*> *types)
{

    generateCylinder(nodes, edges, types, 15, 1);
    generateCylinder(nodes, edges, types, 15, 1);
    generateCylinder(nodes, edges, types, 15, 6);
    generateCylinder(nodes, edges, types, 8, 12);
    generateCylinder(nodes, edges, types, 13, 2);
    generateCylinder(nodes, edges, types, 13, 2);
    generateCylinder(nodes, edges, types, 5, 10);
    generateCylinder(nodes, edges, types, 10, 5);
    generateCylinder(nodes, edges, types, 8, 1);
    generateCylinder(nodes, edges, types, 8, 1);
    generateCylinder(nodes, edges, types, 20, 3);

}
void DataHelper::generatePyramid(std::vector<Node*> *nodes, std::vector<Edge*> *edges,std::vector<Type*> *types)
{
        // budeme pridavat oba typy objektov - hrany aj vrcholy
        std::string tVStr ("vrchol");
        std::string tEStr ("hrana");
        Type* vType = new Type(0,tVStr);
        Type* eType = new Type(1,tEStr);
        types->insert(types->end(),vType);
        types->insert(types->end(),eType);

        // pridame N uzlov
        for(int i = 0; i < 4; i++)
        {
                // ziskame meno uzla
                std::stringstream out; out << i;
                // kazdy uzol ma na zaciatku prazdnu mnozinu hran
                std::vector<Edge*> edges;
                // vytvorime novy uzol a pridame ho medzi ostatne
                Node* node = new Node(i, out.str(), types->at(0), new osg::Vec3f(0,0,0), edges);
                nodes->insert(nodes->end(), node);
        }

        // pridame hranu
        Edge* edge0 = new Edge(0,"a", nodes->at(0), nodes->at(1), types->at(1), true);
        // prepojime ju s uzlami a pridame do struktury
        edge0->linkNodes(edges);

        // to iste pre ostatne hrany
        Edge* edge1 = new Edge(0,"b", nodes->at(1), nodes->at(2), types->at(1), true);
        edge1->linkNodes(edges);
        Edge* edge2 = new Edge(0,"c", nodes->at(2), nodes->at(0), types->at(1), true);
        edge2->linkNodes(edges);

        Edge* edge9 = new Edge(0,"d", nodes->at(3), nodes->at(1), types->at(1), true);
        edge9->linkNodes(edges);
        Edge* edge10 = new Edge(0,"e", nodes->at(3), nodes->at(2), types->at(1), true);
        edge10->linkNodes(edges);
        Edge* edge11 = new Edge(0,"f", nodes->at(3), nodes->at(0), types->at(1), true);
        edge11->linkNodes(edges);

}
*/
void DataHelper::generateCylinder(QMap<qlonglong, osg::ref_ptr<Node> > *nodes, QMap<qlonglong, osg::ref_ptr<Edge> > *edges,QMap<qlonglong, Type*> *types, int pocetUzlovNaPodstave, int pocetUzlovNaVysku)
{
        int startN = nodes->count();
        int startE = edges->count();

        // budeme pridavat oba typy objektov - hrany aj vrcholy
        QString tVStr ("vrchol");
        QString tEStr ("hrana");
        Type* vType = new Type(1,tVStr,0);
        Type* eType = new Type(2,tEStr,0);
        types->insert(1, vType);
        types->insert(2, eType);

        // pridame N uzlov
        for(int i = startN; i < startN + pocetUzlovNaVysku*pocetUzlovNaPodstave; i++)
        {
                // ziskame meno uzla
                std::stringstream out; out << i;
                // kazdy uzol ma na zaciatku prazdnu mnozinu hran - toto je nejaka blbost FIXME!
                //QMap<long, Edge*> edges;
                // vytvorime novy uzol a pridame ho medzi ostatne
                osg::ref_ptr<Node> node = new Node(i, QString::fromStdString(out.str()), types->value(1), NULL, osg::Vec3f(0,0,0));
                //std::cout << nodes->capacity();
                //std::cout << "\n";
                nodes->insert(i, node);
        }

        // ID hrany
        int id = startE;

        //std::cout << startN << "," << startE;
        //std::cout << "\n";
        //std::cout << pocetUzlovNaPodstave << "," << pocetUzlovNaVysku;
        //std::cout << "\n";

        // pre kazdu podstavu
        for(int j = 0; j < pocetUzlovNaVysku; j++)
        {
            // pridame hrany pre podstavu
            for(int i = 0; i < pocetUzlovNaPodstave; i++)
            {   
                // ziskame meno hrany
                std::stringstream out; out << id;

                int to;
                if(i == (pocetUzlovNaPodstave - 1)){
                    to = j*pocetUzlovNaPodstave + startN;
                } else {
                    to = j*pocetUzlovNaPodstave + i+1 + startN;
                }

                //std::cout << id << " : " << j << "," << i << " Edge: " << (j*pocetUzlovNaPodstave + i + startN) << " - " << to;
                //std::cout << "\n";

                // spravime hranu medzi dvoma uzlami podstavy
				osg::ref_ptr<Edge> edge = new Edge(id, QString::fromStdString(out.str()), NULL, nodes->value(j*pocetUzlovNaPodstave + i + startN), nodes->value(to), types->value(2), getRandomNumber(0,1));
                edge->linkNodes(edges);
                id++;

                // ak nejde o spodnu podstavu, tak kazdy vrchol spojime s dalsim ktory je nizsie
                if(j > 0){
                    osg::ref_ptr<Edge> edge0 = new Edge(id, QString::fromStdString(out.str()), NULL, nodes->value(j*pocetUzlovNaPodstave + i + startN), nodes->value((j-1)*pocetUzlovNaPodstave + i + startN), types->value(2), true);
                    edge0->linkNodes(edges);
                    id++;
                }
            }
        }
}
/*
void DataHelper::generateCube2(std::vector<Node*> *nodes, std::vector<Edge*> *edges,std::vector<Type*> *types)
{
        // budeme pridavat oba typy objektov - hrany aj vrcholy
        std::string tVStr ("vrchol");
        std::string tEStr ("hrana");
        Type* vType = new Type(0,tVStr);
        Type* eType = new Type(1,tEStr);
        types->insert(types->end(),vType);
        types->insert(types->end(),eType);

        // pridame N uzlov
        for(int i = 0; i < 8; i++)
        {
                // ziskame meno uzla
                std::stringstream out; out << i;
                // kazdy uzol ma na zaciatku prazdnu mnozinu hran
                std::vector<Edge*> edges;
                // vytvorime novy uzol a pridame ho medzi ostatne
                Node* node = new Node(i, out.str(), types->at(0), new osg::Vec3f(0,0,0), edges);
                nodes->insert(nodes->end(), node);
        }

        // pridame hranu
        Edge* edge0 = new Edge(0,"a", nodes->at(0), nodes->at(1), types->at(1), true);
        // prepojime ju s uzlami a pridame do struktury
        edge0->linkNodes(edges);

        // to iste pre ostatne hrany
        Edge* edge1 = new Edge(0,"b", nodes->at(1), nodes->at(2), types->at(1), true);
        edge1->linkNodes(edges);
        Edge* edge2 = new Edge(0,"c", nodes->at(2), nodes->at(3), types->at(1), true);
        edge2->linkNodes(edges);
        Edge* edge3 = new Edge(0,"d", nodes->at(3), nodes->at(0), types->at(1), true);
        edge3->linkNodes(edges);
        Edge* edge4 = new Edge(0,"e", nodes->at(4), nodes->at(5), types->at(1), true);
        edge4->linkNodes(edges);
        Edge* edge5 = new Edge(0,"f", nodes->at(5), nodes->at(6), types->at(1), true);
        edge5->linkNodes(edges);
        Edge* edge6 = new Edge(0,"g", nodes->at(6), nodes->at(7), types->at(1), true);
        edge6->linkNodes(edges);
        Edge* edge7 = new Edge(0,"h", nodes->at(7), nodes->at(4), types->at(1), true);
        edge7->linkNodes(edges);
        Edge* edge8 = new Edge(0,"i", nodes->at(4), nodes->at(0), types->at(1), true);
        edge8->linkNodes(edges);
        Edge* edge9 = new Edge(0,"j", nodes->at(5), nodes->at(1), types->at(1), true);
        edge9->linkNodes(edges);
        Edge* edge10 = new Edge(0,"k", nodes->at(6), nodes->at(2), types->at(1), true);
        edge10->linkNodes(edges);
        Edge* edge11 = new Edge(0,"l", nodes->at(7), nodes->at(3), types->at(1), true);
        edge11->linkNodes(edges);
}

void DataHelper::generateMultiCube(std::vector<Node*> *nodes, std::vector<Edge*> *edges,std::vector<Type*> *types)
{
        // budeme pridavat oba typy objektov - hrany aj vrcholy
        std::string tVStr ("vrchol");
        std::string tEStr ("hrana");
        Type* vType = new Type(0,tVStr);
        Type* eType = new Type(1,tEStr);
        types->insert(types->end(),vType);
        types->insert(types->end(),eType);

        // pridame N uzlov
        for(int i = 0; i < 3*9; i++)
        {
                // ziskame meno uzla
                std::stringstream out; out << i;
                // kazdy uzol ma na zaciatku prazdnu mnozinu hran
                std::vector<Edge*> edges;
                // vytvorime novy uzol a pridame ho medzi ostatne
                Node* node = new Node(i, out.str(), types->at(0), new osg::Vec3f(0,0,0), edges);
                nodes->insert(nodes->end(), node);
        }

        // Spodna podstava
        Edge* edge0 = new Edge(0,"", nodes->at(0), nodes->at(1), types->at(1), true);
        edge0->linkNodes(edges);
        Edge* edge1 = new Edge(0,"", nodes->at(1), nodes->at(2), types->at(1), true);
        edge1->linkNodes(edges);
        Edge* edge2 = new Edge(0,"", nodes->at(2), nodes->at(3), types->at(1), true);
        edge2->linkNodes(edges);
        Edge* edge3 = new Edge(0,"", nodes->at(3), nodes->at(4), types->at(1), true);
        edge3->linkNodes(edges);
        Edge* edge4 = new Edge(0,"", nodes->at(4), nodes->at(5), types->at(1), true);
        edge4->linkNodes(edges);
        Edge* edge5 = new Edge(0,"", nodes->at(5), nodes->at(6), types->at(1), true);
        edge5->linkNodes(edges);
        Edge* edge6 = new Edge(0,"", nodes->at(6), nodes->at(7), types->at(1), true);
        edge6->linkNodes(edges);
        Edge* edge7 = new Edge(0,"", nodes->at(7), nodes->at(0), types->at(1), true);
        edge7->linkNodes(edges);
        Edge* edge8 = new Edge(0,"", nodes->at(7), nodes->at(8), types->at(1), true);
        edge8->linkNodes(edges);
        Edge* edge9 = new Edge(0,"", nodes->at(5), nodes->at(8), types->at(1), true);
        edge9->linkNodes(edges);
        Edge* edge10 = new Edge(0,"", nodes->at(3), nodes->at(8), types->at(1), true);
        edge10->linkNodes(edges);
        Edge* edge11 = new Edge(0,"", nodes->at(1), nodes->at(8), types->at(1), true);
        edge11->linkNodes(edges);

        // Stredna podstava
        Edge* edge12 = new Edge(0,"", nodes->at(9), nodes->at(10), types->at(1), true);
        edge12->linkNodes(edges);
        Edge* edge13 = new Edge(0,"", nodes->at(10), nodes->at(11), types->at(1), true);
        edge13->linkNodes(edges);
        Edge* edge14 = new Edge(0,"", nodes->at(11), nodes->at(12), types->at(1), true);
        edge14->linkNodes(edges);
        Edge* edge15 = new Edge(0,"", nodes->at(12), nodes->at(13), types->at(1), true);
        edge15->linkNodes(edges);
        Edge* edge16 = new Edge(0,"", nodes->at(13), nodes->at(14), types->at(1), true);
        edge16->linkNodes(edges);
        Edge* edge17 = new Edge(0,"", nodes->at(14), nodes->at(15), types->at(1), true);
        edge17->linkNodes(edges);
        Edge* edge18 = new Edge(0,"", nodes->at(15), nodes->at(16), types->at(1), true);
        edge18->linkNodes(edges);
        Edge* edge19 = new Edge(0,"", nodes->at(16), nodes->at(9), types->at(1), true);
        edge19->linkNodes(edges);
        Edge* edge20 = new Edge(0,"", nodes->at(16), nodes->at(17), types->at(1), true);
        edge20->linkNodes(edges);
        Edge* edge21 = new Edge(0,"", nodes->at(14), nodes->at(17), types->at(1), true);
        edge21->linkNodes(edges);
        Edge* edge22 = new Edge(0,"", nodes->at(12), nodes->at(17), types->at(1), true);
        edge22->linkNodes(edges);
        Edge* edge23 = new Edge(0,"", nodes->at(10), nodes->at(17), types->at(1), true);
        edge23->linkNodes(edges);

        // Vrchna podstava
        Edge* edge24 = new Edge(0,"", nodes->at(18), nodes->at(19), types->at(1), true);
        edge24->linkNodes(edges);
        Edge* edge25 = new Edge(0,"", nodes->at(19), nodes->at(20), types->at(1), true);
        edge25->linkNodes(edges);
        Edge* edge26 = new Edge(0,"", nodes->at(20), nodes->at(21), types->at(1), true);
        edge26->linkNodes(edges);
        Edge* edge27 = new Edge(0,"", nodes->at(21), nodes->at(22), types->at(1), true);
        edge27->linkNodes(edges);
        Edge* edge28 = new Edge(0,"", nodes->at(22), nodes->at(23), types->at(1), true);
        edge28->linkNodes(edges);
        Edge* edge29 = new Edge(0,"", nodes->at(23), nodes->at(24), types->at(1), true);
        edge29->linkNodes(edges);
        Edge* edge30 = new Edge(0,"", nodes->at(24), nodes->at(25), types->at(1), true);
        edge30->linkNodes(edges);
        Edge* edge31 = new Edge(0,"", nodes->at(25), nodes->at(18), types->at(1), true);
        edge31->linkNodes(edges);
        Edge* edge32 = new Edge(0,"", nodes->at(25), nodes->at(26), types->at(1), true);
        edge32->linkNodes(edges);
        Edge* edge33 = new Edge(0,"", nodes->at(23), nodes->at(26), types->at(1), true);
        edge33->linkNodes(edges);
        Edge* edge34 = new Edge(0,"", nodes->at(21), nodes->at(26), types->at(1), true);
        edge34->linkNodes(edges);
        Edge* edge35 = new Edge(0,"", nodes->at(19), nodes->at(26), types->at(1), true);
        edge35->linkNodes(edges);
        
        // Tri vertikalne steny zlava
        // 1
         Edge* edge36 = new Edge(0,"", nodes->at(0), nodes->at(9), types->at(1), true);
        edge36->linkNodes(edges);
        Edge* edge37 = new Edge(0,"", nodes->at(7), nodes->at(16), types->at(1), true);
        edge37->linkNodes(edges);
        Edge* edge38 = new Edge(0,"", nodes->at(6), nodes->at(15), types->at(1), true);
        edge38->linkNodes(edges);
        Edge* edge39 = new Edge(0,"", nodes->at(9), nodes->at(18), types->at(1), true);
        edge39->linkNodes(edges);
        Edge* edge40 = new Edge(0,"", nodes->at(16), nodes->at(25), types->at(1), true);
        edge40->linkNodes(edges);
        Edge* edge41 = new Edge(0,"", nodes->at(15), nodes->at(24), types->at(1), true);
        edge41->linkNodes(edges);
        // 2
         Edge* edge42 = new Edge(0,"", nodes->at(1), nodes->at(10), types->at(1), true);
        edge42->linkNodes(edges);
        Edge* edge43 = new Edge(0,"", nodes->at(8), nodes->at(17), types->at(1), true);
        edge43->linkNodes(edges);
        Edge* edge44 = new Edge(0,"", nodes->at(5), nodes->at(14), types->at(1), true);
        edge44->linkNodes(edges);
        Edge* edge45 = new Edge(0,"", nodes->at(10), nodes->at(19), types->at(1), true);
        edge45->linkNodes(edges);
        Edge* edge46 = new Edge(0,"", nodes->at(17), nodes->at(26), types->at(1), true);
        edge46->linkNodes(edges);
        Edge* edge47 = new Edge(0,"", nodes->at(14), nodes->at(23), types->at(1), true);
        edge47->linkNodes(edges);
        // 3
         Edge* edge48 = new Edge(0,"", nodes->at(2), nodes->at(11), types->at(1), true);
        edge48->linkNodes(edges);
        Edge* edge49 = new Edge(0,"", nodes->at(3), nodes->at(12), types->at(1), true);
        edge49->linkNodes(edges);
        Edge* edge50 = new Edge(0,"", nodes->at(4), nodes->at(13), types->at(1), true);
        edge50->linkNodes(edges);
        Edge* edge51 = new Edge(0,"", nodes->at(11), nodes->at(20), types->at(1), true);
        edge51->linkNodes(edges);
        Edge* edge52 = new Edge(0,"", nodes->at(12), nodes->at(21), types->at(1), true);
        edge52->linkNodes(edges);
        Edge* edge53 = new Edge(0,"", nodes->at(13), nodes->at(22), types->at(1), true);
        edge53->linkNodes(edges);

}

void DataHelper::generateCube(std::vector<Node*> *nodes,std::vector<Edge*> *edges,std::vector<Type*> *types)
{
	long nVertex = 8;
	long nEdges = 12;

	std::stringstream Num;
	std::string str;
	std::string tVStr ("vrchol");
	std::string tEStr ("hrana");
	osg::Vec3f* position = new osg::Vec3f(0,0,0);
	std::vector<Edge*> edges2;

	Type* vType = new Type(0,tVStr);
	Type* eType = new Type(1,tEStr);
	types->insert(types->end(),vType);
	types->insert(types->end(),eType);

	for(int i = 0;i < nVertex;i++)
	{
		Num << "vrchol" << i;
		str = Num.str();		
		Node* vertex = new Node(i,str,types->at(0),position,edges2);
		nodes->insert(nodes->end(),vertex);
		Num.str("");
	}

	Edge* edge0 = new Edge(0,"hrana0",nodes->at(0),nodes->at(1),types->at(1),true);
	Edge* edge1 = new Edge(1,"hrana1",nodes->at(0),nodes->at(2),types->at(1),true);
	Edge* edge2 = new Edge(2,"hrana2",nodes->at(0),nodes->at(4),types->at(1),true);
	Edge* edge3 = new Edge(3,"hrana3",nodes->at(2),nodes->at(3),types->at(1),true);
	Edge* edge4 = new Edge(4,"hrana4",nodes->at(2),nodes->at(6),types->at(1),true);
	Edge* edge5 = new Edge(5,"hrana5",nodes->at(3),nodes->at(7),types->at(1),true);
	Edge* edge6 = new Edge(6,"hrana6",nodes->at(3),nodes->at(1),types->at(1),true);
	Edge* edge7 = new Edge(7,"hrana7",nodes->at(7),nodes->at(6),types->at(1),true);
	Edge* edge8 = new Edge(8,"hrana8",nodes->at(1),nodes->at(5),types->at(1),true);
	Edge* edge9 = new Edge(9,"hrana9",nodes->at(4),nodes->at(5),types->at(1),true);
	Edge* edge10 = new Edge(10,"hrana10",nodes->at(4),nodes->at(6),types->at(1),true);
	Edge* edge11 = new Edge(11,"hrana11",nodes->at(5),nodes->at(7),types->at(1),true);

	edges->insert(edges->end(),edge0);	
	edges->insert(edges->end(),edge1);
	edges->insert(edges->end(),edge2);
	edges->insert(edges->end(),edge3);
	edges->insert(edges->end(),edge4);
	edges->insert(edges->end(),edge5);
	edges->insert(edges->end(),edge6);
	edges->insert(edges->end(),edge7);
	edges->insert(edges->end(),edge8);
	edges->insert(edges->end(),edge9);
	edges->insert(edges->end(),edge10);
	edges->insert(edges->end(),edge11);

	//node0	
	edges2.insert(edges2.end(),edge0);
	edges2.insert(edges2.end(),edge1);	
	edges2.insert(edges2.end(),edge2);

	nodes->at(0)->setEdges(edges2);
	edges2.clear();

	//node1
	edges2.insert(edges2.end(),edge0);
	edges2.insert(edges2.end(),edge6);
	edges2.insert(edges2.end(),edge8);
	nodes->at(1)->setEdges(edges2);
	edges2.clear();

	//node2
	edges2.insert(edges2.end(),edge3);
	edges2.insert(edges2.end(),edge4);	
	edges2.insert(edges2.end(),edge1);
	nodes->at(2)->setEdges(edges2);
	edges2.clear();

	//node3
	edges2.insert(edges2.end(),edge3);
	edges2.insert(edges2.end(),edge5);	
	edges2.insert(edges2.end(),edge6);
	nodes->at(3)->setEdges(edges2);
	edges2.clear();

	//node4
	edges2.insert(edges2.end(),edge2);
	edges2.insert(edges2.end(),edge9);	
	edges2.insert(edges2.end(),edge10);
	nodes->at(4)->setEdges(edges2);
	edges2.clear();

	//node5
	edges2.insert(edges2.end(),edge8);
	edges2.insert(edges2.end(),edge9);	
	edges2.insert(edges2.end(),edge11);
	nodes->at(5)->setEdges(edges2);
	edges2.clear();

	//node6
	edges2.insert(edges2.end(),edge4);
	edges2.insert(edges2.end(),edge7);	
	edges2.insert(edges2.end(),edge10);
	nodes->at(6)->setEdges(edges2);
	edges2.clear();

	//node7
	edges2.insert(edges2.end(),edge5);
	edges2.insert(edges2.end(),edge7);	
	edges2.insert(edges2.end(),edge11);
	nodes->at(7)->setEdges(edges2);
	edges2.clear();
}
*/

osg::ref_ptr<osg::Texture2D> DataHelper::readTextureFromFile(QString path)
{
	osg::ref_ptr<osg::Texture2D> texture = NULL;

	if (path != NULL)
	{
		texture = new osg::Texture2D;
		texture->setImage(osgDB::readImageFile(path.toStdString()));

		texture->setDataVariance(osg::Object::DYNAMIC);
		texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
		texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	}

	if (path == NULL || texture == NULL)
		std::cout << "Unable to read texture from file.";

	return texture;
}

osg::ref_ptr<osg::Texture2D> DataHelper::createTexture(osg::ref_ptr<osg::Image> image)
{
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;

	texture->setImage(image);

	texture->setDataVariance(osg::Object::DYNAMIC);
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

	return texture;
}

osg::Vec3f DataHelper::getMassCenter(osg::ref_ptr<osg::Vec3Array> coordinates)
{
	float x,y,z;
	x = y = z = 0;
	
	int num = coordinates->size();

	for (int i = 0; i < num; i++)
	{
		x += coordinates->at(i).x();
		y += coordinates->at(i).y();
		z += coordinates->at(i).z();
	}

	return osg::Vec3f(x/num, y/num, z/num);
}
