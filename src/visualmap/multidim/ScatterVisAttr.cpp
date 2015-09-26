/*
 * ScatterVisAttr.cpp
 *
 *  Created on: Aug 18, 2009
 *      Author: zyguo
 */

#include "ScatterVisAttr.h"

ScatterVisAttr::ScatterVisAttr() {
	size = 2;
	shape = 0;
	diagType = XmdvTool::DIAGPLOT_ORIGINAL;
	dataDrivenXDimNumber = 0;
	dataDrivenYDimNumber = 1;
	derivedXDimNumber = 0;
	derivedYDimNumber = 1;
	oneDimSortDim = 0;
}

ScatterVisAttr::~ScatterVisAttr() {
}

void ScatterVisAttr::setPointSize(int _size){
	size = _size;
}

int ScatterVisAttr::getPointSize(){
	return size;
}

void ScatterVisAttr::setShape(int _shape){
	shape = _shape;
}

int ScatterVisAttr::getShape(){
	return shape;
}

void ScatterVisAttr::setDiagType(XmdvTool::DIAGPLOT_MODE _diagType) {
	diagType = _diagType;
}

XmdvTool::DIAGPLOT_MODE ScatterVisAttr::getDiagType() {
	return  this->diagType;
}

void ScatterVisAttr::setDataDrivenXDimNumber(int _dataDrivenXDimNumber){
	dataDrivenXDimNumber = _dataDrivenXDimNumber;
}

int ScatterVisAttr::getDataDrivenXDimNumber(){
	return dataDrivenXDimNumber;
}

void ScatterVisAttr::setDataDrivenYDimNumber(int _dataDrivenYDimNumber){
	dataDrivenYDimNumber = _dataDrivenYDimNumber;
}

int ScatterVisAttr::getDataDrivenYDimNumber(){
	return dataDrivenYDimNumber;
}

void ScatterVisAttr::setDerivedXDimNumber(int _derivedXDimNumber){
	derivedXDimNumber = _derivedXDimNumber;
}

int ScatterVisAttr::getDerivedXDimNumber(){
	return derivedXDimNumber;
}

void ScatterVisAttr::setDerivedYDimNumber(int _derivedYDimNumber){
	derivedYDimNumber = _derivedYDimNumber;
}

int ScatterVisAttr::getDerivedYDimNumber(){
	return derivedYDimNumber;
}

void ScatterVisAttr::setOneDimSortDim(int sortDim){
	this->oneDimSortDim = sortDim;
}
int ScatterVisAttr::getOneDimSortDim(){
	return this->oneDimSortDim;
}
