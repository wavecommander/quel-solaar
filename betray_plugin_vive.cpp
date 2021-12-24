#if defined __cplusplus		/* Declare as C symbols for C++ users. */
extern "C" {
#endif
#include "betray_plugin_api.h"

#if defined __cplusplus
}
#endif
#include "openvr.h"
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "openvr_api.lib")

#define GL_RENDERBUFFER_EXT                    0x8D41
#define GL_DEPTH24_STENCIL8_EXT                0x88F0
#define GL_FRAMEBUFFER_EXT                     0x8D40
#define GL_COLOR_ATTACHMENT0_EXT               0x8CE0
#define GL_DEPTH_ATTACHMENT_EXT                0x8D00
#define GL_FRAMEBUFFER_COMPLETE_EXT            0x8CD5

vr::IVRSystem *m_pHMD;
//vr::IVRCompositor *compositor;
vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
vr::TrackedDevicePose_t m_rTrackedController1Pose[vr::k_unMaxTrackedDeviceCount];
vr::TrackedDevicePose_t m_rTrackedController2Pose[vr::k_unMaxTrackedDeviceCount];

vr::HmdMatrix34_t leftEyeProjection;
vr::HmdMatrix34_t rightEyeProjection;

vr::HmdMatrix44_t leftEyePose;
vr::HmdMatrix44_t rightEyePose;
uint render_size[2] = { 1024 , 1024 };

uint render_fbos[2];
uint depth_buffer[2];

float m_fNearClip;
float m_fFarClip;

uint plug_texture_ids[2];
uint plug_framebuffer_ids[2];
uint plug_texture_resolution[2];
bool m_rbShowTrackedDevice[ vr::k_unMaxTrackedDeviceCount ];
vr::VROverlayHandle_t ulOverlayHandle;
vr::VROverlayHandle_t controlOverlayHandle;
vr::IVROverlay *renderController;
vr::RenderModel_ComponentState_t *pComponentState;

vr::VRControllerState_t vrConroler1;
vr::VRControllerState_t vrConroler2;
vr::RenderModel_t *pModel;
vr::RenderModel_TextureMap_t *pTexture;

boolean proximitySensor = TRUE;

uint userID;
uint leftController;
uint rightController;
uint leftPointer;
uint rightPointer;

boolean leftbuttons[3] = {FALSE, FALSE, FALSE};
boolean rightbuttons[3] = {FALSE, FALSE, FALSE};

uint lcForward;
uint lcUp;
uint lcPos;
uint lcTouch;

uint rcForward;
uint rcUp;
uint rcPos;
uint rcTouch;

#if defined __cplusplus		/* Declare as C symbols for C++ users. */
extern "C" {
#endif
	uint betray_plugin_open_vr_texture_allocate(uint x, uint y);
	uint betray_plugin_open_vr_depth_allocate(uint x, uint y);
	uint betray_plugin_open_vr_framebuffer_allocate(uint texture_id, uint depth_id);
	void betray_plugin_open_vr_opengl_init();
	void betray_plugin_open_vr_opengl_draw(uint texture_id);
#if defined __cplusplus
}
#endif

void ThreadSleep( unsigned long nMilliseconds )
{
#if defined(_WIN32)
	::Sleep( nMilliseconds );
#elif defined(POSIX)
	usleep( nMilliseconds * 1000 );
#endif
}


void SetupCameras()
{
	leftEyeProjection = m_pHMD->GetEyeToHeadTransform(vr::Eye_Left);
	rightEyeProjection = m_pHMD->GetEyeToHeadTransform(vr::Eye_Right);
	//convert to float matrix
	m_fNearClip = 0.1f;
	m_fFarClip = 30.0f;
	leftEyePose = m_pHMD->GetProjectionMatrix(vr::Eye_Left,m_fNearClip,m_fFarClip );
	rightEyePose = m_pHMD->GetProjectionMatrix(vr::Eye_Right,m_fNearClip,m_fFarClip);
}



