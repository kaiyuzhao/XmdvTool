/*
 * CardStorage.h
 *
 *  Created on: Apr 7, 2010
 *      Author: Zaixian Xie
 */

#ifndef CARDSTORAGE_H_
#define CARDSTORAGE_H_

/* Class CardStorage stores the OkcData cardinality as
 * a separte OkcData.  This OkcData contains only one datapoint
 * containing the float representation of the cardinality
 *
 *	 The function createCardStorageFromOkcData can convert the cardinality to an OkcData.
 *	 This will be the input for an assistant transformation.
 *	 When users changed the cardinality in the final view,
 *	 we will call the function updateAssisInput() of the assistant transformation,
 *	 so the CardinalityStorage will be changed as well.
 *	 And then we will call the function updateCardinalityByStorage() in the original OkcData
 *	 to update the array cardinality.  This procedure ensures that the users' change
 *	 will not be reset when the pipeline is reassembled.
 *
 */

#include "data/multidim/OkcData.h"

class OkcData;

class CardStorage : public OkcData {
public:
	CardStorage();
	CardStorage( OkcData* storage);
	CardStorage( CardStorage* storage);
	virtual ~CardStorage();

public:
	// Convert the cardinality in an OkcData to a CardStorage
	static CardStorage* createCardStorageFromOkcData(OkcData* okcdata);
	// Convert an int ARR to a CardStorage
	static CardStorage* createCardStorageFromARR(std::vector<int> card);
	// Convert the storage to array
	std::vector<int> toARR();
};

#endif /* CARDSTORAGE_H_ */
