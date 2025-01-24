import sys
import flatbuffers
from StreamInfo import GraphMetadata, Partition, PartitionConfiguration, RunTime, ClusteringMetrics, MemoryConsumption

def read_partition_from_file(file_path):
    with open(file_path, 'rb') as file:
        # Read the FlatBuffer binary file
        buf = file.read()

        # Create a FlatBuffer object
        partition_obj = Partition.Partition.GetRootAsPartition(buf, 0)

        # Access and print the fields
        graph_metadata = partition_obj.GraphMetadata()
        print("Graph Metadata:")
        print(f"  Filename: {graph_metadata.Filename().decode('utf-8')}")
        print(f"  Num Nodes: {graph_metadata.NumNodes()}")
        print(f"  Num Edges: {graph_metadata.NumEdges()}")

        partition_config = partition_obj.PartitionConfiguration()
        print("\nPartition Configuration:")
        print(f"  Vmax Start: {partition_config.VmaxStart()}")
        print(f"  Vmax End: {partition_config.VmaxEnd()}")
        print(f"  Condition: {partition_config.Condition()}")
        print(f"  Seed: {partition_config.Seed()}")
        print(f"  Niter: {partition_config.Niter()}")

        runtime = partition_obj.Runtime()
        print("\nRun Time:")
        print(f"  IO Time: {runtime.IoTime()}")
        print(f"  Mapping Time: {runtime.MappingTime()}")
        print(f"  Total Time: {runtime.TotalTime()}")
        
        clustering_metrics = partition_obj.ClusteringMetrics()
        print("\nClustering Metrics:")
        print(f"  Score: {clustering_metrics.Score()}")
        print(f"  Clusters Amount: {clustering_metrics.ClusteringAmount()}")

        memory_consumption = partition_obj.MemoryConsumption()
        print("\nMemory Consumption:")
        print(f"  Overall Max RSS: {memory_consumption.OverallMaxRss()}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <binary_file_path>")
        sys.exit(1)

    file_path = sys.argv[1]
    read_partition_from_file(file_path)
