/*
 * ReadCfCgException.cpp
 *
 *  Created on: Jun 4, 2009
 *      Author: Zaixian Xie
 */

#include "util/exception/ReadCfCgException.h"

ReadCfCgException::ReadCfCgException() throw() {
}

ReadCfCgException::ReadCfCgException(string strError)  throw() : exception(){
    m_strError = strError;
}

ReadCfCgException::~ReadCfCgException() throw() {
}

const char* ReadCfCgException::what() const throw() {
	return m_strError.c_str();
}
