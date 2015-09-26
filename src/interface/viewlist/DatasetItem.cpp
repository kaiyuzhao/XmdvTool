/*
 * DatasetItem.cpp
 *
 *  Created on: Feb 23, 2010
 *      Author: Zaixian Xie
 */

#include "interface/viewlist/DatasetItem.h"

#include <QStringList>
#include <QIcon>

#include "interface/viewlist/HandleViewList.h"
#include "data/Data.h"


DatasetItem::DatasetItem(QTreeWidget * parent, const QString & str, Data* dataset)
	: QTreeWidgetItem(parent, QStringList(str),  DatasetItemType) {
	m_dataset = dataset;
	setIcon( 0, ICON_DB );
}

DatasetItem::~DatasetItem() {
}

Data* DatasetItem::getDataset() {
	return m_dataset;
}
