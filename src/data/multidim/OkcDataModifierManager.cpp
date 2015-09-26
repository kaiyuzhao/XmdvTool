/*
 * OkcDataModifierManager.cpp
 *
 *  Created on: Feb 6, 2009
 *      Author: Zaixian Xie
 */

#include "data/multidim/OkcDataModifierManager.h"
#include "data/multidim/OkcDataModifier.h"
#include "data/multidim/OkcDataModifierDimOOO.h"
#include "data/multidim/OkcDataModifierHighlight.h"
#include "data/multidim/OkcDataModifierRowIndex.h"
#include "data/cluster/OkcDataModifierClusterNodeMax.h"
#include "data/cluster/OkcDataModifierClusterNodeMin.h"
#include "data/cluster/OkcDataModifierClusterColor.h"
#include "data/cluster/OkcDataModifierClusterHighlight.h"
#include "data/multidim/OkcDataModifierPCADerive.h"
#include "data/multidim/diagonal/HistogramModifier.h"
#include "data/multidim/diagonal/RowIndexOnAllDimModifier.h"
#include "data/cluster/OkcDataModifierClusterEntries.h"
#include "main/XmdvTool.h"
#include <map>
using namespace std;

OkcDataModifierManager::OkcDataModifierManager() {
	m_okcdata = 0;
}

OkcDataModifierManager::OkcDataModifierManager(OkcData* okcdata) {
	m_okcdata = okcdata;
}

OkcDataModifierManager::~OkcDataModifierManager() {
	map<XmdvTool::MODIFIERTYPE, OkcDataModifier*>::iterator it;
	// release the memory occupied by all of modifiers
	for ( it=m_modifiers.begin(); it != m_modifiers.end(); it++ ) {
		// We only release the memory occupied by those non-reference modifier.
		if ( !m_modifierRefFlag[ (*it).first ] ) {
			delete ((*it).second);
		}
	}

}

OkcDataModifier* OkcDataModifierManager::getOkcDataModifier(XmdvTool::MODIFIERTYPE modifierType) {
	if (m_modifiers.find(modifierType)==m_modifiers.end()) {
		// the specified modifier type does not exist
		return 0;
	} else {
		return m_modifiers[modifierType];
	}
}

void OkcDataModifierManager::addOkcDataModifier(XmdvTool::MODIFIERTYPE modifierType,
		OkcDataModifier* modifier) {
	// This action is allowed only if the specified modifier does not exist
	assert(m_modifiers.find(modifierType)==m_modifiers.end());
	m_modifiers[modifierType] = modifier;
	// By default, the new modifier is not a reference
	m_modifierRefFlag[modifierType] = false;
}


void OkcDataModifierManager::setOkcData(OkcData* okcdata){
	m_okcdata = okcdata;
}

OkcData* OkcDataModifierManager::getOkcData() {
	return m_okcdata;
}

int OkcDataModifierManager::getDataSize(int data_size) {
	return data_size;
}

int OkcDataModifierManager::getDimSize(int dims) {
	OkcDataModifier* modifier = getOkcDataModifier(XmdvTool::MODIFIER_DIMOOO);
	if (modifier) {
		// If the modifier exists, call its map function
		OkcDataModifierDimOOO* modifierDimOOO = (OkcDataModifierDimOOO*)modifier;
		return modifierDimOOO->getDimSize();
	} else {
		return dims;
	}
}

int OkcDataModifierManager::getOrigLine(int line) {
	OkcDataModifier* modifier = getOkcDataModifier(XmdvTool::MODIFIER_ROWINDEX);
	if (modifier) {
		// If the modifier exists, call its map function
		OkcDataModifierRowIndex* modifierRowIndex = (OkcDataModifierRowIndex*)modifier;
		return modifierRowIndex->getRowIndex(line);
	} else {
		return line;
	}
}
/*

double OkcDataModifierManager::getComDist(int line) {
	OkcDataModifier* modifier = getOkcDataModifier(XmdvTool::MODIFIER_COMDIST);
	if (modifier) {
		// If the modifier exists, call its map function
		OkcDataModifierComDist* modifierComDist = (OkcDataModifierComDist*)modifier;
		return modifierComDist->getComDist(line);
	} else {
		return 1;
	}
}

double OkcDataModifierManager::getComDistMin() {
	OkcDataModifier* modifier = getOkcDataModifier(XmdvTool::MODIFIER_COMDIST);
	if (modifier) {
		// If the modifier exists, call its map function
		OkcDataModifierComDist* modifierComDist = (OkcDataModifierComDist*)modifier;
		return modifierComDist->getDistMin();
	} else {
		return 1;
	}
}
double OkcDataModifierManager::getComDistMax() {
	OkcDataModifier* modifier = getOkcDataModifier(XmdvTool::MODIFIER_COMDIST);
	if (modifier) {
		// If the modifier exists, call its map function
		OkcDataModifierComDist* modifierComDist = (OkcDataModifierComDist*)modifier;
		return modifierComDist->getDistMax();
	} else {
		return 1;
	}
}
double OkcDataModifierManager::getComDistBrushLevel() {
	OkcDataModifier* modifier = getOkcDataModifier(XmdvTool::MODIFIER_COMDIST);
	if (modifier) {
		// If the modifier exists, call its map function
		OkcDataModifierComDist* modifierComDist = (OkcDataModifierComDist*)modifier;
		return modifierComDist->getBrushLevel();
	} else {
		return 1;
	}
}
*/

