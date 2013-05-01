#include "AIPathManager.h"
#include "core/fileStream.h"
#include "dgl/dgl.h"

#define NULL 0

IMPLEMENT_CONOBJECT(AIPathNodeGraph);

AIPathManager* AIPathManager::_instance = NULL;

/**************
 * AIPathNode *
 **************/

AIPathNode::AIPathNode(Point3F position)
{
	_closed = false;
	_open = false;
	_dangerModifier = 0.0f;
	_fitness = 0.0f;
	_heuristicCostToGoal = 0.0f;
	_lowestCostFromStart = 0.0f;
	_moveModifier = 0.0f;
	_parent = NULL;
	_position = position;
}

AIPathNode::AIPathNode(F32 x, F32 y, F32 z)
{
	_closed = false;
	_open = false;
	_dangerModifier = 0.0f;
	_fitness = 0.0f;
	_heuristicCostToGoal = 0.0f;
	_lowestCostFromStart = 0.0f;
	_moveModifier = 0.0f;
	_parent = NULL;
	_position.x = x;
	_position.y = y;
	_position.z = z;
}

AIPathNode& AIPathNode::operator = (const AIPathNode other)
{
	_closed = other._closed;
	_open = other._open;
	_dangerModifier = other._dangerModifier;
	_fitness = other._fitness;
	_heuristicCostToGoal = other._heuristicCostToGoal;
	_lowestCostFromStart = other._lowestCostFromStart;
	_moveModifier = other._moveModifier;
	_parent = other._parent;
	_position = other._position;
	return *this;
}

//bool AIPathNode::operator == (const AIPathNode other)
//{
//	return this->_fitness == other._fitness;
//}
//
//bool AIPathNode::operator < (const AIPathNode other)
//{
//	return this->_fitness < other._fitness;
//}
//
//bool AIPathNode::operator > (const AIPathNode other)
//{
//	return this->_fitness > other._fitness;
//}
//
//bool operator > (const AIPathNode one, const AIPathNode other)
//{
//	return one.getFitness() > other.getFitness();
//}
//
//bool operator < (const AIPathNode one, const AIPathNode other)
//{
//	return one.getFitness() < other.getFitness();
//}

void AIPathNode::connect(AIPathNode* pNode, bool connectBothWays)
{
	if(pNode != NULL)
	{
		bool alreadyConnected = false;
		for(unsigned int i=0; i < _connections.size(); ++i) {
			if(_connections[i] == pNode) {
				alreadyConnected = true;
				break;
			}
		}
		if(!alreadyConnected) {
			this->_connections.push_back(pNode);
		}

		if(connectBothWays)
		{
			pNode->connect(this, false);
		}
	}
}

void AIPathNode::disconnect(AIPathNode* pNode, bool disconnectBothWays)
{
	if(pNode != NULL)
	{
		for(unsigned int i=0; i < _connections.size(); ++i)
		{
			if(_connections[i] == pNode) {
				_connections.erase( _connections.begin() + i );
				break;
			}
		}
		if(disconnectBothWays)
		{
			pNode->disconnect(this, false);
		}
	}
}

void AIPathNode::getNeighbours(Vector<AIPathNode*> &replyList)
{
	for(unsigned int i=0 ; i<_connections.size() ; ++i )
	{
		replyList.push_back(_connections[i]);
	}
}

/*********************
 * AIPathNodeCompare *
 *********************/

class AIPathNodeCompare
{
public:
	bool operator()(AIPathNode* first, AIPathNode* second)
	{
		return first->getFitness() > second->getFitness();
	}
};

static S32 QSORT_CALLBACK pathNodeFitnessCompare( const void* a, const void* b )
{
	AIPathNode *ea = (AIPathNode *) (a);
	AIPathNode *eb = (AIPathNode *) (b);
	F32 aCol = ea->getFitness();
	F32 bCol = eb->getFitness();
	F32 diff = aCol - bCol;
	S32 reply = diff < 0 ? -1 : (diff > 0 ? 1 : 0);
	return reply; // ( sIncreasing ? reply : -reply );
}

