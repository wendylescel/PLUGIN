#pragma once

#include <LeapC.h>
#include "CircularBuffer.h"

/*defining several objects used as callback functions*/
class connection_callback {
public:
	void operator()();
};

class device_callback {
public:
	void operator ()(const LEAP_DEVICE_INFO * device);
};

class device_lost_callback {
public:
	void operator ()();
};

class device_failure_callback {
public:
	void operator ()(const eLeapDeviceStatus failure_code,
		const LEAP_DEVICE failed_device);
};

class policy_callback {
public:
	void operator ()(const uint32_t current_policies);
};

class tracking_callback {
public:
	void operator()(const LEAP_TRACKING_EVENT *tracking_event, CircularBuffer & buffer);
};

class log_callback {
public:
	void operator()(const eLeapLogSeverity severity,
		const int64_t timestamp,
		const char* message);
};

class config_change_callback {
public:
	void operator()(const uint32_t requestID, const bool success);
};

class config_response__callback {
public:
	void operator()(const uint32_t requestID, LEAP_VARIANT value);
};

class image_callback {
public:
	void operator()(const LEAP_IMAGE_EVENT *image_event);
};

class point_mapping_change_callback {
public:
	void operator()(const LEAP_POINT_MAPPING_CHANGE_EVENT *point_mapping_change_even);
};

class head_pose_callback {
public:
	void operator()(const LEAP_HEAD_POSE_EVENT *head_pose_event);
};

class config_response_callback {
public:
	void operator()(const uint32_t requestID, const LEAP_VARIANT value);
};