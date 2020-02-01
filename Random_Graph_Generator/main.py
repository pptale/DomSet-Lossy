import networkx as nx
import random as random

def random_graph_gnm(nr_graphs):
    """Creates `nr_graphs' many files each containing a graph generated using a function gnm_random_graph 
    
    Following two are selected randomly from the specified range
    nr_nodes = number of nodes in graph
    exp_avg_density = expected average density in the graph
    
    [1] https://en.wikipedia.org/wiki/Erd%C5%91s%E2%80%93R%C3%A9nyi_model
    """
    range_start_nr_nodes = 30000
    range_ends_nr_nodes = 50000
    range_starts_exp_avg_density = 6
    range_ends_exp_avg_density = 20

    for id in range(1, nr_graphs + 1):
        nr_nodes = random.randrange(range_start_nr_nodes, range_ends_nr_nodes, 2) # To keep nr_nodes * exp_ave_density even
        exp_avg_density = random.randrange(range_starts_exp_avg_density, range_ends_exp_avg_density)

        g = nx.gnm_random_graph(nr_nodes, nr_nodes * exp_avg_density/ 2.0)
        filename = "gnm"+"_V" + str(g.number_of_nodes()) + "_E" + str(g.number_of_edges()) + "_ID" + str(id) + ".mtx"
        print filename
        f = open(filename , 'wb')
        f.write("%%MatrixMarket matrix coordinate pattern symmetric\n")
        ##f.write("%%MatrixMarket matrix coordinate real symmetric\n")
        f.write("%d %d %d \n" % (g.number_of_nodes(), g.number_of_nodes(), g.number_of_edges()))
        for edge in g.edges_iter():
            # Add +1 to indices as expected by c code
            f.write("%d %d \n" %(edge[0] + 1, edge[1] + 1))
        f.close()

random_graph_gnm(100)

def random_graph_regular(nr_graphs):
    """ Creates `nr_graphs' many files each containing a graph generated using a function random_regular_graph(d, n, seed=None) 
    
    Following two are selected randomly from the specified range
    nr_nodes = number of nodes in graph
    degree = degree of graph.
    """
    range_start_nr_nodes = 30000
    range_ends_nr_nodes = 50000
    range_starts_degree = 3
    range_ends_degree = 20
    
    for id in range(1, nr_graphs + 1):
        nr_nodes = random.randrange(range_start_nr_nodes, range_ends_nr_nodes, 2) # This is to make sure that nr_nodes * degree is always even
        degree = random.randrange(range_starts_degree, range_ends_degree)
        g = nx.random_regular_graph(degree, nr_nodes)
        filename = "regular"+"_V" + str(g.number_of_nodes()) + "_E" + str(g.number_of_edges()) + "_ID" + str(id) + ".mtx"
        print filename
        f = open(filename , 'wb')
        f.write("%%MatrixMarket matrix coordinate pattern symmetric\n")
        ##f.write("%%MatrixMarket matrix coordinate real symmetric\n")
        f.write("%d %d %d \n" % (g.number_of_nodes(), g.number_of_nodes(), g.number_of_edges()))
        for edge in g.edges_iter():
            # Add +1 to indices as expected by c code
            f.write("%d %d \n" %(edge[0] + 1, edge[1] + 1))
        f.close()

#random_graph_regular(100)

def random_graph_barabasi_albert(nr_graphs):
    """ Creates `nr_graphs' many files each containing a graph generated using a function barabasi_albert_graph(n, m, seed=None) 
    
    Following two are selected randomly from the specified range
    nr_nodes = number of nodes in graph
    nr_edges_add = Number of edges to attach from a new node to existing nodes
    """
    range_start_nr_nodes = 1000
    range_ends_nr_nodes = 5000
    range_starts_edges_add = 3
    range_ends_edges_add = 10
    
    for id in range(1, nr_graphs + 1):
        nr_nodes = random.randrange(range_start_nr_nodes, range_ends_nr_nodes) 
        nr_edges_add = random.randrange(range_starts_edges_add, range_ends_edges_add)
        g = nx.barabasi_albert_graph(nr_nodes, nr_edges_add)
        filename = "barabasi_albert"+"_V" + str(g.number_of_nodes()) + "_E" + str(g.number_of_edges()) + "_ID" + str(id) + ".mtx"
        #print filename
        f = open(filename , 'wb')
        f.write("%%MatrixMarket matrix coordinate pattern symmetric\n")
        ##f.write("%%MatrixMarket matrix coordinate real symmetric\n")
        f.write("%d %d %d \n" % (g.number_of_nodes(), g.number_of_nodes(), g.number_of_edges()))
        for edge in g.edges_iter():
            # Add +1 to indices as expected by c code
            f.write("%d %d \n" %(edge[0] + 1, edge[1] + 1))
        f.close()

