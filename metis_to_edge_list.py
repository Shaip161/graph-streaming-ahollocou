import os
def metis_to_edgelist(input_file, output_file):
    with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
        # Read the first line containing the number of nodes and edges
        header = infile.readline().strip().split()
        num_nodes = int(header[0])
        num_edges = int(header[1])

        # Write the max node ID (n-1) to the output file
        outfile.write(f"{num_nodes - 1} {num_edges}\n")

        # Process each line for adjacency list and generate edges
        for node_idx, line in enumerate(infile):
            neighbors = map(int, line.strip().split())
            for neighbor in neighbors:
                # Convert to zero-based index and ensure edge order is preserved
                edge = (node_idx, neighbor - 1)  # Subtract 1 for zero-based indexing
                if edge[0] < edge[1]:  # To avoid duplicate edges
                    outfile.write(f"{edge[0]} {edge[1]}\n")

def process_metis_files(input_dir, output_dir):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    for filename in os.listdir(input_dir):
        if filename.endswith('.txt'):
            input_file = os.path.join(input_dir, filename)
            output_file = os.path.join(output_dir, os.path.splitext(filename)[0] + '.txt')
            metis_to_edgelist(input_file, output_file)
            print(f"Converted {input_file} to {output_file}")

# Example usage
input_directory = "graphs"  # Replace with your input directory path
output_directory = "edge_graphs"  # Replace with your output directory path
process_metis_files(input_directory, output_directory)