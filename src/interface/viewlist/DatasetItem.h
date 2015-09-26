/*
 * DatasetItem.h
 *
 *  Created on: Feb 23, 2010
 *      Author: Zaixian Xie
 */

/*
 * The class DatasetItem is a item in the tree widget corresponding to
 * a dataset node.  It is a subclass of QTreeWidgetItem.
 */

#ifndef DATASETITEM_H_
#define DATASETITEM_H_

#include <QTreeWidgetItem>

class Data;

class DatasetItem : public QTreeWidgetItem {
public:
	DatasetItem(QTreeWidget * parent, const QString & str, Data* dataset);
	virtual ~DatasetItem();

private:
	// The index of the dataset in the vector m_origDatasets in pipeline manager
	Data* m_dataset;

public:
	Data* getDataset();

};

#endif /* DATASETITEM_H_ */
