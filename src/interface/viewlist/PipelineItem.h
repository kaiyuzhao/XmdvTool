/*
 * PipelineItem.h
 *
 *  Created on: Feb 23, 2010
 *      Author: xiezx
 */

/*
 * The class PipelineItem is a item in the tree widget corresponding to
 * a pipeline node.  It is a subclass of QTreeWidgetItem.
 */

#ifndef PIPELINEITEM_H_
#define PIPELINEITEM_H_

#include <QTreeWidgetItem>

class Pipeline;

class PipelineItem : public QTreeWidgetItem {
public:
	PipelineItem(QTreeWidget * parent, const QString & str, Pipeline* pipeline);
	virtual ~PipelineItem();

private:
	// The pipeline ID associated with this pipeline item
	Pipeline* m_pipeline;

public:
	Pipeline* getPipeline();
};

#endif /* PIPELINEITEM_H_ */
