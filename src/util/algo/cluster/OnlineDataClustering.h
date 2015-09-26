/*
 * OnlineDataClustering.h
 *
 *  Created on: Sep 16, 2009
 *      Author: mukherab
 */

#ifndef ONLINEDATACLUSTERING_H_
#define ONLINEDATACLUSTERING_H_

#include <stdio.h>
#include <list>

class LinkList;
class LinkListNode;
class ClusterNode;

using std::list;
using std::pair;

class OnlineDataClustering {
public:

	struct DataPair{
			LinkListNode *p1, *p2;
		    DataPair(LinkListNode *pointer1, LinkListNode *pointer2) : p1(pointer1), p2(pointer2) {}
		};

		struct ClusterNode{

		    int id;
		    int count;
		    int entries;
			double *data;
		    double radius;
			float clustqual, dataqual;
			char *name;
			ClusterNode *parent;
		    ~ClusterNode()
		    {
		        delete data;
		    }
		};


	OnlineDataClustering();
	virtual ~OnlineDataClustering();

	static bool GetInput(FILE * fp, int &dim, list<OnlineDataClustering::ClusterNode*> & data);
	static list<OnlineDataClustering::ClusterNode*>::iterator WhoIsMaximum( list<OnlineDataClustering::ClusterNode*> &data_link );
	static inline bool ClusterPredicate(ClusterNode* lhs, ClusterNode* rhs);
	static void Clustering(int n, list<ClusterNode*> & pointset, list<ClusterNode*> & clusterset, double min_distance);
	static void WriteOutput(FILE *fp, int dim, list<OnlineDataClustering::ClusterNode*> & data_link);
	static bool OnlineClusterFileToCfFile(const char* okc_file_name, const char* cf_file_name, int times);
	static void GetMaxAndMin(int size, int dimension, int k);
	static bool ReadCfFile(const char * okcFileName, const char * cfFileName);
	static bool WriteAsciiCgFile(const char *oldfile, const char *newfile);
	static bool WriteBinaryCgFile(const char *oldfile, const char *newfile);
	static bool WriteCgFile(const char *oldfile, const char *newfile, bool binary);
	static bool TransCfToCgFile(const char * okcFileName, const char * cfFileName, bool binary);
	static bool OnlineClusterOkcFileToCgFile(const char* okc_file_name, int repeat_times, bool binary);
};
#endif /* ONLINEDATACLUSTERING_H_ */