/*****************
 * AIPathManager *
 *****************/

AIPathManager* AIPathManager::getInstance()
{
	if(_instance == NULL)
	{
		_instance = new AIPathManager();
	}
	return _instance;
}

/****************************************************************************************
 * This is the implementation of the famous A* algorithm. If you ( like many others )
 * want to know what is going on down in this snakes nest of a function, take a look at
 * this tutorial...
 *
 * "A* Pathfinding for Beginners" : http://www.policyalmanac.org/games/aStarTutorial.htm
 *
 * ...or search the web for "astar", "a star", "game pathfinding" or something like that.
 ****************************************************************************************/
bool AIPathManager::aStar(AIPathNode* startNode, AIPathNode* goalNode, Vector<AIPathNode*> &replyList)
{
	// 1) Add the starting square to the open list.
	Vector<AIPathNode*> open;

	//std::list < AIPathNode* > closed;
	startNode->setHeuristicCostToGoal(estimateCostToGoal(startNode, goalNode));
	startNode->setFitness(startNode->getLowestCostFromStart() + startNode->getHeuristicCostToGoal());
	startNode->setOpen(true);
	open.push_back(startNode);

	// 2) Repeat the following:
	while (!open.empty())
	{
		// A structure like the STL priority_queue would be nice here...
		// ...but in its absence we have to continually sort the open list.
		dQsort((void *)&(open[0]), open.size(), sizeof(AIPathNode*), pathNodeFitnessCompare);

		// a) Look for the lowest F cost node on the open list.
		//    We refer to this as the current node.
		AIPathNode* current = open[0];
		open.pop_front();

		// b) Switch it to the closed list.
		current->setOpen(false);
		current->setClosed(true);
		//closed.push_back(current);

		// c) For each of the nodes adjacent to this current node:
		Vector<AIPathNode*> neighbours;
		current->getNeighbours(neighbours);

		for(unsigned int i = 0; i < neighbours.size(); i++)
		{
			AIPathNode* aNeighbour = neighbours[i];
			// If it is not walkable or if it is on the closed list, ignore it.
			if(aNeighbour->isClosed() || aNeighbour->getMoveModifier() >= 1.0 )
			{
				continue;
			}
			// Otherwise do the following:
			// If it isn�t on the open list, add it to the open list.
			if(!aNeighbour->isOpen()) {
				// Make the current square the parent of this node.
				aNeighbour->setParent(current);
				// Record the F, G, and H costs of the node
				aNeighbour->setLowestCostFromStart(aNeighbour->getParent()->getLowestCostFromStart() + estimateCostToGoal(aNeighbour, aNeighbour->getParent()));
				aNeighbour->setHeuristicCostToGoal(estimateCostToGoal(aNeighbour, goalNode));
				aNeighbour->setFitness(aNeighbour->getLowestCostFromStart() + aNeighbour->getHeuristicCostToGoal() + aNeighbour->getDangerModifier()*50 + aNeighbour->getMoveModifier());

				aNeighbour->setOpen(true);
				open.push_back(aNeighbour);
			} else {
				// If it is on the open list already, check to see if this path
				// to that node is better, using G cost as the measure.
				// A lower G cost means that this is a better path. If so, change
				// the parent of the node to the current square, and recalculate
				// the G and F scores of the node.
				if(aNeighbour->getParent() != current) {
					F32 costFromThisNode = current->getLowestCostFromStart() + estimateCostToGoal(aNeighbour, current) + aNeighbour->getDangerModifier()*50 + aNeighbour->getMoveModifier();
					if(aNeighbour->getLowestCostFromStart() > costFromThisNode ) {
						aNeighbour->setLowestCostFromStart( costFromThisNode );
//		aNeighbour->setFitness( aNeighbour->getLowestCostFromStart() + aNeighbour->getHeuristicCostToGoal() + aNeighbour->getDangerModifier()*50 + aNeighbour->getMoveModifier());						
						aNeighbour->setParent( current );
					}
				}
				aNeighbour->setClosed(false);
			}
			if(aNeighbour == goalNode) {
				// Found it, trace back the parent nodes to construct the path
				AIPathNode* currentTraceNode = goalNode;
				
				Con::errorf("Path found...");
				
				while(currentTraceNode) {
					replyList.push_front(currentTraceNode);
					Con::errorf("(%f %f %f)", currentTraceNode->getPosition().x, currentTraceNode->getPosition().y, currentTraceNode->getPosition().z);
					currentTraceNode = currentTraceNode->getParent();
				}

				Con::errorf("...done.");

				return true;
			}
		}
	}
	return false;
}

