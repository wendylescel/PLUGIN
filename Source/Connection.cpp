#include "Connection.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if defined(_MSC_VER)
#include <Windows.h>
#include <process.h>
#define LockMutex EnterCriticalSection
#define UnlockMutex LeaveCriticalSection
#else
#include <unistd.h>
#include <pthread.h>
#define LockMutex pthread_mutex_lock
#define UnlockMutex pthread_mutex_unlock
#endif


//Forward declarations

static void setFrame(const LEAP_TRACKING_EVENT *frame) {};
static void setDevice(const LEAP_DEVICE_INFO *deviceProps) {};


//Threading variables
#if defined(_MSC_VER)
HANDLE pollingThread;
CRITICAL_SECTION dataLock;
#else
pthread_t pollingThread;
pthread_mutex_t dataLock;
#endif

Connection::Connection() : buffer(64)
{
	//External state
	isConnected = false;
	//Internal state
	isRunning = false;
	connectionHandle = NULL;
	lastFrame = new LEAP_TRACKING_EVENT();
	lastDevice = new LEAP_DEVICE_INFO();
}

Connection::~Connection()
{
	delete lastDevice;
	delete lastFrame;
	if (isRunning)
	{
		DestroyConnection();
	}
}

/**
* Creates the connection handle and opens a connection to the Leap Motion
* service. On success, creates a thread to service the LeapC message pump.
*/
LEAP_CONNECTION* Connection::OpenConnection() {
	if (isRunning) {
		return &connectionHandle;
	}
	if (connectionHandle || LeapCreateConnection(NULL, &connectionHandle) == eLeapRS_Success) {
		eLeapRS result = LeapOpenConnection(connectionHandle);
		if (result == eLeapRS_Success) {
			isRunning = true;
#if defined(_MSC_VER)
			InitializeCriticalSection(&dataLock);
			pollingThread = (HANDLE)_beginthreadex(0, 0, (_beginthreadex_proc_type)&Connection::messageThread, this, 0, 0);
#else
			pthread_create(&pollingThread, NULL, serviceMessageLoop, NULL);
#endif
		}
	}
	return &connectionHandle;
}

void Connection::CloseConnection() {
	if (!isRunning) {
		return;
	}
	isRunning = false;
	LeapCloseConnection(connectionHandle);
#if defined(_MSC_VER)
	WaitForSingleObject(pollingThread, INFINITE);
	CloseHandle(pollingThread);
#else
	pthread_join(pollingThread, NULL);
#endif
}

void Connection::DestroyConnection() {
	CloseConnection();
	LeapDestroyConnection(connectionHandle);
}


/** Close the connection and let message thread function end. */
void Connection::CloseConnectionHandle(LEAP_CONNECTION* connectionHandle) {
	LeapDestroyConnection(*connectionHandle);
	setRunning(false);
}

/** Called by serviceMessageLoop() when a connection event is returned by LeapPollConnection(). */
void Connection::handleConnectionEvent(const LEAP_CONNECTION_EVENT *connection_event) {
	setConnect(true);
	struct Callbacks & call = getCallbacks();
	call.on_connection();
}

/** Called by serviceMessageLoop() when a connection lost event is returned by LeapPollConnection(). */
void Connection::handleConnectionLostEvent(const LEAP_CONNECTION_LOST_EVENT *connection_lost_event) {
	setConnect(false);
	struct Callbacks & call = getCallbacks();
	call.on_connection_lost();
}

