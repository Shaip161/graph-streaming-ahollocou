#include "utils.h"
#include <cmath>
#include <iostream>

int myrandom (int i) { return std::rand()%i; }

long unsigned StartClock() {
    timeval time;
    gettimeofday(&time, NULL);
    long unsigned initTime = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    return initTime;
}

long unsigned StopClock(long unsigned initTime) {
    timeval time;
    gettimeofday(&time, NULL);
    long unsigned endTime = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    return endTime - initTime;
}

void LoadGraph(char * graphFileName,
              std::ifstream *& inFile,
              //std::vector< Edge >& edgeList,
              Node & maxNodeId,
              uint32_t nbLinesToSkip) {

    if (!(*(inFile))) {
      std::cerr << "Error opening " << (*graphFileName) << std::endl;
      exit(1);
    }

    // skip comments
    for (uint32_t i = 0; i < nbLinesToSkip; i++) {
        std::string s;
        std::getline((*inFile), s);
    }

    (*inFile) >> maxNodeId;
    
    // Loading edges
    /*Node node1, node2;
    maxNodeId = 0;
    while( inFile >> node1 ) {
        inFile >> node2;
        edgeList.push_back(std::make_pair(node1, node2));
        if (node1 > maxNodeId) {
            maxNodeId = node1;
        }
        if (node2 > maxNodeId) {
            maxNodeId = node2;
        }
    }*/
}



std::pair<double,uint64_t> CalculateScore(  std::vector< Edge > & edgeList,
                                            std::vector< std::vector< uint32_t > > & nodeCommunityList,
                                            std::vector< std::vector< uint32_t > > & communityVolumeList ) {
    
    double best_score = -1;
    uint64_t clusters_amount = 0;
    
    for(int Vmax_ix = 0; Vmax_ix < communityVolumeList.size(); Vmax_ix++) {
        
        double first_sum = 0;
        
        for(auto & edge : edgeList) {
            uint64_t source = edge.first;
            uint64_t target = edge.second;

            if(nodeCommunityList[Vmax_ix][source] == nodeCommunityList[Vmax_ix][target]) {
                first_sum++;
            }
        }

        double second_sum = 0;
        for(auto & cluster : communityVolumeList[Vmax_ix]) {
            if(cluster != 0) { clusters_amount++; }
            second_sum += (cluster * cluster) / static_cast<double>((2 * edgeList.size()));
        }

        double current_score = 0;
        current_score = (1 / static_cast<double>(2 * edgeList.size())) * ((2 * first_sum) - second_sum);
        if(current_score > best_score) {
            best_score = current_score;
        }

    }

    return std::make_pair(best_score , clusters_amount);
}

// Function to extract the base filename without path and extension
std::string extractBaseFilename(const std::string &fullPath) {
    size_t lastSlash = fullPath.find_last_of('/');
    size_t lastDot = fullPath.find_last_of('.');

    if (lastSlash != std::string::npos) {
        // Found a slash, extract the substring after the last slash
        return fullPath.substr(lastSlash + 1, lastDot - lastSlash - 1);
    }
    else {
        // No slash found, just extract the substring before the last dot
        return fullPath.substr(0, lastDot);
    }
}


int BuildNeighborhoods(std::vector< Edge >& edgeList, std::vector< NodeSet >& nodeNeighbors) {
    for (std::vector< Edge >::iterator it = edgeList.begin(); it != edgeList.end(); ++it) {
        Edge edge(*it);
        nodeNeighbors[edge.first].insert(edge.second);
        nodeNeighbors[edge.second].insert(edge.first);
    }
    return 0;
}

int PrintPartition(uint32_t & nbCommunities,
                   const char* fileName,
                   std::map< uint32_t, std::set< Node > >& communities,
                   bool removeSingleton) {
    std::ofstream outFile;
    outFile.open(fileName);
    for (auto kv : communities) {
        if (!removeSingleton || kv.second.size() > 1) {
            std::set<Node>::iterator it2 = kv.second.begin();
            Node nodeId;
            while ( true ) {
                nodeId = *it2;
                ++it2;
                if (it2 != kv.second.end()) {
                    outFile << nodeId << " ";
                } else {
                    break;
                }
            }
            outFile << nodeId << std::endl;
            nbCommunities++;
        }
    }
    //printf("%-32s %i\n", "Number of Communities:", nbCommunities);
    outFile.close();
    return 0;
}

int GetCommunities(const std::vector< uint32_t > nodeCommunity,
                   Node maxNodeId,
                   std::map< uint32_t, std::set< Node > >& communities) {
    for (Node i = 0; i <= maxNodeId; i++) {
        if (nodeCommunity[i] > 0) {
            communities[nodeCommunity[i]].insert(i);
        }
    }
    return 0;
}