F32 AIPathManager::estimateCostToGoal(AIPathNode* from, AIPathNode* goal)
{
	return (goal->getPosition() - from->getPosition()).len();
}

void AIPathManager::getSuccessors(AIPathNode* node,  Vector<AIPathNode*> &replyList)
{
	AIPathNode* current = node;
	while(current->getParent())
	{
		replyList.push_back(node);
		current = node->getParent();
	}
}

/*******************
 * AIPathNodeGraph *
 *******************/

AIPathNodeGraph::AIPathNodeGraph()
{
	_dataIsPreCompiled = false;
	_compiledNodeLinks = NULL;
}

S32 AIPathNodeGraph::addNode(AIPathNode* node)
{
	_nodes.push_back(node);
	// Con::printf("C++ : Added node %d", (_nodes.size() - 1) );
	return (_nodes.size() - 1);
}

AIPathNode* AIPathNodeGraph::getNextNodeOnPath(S32 fromNodeIndex, S32 toNodeIndex)
{
	if(_dataIsPreCompiled) {
		return getNode(_compiledNodeLinks[fromNodeIndex][toNodeIndex]);
	} else {
		return NULL;
	}
}

S32 AIPathNodeGraph::getNextNodeIndexOnPath(S32 fromNodeIndex, S32 toNodeIndex)
{
	if(_dataIsPreCompiled) {
		if(fromNodeIndex < 0 || toNodeIndex < 0) {
			return -1;
		} else {
			return _compiledNodeLinks[fromNodeIndex][toNodeIndex];
		}
	} else {
		return -1;
	}
}

void AIPathNodeGraph::renderPaths() {

	AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");

	if (isHidden()) {
		return;
	}

	for (int i = 0; i < _nodes.size(); ++i) {
		// Get all the nodes adjacent to this node
		Vector<AIPathNode*> neighbours;
		_nodes[i]->getNeighbours(neighbours);

		glColor4ub(255, 0, 0, 255);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_TEXTURE_2D);

		for (int j = 0; j < neighbours.size(); ++j) {
			Point3F start = Point3F(_nodes[i]->getPosition().x, _nodes[i]->getPosition().y, _nodes[i]->getPosition().z + 2.0);
			Point3F end = Point3F(neighbours[j]->getPosition().x, neighbours[j]->getPosition().y, neighbours[j]->getPosition().z + 2.0);
			glBegin(GL_LINES);
			glVertex3fv(start);
			glVertex3fv(end);
			glEnd();
		}

		dglSetCanonicalState();
		AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
	}
}

/*****************************************************
 * Returns exact node matches. Should perhaps do a
 * visibility check?
 *****************************************************/
AIPathNode* AIPathNodeGraph::getClosestAccessibleNode(Point3F position)
{
	// TODO: Not checking accessibility yet...
	return getClosestNode(position);
}

/*****************************************************
 * Returns the closest node without giving a rats ass
 * if you can get to it or not...
 *****************************************************/
AIPathNode* AIPathNodeGraph::getClosestNode(Point3F position)
{
	S32 closestIndex = -1;
	F32 closestDistance = -1;
	for(unsigned int i = 0; i < _nodes.size(); ++i) {
		F32 distance = (position - _nodes[i]->getPosition()).len();
		if( distance < closestDistance || closestDistance == -1) {
			closestDistance = distance;
			closestIndex = i;
		}
	}
	return closestIndex == -1 ? NULL : _nodes[closestIndex];
}

