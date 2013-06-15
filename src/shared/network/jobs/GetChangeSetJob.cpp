/*
 * Copyright 2007-2008, Ingo Weinhold <ingo_weinhold@gmx.de>
 * All rights reserved. Distributed under the terms of the GNU GPL v2.
 */

#include "GetChangeSetJob.h"

#include <stdio.h>
#include <string.h>

#include <Looper.h>

#include "RequestConnection.h"
#include "RequestMessageCodes.h"

#include "StatusOutput.h"

GetChangeSetJob::GetChangeSetJob(const char* changeSetID, BHandler* target,
		BMessage* message)
	: InformingJob(target, message),
	fChangeSetID(changeSetID)
{
}


GetChangeSetJob::~GetChangeSetJob()
{
}


status_t
GetChangeSetJob::Execute(JobConnection* jobConnection)
{
	RequestConnection* connection = jobConnection->GetRequestConnection();
	if (connection == NULL)
		return fError = ENOTCONN;

	StatusOutput* statusOutput = jobConnection->GetStatusOutput();

	// tell the user, that we're starting
	statusOutput->PrintInfoMessage(
		"GetChangeSetJob: Starting getting change set...\n");

	// prepare the request
	BMessage request(REQUEST_GET_CHANGE_SET);
	fError = request.AddString("change set id", fChangeSetID);
	if (fError != B_OK) {
		statusOutput->PrintErrorMessage(
			"GetChangeSetJob: Failed to add change set ID to message: %s\n",
			strerror(fError));
		return fError;
	}

	// send request
	BMessage reply;

	fError = connection->SendRequest(&request, &reply);
	if (fError != B_OK) {
		statusOutput->PrintErrorMessage(
			"GetChangeSetJob: Failed to send request: %s\n", strerror(fError));
		return fError;
	}
	fError = B_ERROR;

	// check error
	if (reply.what != REQUEST_GET_CHANGE_SET_REPLY) {
		if (const char* errorMessage = CheckErrorReply(&reply)) {
			statusOutput->PrintErrorMessage(
				"GetChangeSetJob: Request failed: %s\n", errorMessage);
			return fError = B_ERROR;
		} else {
			statusOutput->PrintErrorMessage(
				"GetChangeSetJob: Unexpected reply from server (0x%lx or %.4s).\n",
					reply.what, (char*)&reply.what);
			return fError = B_ERROR;
		}
	}

//	reply.PrintToStream();

	// inform our target
	BMessage message(reply.what);
	if (fMessage) {
		message = *fMessage;
	}
	message.AddMessage("reply", &reply);
	InformHandler(&message);

	// tell the user, that we're done
	statusOutput->PrintInfoMessage(
		"GetChangeSetJob: Getting change set done\n");

	return fError = B_OK;
}
