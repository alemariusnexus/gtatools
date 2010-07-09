/*
 * IDEStatement.h
 *
 *  Created on: 09.04.2010
 *      Author: alemariusnexus
 */

#ifndef IDESTATEMENT_H_
#define IDESTATEMENT_H_

#include "../gf_config.h"

typedef char idetype_t;


class IDEStatement {
public:
	virtual idetype_t getType() const = 0;
};

#endif /* IDESTATEMENT_H_ */