S32 AIPathNodeGraph::getClosestNodeIndex(Point3F position)
{
	S32 closestIndex = -1;
	F32 closestDistance = -1;
	// Con::printf("C++ : Searching for index among %d nodes...", _nodes.size());
	for(unsigned int i = 0; i < _nodes.size(); ++i) {
		F32 distance = ( position - _nodes[i]->getPosition() ).len();
		if( distance < closestDistance || closestDistance == -1) {
			closestDistance = distance;
			closestIndex = i;
			// Con::printf("C++ : New closest: distance=%f index=%d", closestDistance, i);
		}// else {
		//	Con::printf("C++ : NOT closest: distance=%f index=%d", closestDistance, i);
		// }
	}
	if(closestIndex >= _nodes.size()) {
		Con::errorf("Out of bounds index %d generated for a list of only %d entries. Impossible! Hmm... apparently not I guess...", closestIndex, _nodes.size());
	}
	return closestIndex;
}

S32 AIPathNodeGraph::calculateNodeIndex(AIPathNode* node) {
	for(S32 i = 0; i < _nodes.size(); ++i) {
		if(_nodes[i] == node) {
			return i;
		}
	}
	return -1;
}

bool AIPathNodeGraph::isCompiled()
{
	return _dataIsPreCompiled;
}

void AIPathNodeGraph::clearNodeNetTempValues() {
	AIPathNode* tempNode;
	for(unsigned int i = 0; i < _nodes.size(); ++i) {
		tempNode = _nodes[i];
		tempNode->setOpen(false);
		tempNode->setClosed(false);
		tempNode->setFitness(0);
		tempNode->setHeuristicCostToGoal(0);
		tempNode->setLowestCostFromStart(0);
		tempNode->setParent(NULL);
	}
}

void AIPathNodeGraph::preCompiledLinkTwoWay(S32 start, S32 end, S32 startNext, S32 endPrev) {
	preCompiledLinkOneWay(start, end, startNext);
	preCompiledLinkOneWay(end, start, endPrev);
}

void AIPathNodeGraph::preCompiledLinkOneWay(S32 start, S32 end, S32 startNext) {
	_compiledNodeLinks[start][end] = startNext;
}

bool AIPathNodeGraph::compile()
{
	AIPathManager* pathManager = AIPathManager::getInstance();
	Vector<AIPathNode*> tempPath;

	preCompiledInitDataStorage(_nodes.size());

	for(unsigned int i = 0; i < _nodes.size(); ++i) {
		for(unsigned int j = 0; j < _nodes.size(); ++j) {
			if(i!=j) {
				if( _compiledNodeLinks[i][j] == -1 ) { // If it has another value, it should be correct already
					tempPath.clear();
					clearNodeNetTempValues();
					if(pathManager->aStar(_nodes[i], _nodes[j], tempPath) && tempPath.size() > 1) { // else it is not really a path
						AIPathNode* nextFromStart = tempPath[1];
						AIPathNode* prevFromEnd   = tempPath[tempPath.size()-2];
						S32 nextFromStartIndex = calculateNodeIndex(nextFromStart);
						S32 prevFromEndIndex   = calculateNodeIndex(prevFromEnd);
						if(nextFromStartIndex == -1 || prevFromEndIndex == -1) {
							Con::errorf("The A* algorithm seems to have returned nodes that are not in the graph. Next index from start of path is %d, and previous from end is %d", nextFromStartIndex, prevFromEndIndex);
						}
						_compiledNodeLinks[i][j] = nextFromStartIndex;
						_compiledNodeLinks[j][i] = prevFromEndIndex;
					}
				}
			}
		}
	}
	_dataIsPreCompiled = true;
	return _dataIsPreCompiled;
}

