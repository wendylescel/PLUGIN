#include <iostream>
#include <string>
#include "Callbacks.h"


/*defining several objects used as callback functions*/

/** Callback for when the connection opens. */
void connection_callback::operator()()
{
	std::cout << "connected ! " << std::endl;
};

/** Callback for when a device is found. */
void device_callback::operator()(const LEAP_DEVICE_INFO *props) {
	std::cout << "device " << props->serial << std::endl;
}

void device_lost_callback::operator ()() {};
void device_failure_callback::operator ()(const eLeapDeviceStatus failure_code,
	const LEAP_DEVICE failed_device) {};

void policy_callback::operator ()(const uint32_t current_policies) {};


/** Callback for when a frame of tracking data is available. */
void tracking_callback::operator()(const LEAP_TRACKING_EVENT *frame, CircularBuffer & buff) {
	//std::cout << "Frame " << (long long int) frame->info.frame_id << " with " << frame->nHands << " hands" << std::endl;
	for (uint32_t h = 0; h < frame->nHands; h++) {
		LEAP_HAND* hand = &frame->pHands[h];
		if (hand->index.is_extended)
		{
			/*std::cout << "    Hand id " << hand->id << " is a " <<
				(hand->type == eLeapHandType_Left ? "left" : "right") <<
				" hand with position ( x: " <<
				hand->palm.position.x << ", y: " <<
				hand->palm.position.y << ", z:  " <<
				hand->palm.position.z << ") " << std::endl;

			std::cout << "index tracking : (x:" << hand->index.distal.next_joint.x << ", y: " <<
				hand->index.distal.next_joint.y << ", z: " << hand->index.distal.next_joint.z << ")" << std::endl;*/
			IndexPos i = { hand->index.distal.next_joint.x , hand->index.distal.next_joint.y, hand->index.distal.next_joint.z };
			buff.write(i);
		}
	}
}

void log_callback::operator()(const eLeapLogSeverity severity,
	const int64_t timestamp,
	const char* message) {};
void config_change_callback::operator()(const uint32_t requestID, const bool success) {};
void config_response__callback::operator()(const uint32_t requestID, LEAP_VARIANT value) {};


/** Callback for when an image is available. */
void image_callback::operator()(const LEAP_IMAGE_EVENT *imageEvent) {
	/*std::cout << "Received image set for frame " << (long long int)imageEvent->info.frame_id << " with size " <<
	(long long int)imageEvent->image[0].properties.width*
	(long long int)imageEvent->image[0].properties.height * 2 << std::endl;*/
}

void point_mapping_change_callback::operator()(const LEAP_POINT_MAPPING_CHANGE_EVENT *point_mapping_change_even) {};

void head_pose_callback::operator()(const LEAP_HEAD_POSE_EVENT *head_pose_event) {};
void config_response_callback::operator()(const uint32_t requestID, const LEAP_VARIANT value) {};