boolean betray_plugin_image_warp(BInputState *input)
{
	boolean buttons[4] = {FALSE, FALSE, FALSE, FALSE};
	uint i;
	vr::VREvent_t pEvent;
	float fSecondsSinceLastVsync;
	m_pHMD->GetTimeSinceLastVsync( &fSecondsSinceLastVsync, NULL );

	float fDisplayFrequency = m_pHMD->GetFloatTrackedDeviceProperty( vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_DisplayFrequency_Float );
	float fFrameDuration = 1.f / fDisplayFrequency;
	float fVsyncToPhotons = m_pHMD->GetFloatTrackedDeviceProperty( vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SecondsFromVsyncToPhotons_Float );

	float fPredictedSecondsFromNow = fFrameDuration - fSecondsSinceLastVsync + fVsyncToPhotons;

	float view[3] = {0, 0, 1}, matrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};


	/* set camera for one eye */
	SetupCameras();
	vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	//convert eyes to float matrix 
	//Prop_CameraToHeadTransform_Matrix34			= 2016,
	//vr::HmdMatrix34_t mat = m_pHMD->GetMatrix34TrackedDeviceProperty( 0, vr::Prop_CameraToHeadTransform_Matrix34, 0 );
	m_pHMD->GetDeviceToAbsoluteTrackingPose(vr::VRCompositor()->GetTrackingSpace(), fPredictedSecondsFromNow, VR_ARRAY_COUNT(unTrackedDevicePoseArrayCount) m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount);

	//get index of controller
	uint lh = m_pHMD->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
	uint rh = m_pHMD->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);

	m_pHMD->GetControllerStateWithPose(vr::VRCompositor()->GetTrackingSpace(), lh, &vrConroler1, sizeof(vr::VRControllerState_t), m_rTrackedController1Pose);
	m_pHMD->GetControllerStateWithPose(vr::VRCompositor()->GetTrackingSpace(), rh, &vrConroler2, sizeof(vr::VRControllerState_t), m_rTrackedController2Pose);

	float leftControllerMat[16] = {m_rTrackedDevicePose[lh].mDeviceToAbsoluteTracking.m[0][0], m_rTrackedDevicePose[lh].mDeviceToAbsoluteTracking.m[1][0], m_rTrackedDevicePose[lh].mDeviceToAbsoluteTracking.m[2][0], 0.0,
		m_rTrackedDevicePose[lh].mDeviceToAbsoluteTracking.m[0][1], m_rTrackedDevicePose[lh].mDeviceToAbsoluteTracking.m[1][1], m_rTrackedDevicePose[lh].mDeviceToAbsoluteTracking.m[2][1], 0.0,
		m_rTrackedDevicePose[lh].mDeviceToAbsoluteTracking.m[0][2], m_rTrackedDevicePose[lh].mDeviceToAbsoluteTracking.m[1][2], m_rTrackedDevicePose[lh].mDeviceToAbsoluteTracking.m[2][2], 0.0,
		m_rTrackedDevicePose[lh].mDeviceToAbsoluteTracking.m[0][3], m_rTrackedDevicePose[lh].mDeviceToAbsoluteTracking.m[1][3], m_rTrackedDevicePose[lh].mDeviceToAbsoluteTracking.m[2][3], 1.0f};

	float rightControllerMat[16] = {m_rTrackedDevicePose[rh].mDeviceToAbsoluteTracking.m[0][0], m_rTrackedDevicePose[rh].mDeviceToAbsoluteTracking.m[1][0], m_rTrackedDevicePose[rh].mDeviceToAbsoluteTracking.m[2][0], 0.0,
		m_rTrackedDevicePose[rh].mDeviceToAbsoluteTracking.m[0][1], m_rTrackedDevicePose[rh].mDeviceToAbsoluteTracking.m[1][1], m_rTrackedDevicePose[rh].mDeviceToAbsoluteTracking.m[2][1], 0.0,
		m_rTrackedDevicePose[rh].mDeviceToAbsoluteTracking.m[0][2], m_rTrackedDevicePose[rh].mDeviceToAbsoluteTracking.m[1][2], m_rTrackedDevicePose[rh].mDeviceToAbsoluteTracking.m[2][2], 0.0,
		m_rTrackedDevicePose[rh].mDeviceToAbsoluteTracking.m[0][3], m_rTrackedDevicePose[rh].mDeviceToAbsoluteTracking.m[1][3], m_rTrackedDevicePose[rh].mDeviceToAbsoluteTracking.m[2][3], 1.0f};


	betray_plugin_axis_set(lcForward, -leftControllerMat[8], -leftControllerMat[9], -leftControllerMat[10]);
	betray_plugin_axis_set(lcUp, leftControllerMat[4], leftControllerMat[5], leftControllerMat[6]);
	betray_plugin_axis_set(lcPos, leftControllerMat[12], leftControllerMat[13], leftControllerMat[14]);

	betray_plugin_axis_set(rcForward, -rightControllerMat[8], -rightControllerMat[9], -rightControllerMat[10]);
	betray_plugin_axis_set(rcUp, rightControllerMat[4], rightControllerMat[5], rightControllerMat[6]);
	betray_plugin_axis_set(rcPos, rightControllerMat[12], rightControllerMat[13], rightControllerMat[14]);


	{
		static FILE *f = NULL;
		if(f == NULL)
		{
			f = fopen("vr_debug.txt", "w");

			fprintf(f, "Size %u ", sizeof(vr::VRControllerState_t));
			for(i = 0; i < 64; i++)
				fprintf(f, "In it buttons : %u - %s\n", i, m_pHMD->GetButtonIdNameFromEnum((vr::EVRButtonId)i));
		}
	/*	if(vrConroler1.ulButtonPressed != 0 &&
			vrConroler1.ulButtonPressed != 1 << 63 &&
			vrConroler1.ulButtonPressed != 1 << 31)
			for(i = 0; i < 64; i++)
				if(vrConroler1.ulButtonPressed & (1 << i))
					fprintf(f, "buttons : %llu %llu - %s\n", vrConroler1.ulButtonPressed & (1 << i), vrConroler1.ulButtonPressed, m_pHMD->GetButtonIdNameFromEnum((vr::EVRButtonId)i));

*/
		while(m_pHMD->PollNextEvent(&pEvent, sizeof(vr::VREvent_t)))
		{
			if(pEvent.eventType == vr::VREvent_ButtonPress && pEvent.data.controller.button == vr::k_EButton_ProximitySensor)
				proximitySensor = TRUE;
			if(pEvent.eventType == vr::VREvent_ButtonUnpress && pEvent.data.controller.button == vr::k_EButton_ProximitySensor)
				proximitySensor = FALSE;

			

			if(pEvent.trackedDeviceIndex == lh)
			{
				if(pEvent.eventType == vr::VREvent_ButtonPress)
				{
					if(pEvent.data.controller.button == vr::k_EButton_Axis1)
						leftbuttons[0] = TRUE;
					if(pEvent.data.controller.button == vr::k_EButton_Grip)
						leftbuttons[1] = TRUE;
					if(pEvent.data.controller.button == vr::k_EButton_ApplicationMenu)
						leftbuttons[2] = TRUE;
					if(pEvent.data.controller.button == vr::k_EButton_Axis0)
					{
						if(vrConroler1.rAxis[0].x * vrConroler1.rAxis[0].x + vrConroler1.rAxis[0].y * vrConroler1.rAxis[0].y < 0.5 * 0.5)
							betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_FACE_A, TRUE, -1);
						else if(vrConroler1.rAxis[0].x * vrConroler1.rAxis[0].x > vrConroler1.rAxis[0].y * vrConroler1.rAxis[0].y)						
						{
							if(vrConroler1.rAxis[0].x > 0)
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_RIGHT, TRUE, -1);
							else
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_LEFT, TRUE, -1);
						}else
						{
							if(vrConroler1.rAxis[0].y > 0)
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_UP, TRUE, -1);
							else
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_DOWN, TRUE, -1);
						}
					}
				}
				if(pEvent.eventType == vr::VREvent_ButtonUnpress)
				{
					if(pEvent.data.controller.button == vr::k_EButton_Axis1)
						leftbuttons[0] = FALSE;
					if(pEvent.data.controller.button == vr::k_EButton_Grip)
						leftbuttons[1] = FALSE;
					if(pEvent.data.controller.button == vr::k_EButton_ApplicationMenu)
						leftbuttons[2] = FALSE;
					if(pEvent.data.controller.button == vr::k_EButton_Axis0)
					{
						if(vrConroler1.rAxis[0].x * vrConroler1.rAxis[0].x + vrConroler1.rAxis[0].y * vrConroler1.rAxis[0].y < 0.5 * 0.5)
							betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_FACE_A, FALSE, -1);
						else if(vrConroler1.rAxis[0].x * vrConroler1.rAxis[0].x > vrConroler1.rAxis[0].y * vrConroler1.rAxis[0].y)						
						{
							if(vrConroler1.rAxis[0].x > 0)
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_RIGHT, FALSE, -1);
							else
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_LEFT, FALSE, -1);
						}else
						{
							if(vrConroler1.rAxis[0].y > 0)
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_UP, FALSE, -1);
							else
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_DOWN, FALSE, -1);
						}
					}
				}
			}
			if(pEvent.trackedDeviceIndex == rh)
			{
				if(pEvent.eventType == vr::VREvent_ButtonPress)
				{
					if(pEvent.data.controller.button == vr::k_EButton_Axis1)
						rightbuttons[0] = TRUE;
					if(pEvent.data.controller.button == vr::k_EButton_Grip)
						rightbuttons[1] = TRUE;
					if(pEvent.data.controller.button == vr::k_EButton_ApplicationMenu)
						rightbuttons[2] = TRUE;
					if(pEvent.data.controller.button == vr::k_EButton_Axis0)
					{
						if(vrConroler2.rAxis[0].x * vrConroler2.rAxis[0].x + vrConroler2.rAxis[0].y * vrConroler2.rAxis[0].y < 0.5 * 0.5)
							betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_FACE_A, TRUE, -1);
						else if(vrConroler2.rAxis[0].x * vrConroler2.rAxis[0].x > vrConroler2.rAxis[0].y * vrConroler2.rAxis[0].y)						
						{
							if(vrConroler2.rAxis[0].x > 0)
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_RIGHT, TRUE, -1);
							else
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_LEFT, TRUE, -1);
						}else
						{
							if(vrConroler2.rAxis[0].y > 0)
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_UP, TRUE, -1);
							else
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_DOWN, TRUE, -1);
						}
					}
				}
				if(pEvent.eventType == vr::VREvent_ButtonUnpress)
				{
					if(pEvent.data.controller.button == vr::k_EButton_Axis1)
						rightbuttons[0] = FALSE;
					if(pEvent.data.controller.button == vr::k_EButton_Grip)
						rightbuttons[1] = FALSE;
					if(pEvent.data.controller.button == vr::k_EButton_ApplicationMenu)
						rightbuttons[2] = FALSE;
					if(pEvent.data.controller.button == vr::k_EButton_Axis0)
					{
						if(vrConroler2.rAxis[0].x * vrConroler2.rAxis[0].x + vrConroler2.rAxis[0].y * vrConroler2.rAxis[0].y < 0.5 * 0.5)
							betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_FACE_A, FALSE, -1);
						else if(vrConroler2.rAxis[0].x * vrConroler2.rAxis[0].x > vrConroler2.rAxis[0].y * vrConroler2.rAxis[0].y)						
						{
							if(vrConroler2.rAxis[0].x > 0)
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_RIGHT, FALSE, -1);
							else
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_LEFT, FALSE, -1);
						}else
						{
							if(vrConroler2.rAxis[0].y > 0)
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_UP, FALSE, -1);
							else
								betray_plugin_button_set(userID, BETRAY_ENUM_BUTTON_DOWN, FALSE, -1);
						}
					}
				}
				fprintf(f, "Event %u\n", pEvent.eventType);
			}
		}
	}	
	betray_plugin_axis_set(lcTouch, vrConroler1.rAxis[0].x, vrConroler1.rAxis[0].y, 0);
	betray_plugin_axis_set(rcTouch, vrConroler2.rAxis[0].x, vrConroler2.rAxis[0].y, 0);
	
	
	
	// k_EButton_Axis1 // trigger 33
	// k_EButton_ApplicationMenu // menu 1
	// k_EButton_Grip // right mouse button 2

	// k_EButton_Axis0 // A button. 32


	buttons[0] = vrConroler1.ulButtonPressed & (1 << 63);	
	betray_plugin_pointer_set(leftPointer, leftControllerMat[12] - leftControllerMat[8], 
										leftControllerMat[13] - leftControllerMat[9],
										leftControllerMat[14] - leftControllerMat[10], &leftControllerMat[12], leftbuttons);
	buttons[0] = vrConroler2.ulButtonPressed & (1 << 63);	
	betray_plugin_pointer_set(rightPointer, rightControllerMat[12] - rightControllerMat[8], 
										rightControllerMat[13] - rightControllerMat[9],
										rightControllerMat[14] - rightControllerMat[10], &rightControllerMat[12], rightbuttons);
