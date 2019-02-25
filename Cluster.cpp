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
		std::sort(nodes.begin(), nodes.end(), [](Node left, Node right)
		{
			return left.l > right.l;
		});

		//if biggest eigenvalue is less than threshold, clustering done
		if(nodes[0].l < minVar)
		{
			break;	//goto report clusters' mean and cov
		}

		//cluster pixel by comparing their inner product with eigenvecter
		//and inner product of mean and eigenvector
		Node& node = nodes[0];
		VectorXd we = node.w * node.e;
		int size = we.size(), acnt = 0, bcnt = 0;
		float me = node.mu.dot(node.e);
		VectorXd idx(size);	//indicate whether go to cluster a or b
		for (int i = 0; i < size; i++) {
			if (idx(i) = we(i) <= me) {
				acnt++;
			}
			else {
				bcnt++;
			}
		}
		ss << "idx:"<<idx<< std::endl<<"e:"<<node.e<< std::endl;
		if(acnt && bcnt)	//if able to cluster
		{
			MatrixXd Fa(acnt, 3), Fb(bcnt, 3);
			VectorXd wa(acnt), wb(bcnt);
			for(int i=0; i<size; i++)
			{
				if(idx(i))
				{
					Fa << node.m(i);
					wa << node.w(i);
				}
				else
				{
					Fb << node.m(i);
					wb << node.w(i);
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
	for(Node& n: nodes)
	{
		clusters.emplace_back(n.m, n.c);
	}
}


Cluster::~Cluster()
{
}
