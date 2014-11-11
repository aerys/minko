/************************************************************************************

Filename    :   OVR_CAPI.h
Content     :   Keys for CAPI calls
Created     :   September 25, 2014
Authors     :   

Copyright   :   Copyright 2014 Oculus VR, LLC All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.2 (the "License");
you may not use the Oculus VR Rift SDK except in compliance with the License,
which is provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.2

Unless required by applicable law or agreed to in writing, the Oculus VR SDK
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/



#define OVR_KEY_USER                        "User"
#define OVR_KEY_NAME                        "Name"
#define OVR_KEY_GENDER                      "Gender"
#define OVR_KEY_PLAYER_HEIGHT               "PlayerHeight"
#define OVR_KEY_EYE_HEIGHT                  "EyeHeight"
#define OVR_KEY_IPD                         "IPD"
#define OVR_KEY_NECK_TO_EYE_DISTANCE        "NeckEyeDistance"
#define OVR_KEY_EYE_RELIEF_DIAL             "EyeReliefDial"
#define OVR_KEY_EYE_TO_NOSE_DISTANCE        "EyeToNoseDist"
#define OVR_KEY_MAX_EYE_TO_PLATE_DISTANCE   "MaxEyeToPlateDist"
#define OVR_KEY_EYE_CUP                     "EyeCup"
#define OVR_KEY_CUSTOM_EYE_RENDER           "CustomEyeRender"
#define OVR_KEY_CAMERA_POSITION				"CenteredFromWorld"

// Default measurements empirically determined at Oculus to make us happy
// The neck model numbers were derived as an average of the male and female averages from ANSUR-88
// NECK_TO_EYE_HORIZONTAL = H22 - H43 = INFRAORBITALE_BACK_OF_HEAD - TRAGION_BACK_OF_HEAD
// NECK_TO_EYE_VERTICAL = H21 - H15 = GONION_TOP_OF_HEAD - ECTOORBITALE_TOP_OF_HEAD
// These were determined to be the best in a small user study, clearly beating out the previous default values
#define OVR_DEFAULT_GENDER                  "Unknown"
#define OVR_DEFAULT_PLAYER_HEIGHT           1.778f
#define OVR_DEFAULT_EYE_HEIGHT              1.675f
#define OVR_DEFAULT_IPD                     0.064f
#define OVR_DEFAULT_NECK_TO_EYE_HORIZONTAL  0.0805f
#define OVR_DEFAULT_NECK_TO_EYE_VERTICAL    0.075f
#define OVR_DEFAULT_EYE_RELIEF_DIAL         3
#define OVR_DEFAULT_CAMERA_POSITION			{0,0,0,1,0,0,0}