/**
* Called by serviceMessageLoop() when a device event is returned by LeapPollConnection()
* Demonstrates how to access device properties.
*/
void Connection::handleDeviceEvent(const LEAP_DEVICE_EVENT *device_event) {
	LEAP_DEVICE deviceHandle;
	//Open device using LEAP_DEVICE_REF from event struct.
	eLeapRS result = LeapOpenDevice(device_event->device, &deviceHandle);
	if (result != eLeapRS_Success) {
		std::cout << "Could not open device " << ResultString(result) << std::endl;
		return;
	}

	//Create a struct to hold the device properties, we have to provide a buffer for the serial string
	LEAP_DEVICE_INFO deviceProperties = { sizeof(deviceProperties) };
	// Start with a length of 1 (pretending we don't know a priori what the length is).
	// Currently device serial numbers are all the same length, but that could change in the future
	deviceProperties.serial_length = 1;
	deviceProperties.serial = (char*)malloc(deviceProperties.serial_length);
	//This will fail since the serial buffer is only 1 character long
	// But deviceProperties is updated to contain the required buffer length
	result = LeapGetDeviceInfo(deviceHandle, &deviceProperties);
	if (result == eLeapRS_InsufficientBuffer) {
		//try again with correct buffer size
		deviceProperties.serial = (char*)realloc(deviceProperties.serial, deviceProperties.serial_length);
		result = LeapGetDeviceInfo(deviceHandle, &deviceProperties);
		if (result != eLeapRS_Success) {
			std::cout << " to get device info " << ResultString(result) << std::endl;
			free(deviceProperties.serial);
			return;
		}
	}
	setDevice(&deviceProperties);
	struct Callbacks & call = getCallbacks();
	call.on_device_found(&deviceProperties);

	free(deviceProperties.serial);
	LeapCloseDevice(deviceHandle);
}

/** Called by serviceMessageLoop() when a device lost event is returned by LeapPollConnection(). */
void Connection::handleDeviceLostEvent(const LEAP_DEVICE_EVENT *device_event) {
	struct Callbacks & call = getCallbacks();
	call.on_device_lost();
}

/** Called by serviceMessageLoop() when a device failure event is returned by LeapPollConnection(). */
void Connection::handleDeviceFailureEvent(const LEAP_DEVICE_FAILURE_EVENT *device_failure_event) {
	struct Callbacks & call = getCallbacks();
	call.on_device_failure(device_failure_event->status, device_failure_event->hDevice);
}

/** Called by serviceMessageLoop() when a tracking event is returned by LeapPollConnection(). */
void Connection::handleTrackingEvent(const LEAP_TRACKING_EVENT *tracking_event, CircularBuffer & buff) {
	setFrame(tracking_event); //support polling tracking data from different thread
	struct Callbacks & call = getCallbacks();
	call.on_frame(tracking_event, buff);
}

/** Called by serviceMessageLoop() when a log event is returned by LeapPollConnection(). */
void Connection::handleLogEvent(const LEAP_LOG_EVENT *log_event) {
	struct Callbacks & call = getCallbacks();
	call.on_log_message(log_event->severity, log_event->timestamp, log_event->message);
}

/** Called by serviceMessageLoop() when a log event is returned by LeapPollConnection(). */
void Connection::handleLogEvents(const LEAP_LOG_EVENTS *log_events) {
	struct Callbacks & call = getCallbacks();
	for (int i = 0; i < (int)(log_events->nEvents); i++) {
		const LEAP_LOG_EVENT* log_event = &log_events->events[i];
		call.on_log_message(log_event->severity, log_event->timestamp, log_event->message);
	}
}

/** Called by serviceMessageLoop() when a policy event is returned by LeapPollConnection(). */
void Connection::handlePolicyEvent(const LEAP_POLICY_EVENT *policy_event) {
	struct Callbacks & call = getCallbacks();
	call.on_policy(policy_event->current_policy);
}

/** Called by serviceMessageLoop() when a config change event is returned by LeapPollConnection(). */
void Connection::handleConfigChangeEvent(const LEAP_CONFIG_CHANGE_EVENT *config_change_event) {
	struct Callbacks & call = getCallbacks();
	call.on_config_change(config_change_event->requestID, config_change_event->status);
}

