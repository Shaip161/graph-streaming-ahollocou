#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <sys/time.h>
#include <fstream>
#include <list>
#include <map>
#include <sys/resource.h>
#include <set>
#include <cmath>
#include <algorithm>
#include <string>
#include "types.h"
#include "utils.h"
#include <ctime>
#include "streaming_clustering_alhollocou_generated.h"
#include "FlatBuferWriter.h"
#include <flatbuffers/flatbuffers.h>
#include <iostream>
#include "timer.h"


#define CHECK_ARGUMENT_STRING(index, option,variable,setVariable) \
    if( strcmp(argv[index],option) == 0 ){ \
            setVariable = true; \
            if( (index+1) < argc ) { \
                variable = argv[index+1]; \
            } else { \
                printf( "Invalid options.\n" ); \
                return 1;\
            }\
        }

#define CHECK_ARGUMENT_FLOAT(index, option,variable,setVariable) \
    if( strcmp(argv[index],option) == 0 ){ \
            setVariable = true; \
            if( (index+1) < argc ) { \
                variable = atof(argv[index+1]); \
            } else { \
                printf( "Invalid options.\n" ); \
                return 1;\
            }\
        }

#define CHECK_ARGUMENT_INT(index, option,variable,setVariable) \
    if( strcmp(argv[index],option) == 0 ){ \
            setVariable = true; \
            if( (index+1) < argc ) { \
                variable = atoi(argv[index+1]); \
            } else { \
                printf( "Invalid options.\n" ); \
                return 1;\
            }\
        }

#define CHECK_FLAG(index, option,setVariable) \
    if( strcmp(argv[index],option) == 0 ){ \
            setVariable = true; \
        }

Edge EdgeStream(std::ifstream *& inFile) {
    
    Edge new_edge(-1, -1);
    std::string file_edge;
    
    while( (*inFile) >> new_edge.first ) {
        (*inFile) >> new_edge.second;
        break;
    }

    return new_edge;
}

int StreamComAlgo(const std::vector< Edge >& edgeList,
                 /*std::ifstream *& inFile,
                 EdgeID & edge_amount,*/
                 std::vector< std::vector< uint32_t > >& nodeCommunityList,
                 std::vector< uint32_t >& nodeDegree,
                 std::vector< std::vector< uint32_t > >& communityVolumeList,
                 std::vector< uint32_t >& maxVolumeList,
                 uint32_t condition,
                 timer & io_t,
                 double & io_time,
                 timer & mapping_t,
                 double & mapping_time,
                 uint32_t randSeed=0
                 ) {
    // Random shuffle
    std::srand(randSeed); // use current time as seed for random generator
    std::vector< Edge > shuffledEdgeList (edgeList);
    std::random_shuffle(shuffledEdgeList.begin(), shuffledEdgeList.end(), myrandom);
    // Aggregation
    std::vector< uint32_t > nextCommunityIdList (maxVolumeList.size(), 1);

    mapping_time += io_time; 
    mapping_t.restart();

    for (Edge edge : shuffledEdgeList) {
    //while(true) {
        //io_t.restart();
        //Edge edge = EdgeStream(inFile);
        //if(edge.first == -1 || edge.second == -1) {
        //    break;
        //}

        //io_time += io_t.elapsed();

        //edge_amount++;
        Node sourceNode = edge.first;
        Node targetNode = edge.second;
        nodeDegree[sourceNode]++;
        nodeDegree[targetNode]++;
        for (uint32_t i = 0; i < maxVolumeList.size(); i++) {
            EdgeID maxVolume = maxVolumeList[i];
            if(nodeCommunityList[i][sourceNode] == 0) {
                nodeCommunityList[i][sourceNode] = nextCommunityIdList[i];
                nextCommunityIdList[i]++;
            }
            if(nodeCommunityList[i][targetNode] == 0) {
                nodeCommunityList[i][targetNode] = nextCommunityIdList[i];
                nextCommunityIdList[i]++;
            }
            EdgeID sourceCommunityVolume = ++communityVolumeList[i][nodeCommunityList[i][sourceNode]];
            EdgeID targetCommunityVolume = ++communityVolumeList[i][nodeCommunityList[i][targetNode]];
            if(((condition == 0) && ((sourceCommunityVolume <= maxVolume) && (targetCommunityVolume <= maxVolume)))
            || ((condition == 1) && ((sourceCommunityVolume <= maxVolume) || (targetCommunityVolume <= maxVolume)))) {
                if(sourceCommunityVolume < targetCommunityVolume) {
                    communityVolumeList[i][nodeCommunityList[i][sourceNode]] -= nodeDegree[sourceNode];
                    communityVolumeList[i][nodeCommunityList[i][targetNode]] += nodeDegree[sourceNode];
                    nodeCommunityList[i][sourceNode] = nodeCommunityList[i][targetNode];
                } else {
                    communityVolumeList[i][nodeCommunityList[i][targetNode]] -= nodeDegree[targetNode];
                    communityVolumeList[i][nodeCommunityList[i][sourceNode]] += nodeDegree[targetNode];
                    nodeCommunityList[i][targetNode] = nodeCommunityList[i][sourceNode];
                }
            }
        }
    }
    
    mapping_time += mapping_t.elapsed();
    mapping_time -= io_time;

    return 0;
}


