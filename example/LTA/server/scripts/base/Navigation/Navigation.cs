$NAV_NODES_PRE_COMPILED = true;
$NAV_NODES_ENABLED = true; // Set whether to build the NavNodeSet for AIPathing
$NAV_NODES_HIDDEN = true;  // Sets whether or not to hide the nodes during loading
$NAV_NODE_RADIUS = 80;     // Sets the maximum radius that a node will look for a neighboring node.
// Setting this number low will require more nodes on a map to cover all areas.
// Setting this number high can result in a node having more children nodes than
// needed and slow down calculations. Set a balance based on the size of the map
// and the complexity needed for navigation.

$NAV_DEBUG_LEVEL = 10;

// Prints navigation mode related info to console.
// Setting level to a low value will print it at lower
// values of $escortDebug. That is, set high values on the stuff that
// is least interesting ( not totally intuitive, I know...
// deal with it ;) ).
function navigationTrace(%source, %text, %level) {
	if( $NAV_DEBUG_LEVEL > %level) {
		if(%source $= "") {
			echo("[NAVIGATION] " @ %text);
		} else {
			echo("[NAVIGATION] " @ %source @ "->" @ %text);
		}
	}
}

datablock StaticShapeData(NavNode)
{
	// Mission editor category, this datablock will show up in the
	// specified category under the "shapes" root category.
	category = "NavigationNode";
	// Basic Item properties
	shapeFile = "~/data/shapes/markers/navmarker.dts";
	parent = -1;
	f = 0;	// These nodes are used by the navigation and pathfinding
		// algorithms. Leave them alone.
	g = 0;
	h = 0;
	x = -1;
	y = -1;
	orphaned = false;	// a node is orphaned if there are no other 
				// nodes close enough to it for it to have them
				// as a child node.
	moveMod = 0; // Increase if want to make this node worse to walk through.
};

// - This loads all the nodes from the mission to an AIPathNodeGraph object.
// - Then it makes a second pass to find the nearby children node that the node
//   can reach.
// - Then the nav net is ( optionally ) pre-compiled and saved to disc to make
//   navigation faster.
// - Then the function hides all nodes that are connected to other nodes.
function buildNodeGraph() {
	navigationTrace("", "buildNodeGraph()", 5);
	
	if ($NAV_NODES_ENABLED == true) {
		navigationTrace("", "Loading Navigation Nodes...", 3);
		
		// Create the storage for the NavNodes
		new AIPathNodeGraph(NodeGraph);
		MissionCleanup.add(NodeGraph);

		if($NAV_NODES_PRE_COMPILED == true) {
			// Load the mission specific script file ( if there is one )
			%fileName = filePath($Server::MissionFile) @ "/paths/" @ fileBase($Server::MissionFile) @ ".path";
			if(NodeGraph.load(%fileName)) {
				navigationTrace("", "Navigation nodes loaded from file...", 3);
				
				// The saved data does not contain info on what
				// nodes can see each other, so find out now.
				linkNodeGraphFromMission();
			} else {
				readNodeGraphFromMission();
				linkNodeGraphFromMission();
				if(NodeGraph.compile()) {
					navigationTrace("", "Navigation node net compiled...", 3);
					if(NodeGraph.save(%fileName)) {
						navigationTrace("", "Navigation nodes saved to file...", 3);
					} else {
						navigationTrace("", "WARNING: Failed to save navigation net...", 3);
					}
				} else {
					navigationTrace("", "WARNING: Navigation node compile failed, switching to \"live\" navigation...", 3);
					$NAV_NODES_PRE_COMPILED = false;
				}
			}
		} else {
			navigationTrace("", "\"Live\" navigation selected... dont think that still works actually", 3);
		}
	} else {
		navigationTrace("", "Navigation nodes disabled...", 3);
	}

	if ($NAV_NODES_HIDDEN == true) {
		hideActiveNavNodes();
	}
}

// Reads all nodes from the mission file and adds them to the NodeGraph object
// ( It counts on there beeing a NodeGraph object present already )