void AIPathNodeGraph::preCompiledInitDataStorage(S32 nodes) {
	
	if(_compiledNodeLinks) {
		for(unsigned int i = 0; i < _nodes.size(); ++i) {
			delete[] _compiledNodeLinks[i];
		}
		delete[] _compiledNodeLinks;
	}

	_compiledNodeLinks = new int*[nodes];
	for(unsigned int i = 0; i < nodes; ++i) {
		_compiledNodeLinks[i] = new int[nodes];
		dMemset(_compiledNodeLinks[i], -1, sizeof(S32)*nodes); // All initial values set to -1
	}
}

bool AIPathNodeGraph::save(const char* fileName)
{
	if(_dataIsPreCompiled) {
		// Con::printf("C++ : saveToFile( %s )", fileName);

		char fname[1024];
		if( fileName ) {
			if( Con::expandScriptFilename( fname, sizeof( fname ), fileName ) ) {
				fileName = fname;
			}
		}

		FileStream fs;
		if( !fs.open( fileName, FileStream::Write ) ) {
			Con::errorf( "AIPathNodeGraph: Could not open '%s' for writing.", fileName );
			return false;
		}

		// 1) Write the number of nodes
		fs.write( _nodes.size() );
		
		// 2) Write the actual nodes ( just the 3D point of the node actually )
		char buffer[256];
		Point3F tempPoint;
		F32 tempMoveMod;
		for(int i = 0; i < _nodes.size(); ++i) {
			tempPoint = _nodes[i]->getPosition();
			tempMoveMod = _nodes[i]->getMoveModifier();
			dSprintf( buffer, 256, "%f %f %f %f", tempPoint.x, tempPoint.y, tempPoint.z, tempMoveMod);

			// Con::printf("C++ : fs.writeString( %s )", buffer);
			fs.writeString( buffer, dStrlen( buffer ) );
		}

		// 3) For each node, the closest node to go to when travelling
		//    to the target node from the source node.
		for(int i = 0; i < _nodes.size(); ++i) {
			for(int j = 0; j < _nodes.size(); ++j) {
				fs.write(_compiledNodeLinks[i][j]);
			}
		}

		fs.close();

		return true;
	} else {
		Con::errorf("Atempted to save an AIPathNodeGraph to file %s without compiling it first. Try 'compile()' before saving.", fileName);
		return false;
	}
}

bool AIPathNodeGraph::load(const char* fileName)
{
	char fname[1024];

	if( fileName )
		if( Con::expandScriptFilename( fname, sizeof( fname ), fileName ) )
			fileName = fname;

	FileStream fs;
	if( !fs.open( fileName, FileStream::Read ) )
	{
		Con::errorf( "AIPathNodeGraph: Could not open '%s' for reading.", fileName );
		return false;
	}

	// 0) Read the number of nodes
	S32 entries = 0;
	fs.read( &entries );

	// 1) Reset all current data stored
	preCompiledInitDataStorage(entries);

	// 2) Read the actual nodes ( just the 3D point of the node and the move mod actually )
	char buffer[256];
	Point3F tempPoint;
	F32 tempMoveMod;
	AIPathNode* tempNode;
	for(int i = 0; i < entries; ++i) {
		fs.readString( buffer );
		dSscanf(buffer, "%f %f %f %f", &(tempPoint.x), &(tempPoint.y), &(tempPoint.z), &tempMoveMod);
		tempNode = new AIPathNode(tempPoint);
		tempNode->setMoveModifier(tempMoveMod);
		addNode(tempNode);
	}

	// 3) For each node, read the closest node to go to when travelling
	//    to the target node from the source node.
	S32 index = 0;
	// mNodeLinks.setSize(entries);
	for(int i = 0; i < entries; ++i) {
		for(int j = 0; j < entries; ++j) {
			fs.read(&index);
			preCompiledLinkOneWay(i, j, index);
		}
	}

	fs.close();
	_dataIsPreCompiled = true;
	return true;
}
void AIPathNodeGraph::clear() {
	
	for(unsigned int i = 0; i < _nodes.size(); ++i) {
		delete _nodes[i];
	}
	_nodes.clear();

	preCompiledInitDataStorage(0);
}
/***********************
 * AIPathNodeGrid30x30 *
 ***********************/
