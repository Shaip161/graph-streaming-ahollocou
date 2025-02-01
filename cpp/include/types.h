#ifndef DYCOLA_TYPES_H
#define DYCOLA_TYPES_H

#include <utility>
#include <set>

//Types needed for the graph ds
//#ifdef MODE64BITVERTICES
//typedef uint64_t 	    NodeID;
//#else
//typedef unsigned int 	NodeID;
//#endif

typedef unsigned int Node;
typedef std::pair< Node, Node >     Edge;
typedef std::set< Node >            NodeSet;

//typedef double 		    EdgeRatingType;
//typedef unsigned int 	PathID;
typedef unsigned int 	PartitionID;

#ifdef MODE64BITEDGES
//typedef int64_t 	EdgeWeight;
//typedef unsigned 	NodeWeight;
typedef uint64_t 	EdgeID;
#else
//typedef int 		    EdgeWeight;
//typedef unsigned int 	NodeWeight;
typedef unsigned int 	EdgeID;
#endif

//typedef uint64_t LongNodeID;
//typedef uint64_t LongEdgeID;
//typedef int32_t	ShortEdgeWeight;

#endif