#random_graph_barabasi_albert(100)

def random_graph_watts_strogatz(nr_graphs):
    """ Creates `nr_graphs' many files each containing a graph generated using a function watts_strogatz_graph(n, k, p, seed=None)  
    
    Following two are selected randomly from the specified range
    nr_nodes = number of nodes in graph
    nr_edges_add = Number of edges to attach from a new node to existing nodes
    nr_nodes -- The number of nodes
    adj_nbr -- Each node is joined with its adj_nbr nearest neighbors in a ring topology.
    prob -- The probability of rewiring each edge (higher p leads to random graphs)
    """
    range_start_nr_nodes = 30000
    range_ends_nr_nodes = 50000
    range_starts_adj_nbr = 3
    range_ends_adj_nbr = 20
    range_prob = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
    
    for id in range(1, nr_graphs + 1):
        nr_nodes = random.randrange(range_start_nr_nodes, range_ends_nr_nodes) 
        adj_nbr = random.randrange(range_starts_adj_nbr, range_ends_adj_nbr)
        prob = random.choice(range_prob)
        g = nx.watts_strogatz_graph(nr_nodes, adj_nbr, prob)
        filename = "watts_strogatz"+"_V" + str(g.number_of_nodes()) + "_E" + str(g.number_of_edges()) + "_ID" + str(id) + ".mtx"
        print filename
        #print g.number_of_nodes(), "\t", g.number_of_edges(), "\t", adj_nbr, "\t", prob

        f = open(filename , 'wb')
        f.write("%%MatrixMarket matrix coordinate pattern symmetric\n")
        ##f.write("%%MatrixMarket matrix coordinate real symmetric\n")
        f.write("%d %d %d \n" % (g.number_of_nodes(), g.number_of_nodes(), g.number_of_edges()))
        for edge in g.edges_iter():
            # Add +1 to indices as expected by c code
            f.write("%d %d \n" %(edge[0] + 1, edge[1] + 1))
        f.close()

#random_graph_watts_strogatz(100)

def random_graph_erdos_renyi(nr_graphs):
    """ Creates `nr_graphs' many files each containing a graph generated using a function erdos_renyi_graph(n, p, seed=None, directed=False)
    or fast_gnp_random_graph(n, p, seed=None, directed=False)

    Following two are selected randomly from the specified range
    nr_nodes = number of nodes in graph
    exp_avg_density = expected average density in the graph
    
    prob of adding an edge is set at 2 * exp_ave_density / nr_nodes
    """
    range_start_nr_nodes = 30000
    range_ends_nr_nodes = 50000
    range_starts_exp_avg_density = 6
    range_ends_exp_avg_density = 20

    for id in range(1, nr_graphs + 1):
        nr_nodes = random.randrange(range_start_nr_nodes, range_ends_nr_nodes) 
        exp_ave_density = random.randrange(range_starts_exp_avg_density, range_ends_exp_avg_density)
        prob = 1.0 * exp_ave_density / nr_nodes
        #g = nx.erdos_renyi_graph(nr_nodes, prob)
        g = nx.fast_gnp_random_graph(nr_nodes, prob)
        filename = "erdos_renyi"+"_V" + str(g.number_of_nodes()) + "_E" + str(g.number_of_edges()) + "_ID" + str(id) + ".mtx"
        print filename
        #print g.number_of_nodes(), "\t", g.number_of_edges(), "\t", adj_nbr, "\t", prob
        f = open(filename , 'wb')
        f.write("%%MatrixMarket matrix coordinate pattern symmetric\n")
        ##f.write("%%MatrixMarket matrix coordinate real symmetric\n")
        f.write("%d %d %d \n" % (g.number_of_nodes(), g.number_of_nodes(), g.number_of_edges()))
        for edge in g.edges_iter():
            # Add +1 to indices as expected by c code
            f.write("%d %d \n" %(edge[0] + 1, edge[1] + 1))
        f.close()


random_graph_erdos_renyi(100)