AIPathNodeGrid30x30::AIPathNodeGrid30x30()
{
	for(unsigned int x = 0;x<30;++x)
	{
		for(unsigned int y = 0;y<30;++y)
		{
			_nodes[x][y] = new AIPathNode((F32)x,(F32)y, 0.0f);
		}
	}
	for(unsigned int x = 0;x<30;++x)
	{
		for(unsigned int y = 0;y<30;++y)
		{
			// Straight
			if(x<(30-1))
				_nodes[x][y]->connect(_nodes[x+1][y  ], false);
			if(x>0)
				_nodes[x][y]->connect(_nodes[x-1][y  ], false);
			if(y<(30-1))
				_nodes[x][y]->connect(_nodes[x  ][y+1], false);
			if(y>0)
				_nodes[x][y]->connect(_nodes[x  ][y-1], false);
			// Diagonal
			if(x<(30-1) && y<(30-1))
				_nodes[x][y]->connect(_nodes[x+1][y+1], false);
			if(x>0 && y>0)
				_nodes[x][y]->connect(_nodes[x-1][y-1], false);
			if(x<(30-1) && y>0)
				_nodes[x][y]->connect(_nodes[x+1][y-1], false);
			if(x>0 && y<(30-1))
				_nodes[x][y]->connect(_nodes[x-1][y+1], false);
		}
	}
}

AIPathNode* AIPathNodeGrid30x30::setNode(int x, int y, AIPathNode* node)
{
	AIPathNode* reply = _nodes[x][y];
	_nodes[x][y] = node;
	return reply;
}

AIPathNode* AIPathNodeGrid30x30::getNode(int x, int y)
{
	return _nodes[x][y];
}

/*****************************
 * Script acsessible methods *
 *****************************/
