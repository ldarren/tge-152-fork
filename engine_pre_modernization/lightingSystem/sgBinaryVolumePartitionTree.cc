//---------------------------------------------------------------
// Synapse Gaming - Binary Volume Partition Tree
// Copyright © Synapse Gaming 2004 - 2005
// Written by John Kabus
//
// Overview:
//  The Binary Volume Partition Tree (BVPT) is a hybrid of the
//  BSP and Oct-tree spacial partitioning schemes that combines
//  the strengths of both into a system that can efficiently
//  processes large amounts of geometry and geometry with large
//  surface area, which allows optimal handling of both brush
//  and mesh geometry.
//
//  Similar to Oct-tree partitioning, BVPT splits the geometric
//  'world' into equal volumes and each volume into sub-volumes
//  creating a tree that assigns each surface or object into the
//  smallest volume that completely contains it.
//
//  The main disadvantage to using Oct-trees is that all geometry
//  that rides the border between *any* two octants is maintained
//  by the parent volume, which means the geometry is used every
//  time the parent volume is traversed regardless of the
//  relationship to the requested octant.
//
//  BVPT avoids Oct-tree inefficiencies by splitting volumes in
//  half (instead of into eighths).  Because of this all geometry
//  maintained by the parent volume spans both of the two sub-volumes,
//  meaning that the parent geometry has a definite relationship
//  to the requested sub-volume.
//---------------------------------------------------------------

#include "lightingSystem/sgBinaryVolumePartitionTree.h"




