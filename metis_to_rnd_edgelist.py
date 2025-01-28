import sys
import os
import random


def metis_to_edgelist(metis_file, output_dir):
    with open(metis_file, 'r') as infile:
        lines = infile.readlines()

    # Read the first line to get the number of nodes and edges
    first_line = lines[0].strip().split()
    num_nodes = int(first_line[0])
    num_edges = int(first_line[1])

    edges = set()  # Use a set to store unique edges

    # Parse the rest of the file for neighbors
    for node_id, line in enumerate(lines[1:], start=1):
        neighbors = map(int, line.strip().split())
        for neighbor in neighbors:
            # Add edge in a consistent (smaller, larger) order to ensure undirected edges are unique
            edges.add(tuple(sorted((node_id - 1, neighbor - 1))))

    edges_list = list(edges)  # Convert the set to a list
    random.shuffle(edges_list)  # Shuffle the list
    shuffled_edges = set(edges_list)  # Convert back to a set if required 

    # Generate output file name based on the input file name
    base_filename = os.path.splitext(os.path.basename(metis_file))[0]
    edgelist_file = os.path.join(output_dir, f"{base_filename}.txt")

    # Write the output to the edge list file
    with open(edgelist_file, 'w') as outfile:
        # Write the number of nodes
        outfile.write(f"{num_nodes} {num_edges}\n")

        # Write each edge
        for edge in shuffled_edges:
            outfile.write(f"{edge[0]} {edge[1]}\n")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <input_metis_file> <output_directory>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_dir = sys.argv[2]

    if not os.path.isdir(output_dir):
        print(f"Error: {output_dir} is not a valid directory.")
        sys.exit(1)

    metis_to_edgelist(input_file, output_dir)