ConsoleMethod( AIPathNodeGraph, getClosestNode, const char*, 3, 3, "AIPathNodeGraph.getClosestNode(Point3F pos) - Get the node closest to the supplied point.")
{
	argc;
	char *returnBuffer = Con::getReturnBuffer( 256 );

	if(dStrlen(argv[2]) == 0) {
		Con::errorf("Zero length string sent as position to function getClosestNode(Point3F pos), expected string similar to \"200.345 -34.678 345.000\"");
		dSprintf( returnBuffer, 256, "0 0 0");
	} else {
		Point3F point;
		// Con::printf("Scanning point: '%s' in function getClosestNode(Point3F pos)", argv[2]);
		dSscanf(argv[2], "%f %f %f", &point.x, &point.y, &point.z);
	
		AIPathNode* node = object->getClosestNode(point);

		if(node == NULL) {
			Con::errorf("No node found! Returning \"0 0 0\"");
			dSprintf( returnBuffer, 256, "0 0 0");
		} else {
			if(node->getPosition()) {
				dSprintf( returnBuffer, 256, "%f %f %f", node->getPosition().x, node->getPosition().y, node->getPosition().z);
			} else {
				Con::errorf("Node contained no position! Returning \"0 0 0\"");
				dSprintf( returnBuffer, 256, "0 0 0");
			}
		}
	}
	return returnBuffer;
}
ConsoleMethod( AIPathNodeGraph, getClosestNodeIndex, S32, 3, 3, "AIPathNodeGraph.getClosestNodeIndex(Point3F pos) - Get the index of the node closest to the supplied point.")
{
	argc;

	if(dStrlen(argv[2]) == 0) {
		Con::errorf(ConsoleLogEntry::Script, "Zero length string sent as position to function getClosestNodeIndex(Point3F pos), expected string similar to \"200.345 -34.678 345.000\"");
		return -1;
	} else {
		Point3F point;
		// Con::printf("Scanning point: '%s' in function getClosestNodeIndex(Point3F pos)", argv[2]);
		dSscanf(argv[2], "%f %f %f", &point.x, &point.y, &point.z);
	
		return object->getClosestNodeIndex(point);
	}
}
ConsoleMethod( AIPathNodeGraph, getNextNodeIndexOnPath, S32, 4, 4, "AIPathNodeGraph.getNextNodeIndexOnPath(S32 fromNodeIndex, S32 toNodeIndex) - Get the next node as pre-compiled on the path from-to the supplied indexes.")
{
	argc;
	if(dStrlen(argv[2]) == 0) {
		Con::errorf(ConsoleLogEntry::Script, "Zero length string sent as from index to function getNextNodeIndexOnPath(S32 fromNodeIndex, S32 toNodeIndex)");
		return -1;
	}
	if(dStrlen(argv[3]) == 0) {
		Con::errorf(ConsoleLogEntry::Script, "Zero length string sent as to index to function getNextNodeIndexOnPath(S32 fromNodeIndex, S32 toNodeIndex)");
		return -1;
	}
	// Con::printf("Scanning fromIndex: '%s' in function getNextNodeIndexOnPath(S32 fromNodeIndex, S32 toNodeIndex)", argv[2]);
	S32 from = dAtoi(argv[2]);
	// Con::printf("Scanning toIndex: '%s' in function getNextNodeIndexOnPath(S32 fromNodeIndex, S32 toNodeIndex)", argv[3]);
	S32 to   = dAtoi(argv[3]);
	
	S32 nodeIndex = object->getNextNodeIndexOnPath(from, to);
	return nodeIndex;
}
ConsoleMethod( AIPathNodeGraph, addNode, S32, 4, 4, "AIPathNodeGraph.addNode(Point3F pos, F32 moveMod) - Add a node to the list of nodes in the path grid.")
{
	argc;
	if(dStrlen(argv[2]) == 0) {
		Con::errorf(ConsoleLogEntry::Script, "Zero length string sent as position to function addNode(Point3F pos, F32 moveMod), expected string similar to \"200.345 -34.678 345.000\"");
		return -1;
	}
	if(dStrlen(argv[3]) == 0) {
		Con::errorf(ConsoleLogEntry::Script, "Zero length string sent as float to function addNode(Point3F pos, F32 moveMod), expected string similar to \"0.2500\"");
		return -1;
	}
	Point3F point;
	// Con::printf("Scanning point: '%s' in function addNode(Point3F pos, F32 moveMod)", argv[2]);
	dSscanf(argv[2], "%f %f %f", &point.x, &point.y, &point.z);
	AIPathNode* newNode = new AIPathNode(point);
	// Con::printf("Scanning float: '%s' in function addNode(Point3F pos, F32 moveMod)", argv[3]);
	newNode->setMoveModifier(dAtof(argv[3]));
	return object->addNode(newNode);
}
ConsoleMethod( AIPathNodeGraph, nodeCount, S32, 2, 2, "AIPathNodeGraph.nodeCount() - Get number of nodes in the path grid.")
{
	argc;
	return object->nodeCount();
}
ConsoleMethod( AIPathNodeGraph, getNode, const char*, 3, 3, "AIPathNodeGraph.getNode(S32 nodeIndex) - Get a node.")
{
	argc;
	char *returnBuffer = Con::getReturnBuffer( 256 );
	if(dStrlen(argv[2]) == 0) {
		Con::errorf(ConsoleLogEntry::Script, "Zero length string sent as integer to function getNode(S32 nodeIndex), expected string similar to \"67\"");
		dSprintf( returnBuffer, 256, "0 0 0");
	} else {
		// Con::printf("Scanning index: '%s' in function getNode(S32 nodeIndex)", argv[2]);
		S32 nodeIndex = dAtoi(argv[2]);
		AIPathNode* node = object->getNode(nodeIndex);
		Point3F position = node->getPosition();
		dSprintf( returnBuffer, 256, "%f %f %f", position.x, position.y, position.z);
	}
	return returnBuffer;
}

