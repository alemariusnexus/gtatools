/*
 * IPLStatement.h
 *
 *  Created on: 18.04.2010
 *      Author: alemariusnexus
 */

#ifndef IPLSTATEMENT_H_
#define IPLSTATEMENT_H_

#include "../gf_config.h"

typedef char ipltype_t;


class IPLStatement {
public:
	virtual ipltype_t getType() const = 0;
};

#endif /* IPLSTATEMENT_H_ */
