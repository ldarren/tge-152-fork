#ifndef _AIPPATHMANAGER_H_
#define _AIPPATHMANAGER_H_

#include "platform/platform.h"
#include "console/simBase.h"
#include "math/mPoint.h"

/****************************************************************************
 * The AIPathNode has lots of stuff in it, most of it is used by the
 * AIPathManager object when calculating the pathfinding.
 ****************************************************************************/
class AIPathNode {
public:
	AIPathNode();
	AIPathNode(Point3F position);
	AIPathNode(F32 x, F32 y, F32 z);
	//bool operator == (const AIPathNode other);
	AIPathNode& operator = (const AIPathNode other);
	//bool operator < (const AIPathNode other);
	//bool operator > (const AIPathNode other);
	void connect(AIPathNode* node, bool connectBothWays = true);
	void disconnect(AIPathNode* node, bool disconnectBothWays = true);
	bool isConnected(AIPathNode* node);
	F32 getMoveModifier() { return _moveModifier; };
	void setMoveModifier(F32 moveModifier) { _moveModifier = moveModifier; };
	F32 getDangerModifier() { return _dangerModifier; };
	void setDangerModifier(F32 dangerModifier) { _dangerModifier = dangerModifier; };
	F32 getFitness() const { return _fitness; };
	void setFitness(F32 fitness) { _fitness = fitness; };
	F32 getLowestCostFromStart() { return _lowestCostFromStart; };
	void setLowestCostFromStart(F32 lowestCostFromStart) { _lowestCostFromStart = lowestCostFromStart; };
	F32 getHeuristicCostToGoal() { return _heuristicCostToGoal; };
	void setHeuristicCostToGoal(F32 heuristicCostToGoal) { _heuristicCostToGoal = heuristicCostToGoal; };
	AIPathNode* getParent() { return _parent; };
	void setParent(AIPathNode* parent) { _parent = parent; };
	bool isOpen() { return _open; };
	void setOpen(bool open) { _open = open; };
	bool isClosed() { return _closed; };
	void setClosed(bool closed) { _closed = closed; };
	Point3F getPosition() { return _position; };
	void setPosition(Point3F position) { _position = position; };
	void getNeighbours(Vector<AIPathNode*> &replyList);
	S32 getNeighbourCount() { return _connections.size(); };
private:
	F32 _moveModifier;
	F32 _dangerModifier;
	F32 _fitness;
	F32 _lowestCostFromStart;
	F32 _heuristicCostToGoal;
	AIPathNode* _parent;
	bool _open;
	bool _closed;
	Point3F _position;
	Vector<AIPathNode*> _connections;
};

/****************************************************************************
 * This is the reprecentation of a collection of nodes that has been linked
 * together to form a net of nodes in 3D space.
 ****************************************************************************/
class AIPathNodeGraph : public SimObject
{
	typedef SimObject Parent;
private:
	// Use a mapped storage to find nodes by their location?
	Vector<AIPathNode*> _nodes;
	bool _dataIsPreCompiled;
	S32** _compiledNodeLinks;
	void preCompiledInitDataStorage(S32 nodes);
	void preCompiledLinkTwoWay(S32 start, S32 end, S32 startNext, S32 endPrev);
	void preCompiledLinkOneWay(S32 start, S32 end, S32 startNext);
	void clearNodeNetTempValues();
public:
	DECLARE_CONOBJECT(AIPathNodeGraph);

	AIPathNodeGraph();
	S32 addNode(AIPathNode* node);
	AIPathNode* getNode(S32 nodeIndex) { return _nodes[nodeIndex]; };
	S32 calculateNodeIndex(AIPathNode* node);
	S32 nodeCount() { return _nodes.size(); };
	AIPathNode* removeNode(AIPathNode* node);
	AIPathNode* getClosestNode(Point3F position);
	S32 getClosestNodeIndex(Point3F position);
	AIPathNode* getClosestAccessibleNode(Point3F position);
	AIPathNode* getNextNodeOnPath(S32 fromNodeIndex, S32 toIndex);
	S32 getNextNodeIndexOnPath(S32 fromNodeIndex, S32 toNodeIndex);
	bool isCompiled();
	bool compile();
	bool load(const char* fileName);
	bool save(const char* fileName);
	void clear();
	void renderPaths();
};

/****************************************************************************
 * A special case node graph used to show pathfinding in a graph made up of
 * squares.
 *
 * Ignore at will :)
 ****************************************************************************/
class AIPathNodeGrid30x30
{
public:
	AIPathNodeGrid30x30();
	AIPathNode* setNode(int x, int y, AIPathNode* node);
	AIPathNode* getNode(int x, int y);
private:
	// Use a mapped storage to find nodes by their location?
	AIPathNode* _nodes[30][30];
};

/****************************************************************************
 * The "brain" in all its glory.
 * It is a Singleton ( http://en.wikipedia.org/wiki/Singleton_pattern ) and
 * can not be created as normal, instead you call AIPathManager::getInstance()
 * to get a pointer to the only instance of this class that is allowed in
 * the system.
 ****************************************************************************/
class AIPathManager
{
public:
	~AIPathManager() {};
	static AIPathManager* getInstance();
	bool aStar(AIPathNode* startNode, AIPathNode* goalNode, Vector<AIPathNode*> &replyList);
private:
	AIPathManager() {};
	static AIPathManager* _instance;
	void getSuccessors(AIPathNode* node,  Vector<AIPathNode*> &replyList);
	F32 estimateCostToGoal(AIPathNode* node, AIPathNode* goal);
};

#endif