ConsoleMethod( AIPathNodeGraph, connectNodes, void, 4, 5, "AIPathNodeGraph.addNode(S32 fromIndex, S32 toIndex, bool connectBothWays) - Connect 2 nodes, indicating you can access one from the other.")
{
	argc;
	if(dStrlen(argv[2]) == 0) {
		Con::errorf(ConsoleLogEntry::Script, "Zero length string sent as from index to function addNode(S32 fromIndex, S32 toIndex, bool connectBothWays)");
		return;
	}
	if(dStrlen(argv[3]) == 0) {
		Con::errorf(ConsoleLogEntry::Script, "Zero length string sent as to index to function addNode(S32 fromIndex, S32 toIndex, bool connectBothWays)");
		return;
	}
	if(argc > 4  && dStrlen(argv[4]) == 0) {
		Con::errorf(ConsoleLogEntry::Script, "Zero length string sent as boolean to function addNode(S32 fromIndex, S32 toIndex, bool connectBothWays)");
		return;
	}
	// Con::printf("Scanning fromIndex: '%s' in function addNode(S32 fromIndex, S32 toIndex, bool connectBothWays)", argv[2]);
	S32 fromIndex = dAtoi(argv[2]);
	// Con::printf("Scanning toIndex: '%s' in function addNode(S32 fromIndex, S32 toIndex, bool connectBothWays)", argv[3]);
	S32 toIndex = dAtoi(argv[3]);
	
	bool connectBothWays = true;
	if(argc > 4) {
		// Con::printf("Scanning boolean: '%s' in function addNode(S32 fromIndex, S32 toIndex, bool connectBothWays)", argv[4]);
		connectBothWays = dAtob(argv[4]);
	}
	AIPathNode* fromNode = object->getNode(fromIndex);
	AIPathNode* toNode = object->getNode(toIndex);
	fromNode->connect(toNode, connectBothWays);
}
ConsoleMethod( AIPathNodeGraph, getNodeConnectionCount, S32, 3, 3, "AIPathNodeGraph.getNodeConnectionCount(S32 nodeIndex) - Get a nodes connection count(number of neighbours).")
{
	argc;
	if(dStrlen(argv[2]) == 0) {
		Con::errorf(ConsoleLogEntry::Script, "Zero length string sent as integer to function getNodeConnectionCount(S32 nodeIndex), expected string similar to \"67\"");
		return -1;
	}
	S32 nodeIndex = dAtoi(argv[2]);
	AIPathNode* node = object->getNode(nodeIndex);
	return node->getNeighbourCount();
}
ConsoleMethod( AIPathNodeGraph, isCompiled, bool, 2, 2, "AIPathNodeGraph.isCompiled() - Is the path node graph pre-compiled.")
{
	return object->isCompiled();
}
ConsoleMethod( AIPathNodeGraph, compile, bool, 2, 2, "AIPathNodeGraph.compile() - Pre-compile the path node graph to make A* searches lightning fast.")
{
	return object->compile();
}
ConsoleMethod( AIPathNodeGraph, load, bool, 3, 3, "AIPathNodeGraph.load(char* fileName) - Load a pre-compiled path node graph from file.")
{
	if(dStrlen(argv[2]) == 0) {
		Con::errorf(ConsoleLogEntry::Script, "Zero length string sent as file name to function load(char* fileName), expected string similar to \"aName.path\"");
		return false;
	}
	// Con::printf("File name: '%s' in function load(char* fileName)", argv[2]);
	return object->load(argv[2]);
}
ConsoleMethod( AIPathNodeGraph, save, bool, 3, 3, "AIPathNodeGraph.save(char* fileName) - Save a pre-compiled path node graph to file.")
{
	if(dStrlen(argv[2]) == 0) {
		Con::errorf(ConsoleLogEntry::Script, "Zero length string sent as file name to function save(char* fileName), expected string similar to \"aName.path\"");
		return false;
	}
	// Con::printf("File name: '%s' in function save(char* fileName)", argv[2]);
	return object->save(argv[2]);
}
ConsoleMethod( AIPathNodeGraph, clear, void, 2, 2, "AIPathNodeGraph.clear() - Deletes all nodes and connections.")
{
	return object->clear();;
}
