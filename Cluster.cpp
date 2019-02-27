#include "Cluster.h"
#include "Bayesian.h"
#include <queue>

extern std::stringstream ss;

Cluster::Cluster(MatrixXd m, VectorXd w, double minVar)
{
	std::vector<Node> nodes;
	nodes.emplace_back(m, w);
	while(true)
	{
		//get node with biggest eigenvalue
		std::sort(nodes.begin(),nodes.end(),[](Node& a, Node& b)
		{
			return abs(a.l > b.l);
		});

		//if biggest eigenvalue is less than threshold, clustering done
		if(nodes[0].l < minVar)
		{
			break;	//goto report clusters' mean and cov
		}

		//cluster pixel by comparing their inner product with eigenvecter
		//and inner product of mean and eigenvector
		Node& node = nodes[0];
		VectorXd me = node.m*node.e;
		int size = node.m.rows(), acnt = 0, bcnt = 0;
		float mue = node.mu.dot(node.e);
		VectorXd idx(size);	//indicate whether go to cluster a or b
		for (int i = 0; i < size; i++) {
			if (idx(i) = me(i) <= mue) {
				acnt++;
			}
			else {
				bcnt++;
			}
		}
		if(acnt && bcnt)	//if able to cluster
		{
			MatrixXd Fa(acnt, 3), Fb(bcnt, 3);
			VectorXd wa(acnt), wb(bcnt);
			for(int i=0, a=0, b=0; i<size; i++)
			{
				if(idx(i))
				{
					Fa.row(a) = node.m.row(i);
					wa(a) = node.w(i);
					a++;
				}
				else
				{
					Fb.row(b) = node.m.row(i);
					wb(b) = node.w(i);
					b++;
				}
			}
			nodes.erase(nodes.begin());
			nodes.emplace_back(Fa, wa);
			nodes.emplace_back(Fb, wb);
		}else
		{
			break;	//cannot further cluster then goto report clusters' mean and cov
		}
	}
	// int index = 0;
	for(Node& n: nodes)
	{
		// ss << "cluster " << index++ << ":\n m\n" << n.m << "\nc\n" << n.c << std::endl;
		clusters.emplace_back(n.mu, n.c);
	}
}


Cluster::~Cluster()
{
}