static void PrintUsage() {
    printf("Usage: streamcom <flags>\n");
    printf("Availaible flags:\n");
    printf("\t-f [graph file name] : Specifies the graph file.\n");
    printf("\t--skip [number of lines] : Specifies the number of lines to skip in the graph file.\n");
    printf("\t-o [output file name] : Specifies the output file for communities.\n");
    printf("\t--vmax-start [maximum volume range start] : Specifies the maximum volume for the aggregation phase, beginning of the range.\n");
    printf("\t--vmax-end [maximum volume range end] : Specifies the maximum volume for the aggregation phase, end of the range.\n");
    printf("\t-c [condition] : Specifies the aggregation condition for the aggregation phase: 0 is AND and 1 is OR.\n");
    printf("\t--seed [random seed]: Specifies the random seed if the edges need to be shuffle.\n");
    printf("\t--niter [number of iteration]: Specifies the number of iteration of the algorithm.\n");
}

long getMaxRSS() {
    struct rusage usage;

    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        // The maximum resident set size is in kilobytes
        return usage.ru_maxrss;
    } else {
        std::cerr << "Error getting resource usage information." << std::endl;
        // Return a sentinel value or handle the error in an appropriate way
        return -1;
    }
}

int main(int argc, char ** argv) {
    bool graphFileNameSet = false;
    bool outputFileNameSet = false;
    bool outputPathSet = false;
    bool volumeThresholdStartSet = false;
    bool volumeThresholdEndSet = false;
    bool conditionSet = false;
    bool randomSeedSet = false;
    bool nIterSet = false;
    bool nbLinesToSkipSet = false;
    char * output_path = NULL;
    char * graphFileName = NULL;
    char * outputFileName = NULL;
    uint32_t volumeThresholdStart = 9;
    uint32_t volumeThresholdEnd = 9;
    uint32_t condition = 0;
    int randomSeed = 0;
    uint32_t nIter = 1;
    uint32_t nbLinesToSkip = 0;

    timer io_t, mapping_t, total_t;

    double io_time = 0;
    double mapping_time = 0;
    double total_time = 0;

    for(int i = 1; i < argc; i++) {
        CHECK_ARGUMENT_STRING(i, "-f", graphFileName, graphFileNameSet);
        CHECK_ARGUMENT_INT(i, "--skip", nbLinesToSkip, nbLinesToSkipSet);
        CHECK_ARGUMENT_STRING(i, "-o", outputFileName, outputFileNameSet)
        CHECK_ARGUMENT_INT(i, "--vmax-start", volumeThresholdStart, volumeThresholdStartSet);
        CHECK_ARGUMENT_INT(i, "--vmax-end", volumeThresholdEnd, volumeThresholdEndSet);
        CHECK_ARGUMENT_INT(i, "-c", condition, conditionSet);
        CHECK_ARGUMENT_INT(i, "--seed", randomSeed, randomSeedSet);
        CHECK_ARGUMENT_INT(i, "--niter", nIter, nIterSet);
        CHECK_ARGUMENT_STRING(i, "--output_path", output_path, outputPathSet)
    }

    if (!graphFileNameSet) {
        printf("Graph filename not set\n");
        PrintUsage();
        return 1;
    }

    if (!outputFileNameSet) {
        printf("Output filename not set\n");
        PrintUsage();
        return 1;
    }

    if (!randomSeedSet) {
        randomSeed = std::time(0);
    }

    if (!volumeThresholdEndSet) {
        volumeThresholdEnd = volumeThresholdStart;
    }

    //printf("%-32s %i\n", "Volume threshold (Range Start):", volumeThresholdStart);
    //printf("%-32s %i\n", "Volume threshold (Range End):", volumeThresholdEnd);

    //printf("%-32s %i\n", "Random Seed:", randomSeed);

    /*long unsigned totalTime = 0,
                  initTime = 0,
                  spentTime = 0,
                  loadingTime = 0,
                  algorithmTime = 0;*/

    total_t.restart();
    // Allocating list for edges
    std::vector< Edge > edgeList;


    //std::ifstream * inFile = NULL;
    std::string filename = graphFileName;
    //inFile = new std::ifstream(filename.c_str());

    std::string baseFilename = extractBaseFilename(filename);

    //==================== LOAD THE GRAPH ==================================
    //printf("%-32s %s\n", "Graph file name:", graphFileName);
    //printf("%-32s %i\n", "Number of lines to skip:", nbLinesToSkip);
    Node maxNodeId;
    //initTime = StartClock();
    io_t.restart();
    LoadGraph(graphFileName, edgeList, /*inFile,*/ maxNodeId, /*total_edges,*/ nbLinesToSkip);
    io_time += io_t.elapsed();
    //spentTime = StopClock(initTime);
    //loadingTime = spentTime;
    //totalTime += spentTime;
    //printf("%-32s %lu\n", "Nb of edges:", edgeList.size());
    //printf("%-32s %lu ms\n", "Loading time:", loadingTime);
    //======================================================================

    EdgeID edge_amount = 0; 
    PartitionID nbCommunities = 0;
    long overall_max_RSS = 0;
   
    for (uint32_t iter = 0; iter < nIter; iter ++) {
        
        std::vector< uint32_t > volumeThresholdList;
        for (uint32_t volumeThreshold = volumeThresholdStart; volumeThreshold <= volumeThresholdEnd; volumeThreshold++) {
            volumeThresholdList.push_back(volumeThreshold);
        }
        std::vector< uint32_t > nodeDegree (maxNodeId + 1);
        std::vector< std::vector< uint32_t > > nodeCommunityList (volumeThresholdList.size(), std::vector< uint32_t > (maxNodeId + 1, 0));
        std::vector< std::vector< uint32_t > > communityVolumeList (volumeThresholdList.size(), std::vector< uint32_t > (maxNodeId + 1, 0));

        //=================== ALGORITHM  =======================================
        //printf("Start algorithm...\n");
        //initTime = StartClock();
        StreamComAlgo(edgeList, /*inFile, edge_amount,*/ nodeCommunityList, nodeDegree, communityVolumeList, volumeThresholdList, condition, io_t, io_time, mapping_t, mapping_time, randomSeed * (1 + iter));
        //spentTime = StopClock(initTime);
        //algorithmTime += spentTime;
        //totalTime += spentTime;
        //printf("%-32s %lu ms\n", "Algorithm time:", spentTime);
        //======================================================================

        total_time += total_t.elapsed();

        //NodeID biggest_partition_id = 0;

        //for (NodeID j = 0; j <= maxNodeId; j++) {
        //    if(nodeCommunityList[0][j] > biggest_partition_id) {
        //        biggest_partition_id = nodeCommunityList[0][j];
        //    } 
        //}
        //biggest_partition_id++;
        overall_max_RSS = getMaxRSS();
        //======================== PRINT RESULTS ===============================
        //initTime = StartClock();
        for (uint32_t i = 0; i < volumeThresholdList.size(); i++) {
            std::map< uint32_t, std::set< Node > > communities;
            //for (NodeID j = 0; j <= maxNodeId; j++) {
            //    if (nodeCommunityList[i][j] > 0) {
            //        communities[nodeCommunityList[i][j]].insert(j);
            //    } else {
            //        communities[biggest_partition_id].insert(j);
            //        biggest_partition_id++;
            //    }
            //}
            GetCommunities(nodeCommunityList[i], maxNodeId, communities);
            std::string outputFileName_i = output_path + baseFilename + ".txt";
            //printf("%-32s %i\t", "Volume theshold: ", volumeThresholdList[i]);
            nbCommunities = 0;
            PrintPartition(nbCommunities, outputFileName_i.c_str(), communities, false);
        }
        //spentTime = StopClock(initTime);
        //totalTime += spentTime;
        //printf("%-32s %lu ms\n", "Print partition time:", spentTime);
        //======================================================================
    }

    //printf("\n");
    //printf("*******************************************************\n");
    //printf("%-32s %-10lu ms\n", "Loading time:", loadingTime);
    //double avgAlgorithmTime = ((float) algorithmTime) / ((float) nIter);
    //printf("%-32s %0.2f ms\n", "Average algorithm time:", avgAlgorithmTime);
    //printf("%-32s %-10lu ms\n", "Total execution time:", totalTime);
    //printf("*******************************************************\n");

    FlatBufferWriter fb_writer;
    fb_writer.updateResourceConsumption(io_time, mapping_time,
                                        total_time, overall_max_RSS);

    fb_writer.updateClustreConfig(volumeThresholdStart, volumeThresholdEnd, condition, randomSeed, nIter); 
    
    double score = -1;
    std::string output_path_string = output_path;

    fb_writer.updateClusteringMetrics(score, nbCommunities);
    
    fb_writer.updateGraphMetaData(baseFilename, ++maxNodeId, edge_amount);

    fb_writer.writeClustering(baseFilename, output_path);
    
    return 0;
}
