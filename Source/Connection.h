/******************************************************************************\
* Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/
#pragma once

#include "LeapC.h"
#include <iostream>
#include <string>
#include "Callbacks.h"
#include "CircularBuffer.h"

/*Have all the callbacks functors in a structure*/
struct Callbacks {
	connection_callback      on_connection;
	connection_callback      on_connection_lost;
	device_callback          on_device_found;
	device_lost_callback     on_device_lost;
	device_failure_callback  on_device_failure;
	policy_callback          on_policy;
	tracking_callback        on_frame;
	log_callback             on_log_message;
	config_change_callback   on_config_change;
	config_response_callback on_config_response;
	image_callback           on_image;
	point_mapping_change_callback on_point_mapping_change;
	head_pose_callback       on_head_pose;
};

class Connection
{
public:
	/* Client functions */
	Connection();
	~Connection();
	LEAP_CONNECTION * OpenConnection();
	void CloseConnection();
	void DestroyConnection();
	void setFrame(const LEAP_TRACKING_EVENT *frame);
	LEAP_TRACKING_EVENT* GetFrame(); //Used in polling example
	LEAP_DEVICE_INFO* GetDeviceProperties(); //Used in polling example
	const std::string ResultString(eLeapRS r);
	void millisleep(int milliseconds);

	/*a set of access/ set functions*/
	inline void setRunning(bool b) { isRunning = b; };
	inline void setConnect(bool b) { isConnected = b; };
	inline bool getConnect() { return isConnected; };
	inline bool getRunning() { return isRunning; };


	inline Callbacks & getCallbacks() { return connectionCallbacks; };
	inline LEAP_CONNECTION & getHandler() { return connectionHandle; };
	inline LEAP_DEVICE_INFO * getLastDevice() { return lastDevice; };
private:

	/* external tate */
	bool isConnected;
	/*internal state*/
	bool isRunning;
	struct Callbacks connectionCallbacks;
	LEAP_CONNECTION connectionHandle;
	LEAP_TRACKING_EVENT * lastFrame;
	LEAP_DEVICE_INFO * lastDevice;
	CircularBuffer buffer;


	void CloseConnectionHandle(LEAP_CONNECTION* connectionHandle);

	void handleConnectionEvent(const LEAP_CONNECTION_EVENT *connection_event);
	void handleConnectionLostEvent(const LEAP_CONNECTION_LOST_EVENT *connection_lost_event);
	void handleDeviceEvent(const LEAP_DEVICE_EVENT *device_event);
	void handleDeviceLostEvent(const LEAP_DEVICE_EVENT *device_event);
	void handleDeviceFailureEvent(const LEAP_DEVICE_FAILURE_EVENT *device_failure_event);
	void handleTrackingEvent(const LEAP_TRACKING_EVENT *tracking_event, CircularBuffer & buffer);
	void handleLogEvent(const LEAP_LOG_EVENT *log_event);
	void handleLogEvents(const LEAP_LOG_EVENTS *log_events);
	void handlePolicyEvent(const LEAP_POLICY_EVENT *policy_event);
	void handleConfigChangeEvent(const LEAP_CONFIG_CHANGE_EVENT *config_change_event);
	void handleConfigResponseEvent(const LEAP_CONFIG_RESPONSE_EVENT *config_response_event);
	void handleImageEvent(const LEAP_IMAGE_EVENT *image_event);
	void handlePointMappingChangeEvent(const LEAP_POINT_MAPPING_CHANGE_EVENT *point_mapping_change_event);
	void handleHeadPoseEvent(const LEAP_HEAD_POSE_EVENT *head_pose_event);
	void serviceMessageLoop(void * unused);
	static void __stdcall messageThread(void * p_this)
	{
		Connection * p_connect = static_cast<Connection *> (p_this);
		p_connect->serviceMessageLoop(NULL);
	}
};

