/*
 * CardStorage.cpp
 *
 *  Created on: Apr 7, 2010
 *      Author: xiezx
 */

#include "data/storage/CardStorage.h"

CardStorage::CardStorage()
	: OkcData() {
}

CardStorage::CardStorage( OkcData* storage)
	: OkcData(storage) {
}

CardStorage::CardStorage( CardStorage* storage)
	: OkcData( (OkcData*)storage)  {

}

CardStorage::~CardStorage() {
}

CardStorage* CardStorage::createCardStorageFromOkcData(OkcData* okcdata) {
	int i;
	CardStorage* storage = new CardStorage();

	storage->setBaseFlag(true);
	int dimSize = okcdata->getDimSize();
	// This OkcData contains only one datapoint to
	// represent the cardinality for each dimension
	storage->data_size = 1;
	storage->dims = dimSize;
	storage->data_buf->resize(1*dimSize);
	// put all of brush attribute into the data_buf
	std::vector<double> card;
	card.resize(dimSize);
	for (i=0; i<dimSize; i++) {
		card[i] = (double)(okcdata->cardinality[i]);
	}
	storage->setOrigData(card, 0);

	// The names, cardinality, dim_min and dim_max
	// does not make sense in this storage
	storage->names.resize(dimSize);
	storage->cardinality.resize(dimSize);
	storage->dim_min.resize(dimSize);
	storage->dim_max.resize(dimSize);
	for (i=0; i<dimSize; i++) {
		storage->names[i] = 0;
		// This cardinality array is not the information we want to store.
		storage->cardinality[i] = 0;
		storage->dim_min[i] = 0.0;
		storage->dim_max[i] = 0.0;
	}

	return storage;

}

CardStorage* CardStorage::createCardStorageFromARR(std::vector<int> card) {
	int i;
	CardStorage* storage = new CardStorage();

	storage->setBaseFlag(true);
	int dimSize = card.size();
	// This OkcData contains only one datapoint to
	// represent the cardinality for each dimension
	storage->data_size = 1;
	storage->dims = dimSize;
	storage->data_buf->resize(1*dimSize);
	// The double representation of the cardinality
	std::vector<double> card_d;
	card_d.resize(dimSize);
	for (i=0; i<dimSize; i++) {
		card_d[i] = (double)card[i];
	}
	storage->setOrigData(card_d, 0);

	// The names, cardinality, dim_min and dim_max
	// does not make sense in this storage
	storage->names.resize(dimSize);
	storage->cardinality.resize(dimSize);
	storage->dim_min.resize(dimSize);
	storage->dim_max.resize(dimSize);
	for (i=0; i<dimSize; i++) {
		storage->names[i] = 0;
		// This cardinality array is not the information we want to store.
		storage->cardinality[i] = 0;
		storage->dim_min[i] = 0.0;
		storage->dim_max[i] = 0.0;
	}

	return storage;
}

std::vector<int> CardStorage::toARR() {
	std::vector<double> card_d;
	this->getData(card_d, 0);

	std::vector<int> card_i;
	int i, n = card_d.size();
	card_i.resize(n);
	for (i=0; i<n; i++) {
		card_i[i] = (int)(card_d[i]+0.1);
	}

	return card_i;

}
