import sys

# Check for correct usage
if len(sys.argv) != 3:
    print("Usage: python script.py <input_file> <output_file>")
    sys.exit(1)

# Get input and output file paths from command-line arguments
input_file = sys.argv[1]
output_file = sys.argv[2]

# Read the input file and parse the data
cluster_map = {}
with open(input_file, "r") as file:
    for cluster_id, line in enumerate(file):
        nodes = map(int, line.split())
        for node in nodes:
            cluster_map[node] = cluster_id

# Determine the maximum node ID to ensure proper ordering
max_node_id = max(cluster_map.keys())

# Write the output file
with open(output_file, "w") as file:
    for node_id in range(max_node_id + 1):  # Iterate over all node IDs from 0 to max_node_id
        cluster_id = cluster_map.get(node_id, -1)  # Default to -1 if a node ID is not in the map
        file.write(f"{cluster_id}\n")

print(f"Node-to-cluster mapping has been written to {output_file}")
