/*
 * ErrorLog.h
 *
 *  Created on: 11.10.2010
 *      Author: alemariusnexus
 */

#ifndef ERRORLOG_H_
#define ERRORLOG_H_

#include <queue>

using std::queue;


class ErrorLog {
public:
	~ErrorLog();
	void log(const char* message);
	char* nextMessage();
	int getMessageCount() { return data.size(); }

private:
	queue<char*> data;
};

#endif /* ERRORLOG_H_ */
