import networkx as nx
import argparse
from community import modularity
from community import community_louvain

def read_metis_graph(file_path):
    """Reads a graph in METIS format and returns a NetworkX graph."""
    with open(file_path, 'r') as f:
        lines = f.readlines()

    # Parse the first line to get the number of nodes and edges
    first_line = lines[0].strip()
    num_nodes, num_edges = map(int, first_line.split()[:2])
    print("Input num nodes = ", num_nodes)
    print("Input num edges = ", num_edges)
    # Create an empty graph
    G = nx.Graph()

    # Add nodes
    G.add_nodes_from(range(1, num_nodes + 1))

    # Parse the remaining lines to add edges
    for i, line in enumerate(lines[1:]):
        neighbors = list(map(int, line.strip().split()))
        for neighbor in neighbors:
            G.add_edge(i + 1, neighbor)

    return G

def read_cluster_assignments(file_path):
    """Reads the cluster assignments and returns a dictionary mapping nodes to clusters."""
    with open(file_path, 'r') as f:
        lines = f.readlines()

    cluster_assignments = {i + 1: int(line.strip()) for i, line in enumerate(lines)}
    return cluster_assignments

def calculate_modularity(G, cluster_assignments):
    """Calculates the modularity of the clustering."""
    m = G.size(weight='weight')  # Total weight of edges
    degrees = dict(G.degree(weight='weight'))

    modularity = 0.0
    for u, v, data in G.edges(data=True):
        weight = data.get('weight', 1.0)
        if cluster_assignments[u] == cluster_assignments[v]:
            modularity += weight - (degrees[u] * degrees[v]) / (2 * m)

    modularity /= (2 * m)
    return modularity

def main():
    parser = argparse.ArgumentParser(description="Calculate modularity of a clustering for a graph in METIS format.")
    parser.add_argument("graph_file", type=str, help="Path to the METIS graph file.")
    parser.add_argument("cluster_file", type=str, help="Path to the cluster assignment file.")

    args = parser.parse_args()

    # Read the graph and cluster assignments
    G = read_metis_graph(args.graph_file)
    cluster_assignments = read_cluster_assignments(args.cluster_file)

    # Calculate modularity
    #modularity_self = calculate_modularity(G, cluster_assignments)
    #print(f"The modularity of the clustering is: {modularity_self:.4f}")
    
    # Calculate modularity using NetworkX
    modularity_score = community_louvain.modularity(cluster_assignments, G)
    print(f"NX: The modularity of the clustering is: {modularity_score:.4f}")

if __name__ == "__main__":
    main()