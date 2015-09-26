/*
 * PipelineItem.cpp
 *
 *  Created on: Feb 23, 2010
 *      Author: Zaixian Xie
 */

#include "interface/viewlist/PipelineItem.h"

#include <QStringList>
#include <QIcon>

#include "interface/viewlist/HandleViewList.h"
#include "pipeline/Pipeline.h"

PipelineItem::PipelineItem(QTreeWidget * parent, const QString & str, Pipeline* pipeline)
	: QTreeWidgetItem(parent, QStringList(str),  PipelineItemType) {
	m_pipeline = pipeline;
	setIcon( 0, ICON_PL );
}

PipelineItem::~PipelineItem() {
}

Pipeline* PipelineItem::getPipeline() {
	return m_pipeline;
}

