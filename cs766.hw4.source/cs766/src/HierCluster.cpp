#include "HierCluster.h"
#include "Util.h"
#include <queue>
#include <list>
#include <iostream>
using namespace std;

class ClusterHelper {
public:
    ClusterHelper(double score, int index1, int index2) :
      m_score(score), m_index1(index1), m_index2(index2) {}
    double m_score;
    int m_index1;
    int m_index2;
    bool operator <(const ClusterHelper& rhs) const {
        return m_score > rhs.m_score; }
};

HierClusterNode::HierClusterNode() : m_leafIndex(-1), m_numVals(1), m_helper(0) {}

HierClusterNode::~HierClusterNode() {
    std::vector<HierClusterNode*>::iterator i;
    for (i = m_subClusters.begin(); i != m_subClusters.end(); ++i)  {
        delete *i;
    }
}

void HierClusterNode::output(const std::vector<std::vector<double> >& vals) {
    output(0, vals);
}

void HierClusterNode::output(int level, const std::vector<std::vector<double> >& vals) {
    if (m_leafIndex != -1) {
        for (int i=0; i<level; ++i)
            cout << " ";
        for (int i=0; i<(int)vals[m_leafIndex].size(); ++i)
            cout << vals[m_leafIndex][i] << " ";
        cout << endl;
    }
    for (int i=0; i<(int)m_subClusters.size(); ++i)
        m_subClusters[i]->output(level+1, vals);
}


HierCluster::~HierCluster() {
    delete m_root;
}

void HierCluster::propogate(HierClusterNode* root,
                            HierClusterNode** list,
                            HierClusterNode* newVal)
{
    std::vector<HierClusterNode*>::iterator i;
    for (i = root->m_subClusters.begin(); i != root->m_subClusters.end(); ++i)  {
        propogate(*i, list, newVal);
    }
    if (root->m_leafIndex != -1)
        list[root->m_leafIndex] = newVal;
}

HierCluster::HierCluster(const std::vector<std::vector<double> >& vals, eMergeMethod method) {
    if (method == SINGLE_LINK || method == COMPLETE_LINK) {
        std::priority_queue<ClusterHelper> pq;
        HierClusterNode** clusters = new HierClusterNode*[vals.size()];
        for (int i=0; i<(int)vals.size(); ++i) {
           clusters[i] = new HierClusterNode();
           clusters[i]->m_leafIndex = i;
        }
        for (int i=0; i<(int)vals.size(); ++i)
            for (int j=i+1; j<(int)vals.size(); ++j)
                pq.push(ClusterHelper(Util::distance(vals[i], vals[j]), i, j));

        while (pq.size() && clusters[0]->m_numVals < (int)vals.size()) {
            ClusterHelper top = pq.top();
            pq.pop();
            HierClusterNode *a = clusters[top.m_index1];
            HierClusterNode *b = clusters[top.m_index2];
            if (a != b) {
                if (method == SINGLE_LINK) {
                    // First pairwise score joining two clusters means we merge
                    HierClusterNode *newNode = new HierClusterNode();
                    newNode->m_numVals = a->m_numVals+b->m_numVals;
                    newNode->m_subClusters.push_back(a);
                    newNode->m_subClusters.push_back(b);
                    propogate(a, clusters, newNode);
                    propogate(b, clusters, newNode);
                }
            }
        }
        m_root = clusters[0];
        delete []clusters;
    }
    m_dist.resize(vals.size());
    for (int i=0; i<(int)m_dist.size(); ++i)
        m_dist[i].resize(vals.size());
    computeDistances();
}

void HierCluster::computeDistances() {
    int numVals = (int)m_dist.size();
    for (int i=0; i<numVals; ++i) {
        for (int j=0; j<numVals; ++j) {
            m_dist[i][j] = distance(m_root, i, j);
        }
    }
}

int HierCluster::distance(HierClusterNode* node, int index1, int index2) {
    if (node->m_subClusters.size() == 0) {
        if (node->m_leafIndex==index1 || node->m_leafIndex==index2)
            return -1;
        else
            return 0;
    } else {
        int valLeft = distance(node->m_subClusters[0], index1, index2);
        if (valLeft > 0) return valLeft;
        int valRight = distance(node->m_subClusters[1], index1, index2);
        if (valRight > 0) return valRight;
        if (valLeft < 0 && valRight < 0) return -(valLeft+valRight);
        if (valLeft == 0 && valRight == 0) return 0;
        return valLeft+valRight-1;
    }
}