/** Called by serviceMessageLoop() when a config response event is returned by LeapPollConnection(). */
void Connection::handleConfigResponseEvent(const LEAP_CONFIG_RESPONSE_EVENT *config_response_event) {
	struct Callbacks & call = getCallbacks();
	call.on_config_response(config_response_event->requestID, config_response_event->value);
}

/** Called by serviceMessageLoop() when a point mapping change event is returned by LeapPollConnection(). */
void Connection::handleImageEvent(const LEAP_IMAGE_EVENT *image_event) {
	struct Callbacks & call = getCallbacks();
	call.on_image(image_event);
}

/** Called by serviceMessageLoop() when a point mapping change event is returned by LeapPollConnection(). */
void Connection::handlePointMappingChangeEvent(const LEAP_POINT_MAPPING_CHANGE_EVENT *point_mapping_change_event) {
	struct Callbacks & call = getCallbacks();
	call.on_point_mapping_change(point_mapping_change_event);
}

/** Called by serviceMessageLoop() when a point mapping change event is returned by LeapPollConnection(). */
void Connection::handleHeadPoseEvent(const LEAP_HEAD_POSE_EVENT *head_pose_event) {
	struct Callbacks & call = getCallbacks();
	call.on_head_pose(head_pose_event);
}


/**
* Services the LeapC message pump by calling LeapPollConnection().
* The average polling time is determined by the framerate of the Leap Motion service.
*/
#if defined(_MSC_VER)
void __stdcall Connection::serviceMessageLoop(void * unused) {
#else
void* Connection::serviceMessageLoop(void * unused) {
#endif
	eLeapRS result;
	LEAP_CONNECTION_MESSAGE msg;

	LEAP_CONNECTION & handler = getHandler();
	while (getRunning()) {
		unsigned int timeout = 1000;
		result = LeapPollConnection(handler, timeout, &msg);

		if (result != eLeapRS_Success) {
			std::cout << " PollConnection call was " << ResultString(result) << std::endl;
			continue;
		}


		switch (msg.type) {
		case eLeapEventType_Connection:
			handleConnectionEvent(msg.connection_event);
			break;
		case eLeapEventType_ConnectionLost:
			handleConnectionLostEvent(msg.connection_lost_event);
			break;
		case eLeapEventType_Device:
			handleDeviceEvent(msg.device_event);
			break;
		case eLeapEventType_DeviceLost:
			handleDeviceLostEvent(msg.device_event);
			break;
		case eLeapEventType_DeviceFailure:
			handleDeviceFailureEvent(msg.device_failure_event);
			break;
		case eLeapEventType_Tracking:
			handleTrackingEvent(msg.tracking_event, buffer);
			break;
		case eLeapEventType_ImageComplete:
			// Ignore
			break;
		case eLeapEventType_ImageRequestError:
			// Ignore
			break;
		case eLeapEventType_LogEvent:
			handleLogEvent(msg.log_event);
			break;
		case eLeapEventType_Policy:
			handlePolicyEvent(msg.policy_event);
			break;
		case eLeapEventType_ConfigChange:
			handleConfigChangeEvent(msg.config_change_event);
			break;
		case eLeapEventType_ConfigResponse:
			handleConfigResponseEvent(msg.config_response_event);
			break;
		case eLeapEventType_Image:
			handleImageEvent(msg.image_event);
			break;
		case eLeapEventType_PointMappingChange:
			handlePointMappingChangeEvent(msg.point_mapping_change_event);
			break;
		case eLeapEventType_LogEvents:
			handleLogEvents(msg.log_events);
			break;
		case eLeapEventType_HeadPose:
			handleHeadPoseEvent(msg.head_pose_event);
			break;
		default:
			//discard unknown message types
			std::cout << "Unhandled message type. " << msg.type << std::endl;
		} //switch on msg.type
	}
#if !defined(_MSC_VER)
	return NULL;
#endif
}

/* Used in Polling Example: */

/**
* Caches the newest frame by copying the tracking event struct returned by
* LeapC.
*/
void Connection::setFrame(const LEAP_TRACKING_EVENT *frame) {
	LockMutex(&dataLock);
	if (!lastFrame) lastFrame = (LEAP_TRACKING_EVENT *)malloc(sizeof(*frame));
	*lastFrame = *frame;
	UnlockMutex(&dataLock);
}

/** Returns a pointer to the cached tracking frame. */
LEAP_TRACKING_EVENT* Connection::GetFrame() {
	LEAP_TRACKING_EVENT *currentFrame;

	LockMutex(&dataLock);
	currentFrame = lastFrame;
	UnlockMutex(&dataLock);

	return currentFrame;
}

/**
* Caches the last device found by copying the device info struct returned by
* LeapC.
*/
static void setDevice(Connection & c, const LEAP_DEVICE_INFO *deviceProps) {
	LockMutex(&dataLock);
	LEAP_DEVICE_INFO * lastDevice = c.getLastDevice();
	if (lastDevice) {
		free(lastDevice->serial);
	}
	else {
		lastDevice = (LEAP_DEVICE_INFO*)malloc(sizeof(*deviceProps));
	}
	*lastDevice = *deviceProps;
	lastDevice->serial = (char*)malloc(deviceProps->serial_length);
	memcpy(lastDevice->serial, deviceProps->serial, deviceProps->serial_length);
	UnlockMutex(&dataLock);
}

/** Returns a pointer to the cached device info. */
LEAP_DEVICE_INFO* Connection::GetDeviceProperties() {
	LEAP_DEVICE_INFO *currentDevice;
	LockMutex(&dataLock);
	currentDevice = lastDevice;
	UnlockMutex(&dataLock);
	return currentDevice;
}

//End of polling example-specific code

/** Translates eLeapRS result codes into a human-readable string. */
const std::string Connection::ResultString(eLeapRS r) {
	switch (r) {
	case eLeapRS_Success:                  return "eLeapRS_Success";
	case eLeapRS_UnknownError:             return "eLeapRS_UnknownError";
	case eLeapRS_InvalidArgument:          return "eLeapRS_InvalidArgument";
	case eLeapRS_InsufficientResources:    return "eLeapRS_InsufficientResources";
	case eLeapRS_InsufficientBuffer:       return "eLeapRS_InsufficientBuffer";
	case eLeapRS_Timeout:                  return "eLeapRS_Timeout";
	case eLeapRS_NotConnected:             return "eLeapRS_NotConnected";
	case eLeapRS_HandshakeIncomplete:      return "eLeapRS_HandshakeIncomplete";
	case eLeapRS_BufferSizeOverflow:       return "eLeapRS_BufferSizeOverflow";
	case eLeapRS_ProtocolError:            return "eLeapRS_ProtocolError";
	case eLeapRS_InvalidClientID:          return "eLeapRS_InvalidClientID";
	case eLeapRS_UnexpectedClosed:         return "eLeapRS_UnexpectedClosed";
	case eLeapRS_UnknownImageFrameRequest: return "eLeapRS_UnknownImageFrameRequest";
	case eLeapRS_UnknownTrackingFrameID:   return "eLeapRS_UnknownTrackingFrameID";
	case eLeapRS_RoutineIsNotSeer:         return "eLeapRS_RoutineIsNotSeer";
	case eLeapRS_TimestampTooEarly:        return "eLeapRS_TimestampTooEarly";
	case eLeapRS_ConcurrentPoll:           return "eLeapRS_ConcurrentPoll";
	case eLeapRS_NotAvailable:             return "eLeapRS_NotAvailable";
	case eLeapRS_NotStreaming:             return "eLeapRS_NotStreaming";
	case eLeapRS_CannotOpenDevice:         return "eLeapRS_CannotOpenDevice";
	default:                               return "unknown result type.";
	}
}
/** Cross-platform sleep function */
void Connection::millisleep(int milliseconds) {
#ifdef _WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif
}
//End-of-ExampleConnection.c