/*	{
		static FILE *f = NULL;
		if(f == NULL)
			f = fopen("debug.txt", "w");
		for(i = 0; i < 16; i++)
			//fprintf(f, "%u\n", lh);
			fprintf(f, " leftControllerMat[%u] = %f\n", i, leftControllerMat[i]);
		fprintf(f, "--------------------------------------------- \n");	
	}*/

	
	if(proximitySensor)
	{

		float headMat[16] = {m_rTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[0][0], m_rTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[1][0], m_rTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[2][0], 0.0,
			m_rTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[0][1], m_rTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[1][1], m_rTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[2][1], 0.0,
			m_rTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[0][2], m_rTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[1][2], m_rTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[2][2], 0.0,
			m_rTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[0][3], m_rTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[1][3], m_rTrackedDevicePose[0].mDeviceToAbsoluteTracking.m[2][3], 1.0f};


		float leftEyeMat[16] = {leftEyeProjection.m[0][0], leftEyeProjection.m[1][0], leftEyeProjection.m[2][0], 0.0,
			leftEyeProjection.m[0][1], leftEyeProjection.m[1][1], leftEyeProjection.m[2][1], 0.0,
			leftEyeProjection.m[0][2], leftEyeProjection.m[1][2], leftEyeProjection.m[2][2], 0.0,
			leftEyeProjection.m[0][3], leftEyeProjection.m[1][3], leftEyeProjection.m[2][3], 1.0f};

		float rightEyeMat[16] = {rightEyeProjection.m[0][0], rightEyeProjection.m[1][0], rightEyeProjection.m[2][0], 0.0,
			rightEyeProjection.m[0][1], rightEyeProjection.m[1][1], rightEyeProjection.m[2][1], 0.0,
			rightEyeProjection.m[0][2], rightEyeProjection.m[1][2], rightEyeProjection.m[2][2], 0.0,
			rightEyeProjection.m[0][3], rightEyeProjection.m[1][3], rightEyeProjection.m[2][3], 1.0f};

	

		float headMatInv[16];
		float eyeInv[16];
		float output[16];
	
		view[0] = -0.1;
		view[1] = 0.0;
		for(i = 0; i < 16; i++)
			output[i] = headMat[i];
		view[2] = 0.8;

		f_matrix_reverse4f(headMatInv, headMat);
		f_matrix_reverse4f(eyeInv, leftEyeMat);


		f_matrix_multiplyf(output, leftEyeMat, headMatInv);

		float pos[3];

		headMatInv[12] += leftEyeMat[12];
		headMatInv[13] += leftEyeMat[13];
		headMatInv[14] += leftEyeMat[14];
		//f_matrix_multiplyf(headMatInv, headMatInv, leftControllerMat);
		betray_plugin_application_draw(plug_framebuffer_ids[0], plug_texture_resolution[0], plug_texture_resolution[1], view, FALSE, headMatInv);

		/* set camera for other eye */

		view[0] = 0.1;
		for(i = 0; i < 16; i++)
			output[i] = headMat[i];

		f_matrix_reverse4f(headMatInv, headMat);
		f_matrix_reverse4f(eyeInv, rightEyeMat);

		headMatInv[12] += rightEyeMat[12];
		headMatInv[13] += rightEyeMat[13];
		headMatInv[14] += rightEyeMat[14];
		betray_plugin_application_draw(plug_framebuffer_ids[1], plug_texture_resolution[0], plug_texture_resolution[1], view, FALSE, headMatInv);
		betray_plugin_open_vr_opengl_draw(plug_texture_ids[1]);
	}else
	{
		uint x, y;
		betray_plugin_screen_mode_get(&x, &y, NULL);
		betray_plugin_application_draw(0, x, y, NULL, FALSE, NULL);
	}

	if (m_pHMD)
	{
		vr::VROverlay()->HandleControllerOverlayInteractionAsMouse(ulOverlayHandle, 2);
		vr::VROverlay()->HandleControllerOverlayInteractionAsMouse(ulOverlayHandle, 3);

		vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)plug_texture_ids[1], vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);

		
		vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)plug_texture_ids[0], vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
	}
	vr::EVREventType eType;
	return TRUE;
}





