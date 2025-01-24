import sys
import flatbuffers
from StreamInfo import GraphMetadata, Partition, PartitionConfiguration, RunTime, ClusteringMetrics, MemoryConsumption

def update_score_in_partition(file_path, output_path, new_score):
    # Read the FlatBuffer binary file
    with open(file_path, 'rb') as file:
        buf = file.read()

    # Get the root of the FlatBuffer
    partition_obj = Partition.Partition.GetRootAsPartition(buf, 0)

    # Extract all fields for reconstruction
    builder = flatbuffers.Builder(1024)

    # GraphMetadata
    graph_metadata = partition_obj.GraphMetadata()
    filename = graph_metadata.Filename()
    filename_offset = builder.CreateString(filename)
    GraphMetadata.GraphMetadataStart(builder)
    GraphMetadata.GraphMetadataAddFilename(builder, filename_offset)
    GraphMetadata.GraphMetadataAddNumNodes(builder, graph_metadata.NumNodes())
    GraphMetadata.GraphMetadataAddNumEdges(builder, graph_metadata.NumEdges())
    graph_metadata_offset = GraphMetadata.GraphMetadataEnd(builder)

    # PartitionConfiguration
    partition_config = partition_obj.PartitionConfiguration()
    PartitionConfiguration.PartitionConfigurationStart(builder)
    PartitionConfiguration.PartitionConfigurationAddVmaxStart(builder, partition_config.VmaxStart())
    PartitionConfiguration.PartitionConfigurationAddVmaxEnd(builder, partition_config.VmaxEnd())
    PartitionConfiguration.PartitionConfigurationAddCondition(builder, partition_config.Condition())
    PartitionConfiguration.PartitionConfigurationAddSeed(builder, partition_config.Seed())
    PartitionConfiguration.PartitionConfigurationAddNiter(builder, partition_config.Niter())
    partition_config_offset = PartitionConfiguration.PartitionConfigurationEnd(builder)

    # Runtime
    runtime = partition_obj.Runtime()
    RunTime.RunTimeStart(builder)
    RunTime.RunTimeAddIoTime(builder, runtime.IoTime())
    RunTime.RunTimeAddMappingTime(builder, runtime.MappingTime())
    RunTime.RunTimeAddTotalTime(builder, runtime.TotalTime())
    runtime_offset = RunTime.RunTimeEnd(builder)

    # ClusteringMetrics (update score here)
    clustering_metrics = partition_obj.ClusteringMetrics()
    ClusteringMetrics.ClusteringMetricsStart(builder)
    ClusteringMetrics.ClusteringMetricsAddScore(builder, new_score)
    ClusteringMetrics.ClusteringMetricsAddClusteringAmount(builder, clustering_metrics.ClusteringAmount())
    clustering_metrics_offset = ClusteringMetrics.ClusteringMetricsEnd(builder)

    # MemoryConsumption
    memory_consumption = partition_obj.MemoryConsumption()
    MemoryConsumption.MemoryConsumptionStart(builder)
    MemoryConsumption.MemoryConsumptionAddOverallMaxRss(builder, memory_consumption.OverallMaxRss())
    memory_consumption_offset = MemoryConsumption.MemoryConsumptionEnd(builder)

    # Partition
    Partition.PartitionStart(builder)
    Partition.PartitionAddGraphMetadata(builder, graph_metadata_offset)
    Partition.PartitionAddPartitionConfiguration(builder, partition_config_offset)
    Partition.PartitionAddRuntime(builder, runtime_offset)
    Partition.PartitionAddClusteringMetrics(builder, clustering_metrics_offset)
    Partition.PartitionAddMemoryConsumption(builder, memory_consumption_offset)
    partition_offset = Partition.PartitionEnd(builder)

    # Finish the buffer
    builder.Finish(partition_offset)

    # Write the modified FlatBuffer to a new file
    with open(output_path, 'wb') as out_file:
        out_file.write(builder.Output())

    print(f"Updated FlatBuffer file written to {output_path}")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python script.py <input_file_path> <output_file_path> <new_score>")
        sys.exit(1)

    input_file_path = sys.argv[1]
    output_file_path = sys.argv[2]
    new_score = float(sys.argv[3])

    update_score_in_partition(input_file_path, output_file_path, new_score)
