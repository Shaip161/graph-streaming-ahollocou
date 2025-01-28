#ifndef DYCOLA_UTILS_H
#define DYCOLA_UTILS_H

#include <stdlib.h>
#include <map>
#include <vector>
#include <set>
#include <utility>
#include <sys/time.h>
#include <fstream>
#include "types.h"

int myrandom (int i);
long unsigned StartClock();
long unsigned StopClock(long unsigned initTime);
void LoadGraph(char * graphFileName,
              std::ifstream *& inFile,
              //std::vector< Edge >& edgeList,
              NodeID & maxNodeId,
              EdgeID & total_edges,
              EdgeID nbLinesToSkip=0);
int BuildNeighborhoods(std::vector< Edge >& edgeList, std::vector< NodeSet >& nodeNeighbors);
int PrintPartition(PartitionID & nbCommunities,
                   const char* fileName,
                   std::map< uint32_t, std::set< NodeID > >& communities,
                   bool removeSingleton=false);
int PrintStats(const char* fileName,
               uint32_t nbCommunities,
               unsigned long algoTime);
int PrintStats(const char* fileName,
               uint32_t nbRuns,
               std::vector< uint32_t > nbCommunities,
               std::vector< unsigned long > executionTimes);
int GetCommunities(const std::vector< uint32_t > nodeCommunity,
                   NodeID maxNodeId,
                   std::map< uint32_t, std::set< NodeID > >& communities);

std::string extractBaseFilename(const std::string &fullPath);

#endif
