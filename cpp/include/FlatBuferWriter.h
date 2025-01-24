/******************************************************************************
 * FlatBufferWriter.h
 * *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 * Adil Chhabra <adil.chhabra@informatik.uni-heidelberg.de>
 *****************************************************************************/

#ifndef KAHIP_FLATBUFFERWRITER_H
#define KAHIP_FLATBUFFERWRITER_H

#include <fstream>
#include <iostream>
#include <vector>
#include "streaming_clustering_alhollocou_generated.h"

class FlatBufferWriter {
private:

    std::string input_file;
    std::string output_file;

    double buffer_io_time_;
    double global_mapping_time_;
    double total_time_;

    uint64_t maxRSS_;

    double score_;
    uint64_t clusters_amount_;

    uint32_t vmax_start_;
    uint32_t vmax_end_;
    uint32_t condition_;
    uint32_t seed_;
    uint32_t niter_;

    std::string filename_;
    uint64_t num_nodes_;
    uint64_t num_edges_;


public:
    FlatBufferWriter()
            : buffer_io_time_(0.0), global_mapping_time_(0.0),
              total_time_(0.0), maxRSS_(0) {}

    void updateResourceConsumption(double & buffer_io_time,
                                   double & global_mapping_time, 
                                   double & total_time, 
                                   long &maxRSS) {
        buffer_io_time_ = buffer_io_time;
        global_mapping_time_ = global_mapping_time;
        total_time_ = total_time;
        
        maxRSS_ = maxRSS;
    }

    void updateClusteringMetrics(double &score, uint32_t & clusters_amount) {
        score_ = score;
        clusters_amount_ = clusters_amount;
    }
    
    void updateGraphMetaData(   std::string & filename,
                                uint32_t & num_nodes,
                                uint32_t & num_edges) {

        filename_ = filename;
        num_edges_ = num_edges;
        num_nodes_ = num_nodes;
    }

    void updateClustreConfig(   uint32_t & vmax_start, 
                                uint32_t & vmax_end,
                                uint32_t & condition,
                                uint32_t & seed,
                                uint32_t & niter) {

        vmax_start_ = vmax_start;
        vmax_end_ = vmax_end;
        condition_ = condition;
        seed_ = seed;
        niter_ = niter;
    }

    // Function to extract the base filename without path and extension
    static std::string extractBaseFilename(const std::string &fullPath) {
        size_t lastSlash = fullPath.find_last_of('/');
        size_t lastDot = fullPath.find_last_of('.');

        if (lastSlash != std::string::npos) {
            // Found a slash, extract the substring after the last slash
            return fullPath.substr(lastSlash + 1, lastDot - lastSlash - 1);
        } else {
            // No slash found, just extract the substring before the last dot
            return fullPath.substr(0, lastDot);
        }
    }

    void writeClustering(const std::string &baseFilename, const std::string & output_path_string) const {
        // output some information about the partition that we have computed
        flatbuffers::FlatBufferBuilder builder(1024);        

        // Create GraphMetadata
        auto filenameOffset = builder.CreateString(baseFilename);
        StreamInfo::GraphMetadataBuilder metadata_builder(builder);
        metadata_builder.add_filename(filenameOffset);
        metadata_builder.add_num_nodes(num_nodes_);
        metadata_builder.add_num_edges(num_edges_);
        auto metadata = metadata_builder.Finish();
        //std::cout << "Graph: " << baseFilename << std::endl;
        //std::cout << "Nodes: " << num_nodes_ << std::endl;
        //std::cout << "Edges: " << num_edges_ << std::endl;

        // Create RunTime
        StreamInfo::RunTimeBuilder runtime_builder(builder);
        runtime_builder.add_io_time(buffer_io_time_);
        runtime_builder.add_mapping_time(global_mapping_time_);
        runtime_builder.add_total_time(total_time_);
        auto runtimedata = runtime_builder.Finish();
        //std::cout << "IO Time: " << buffer_io_time_ << std::endl;
        //std::cout << "Mapping Time: " << global_mapping_time_ << std::endl;
        //std::cout << "Total Time: " << total_time_ << std::endl;

        // Create ClusteringMetrics
        auto clustering_metrics = StreamInfo::CreateClusteringMetrics(builder, score_, clusters_amount_);
        //std::cout << "Score: " << score_ << std::endl;
        //std::cout << "Clusters Amount: " << clusters_amount_ << std::endl;


        // Create MemoryConsumption
        auto memory_consumption = StreamInfo::CreateMemoryConsumption(builder, maxRSS_);
        //std::cout << "Maximum Resident Set Size (KB): " << maxRSS_ << std::endl;

        StreamInfo::PartitionConfigurationBuilder config_builder(builder);
        config_builder.add_seed(seed_);
        config_builder.add_condition(condition_);
        config_builder.add_niter(niter_);
        config_builder.add_vmax_start(vmax_start_);
        config_builder.add_vmax_end(vmax_end_);
        auto configdata = config_builder.Finish();        
        //std::cout << "Condition: " << condition_ << std::endl;
        //std::cout << "Seed: " << seed_ << std::endl;
        //std::cout << "Number Iteration: " << niter_ << std::endl;
        //std::cout << "Vmax Start: " << vmax_start_ << std::endl;
        //std::cout << "Vmax End: " << vmax_end_ << std::endl;

        // Create the final Partition object
        StreamInfo::PartitionBuilder partition_builder(builder);
        partition_builder.add_graph_metadata(metadata);
        partition_builder.add_runtime(runtimedata);
        partition_builder.add_memory_consumption(memory_consumption);
        partition_builder.add_partition_configuration(configdata);
        partition_builder.add_clustering_metrics(clustering_metrics);
        auto partition = partition_builder.Finish();

        // Now, call Finish only once with the final partition object
        builder.Finish(partition);

        // Step 4: Write to File
        const uint8_t *bufferPointer = builder.GetBufferPointer();
        int bufferSize = builder.GetSize();

        std::string outputFileNameStream;

        outputFileNameStream = output_path_string + baseFilename + ".bin";
        const char *outputFileName = outputFileNameStream.c_str();
        FILE *file = fopen(outputFileName, "wb");
        fwrite(bufferPointer, 1, bufferSize, file);
        fclose(file);
    }
};

#endif //KAHIP_FLATBUFFERWRITER_H
