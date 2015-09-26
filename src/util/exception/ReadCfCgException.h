/*
 * ReadCfCgException.h
 *
 *  Created on: Jun 4, 2009
 *      Author: Zaixian Xie
 */

/*
 * This class represents the exception
 * during reading Cf or Cg files
 */

#ifndef READCFCGEXCEPTION_H_
#define READCFCGEXCEPTION_H_

#include <exception>
#include <string>
using namespace std;

class ReadCfCgException : public exception {
public:
	virtual ~ReadCfCgException() throw();

private:
	ReadCfCgException() throw(); // not meant to be called
	string m_strError;

public:
	ReadCfCgException(string strError) throw();
	const char* what() const throw();
};

#endif /* READCFCGEXCEPTION_H_ */
