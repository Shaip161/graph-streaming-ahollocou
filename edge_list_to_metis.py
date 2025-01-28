import sys
import os
from collections import defaultdict

def edgelist_to_metis(edgelist_file, output_dir):
    with open(edgelist_file, 'r') as infile:
        lines = infile.readlines()

    # Read the number of nodes from the first line
    num_nodes = int(lines[0].strip())

    # Create an adjacency list
    adjacency_list = defaultdict(list)

    # Parse the edges and populate the adjacency list
    for line in lines[1:]:
        node_a, node_b = map(int, line.strip().split())
        adjacency_list[node_a + 1].append(node_b + 1)  # Convert to 1-indexed
        adjacency_list[node_b + 1].append(node_a + 1)  # Convert to 1-indexed

    # Count the total number of edges
    num_edges = sum(len(neighbors) for neighbors in adjacency_list.values()) // 2

    # Generate output file name based on the input file name
    base_filename = os.path.splitext(os.path.basename(edgelist_file))[0]
    metis_file = os.path.join(output_dir, f"{base_filename}.txt")

    # Write the METIS file
    with open(metis_file, 'w') as outfile:
        # Write the number of nodes and edges
        outfile.write(f"{num_nodes} {num_edges}\n")

        # Write the adjacency list
        for node_id in range(1, num_nodes + 1):
            neighbors = " ".join(map(str, sorted(adjacency_list[node_id])))
            outfile.write(f"{neighbors}\n")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <input_edgelist_file> <output_directory>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_dir = sys.argv[2]

    if not os.path.isdir(output_dir):
        print(f"Error: {output_dir} is not a valid directory.")
        sys.exit(1)

    edgelist_to_metis(input_file, output_dir)