/*
 * Cluster.cpp
 *
 *  Created on: Jun 4, 2009
 *      Author: Zaixian Xie
 */

#include "data/cluster/Cluster.h"

#include "main/XmdvTool.h"
#include "data/multidim/OkcData.h"
#include "data/multidim/OkcDataModifierManager.h"
#include "data/cluster/OkcDataModifierClusterNodeMax.h"
#include "data/cluster/OkcDataModifierClusterNodeMin.h"
#include "data/cluster/OkcDataModifierClusterColor.h"
#include "data/cluster/OkcDataModifierClusterHighlight.h"
#include "data/cluster/OkcDataModifierClusterEntries.h"

Cluster::Cluster() {
}

Cluster::~Cluster() {
}

void Cluster::addIntoOkcDataAsTuple(OkcData* okcdata, bool brushed) {
	int dim = okcdata->getOrigDimSize();
	int i;

	// insert the dimension values into the data buffer of the okcdata
	for (i=0; i<dim; i++) {
		okcdata->data_buf->push_back(this->mean[i]);
	}
	okcdata->data_size++;
	// the index is the order of current node in the okcdata
	//int index = okcdata->data_size-1;

	OkcDataModifierManager* mm = okcdata->getOkcDataModifierManager();

	// Add the maximum and minimum values for this cluster into the data modifier
	OkcDataModifierClusterNodeMax* modifierMax = dynamic_cast<OkcDataModifierClusterNodeMax*>
		(mm->getOkcDataModifier(XmdvTool::MODIFIER_CLUSTERNODEMAX));
	modifierMax->insertTail(this->max);

	OkcDataModifierClusterNodeMin* modifierMin = dynamic_cast<OkcDataModifierClusterNodeMin*>
		(mm->getOkcDataModifier(XmdvTool::MODIFIER_CLUSTERNODEMIN));
	modifierMin->insertTail(this->min);

	// Add the cluster color into a data modifier
	OkcDataModifierClusterColor* modifierColor = dynamic_cast<OkcDataModifierClusterColor*>
		(mm->getOkcDataModifier(XmdvTool::MODIFIER_CLUSTERCOLOR));
	modifierColor->insertTail(this->color);

	// Add the cluster highlight into a data modifier
	OkcDataModifierClusterHighlight* modifierCH = dynamic_cast<OkcDataModifierClusterHighlight*>
		(mm->getOkcDataModifier(XmdvTool::MODIFIER_CLUSTERHIGHLIGHT));
	modifierCH->insertTail(brushed);

	// Add the cluster entries into a data modifier
	OkcDataModifierClusterEntries* modifierCE = dynamic_cast<OkcDataModifierClusterEntries*>
		(mm->getOkcDataModifier(XmdvTool::MODIFIER_CLUSTERENTRIES));
	modifierCE->insertTail(this->entries);
}