function readNodeGraphFromMission() {
	navigationTrace("", "readNodeGraphFromMission()", 5);
	
	NodeGraph.clear();
	
	%groupName = "MissionGroup/NavigationNet";
	%group = nameToID(%groupName);
	if (%group != -1) {
		navigationTrace(%this, "Group " @ %groupName @ " loaded(id=" @ %group @ "), it has " @ %group.getCount() @ " elements", 5);
		for(%index = 0; %index < %group.getCount(); %index++) {
			// Really dont care what you use as nodes, as long as they have a position
			%navNode = %group.getObject(%index);
			if(%navNode.moveMod $= "") {
				%navNode.moveMod = 0;
			}
			NodeGraph.addNode(%navNode.getPosition(), %navNode.moveMod);
		}
		linkNodeGraphFromMission();
	}
}

function linkNodeGraphFromMission() {

	// Perform a second pass through the nodes to set the children to any nodes
	// that are within the given radius and are in the LOS of the parent node.

	// Loop through all nodes in NodeGraph again 
	navigationTrace(%this, "NodeGraph has " @ NodeGraph.nodeCount() @ " elements", 8);
	for(%i=0; %i < NodeGraph.nodeCount(); %i++) {
		// Set %node to the current indexed node
		%fromNode = NodeGraph.getNode(%i);
		for(%j=0; %j < NodeGraph.nodeCount(); %j++) {
			%toNode = NodeGraph.getNode(%j);
			if (%fromNode !$= %toNode) {
				// The next few lines run a LOS check from the parent to the target.
				// If the node can be reached without hitting an obstruction it is added a a child node.

				%searchResult = containerRayCast(%fromNode, %toNode, $TypeMasks::TerrainObjectType
					| $TypeMasks::InteriorObjectType | $TypeMasks::StaticObjectType 
					| $TypeMasks::StaticShapeObjectType | $TypeMasks::MarkerObjectType);
				%foundObject = getword(%searchResult,0);
				if(%foundObject == 0) {
					// There was no obstruction so we may be good to add this node as a child

					// WARNING: I now count on the fact that %i and %j are correct indexes into
					// the AIPathNodeGraph classes intrenal representation for the nodes...

					%dist = vectorDist(%fromNode, %toNode); // Do before raycast? Yes, probably...

					if(%dist <= $NAV_NODE_RADIUS) {
						navigationTrace(%this, "..." @ %fromNode @ " can see " @ %toNode @ " and is within a " @ $NAV_NODE_RADIUS @ " unit radius.", 15);
						NodeGraph.connectNodes(%i, %j, false); // Same result as true, but faster?
					}
				}
			}
		}
	}
}

// This function shows or hides nodes, used in debugging mostly.
function hideNavNodes(%visible) {
	navigationTrace("", "hideNavNodes(" @ %visible @ ")", 5);
	
	if(%visible $= "") {
		%visible = true;
	}
	%groupName = "MissionGroup/NavigationNet";
	%group = nameToID(%groupName);

	if (%group != -1) {
		if(%visible) {
			navigationTrace("", "Hiding Nav nodes...", 3);
		} else {
			navigationTrace("", "Showing Nav nodes...", 3);
		}
		for(%index = 0; %index < %group.getCount(); %index++) {
			%targetobject = %group.getObject(%index);
			%targetobject.setHidden(%visible);
		}
	}
}

// This function hides only the active (non orphaned) nodes.
function hideActiveNavNodes() {
	navigationTrace("", "hideActiveNavNodes()", 5);
	
	%groupName = "MissionGroup/NavigationNet";
	%group = nameToID(%groupName);

	if (%group != -1) {
		navigationTrace("", "There are " @ %group.getCount() @ " Nav nodes, lets hide the ones that are connected to other nodes...", 5);
		for(%index = 0; %index < %group.getCount(); %index++) {
			// We only want to hide non-abandoned nodes. This will make it easy to spot
			// nodes that are lost or too isolated.
			%targetobject = %group.getObject(%index);
			if (NodeGraph.getNodeConnectionCount(%index) > 0) {
				%targetobject.setHidden(true);
			} else {
				navigationTrace("", "Index " @ %index @ " is not connected to the rest of the nav net.", 5);
			}
		}
	}
}
