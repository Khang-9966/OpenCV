#ifndef HIERCLUSTER_H
#define HIERCLUSTER_H
#include <vector>

class HierClusterNode {
public:
    HierClusterNode();
    ~HierClusterNode();

public:
    int m_helper;
    int m_numVals;
    int m_leafIndex;
    std::vector<HierClusterNode*> m_subClusters;
    
    void output(const std::vector<std::vector<double> >& vals);
    void output(int level, const std::vector<std::vector<double> >& vals);
};

class HierCluster {
public:
    enum eMergeMethod {
        SINGLE_LINK,
        COMPLETE_LINK,
        AVERAGE_LINK
    };

    HierCluster(const std::vector<std::vector<double> >& vals, eMergeMethod method);
    ~HierCluster();
    HierClusterNode* getRoot() {
        return m_root; }
    int distance(int index1, int index2) {
        return m_dist[index1][index2]; }

private:
    void computeDistances();
    int distance(HierClusterNode* node, int index1, int index2);

private:
    HierClusterNode* m_root;
    std::vector<std::vector<int> > m_dist;

    static void propogate(HierClusterNode* root, HierClusterNode** list, HierClusterNode* newVal);
};
#endif