void OkcDataModifierManager::copyFrom(const OkcDataModifierManager* copy) {
	m_okcdata = copy->m_okcdata;
	m_modifiers.clear();
	map<XmdvTool::MODIFIERTYPE, OkcDataModifier*>::iterator it;
	map<XmdvTool::MODIFIERTYPE, OkcDataModifier*> copymap;
	copymap = copy->m_modifiers;
	// copy each modifier
	for ( it=copymap.begin(); it!=copymap.end(); it++) {
		m_modifiers[ (*it).first ] = copymap[ (*it).first ];
		m_modifierRefFlag[ (*it).first ] = true;
/*

		switch ((*it).first) {
		case XmdvTool::MODIFIER_HIGHLIGHT :
		{
			OkcDataModifierHighlight* modifier = new OkcDataModifierHighlight();
			*modifier = *((OkcDataModifierHighlight*)copymap[XmdvTool::MODIFIER_HIGHLIGHT]);
			m_modifiers[XmdvTool::MODIFIER_HIGHLIGHT] = modifier;
		}
		break;
		case XmdvTool::MODIFIER_DIMOOO :
		{
			OkcDataModifierDimOOO* modifier = new OkcDataModifierDimOOO();
			*modifier = *((OkcDataModifierDimOOO*)copymap[XmdvTool::MODIFIER_DIMOOO]);
			m_modifiers[XmdvTool::MODIFIER_DIMOOO] = modifier;
		}
		break;
		case XmdvTool::MODIFIER_ROWINDEX :
		{
			OkcDataModifierRowIndex* modifier = new OkcDataModifierRowIndex();
			*modifier = *((OkcDataModifierRowIndex*)copymap[XmdvTool::MODIFIER_ROWINDEX]);
			m_modifiers[XmdvTool::MODIFIER_ROWINDEX] = modifier;
		}
		break;
		case XmdvTool::MODIFIER_COMDIST :
		{
			OkcDataModifierComDist* modifier = new OkcDataModifierComDist();
			*modifier = *((OkcDataModifierComDist*)copymap[XmdvTool::MODIFIER_COMDIST]);
			m_modifiers[XmdvTool::MODIFIER_COMDIST] = modifier;
		}
		break;
		case XmdvTool::MODIFIER_CLUSTERNODEMAX :
		{
			OkcDataModifierClusterNodeMax* modifier = new OkcDataModifierClusterNodeMax();
			*modifier = *((OkcDataModifierClusterNodeMax*)copymap[XmdvTool::MODIFIER_CLUSTERNODEMAX]);
			m_modifiers[XmdvTool::MODIFIER_CLUSTERNODEMAX] = modifier;
		}
		case XmdvTool::MODIFIER_CLUSTERNODEMIN :
		{
			OkcDataModifierClusterNodeMin* modifier = new OkcDataModifierClusterNodeMin();
			*modifier = *((OkcDataModifierClusterNodeMin*)copymap[XmdvTool::MODIFIER_CLUSTERNODEMIN]);
			m_modifiers[XmdvTool::MODIFIER_CLUSTERNODEMIN] = modifier;
		}
		break;
		case XmdvTool::MODIFIER_CLUSTERCOLOR :
		{
			OkcDataModifierClusterColor* modifier = new OkcDataModifierClusterColor();
			*modifier = *((OkcDataModifierClusterColor*)copymap[XmdvTool::MODIFIER_CLUSTERCOLOR]);
			m_modifiers[XmdvTool::MODIFIER_CLUSTERCOLOR] = modifier;
		}
		break;
		case XmdvTool::MODIFIER_CLUSTERHIGHLIGHT :
		{
			OkcDataModifierClusterHighlight* modifier = new OkcDataModifierClusterHighlight();
			*modifier = *((OkcDataModifierClusterHighlight*)copymap[XmdvTool::MODIFIER_CLUSTERHIGHLIGHT]);
			m_modifiers[XmdvTool::MODIFIER_CLUSTERHIGHLIGHT] = modifier;
		}
		break;
		case XmdvTool::MODIFIER_HISTOGRAM :
		{
			HistogramModifier* modifier = new HistogramModifier();
			*modifier = *((HistogramModifier*)copymap[XmdvTool::MODIFIER_HISTOGRAM]);
			m_modifiers[XmdvTool::MODIFIER_HISTOGRAM] = modifier;
		}
		break;
		case XmdvTool::MODIFIER_ALLDIMROWINDEX :
		{
			RowIndexOnAllDimModifier* modifier = new RowIndexOnAllDimModifier();
			*modifier = *((RowIndexOnAllDimModifier*)copymap[XmdvTool::MODIFIER_ALLDIMROWINDEX]);
			m_modifiers[XmdvTool::MODIFIER_ALLDIMROWINDEX] = modifier;
		}
		break;
		case XmdvTool::MODIFIER_CLUSTERENTRIES :
		{
			OkcDataModifierClusterEntries* modifier = new OkcDataModifierClusterEntries();
			*modifier = *((OkcDataModifierClusterEntries*)copymap[XmdvTool::MODIFIER_CLUSTERENTRIES]);
			m_modifiers[XmdvTool::MODIFIER_CLUSTERENTRIES] = modifier;
		}
		break;
		case XmdvTool::MODIFIER_PCADERIVE :
		{
			OkcDataModifierPCADerive* modifier = new OkcDataModifierPCADerive();
			*modifier = *((OkcDataModifierPCADerive*)copymap[XmdvTool::MODIFIER_PCADERIVE]);
			m_modifiers[XmdvTool::MODIFIER_PCADERIVE] = modifier;
		}
		break;
		default:
			break;
		};
*/
	}
}

void OkcDataModifierManager::operator=(const OkcDataModifierManager& copy) {
	copyFrom(&copy);
}
