#include "VisualMapResult.h"
#include "data/Data.h"

VisualMapResult::VisualMapResult() {
	m_data = 0;
};

VisualMapResult::VisualMapResult(Data* data) {
	m_data = data;
};

VisualMapResult::~VisualMapResult() {
};

void VisualMapResult::setData(Data* data) {
	m_data = data;
}

Data* VisualMapResult::getData() {
	return m_data;
}