void betray_plugin_init_internal(void)
{

	vr::EVRInitError eError = vr::VRInitError_None;
	m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);
	betray_plugin_open_vr_opengl_init();


	/* Check if HMD is connected, if not return*/
	if (eError != vr::VRInitError_None)
	{

		m_pHMD = NULL;
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		return;
	}

	uint x, y;
	uint i, depth;

	userID = betray_plugin_user_allocate();
	leftController = betray_plugin_input_device_allocate(userID, "Controller One");
	rightController = betray_plugin_input_device_allocate(userID, "Controller Two");
	lcForward = betray_plugin_axis_allocate(userID, leftController, "Left Forward", B_AXIS_SCREEN_FORWARD, 1);
	lcUp = betray_plugin_axis_allocate(userID, leftController, "Left Up", B_AXIS_SCREEN_UP, 1);
	lcPos = betray_plugin_axis_allocate(userID, leftController, "Left Pos", B_AXIS_SCREEN_POS, 1);
	lcTouch = betray_plugin_axis_allocate(userID, leftController, "Left Touchpad", B_AXIS_SUBSTICK, 2);
	rcForward = betray_plugin_axis_allocate(userID, rightController, "Right Forward", B_AXIS_SCREEN_FORWARD, 1);
	rcUp = betray_plugin_axis_allocate(userID, rightController, "Right Up", B_AXIS_SCREEN_UP, 1);
	rcPos = betray_plugin_axis_allocate(userID, rightController, "Right Pos", B_AXIS_SCREEN_POS, 1);
	rcTouch = betray_plugin_axis_allocate(userID, rightController, "Right Touchpad", B_AXIS_SUBSTICK, 2);

	
	leftPointer = betray_plugin_pointer_allocate(userID, leftController, 3, 0, 0, -1, NULL, "Left Controller", TRUE);
	rightPointer = betray_plugin_pointer_allocate(userID, rightController, 3, 0, 0, -1, NULL, "Right Controller", TRUE);

	/*if(m_pHMD->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand) == NULL){
		betray_plugin_input_device_free(leftController);
		betray_plugin_pointer_free(leftPointer);
	
	}

	if(m_pHMD->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand) == NULL){
		betray_plugin_input_device_free(rightController);
		betray_plugin_pointer_free(rightPointer);
	}*/

	/* query recomended resolution */
	plug_texture_resolution[0] = 1024;
	plug_texture_resolution[1] = 1024;

	betray_plugin_callback_set_image_warp(betray_plugin_image_warp, "openVR");



	for (i = 0; i < 2; i++)
	{
		plug_texture_ids[i] = betray_plugin_open_vr_texture_allocate(plug_texture_resolution[0], plug_texture_resolution[1]);
		depth = betray_plugin_open_vr_depth_allocate(plug_texture_resolution[0], plug_texture_resolution[1]);
		plug_framebuffer_ids[i] = betray_plugin_open_vr_framebuffer_allocate(plug_texture_ids[i], depth);

	}

	
}


#if defined __cplusplus		/* Declare as C symbols for C++ users. */
extern "C" {
#endif

	void betray_plugin_open_vr_opengl_init();

	void betray_plugin_init(void)
	{
		betray_plugin_open_vr_opengl_init();
		betray_plugin_init_internal();
	}
#if defined __cplusplus
}
#endif