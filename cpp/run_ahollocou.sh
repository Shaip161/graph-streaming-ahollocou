#!/bin/bash

# Directory containing graph files
graph_dir="../edge_graphs/" # Change this to your actual directory containing graph files
output_dir="results/"
metis_dir="../graphs/"
VmaxStart=15
VmaxEnd=15
mkdir -p "$output_dir"

# Loop over each graph file in the directory
for graph_file in "$graph_dir"*; do
    # Extract the base name of the graph file
    graph_name=$(basename "$graph_file" | sed 's/\.[^.]*$//')

    echo "Processing graph: $graph_name"

    # Step 1: Run the streamcom command
    build/./streamcom -f "$graph_file" -o "dummy" --vmax-start "$VmaxEnd" --vmax-end "$VmaxStart" --output_path "$output_dir"

    # Step 2: Convert the clustering file
    python3 cluster_file_converter.py "$output_dir$graph_name.txt" "$output_dir${graph_name}_vieclus.txt"

    # Step 3: Run the VieClus evaluator
    evaluator_output=$(build/extern/VieClus/./evaluator "$metis_dir$graph_name.txt" --input_partition="$output_dir${graph_name}_vieclus.txt")

    # Step 4: Extract the new score from the evaluator output
    new_score=$(echo "$evaluator_output" | grep -oP '\b\d+\.\d+\b')

    echo "Extracted score for $graph_name: $new_score"

    # Step 5: Add the cluster score to the binary file
    python3 add_cluster_score.py "$output_dir$graph_name.bin" "$output_dir$graph_name.bin" "$new_score"

done

echo "Processing complete."
