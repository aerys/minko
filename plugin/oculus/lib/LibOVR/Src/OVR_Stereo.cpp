/************************************************************************************

Filename    :   OVR_Stereo.cpp
Content     :   Stereo rendering functions
Created     :   November 30, 2013
Authors     :   Tom Fosyth

Copyright   :   Copyright 2014 Oculus VR, Inc. All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.1 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.1 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*************************************************************************************/

#include "OVR_Stereo.h"
#include "OVR_Profile.h"
#include "Kernel/OVR_Log.h"
#include "Kernel/OVR_Alg.h"

//To allow custom distortion to be introduced to CatMulSpline.
float (*CustomDistortion)(float) = NULL;
float (*CustomDistortionInv)(float) = NULL;


namespace OVR {


using namespace Alg;

//-----------------------------------------------------------------------------------

// Inputs are 4 points (pFitX[0],pFitY[0]) through (pFitX[3],pFitY[3])
// Result is four coefficients in pResults[0] through pResults[3] such that
//      y = pResult[0] + x * ( pResult[1] + x * ( pResult[2] + x * ( pResult[3] ) ) );
// passes through all four input points.
// Return is true if it succeeded, false if it failed (because two control points
// have the same pFitX value).
bool FitCubicPolynomial ( float *pResult, const float *pFitX, const float *pFitY )
{
    float d0 = ( ( pFitX[0]-pFitX[1] ) * ( pFitX[0]-pFitX[2] ) * ( pFitX[0]-pFitX[3] ) );
    float d1 = ( ( pFitX[1]-pFitX[2] ) * ( pFitX[1]-pFitX[3] ) * ( pFitX[1]-pFitX[0] ) );
    float d2 = ( ( pFitX[2]-pFitX[3] ) * ( pFitX[2]-pFitX[0] ) * ( pFitX[2]-pFitX[1] ) );
    float d3 = ( ( pFitX[3]-pFitX[0] ) * ( pFitX[3]-pFitX[1] ) * ( pFitX[3]-pFitX[2] ) );

    if ( ( d0 == 0.0f ) || ( d1 == 0.0f ) || ( d2 == 0.0f ) || ( d3 == 0.0f ) )
    {
        return false;
    }

    float f0 = pFitY[0] / d0;
    float f1 = pFitY[1] / d1;
    float f2 = pFitY[2] / d2;
    float f3 = pFitY[3] / d3;

    pResult[0] = -( f0*pFitX[1]*pFitX[2]*pFitX[3]
                  + f1*pFitX[0]*pFitX[2]*pFitX[3]
                  + f2*pFitX[0]*pFitX[1]*pFitX[3]
                  + f3*pFitX[0]*pFitX[1]*pFitX[2] );
    pResult[1] = f0*(pFitX[1]*pFitX[2] + pFitX[2]*pFitX[3] + pFitX[3]*pFitX[1])
               + f1*(pFitX[0]*pFitX[2] + pFitX[2]*pFitX[3] + pFitX[3]*pFitX[0])
               + f2*(pFitX[0]*pFitX[1] + pFitX[1]*pFitX[3] + pFitX[3]*pFitX[0])
               + f3*(pFitX[0]*pFitX[1] + pFitX[1]*pFitX[2] + pFitX[2]*pFitX[0]);
    pResult[2] = -( f0*(pFitX[1]+pFitX[2]+pFitX[3])
                  + f1*(pFitX[0]+pFitX[2]+pFitX[3])
                  + f2*(pFitX[0]+pFitX[1]+pFitX[3])
                  + f3*(pFitX[0]+pFitX[1]+pFitX[2]) );
    pResult[3] = f0 + f1 + f2 + f3;

    return true;
}

#define TPH_SPLINE_STATISTICS 0
#if TPH_SPLINE_STATISTICS
static float max_scaledVal = 0;
static float average_total_out_of_range = 0;
static float average_out_of_range;
static int num_total = 0;
static int num_out_of_range = 0;
static int num_out_of_range_over_1 = 0;
static int num_out_of_range_over_2 = 0;
static int num_out_of_range_over_3 = 0;
static float percent_out_of_range;
#endif

float EvalCatmullRom10Spline ( float const *K, float scaledVal )
{
    int const NumSegments = LensConfig::NumCoefficients;

	#if TPH_SPLINE_STATISTICS
	//Value should be in range of 0 to (NumSegments-1) (typically 10) if spline is valid. Right?
	if (scaledVal > (NumSegments-1))
	{
		num_out_of_range++;
		average_total_out_of_range+=scaledVal;
		average_out_of_range = average_total_out_of_range / ((float) num_out_of_range); 
		percent_out_of_range = 100.0f*(num_out_of_range)/num_total;
	}
	if (scaledVal > (NumSegments-1+1)) num_out_of_range_over_1++;
	if (scaledVal > (NumSegments-1+2)) num_out_of_range_over_2++;
	if (scaledVal > (NumSegments-1+3)) num_out_of_range_over_3++;
	num_total++;
	if (scaledVal > max_scaledVal)
	{
		max_scaledVal = scaledVal;
		max_scaledVal = scaledVal;
	}
	#endif

    float scaledValFloor = floorf ( scaledVal );
    scaledValFloor = Alg::Max ( 0.0f, Alg::Min ( (float)(NumSegments-1), scaledValFloor ) );
    float t = scaledVal - scaledValFloor;
    int k = (int)scaledValFloor;

    float p0, p1;
    float m0, m1;
    switch ( k )
    {
    case 0:
        // Curve starts at 1.0 with gradient K[1]-K[0]
        p0 = 1.0f;
        m0 =        ( K[1] - K[0] );    // general case would have been (K[1]-K[-1])/2
        p1 = K[1];
        m1 = 0.5f * ( K[2] - K[0] );
        break;
    default:
        // General case
        p0 = K[k  ];
        m0 = 0.5f * ( K[k+1] - K[k-1] );
        p1 = K[k+1];
        m1 = 0.5f * ( K[k+2] - K[k  ] );
        break;
    case NumSegments-2:
        // Last tangent is just the slope of the last two points.
        p0 = K[NumSegments-2];
        m0 = 0.5f * ( K[NumSegments-1] - K[NumSegments-2] );
        p1 = K[NumSegments-1];
        m1 = K[NumSegments-1] - K[NumSegments-2];
        break;
    case NumSegments-1:
        // Beyond the last segment it's just a straight line
        p0 = K[NumSegments-1];
        m0 = K[NumSegments-1] - K[NumSegments-2];
        p1 = p0 + m0;
        m1 = m0;
        break;
    }

    float omt = 1.0f - t;
    float res  = ( p0 * ( 1.0f + 2.0f *   t ) + m0 *   t ) * omt * omt
               + ( p1 * ( 1.0f + 2.0f * omt ) - m1 * omt ) *   t *   t;

    return res;
}




// Converts a Profile eyecup string into an eyecup enumeration
void SetEyeCup(HmdRenderInfo* renderInfo, const char* cup)
{
    if (OVR_strcmp(cup, "A") == 0)
        renderInfo->EyeCups = EyeCup_DK1A;
    else if (OVR_strcmp(cup, "B") == 0)
        renderInfo->EyeCups = EyeCup_DK1B;
    else if (OVR_strcmp(cup, "C") == 0)
        renderInfo->EyeCups = EyeCup_DK1C;
    else if (OVR_strcmp(cup, "Orange A") == 0)
        renderInfo->EyeCups =  EyeCup_OrangeA;
    else if (OVR_strcmp(cup, "Red A") == 0)
        renderInfo->EyeCups = EyeCup_RedA;
    else if (OVR_strcmp(cup, "Pink A") == 0)
        renderInfo->EyeCups = EyeCup_PinkA;
    else if (OVR_strcmp(cup, "Blue A") == 0)
        renderInfo->EyeCups = EyeCup_BlueA;
    else
        renderInfo->EyeCups = EyeCup_DK1A;
}



//-----------------------------------------------------------------------------------


// The result is a scaling applied to the distance.
float LensConfig::DistortionFnScaleRadiusSquared (float rsq) const
{
    float scale = 1.0f;
    switch ( Eqn )
    {
    case Distortion_Poly4:
        // This version is deprecated! Prefer one of the other two.
        scale = ( K[0] + rsq * ( K[1] + rsq * ( K[2] + rsq * K[3] ) ) );
        break;
    case Distortion_RecipPoly4:
        scale = 1.0f / ( K[0] + rsq * ( K[1] + rsq * ( K[2] + rsq * K[3] ) ) );
        break;
    case Distortion_CatmullRom10:{
        // A Catmull-Rom spline through the values 1.0, K[1], K[2] ... K[10]
        // evenly spaced in R^2 from 0.0 to MaxR^2
        // K[0] controls the slope at radius=0.0, rather than the actual value.
        const int NumSegments = LensConfig::NumCoefficients;
        OVR_ASSERT ( NumSegments <= NumCoefficients );
        float scaledRsq = (float)(NumSegments-1) * rsq / ( MaxR * MaxR );
        scale = EvalCatmullRom10Spline ( K, scaledRsq );


		//Intercept, and overrule if needed
		if (CustomDistortion)
		{
			scale = CustomDistortion(rsq);
		}

        }break;
    default:
        OVR_ASSERT ( false );
        break;
    }
    return scale;
}

// x,y,z components map to r,g,b
Vector3f LensConfig::DistortionFnScaleRadiusSquaredChroma (float rsq) const
{
    float scale = DistortionFnScaleRadiusSquared ( rsq );
    Vector3f scaleRGB;
    scaleRGB.x = scale * ( 1.0f + ChromaticAberration[0] + rsq * ChromaticAberration[1] );     // Red
    scaleRGB.y = scale;                                                                        // Green
    scaleRGB.z = scale * ( 1.0f + ChromaticAberration[2] + rsq * ChromaticAberration[3] );     // Blue
    return scaleRGB;
}

// DistortionFnInverse computes the inverse of the distortion function on an argument.
float LensConfig::DistortionFnInverse(float r) const
{    
    OVR_ASSERT((r <= 20.0f));

    float s, d;
    float delta = r * 0.25f;

    // Better to start guessing too low & take longer to converge than too high
    // and hit singularities. Empirically, r * 0.5f is too high in some cases.
    s = r * 0.25f;
    d = fabs(r - DistortionFn(s));

    for (int i = 0; i < 20; i++)
    {
        float sUp   = s + delta;
        float sDown = s - delta;
        float dUp   = fabs(r - DistortionFn(sUp));
        float dDown = fabs(r - DistortionFn(sDown));

        if (dUp < d)
        {
            s = sUp;
            d = dUp;
        }
        else if (dDown < d)
        {
            s = sDown;
            d = dDown;
        }
        else
        {
            delta *= 0.5f;
        }
    }

    return s;
}



float LensConfig::DistortionFnInverseApprox(float r) const
{
    float rsq = r * r;
    float scale = 1.0f;
    switch ( Eqn )
    {
    case Distortion_Poly4:
        // Deprecated
        OVR_ASSERT ( false );
        break;
    case Distortion_RecipPoly4:
        scale = 1.0f / ( InvK[0] + rsq * ( InvK[1] + rsq * ( InvK[2] + rsq * InvK[3] ) ) );
        break;
    case Distortion_CatmullRom10:{
        // A Catmull-Rom spline through the values 1.0, K[1], K[2] ... K[9]
        // evenly spaced in R^2 from 0.0 to MaxR^2
        // K[0] controls the slope at radius=0.0, rather than the actual value.
        const int NumSegments = LensConfig::NumCoefficients;
        OVR_ASSERT ( NumSegments <= NumCoefficients );
        float scaledRsq = (float)(NumSegments-1) * rsq / ( MaxInvR * MaxInvR );
        scale = EvalCatmullRom10Spline ( InvK, scaledRsq );

		//Intercept, and overrule if needed
		if (CustomDistortionInv)
		{
			scale = CustomDistortionInv(rsq);
		}

        }break;
    default:
        OVR_ASSERT ( false );
        break;
    }
    return r * scale;
}

void LensConfig::SetUpInverseApprox()
{
    float maxR = MaxInvR;

    switch ( Eqn )
    {
    case Distortion_Poly4:
        // Deprecated
        OVR_ASSERT ( false );
        break;
    case Distortion_RecipPoly4:{

        float sampleR[4];
        float sampleRSq[4];
        float sampleInv[4];
        float sampleFit[4];

        // Found heuristically...
        sampleR[0] = 0.0f;
        sampleR[1] = maxR * 0.4f;
        sampleR[2] = maxR * 0.8f;
        sampleR[3] = maxR * 1.5f;
        for ( int i = 0; i < 4; i++ )
        {
            sampleRSq[i] = sampleR[i] * sampleR[i];
            sampleInv[i] = DistortionFnInverse ( sampleR[i] );
            sampleFit[i] = sampleR[i] / sampleInv[i];
        }
        sampleFit[0] = 1.0f;
        FitCubicPolynomial ( InvK, sampleRSq, sampleFit );

    #if 0
        // Should be a nearly exact match on the chosen points.
        OVR_ASSERT ( fabs ( DistortionFnInverse ( sampleR[0] ) - DistortionFnInverseApprox ( sampleR[0] ) ) / maxR < 0.0001f );
        OVR_ASSERT ( fabs ( DistortionFnInverse ( sampleR[1] ) - DistortionFnInverseApprox ( sampleR[1] ) ) / maxR < 0.0001f );
        OVR_ASSERT ( fabs ( DistortionFnInverse ( sampleR[2] ) - DistortionFnInverseApprox ( sampleR[2] ) ) / maxR < 0.0001f );
        OVR_ASSERT ( fabs ( DistortionFnInverse ( sampleR[3] ) - DistortionFnInverseApprox ( sampleR[3] ) ) / maxR < 0.0001f );
        // Should be a decent match on the rest of the range.
        const int maxCheck = 20;
        for ( int i = 0; i < maxCheck; i++ )
        {
            float checkR = (float)i * maxR / (float)maxCheck;
            float realInv = DistortionFnInverse       ( checkR );
            float testInv = DistortionFnInverseApprox ( checkR );
            float error = fabsf ( realInv - testInv ) / maxR;
            OVR_ASSERT ( error < 0.1f );
        }
    #endif

        }break;
    case Distortion_CatmullRom10:{

        const int NumSegments = LensConfig::NumCoefficients;
        OVR_ASSERT ( NumSegments <= NumCoefficients );
        for ( int i = 1; i < NumSegments; i++ )
        {
            float scaledRsq = (float)i;
            float rsq = scaledRsq * MaxInvR * MaxInvR / (float)( NumSegments - 1);
            float r = sqrtf ( rsq );
            float inv = DistortionFnInverse ( r );
            InvK[i] = inv / r;
            InvK[0] = 1.0f;     // TODO: fix this.
        }

#if 0
        const int maxCheck = 20;
        for ( int i = 0; i <= maxCheck; i++ )
        {
            float checkR = (float)i * MaxInvR / (float)maxCheck;
            float realInv = DistortionFnInverse       ( checkR );
            float testInv = DistortionFnInverseApprox ( checkR );
            float error = fabsf ( realInv - testInv ) / MaxR;
            OVR_ASSERT ( error < 0.01f );
        }
#endif

        }break;

    default:
        break;
    }
}


void LensConfig::SetToIdentity()
{
    for ( int i = 0; i < NumCoefficients; i++ )
    {
        K[i] = 0.0f;
        InvK[i] = 0.0f;
    }
    Eqn = Distortion_RecipPoly4;
    K[0] = 1.0f;
    InvK[0] = 1.0f;
    MaxR = 1.0f;
    MaxInvR = 1.0f;
    ChromaticAberration[0] = 0.0f;
    ChromaticAberration[1] = 0.0f;
    ChromaticAberration[2] = 0.0f;
    ChromaticAberration[3] = 0.0f;
    MetersPerTanAngleAtCenter = 0.05f;
}


enum LensConfigStoredVersion
{
    LCSV_CatmullRom10Version1 = 1
};

// DO NOT CHANGE THESE ONCE THEY HAVE BEEN BAKED INTO FIRMWARE.
// If something needs to change, add a new one!
struct LensConfigStored_CatmullRom10Version1
{
    // All these items must be fixed-length integers - no "float", no "int", etc.
    uint16_t    VersionNumber;      // Must be LCSV_CatmullRom10Version1

    uint16_t    K[11];
    uint16_t    MaxR;
    uint16_t    MetersPerTanAngleAtCenter;
    uint16_t    ChromaticAberration[4];
    // InvK and MaxInvR are calculated on load.
};

uint16_t EncodeFixedPointUInt16 ( float val, uint16_t zeroVal, int fractionalBits )
{
    OVR_ASSERT ( ( fractionalBits >= 0 ) && ( fractionalBits < 31 ) );
    float valWhole = val * (float)( 1 << fractionalBits );
    valWhole += (float)zeroVal + 0.5f;
    valWhole = floorf ( valWhole );
    OVR_ASSERT ( ( valWhole >= 0.0f ) && ( valWhole < (float)( 1 << 16 ) ) );
    return (uint16_t)valWhole;
}

float DecodeFixedPointUInt16 ( uint16_t val, uint16_t zeroVal, int fractionalBits )
{
    OVR_ASSERT ( ( fractionalBits >= 0 ) && ( fractionalBits < 31 ) );
    float valFloat = (float)val;
    valFloat -= (float)zeroVal;
    valFloat *= 1.0f / (float)( 1 << fractionalBits );
    return valFloat;
}


// Returns true on success.
bool LoadLensConfig ( LensConfig *presult, uint8_t const *pbuffer, int bufferSizeInBytes )
{
    if ( bufferSizeInBytes < 2 )
    {
        // Can't even tell the version number!
        return false;
    }
    uint16_t version = DecodeUInt16 ( pbuffer + 0 );
    switch ( version )
    {
    case LCSV_CatmullRom10Version1:
        {
            if ( bufferSizeInBytes < (int)sizeof(LensConfigStored_CatmullRom10Version1) )
            {
                return false;
            }
            LensConfigStored_CatmullRom10Version1 lcs;
            lcs.VersionNumber               = DecodeUInt16 ( pbuffer + 0 );
            for ( int i = 0; i < 11; i++ )
            {
                lcs.K[i]                    = DecodeUInt16 ( pbuffer + 2 + 2*i );
            }
            lcs.MaxR                        = DecodeUInt16 ( pbuffer + 24 );
            lcs.MetersPerTanAngleAtCenter   = DecodeUInt16 ( pbuffer + 26 );
            for ( int i = 0; i < 4; i++ )
            {
                lcs.ChromaticAberration[i]  = DecodeUInt16 ( pbuffer + 28 + 2*i );
            }
            OVR_COMPILER_ASSERT ( sizeof(lcs) ==                       36 );

            // Convert to the real thing.
            LensConfig result;
            result.Eqn = Distortion_CatmullRom10;
            for ( int i = 0; i < 11; i++ )
            {
                // K[] are mostly 1.something. They may get significantly bigger, but they never hit 0.0.
                result.K[i] = DecodeFixedPointUInt16 ( lcs.K[i], 0, 14 );
            }
            // MaxR is tan(angle), so always >0, typically just over 1.0 (45 degrees half-fov),
            // but may get arbitrarily high. tan(76)=4 is a very reasonable limit!
            result.MaxR = DecodeFixedPointUInt16 ( lcs.MaxR, 0, 14 );
            // MetersPerTanAngleAtCenter is also known as focal length!
            // Typically around 0.04 for our current screens, minimum of 0, sensible maximum of 0.125 (i.e. 3 "extra" bits of fraction)
            result.MetersPerTanAngleAtCenter = DecodeFixedPointUInt16 ( lcs.MetersPerTanAngleAtCenter, 0, 16+3 );
            for ( int i = 0; i < 4; i++ )
            {
                // ChromaticAberration[] are mostly 0.0something, centered on 0.0. Largest seen is 0.04, so set max to 0.125 (i.e. 3 "extra" bits of fraction)
                result.ChromaticAberration[i] = DecodeFixedPointUInt16 ( lcs.ChromaticAberration[i], 0x8000, 16+3 );
            }
            result.MaxInvR = result.DistortionFn ( result.MaxR );
            result.SetUpInverseApprox();

            OVR_ASSERT ( version == lcs.VersionNumber );

            *presult = result;
        }
        break;
    default:
        // Unknown format.
        return false;
        break;
    }
    return true;
}

// Returns number of bytes needed.
int SaveLensConfigSizeInBytes ( LensConfig const &config )
{
    OVR_UNUSED ( config );
    return sizeof ( LensConfigStored_CatmullRom10Version1 );
}

// Returns true on success.
bool SaveLensConfig ( uint8_t *pbuffer, int bufferSizeInBytes, LensConfig const &config )
{
    if ( bufferSizeInBytes < (int)sizeof ( LensConfigStored_CatmullRom10Version1 ) )
    {
        return false;
    }

    // Construct the values.
    LensConfigStored_CatmullRom10Version1 lcs;
    lcs.VersionNumber = LCSV_CatmullRom10Version1;
    for ( int i = 0; i < 11; i++ )
    {
        // K[] are mostly 1.something. They may get significantly bigger, but they never hit 0.0.
        lcs.K[i] = EncodeFixedPointUInt16 ( config.K[i], 0, 14 );
    }
    // MaxR is tan(angle), so always >0, typically just over 1.0 (45 degrees half-fov),
    // but may get arbitrarily high. tan(76)=4 is a very reasonable limit!
    lcs.MaxR = EncodeFixedPointUInt16 ( config.MaxR, 0, 14 );
    // MetersPerTanAngleAtCenter is also known as focal length!
    // Typically around 0.04 for our current screens, minimum of 0, sensible maximum of 0.125 (i.e. 3 "extra" bits of fraction)
    lcs.MetersPerTanAngleAtCenter = EncodeFixedPointUInt16 ( config.MetersPerTanAngleAtCenter, 0, 16+3 );
    for ( int i = 0; i < 4; i++ )
    {
        // ChromaticAberration[] are mostly 0.0something, centered on 0.0. Largest seen is 0.04, so set max to 0.125 (i.e. 3 "extra" bits of fraction)
        lcs.ChromaticAberration[i] = EncodeFixedPointUInt16 ( config.ChromaticAberration[i], 0x8000, 16+3 );
    }


    // Now store them out, sensitive to endianness.
    EncodeUInt16 (      pbuffer + 0,        lcs.VersionNumber );
    for ( int i = 0; i < 11; i++ )
    {
        EncodeUInt16 (  pbuffer + 2 + 2*i,  lcs.K[i] );
    }
    EncodeUInt16 (      pbuffer + 24,       lcs.MaxR );
    EncodeUInt16 (      pbuffer + 26,       lcs.MetersPerTanAngleAtCenter );
    for ( int i = 0; i < 4; i++ )
    {
        EncodeUInt16 (  pbuffer + 28 + 2*i, lcs.ChromaticAberration[i] );
    }
    OVR_COMPILER_ASSERT (         36        == sizeof(lcs) );

    return true;
}

#ifdef OVR_BUILD_DEBUG
void TestSaveLoadLensConfig ( LensConfig const &config )
{
    OVR_ASSERT ( config.Eqn == Distortion_CatmullRom10 );
    // As a test, make sure this can be encoded and decoded correctly.
    const int bufferSize = 256;
    uint8_t buffer[bufferSize];
    OVR_ASSERT ( SaveLensConfigSizeInBytes ( config ) < bufferSize );
    bool success;
    success = SaveLensConfig ( buffer, bufferSize, config );
    OVR_ASSERT ( success );
    LensConfig testConfig;
    success = LoadLensConfig ( &testConfig, buffer, bufferSize );
    OVR_ASSERT ( success );
    OVR_ASSERT ( testConfig.Eqn == config.Eqn );
    for ( int i = 0; i < 11; i++ )
    {
        OVR_ASSERT ( fabs ( testConfig.K[i] - config.K[i] ) < 0.0001f );
    }
    OVR_ASSERT ( fabsf ( testConfig.MaxR - config.MaxR ) < 0.0001f );
    OVR_ASSERT ( fabsf ( testConfig.MetersPerTanAngleAtCenter - config.MetersPerTanAngleAtCenter ) < 0.00001f );
    for ( int i = 0; i < 4; i++ )
    {
        OVR_ASSERT ( fabsf ( testConfig.ChromaticAberration[i] - config.ChromaticAberration[i] ) < 0.00001f );
    }
}
#endif



//-----------------------------------------------------------------------------------

// TBD: There is a question of whether this is the best file for CreateDebugHMDInfo. As long as there are many
// constants for HmdRenderInfo here as well it is ok. The alternative would be OVR_Common_HMDDevice.cpp, but
// that's specialized per platform... should probably move it there onces the code is in the common base class.

HMDInfo CreateDebugHMDInfo(HmdTypeEnum hmdType)
{
    HMDInfo info;    

    if ((hmdType != HmdType_DK1) &&
        (hmdType != HmdType_CrystalCoveProto) &&
        (hmdType != HmdType_DK2))
    {
        LogText("Debug HMDInfo - HmdType not supported. Defaulting to DK1.\n");
        hmdType = HmdType_DK1;
    }

    // The alternative would be to initialize info.HmdType to HmdType_None instead. If we did that,
    // code wouldn't be "maximally compatible" and devs wouldn't know what device we are
    // simulating... so if differentiation becomes necessary we better add Debug flag in the future.
    info.HmdType      = hmdType;
    info.Manufacturer = "Oculus VR";    

    switch(hmdType)
    {
    case HmdType_DK1:
        info.ProductName                            = "Oculus Rift DK1";
        info.ResolutionInPixels                     = Sizei ( 1280, 800 );
        info.ScreenSizeInMeters                     = Sizef ( 0.1498f, 0.0936f );
        info.ScreenGapSizeInMeters                  = 0.0f;
        info.CenterFromTopInMeters                  = 0.0468f;
        info.LensSeparationInMeters                 = 0.0635f;
        info.Shutter.Type                           = HmdShutter_RollingTopToBottom;
        info.Shutter.VsyncToNextVsync               = ( 1.0f / 60.0f );
        info.Shutter.VsyncToFirstScanline           = 0.000052f;
        info.Shutter.FirstScanlineToLastScanline    = 0.016580f;
        info.Shutter.PixelSettleTime                = 0.015f;
        info.Shutter.PixelPersistence               = ( 1.0f / 60.0f );
        break;

    case HmdType_CrystalCoveProto:
        info.ProductName                            = "Oculus Rift Crystal Cove";        
        info.ResolutionInPixels                     = Sizei ( 1920, 1080 );
        info.ScreenSizeInMeters                     = Sizef ( 0.12576f, 0.07074f );
        info.ScreenGapSizeInMeters                  = 0.0f;
        info.CenterFromTopInMeters                  = info.ScreenSizeInMeters.h * 0.5f;
        info.LensSeparationInMeters                 = 0.0635f;
        info.Shutter.Type                           = HmdShutter_RollingRightToLeft;
        info.Shutter.VsyncToNextVsync               = ( 1.0f / 76.0f );
        info.Shutter.VsyncToFirstScanline           = 0.0000273f;
        info.Shutter.FirstScanlineToLastScanline    = 0.0131033f;
        info.Shutter.PixelSettleTime                = 0.0f;
        info.Shutter.PixelPersistence               = 0.18f * info.Shutter.VsyncToNextVsync;
        break;

    case HmdType_DK2:
        info.ProductName                            = "Oculus Rift DK2";        
        info.ResolutionInPixels                     = Sizei ( 1920, 1080 );
        info.ScreenSizeInMeters                     = Sizef ( 0.12576f, 0.07074f );
        info.ScreenGapSizeInMeters                  = 0.0f;
        info.CenterFromTopInMeters                  = info.ScreenSizeInMeters.h * 0.5f;
        info.LensSeparationInMeters                 = 0.0635f;
        info.Shutter.Type                           = HmdShutter_RollingRightToLeft;
        info.Shutter.VsyncToNextVsync               = ( 1.0f / 76.0f );
        info.Shutter.VsyncToFirstScanline           = 0.0000273f;
        info.Shutter.FirstScanlineToLastScanline    = 0.0131033f;
        info.Shutter.PixelSettleTime                = 0.0f;
        info.Shutter.PixelPersistence               = 0.18f * info.Shutter.VsyncToNextVsync;
        break;

    default:
        break;
    }

    return info;
}

// profile may be NULL, in which case it uses the hard-coded defaults.
HmdRenderInfo GenerateHmdRenderInfoFromHmdInfo ( HMDInfo const &hmdInfo,
                                                 Profile const *profile /*=NULL*/,
                                                 DistortionEqnType distortionType /*= Distortion_CatmullRom10*/,
                                                 EyeCupType eyeCupOverride /*= EyeCup_LAST*/ )
{
    OVR_ASSERT(profile);  // profiles are required
    
    HmdRenderInfo renderInfo;

    renderInfo.HmdType                              = hmdInfo.HmdType;
    renderInfo.ResolutionInPixels                   = hmdInfo.ResolutionInPixels;
    renderInfo.ScreenSizeInMeters                   = hmdInfo.ScreenSizeInMeters;
    renderInfo.CenterFromTopInMeters                = hmdInfo.CenterFromTopInMeters;
    renderInfo.ScreenGapSizeInMeters                = hmdInfo.ScreenGapSizeInMeters;
    renderInfo.LensSeparationInMeters               = hmdInfo.LensSeparationInMeters;

    OVR_ASSERT ( sizeof(renderInfo.Shutter) == sizeof(hmdInfo.Shutter) );   // Try to keep the files in sync!
    renderInfo.Shutter.Type                         = hmdInfo.Shutter.Type;
    renderInfo.Shutter.VsyncToNextVsync             = hmdInfo.Shutter.VsyncToNextVsync;
    renderInfo.Shutter.VsyncToFirstScanline         = hmdInfo.Shutter.VsyncToFirstScanline;
    renderInfo.Shutter.FirstScanlineToLastScanline  = hmdInfo.Shutter.FirstScanlineToLastScanline;
    renderInfo.Shutter.PixelSettleTime              = hmdInfo.Shutter.PixelSettleTime;
    renderInfo.Shutter.PixelPersistence             = hmdInfo.Shutter.PixelPersistence;

    renderInfo.LensDiameterInMeters                 = 0.035f;
    renderInfo.LensSurfaceToMidplateInMeters        = 0.025f;
    renderInfo.EyeCups                              = EyeCup_DK1A;

#if 0       // Device settings are out of date - don't use them.
    if (Contents & Contents_Distortion)
    {
        memcpy(renderInfo.DistortionK, DistortionK, sizeof(float)*4);
        renderInfo.DistortionEqn = Distortion_RecipPoly4;
    }
#endif

    // Defaults in case of no user profile.
    renderInfo.EyeLeft.NoseToPupilInMeters   = 0.032f;
    renderInfo.EyeLeft.ReliefInMeters        = 0.012f;

    // 10mm eye-relief laser numbers for DK1 lenses.
    // These are a decent seed for finding eye-relief and IPD.
    // These are NOT used for rendering!
    // Rendering distortions are now in GenerateLensConfigFromEyeRelief()
    // So, if you're hacking in new distortions, don't do it here!
    renderInfo.EyeLeft.Distortion.SetToIdentity();
    renderInfo.EyeLeft.Distortion.MetersPerTanAngleAtCenter = 0.0449f;
    renderInfo.EyeLeft.Distortion.Eqn       = Distortion_RecipPoly4;
    renderInfo.EyeLeft.Distortion.K[0]      =  1.0f;
    renderInfo.EyeLeft.Distortion.K[1]      = -0.494165344f;
    renderInfo.EyeLeft.Distortion.K[2]      = 0.587046423f;
    renderInfo.EyeLeft.Distortion.K[3]      = -0.841887126f;
    renderInfo.EyeLeft.Distortion.MaxR      = 1.0f;

    renderInfo.EyeLeft.Distortion.ChromaticAberration[0] = -0.006f;
    renderInfo.EyeLeft.Distortion.ChromaticAberration[1] =  0.0f;
    renderInfo.EyeLeft.Distortion.ChromaticAberration[2] =  0.014f;
    renderInfo.EyeLeft.Distortion.ChromaticAberration[3] =  0.0f;

    renderInfo.EyeRight = renderInfo.EyeLeft;

    // Obtain data from profile.
    char eyecup[16];
    if (profile->GetValue(OVR_KEY_EYE_CUP, eyecup, 16))
    {
        SetEyeCup(&renderInfo, eyecup);
    }
    
    switch ( hmdInfo.HmdType )
    {
    case HmdType_None:
    case HmdType_DKProto:
    case HmdType_DK1:
        // Slight hack to improve usability.
        // If you have a DKHD-style lens profile enabled,
        // but you plug in DK1 and forget to change the profile,
        // obviously you don't want those lens numbers.
        if ( ( renderInfo.EyeCups != EyeCup_DK1A ) &&
             ( renderInfo.EyeCups != EyeCup_DK1B ) &&
             ( renderInfo.EyeCups != EyeCup_DK1C ) )
        {
            renderInfo.EyeCups = EyeCup_DK1A;
        }
        break;

    case HmdType_DKHD2Proto:
        renderInfo.EyeCups = EyeCup_DKHD2A;
        break;
    case HmdType_CrystalCoveProto:
        renderInfo.EyeCups = EyeCup_PinkA;
        break;
    case HmdType_DK2:
        renderInfo.EyeCups = EyeCup_DK2A;
        break;
    default:
        break;
    }

    if ( eyeCupOverride != EyeCup_LAST )
    {
        renderInfo.EyeCups = eyeCupOverride;
    }

    switch ( renderInfo.EyeCups )
    {
    case EyeCup_DK1A:
    case EyeCup_DK1B:
    case EyeCup_DK1C:
        renderInfo.LensDiameterInMeters                   = 0.035f;
        renderInfo.LensSurfaceToMidplateInMeters          = 0.02357f;
        // Not strictly lens-specific, but still wise to set a reasonable default for relief.
        renderInfo.EyeLeft.ReliefInMeters                 = 0.010f; 
        renderInfo.EyeRight.ReliefInMeters                = 0.010f; 
        break;
    case EyeCup_DKHD2A:
        renderInfo.LensDiameterInMeters                   = 0.035f;
        renderInfo.LensSurfaceToMidplateInMeters          = 0.02357f;
        // Not strictly lens-specific, but still wise to set a reasonable default for relief.
        renderInfo.EyeLeft.ReliefInMeters                 = 0.010f; 
        renderInfo.EyeRight.ReliefInMeters                = 0.010f; 
        break;
    case EyeCup_PinkA:
    case EyeCup_DK2A:
        renderInfo.LensDiameterInMeters                   = 0.04f;      // approximate
        renderInfo.LensSurfaceToMidplateInMeters          = 0.01965f;
        // Not strictly lens-specific, but still wise to set a reasonable default for relief.
        renderInfo.EyeLeft.ReliefInMeters                 = 0.012f;
        renderInfo.EyeRight.ReliefInMeters                = 0.012f;
        break;
    default: OVR_ASSERT ( false ); break;
    }

    Profile* def = ProfileManager::GetInstance()->GetDefaultProfile(hmdInfo.HmdType);

    // Set the eye position
    // Use the user profile value unless they have elected to use the defaults
    if (!profile->GetBoolValue(OVR_KEY_CUSTOM_EYE_RENDER, true))
        profile = def;  // use the default

    char user[32];
    profile->GetValue(OVR_KEY_USER, user, 32);   // for debugging purposes

    // TBD: Maybe we should separate custom camera positioning from custom distortion rendering ??
    float eye2nose[2] = { OVR_DEFAULT_IPD / 2, OVR_DEFAULT_IPD / 2 };
    if (profile->GetFloatValues(OVR_KEY_EYE_TO_NOSE_DISTANCE, eye2nose, 2) == 2)
    {   
        renderInfo.EyeLeft.NoseToPupilInMeters = eye2nose[0];
        renderInfo.EyeRight.NoseToPupilInMeters = eye2nose[1];
    }
    else
    {   // Legacy profiles may not include half-ipd, so use the regular IPD value instead
        float ipd = profile->GetFloatValue(OVR_KEY_IPD, OVR_DEFAULT_IPD);
        renderInfo.EyeLeft.NoseToPupilInMeters = 0.5f * ipd;
        renderInfo.EyeRight.NoseToPupilInMeters = 0.5f * ipd;
    }
        
    float eye2plate[2];
    if ((profile->GetFloatValues(OVR_KEY_MAX_EYE_TO_PLATE_DISTANCE, eye2plate, 2) == 2) ||
        (def->GetFloatValues(OVR_KEY_MAX_EYE_TO_PLATE_DISTANCE, eye2plate, 2) == 2))
    {   // Subtract the eye-cup height from the plate distance to get the eye-to-lens distance
        // This measurement should be the the distance at maximum dial setting
        // We still need to adjust with the dial offset
        renderInfo.EyeLeft.ReliefInMeters = eye2plate[0] - renderInfo.LensSurfaceToMidplateInMeters;
        renderInfo.EyeRight.ReliefInMeters = eye2plate[1] - renderInfo.LensSurfaceToMidplateInMeters;

        // Adjust the eye relief with the dial setting (from the assumed max eye relief)
        int dial = profile->GetIntValue(OVR_KEY_EYE_RELIEF_DIAL, OVR_DEFAULT_EYE_RELIEF_DIAL);
        renderInfo.EyeLeft.ReliefInMeters -= ((10 - dial) * 0.001f);
        renderInfo.EyeRight.ReliefInMeters -= ((10 - dial) * 0.001f);
    }
    else
    {
        // We shouldn't be here.  The user or default profile should have the eye relief
        OVR_ASSERT(false);

        // Set the eye relief with the user configured dial setting
		//int dial = profile->GetIntValue(OVR_KEY_EYE_RELIEF_DIAL, OVR_DEFAULT_EYE_RELIEF_DIAL);

        // Assume a default of 7 to 17 mm eye relief based on the dial.  This corresponds
        // to the sampled and tuned distortion range on the DK1.
        //renderInfo.EyeLeft.ReliefInMeters = 0.007f + (dial * 0.001f);
        //renderInfo.EyeRight.ReliefInMeters = 0.007f + (dial * 0.001f);
    }

    def->Release();


    // Now we know where the eyes are relative to the lenses, we can compute a distortion for each.
    // TODO: incorporate lateral offset in distortion generation.
    // TODO: we used a distortion to calculate eye-relief, and now we're making a distortion from that eye-relief. Close the loop!

    for ( int eyeNum = 0; eyeNum < 2; eyeNum++ )
    {
        HmdRenderInfo::EyeConfig *pHmdEyeConfig = ( eyeNum == 0 ) ? &(renderInfo.EyeLeft) : &(renderInfo.EyeRight);

        float eye_relief = pHmdEyeConfig->ReliefInMeters;
        LensConfig distortionConfig = GenerateLensConfigFromEyeRelief ( eye_relief, renderInfo, distortionType );
        pHmdEyeConfig->Distortion = distortionConfig;
    }

    return renderInfo;
}


LensConfig GenerateLensConfigFromEyeRelief ( float eyeReliefInMeters, HmdRenderInfo const &hmd, DistortionEqnType distortionType /*= Distortion_CatmullRom10*/ )
{
    struct DistortionDescriptor
    {
        float EyeRelief;
        // The three places we're going to sample & lerp the curve at.
        // One sample is always at 0.0, and the distortion scale should be 1.0 or else!
        // Only use for poly4 numbers - CR has an implicit scale.
        float SampleRadius[3];
        // Where the distortion has actually been measured/calibrated out to.
        // Don't try to hallucinate data out beyond here.
        float MaxRadius;
        // The config itself.
        LensConfig Config;
    };

	static const int MaxDistortions = 10;
	DistortionDescriptor distortions[MaxDistortions];
	for (int i = 0; i < MaxDistortions; i++)
    {
        distortions[i].Config.SetToIdentity(); // Note: This line causes a false static analysis error -cat
        distortions[i].EyeRelief = 0.0f;
        distortions[i].MaxRadius = 1.0f;
    }
    int numDistortions = 0;
    int defaultDistortion = 0;     // index of the default distortion curve to use if zero eye relief supplied

    if ( ( hmd.EyeCups == EyeCup_DK1A ) ||
         ( hmd.EyeCups == EyeCup_DK1B ) ||
         ( hmd.EyeCups == EyeCup_DK1C ) )
    {

        numDistortions = 0;
                
        // Tuned at minimum dial setting - extended to r^2 == 1.8
        distortions[numDistortions].Config.Eqn = Distortion_CatmullRom10;
        distortions[numDistortions].EyeRelief                            = 0.012760465f - 0.005f;
        distortions[numDistortions].Config.MetersPerTanAngleAtCenter     = 0.0425f;
        distortions[numDistortions].Config.K[0]                          = 1.0000f;
        distortions[numDistortions].Config.K[1]                          = 1.06505f;
        distortions[numDistortions].Config.K[2]                          = 1.14725f;
        distortions[numDistortions].Config.K[3]                          = 1.2705f;
        distortions[numDistortions].Config.K[4]                          = 1.48f;
        distortions[numDistortions].Config.K[5]                          = 1.87f;
        distortions[numDistortions].Config.K[6]                          = 2.534f;
        distortions[numDistortions].Config.K[7]                          = 3.6f;
        distortions[numDistortions].Config.K[8]                          = 5.1f;
        distortions[numDistortions].Config.K[9]                          = 7.4f;
        distortions[numDistortions].Config.K[10]                         = 11.0f;
        distortions[numDistortions].MaxRadius                            = sqrt(1.8f);
        defaultDistortion = numDistortions;                      // this is the default
        numDistortions++;
        
        // Tuned at middle dial setting
        distortions[numDistortions].Config.Eqn = Distortion_CatmullRom10;
        distortions[numDistortions].EyeRelief                            = 0.012760465f;  // my average eye-relief
        distortions[numDistortions].Config.MetersPerTanAngleAtCenter     = 0.0425f;
        distortions[numDistortions].Config.K[0]                          = 1.0f;
        distortions[numDistortions].Config.K[1]                          = 1.032407264f;
        distortions[numDistortions].Config.K[2]                          = 1.07160462f;
        distortions[numDistortions].Config.K[3]                          = 1.11998388f;
        distortions[numDistortions].Config.K[4]                          = 1.1808606f;
        distortions[numDistortions].Config.K[5]                          = 1.2590494f;
        distortions[numDistortions].Config.K[6]                          = 1.361915f;
        distortions[numDistortions].Config.K[7]                          = 1.5014339f;
        distortions[numDistortions].Config.K[8]                          = 1.6986004f;
        distortions[numDistortions].Config.K[9]                          = 1.9940577f;
        distortions[numDistortions].Config.K[10]                         = 2.4783147f;
        distortions[numDistortions].MaxRadius                            = 1.0f;
        numDistortions++;

        // Tuned at maximum dial setting
        distortions[numDistortions].Config.Eqn = Distortion_CatmullRom10;
        distortions[numDistortions].EyeRelief                            = 0.012760465f + 0.005f;
        distortions[numDistortions].Config.MetersPerTanAngleAtCenter     = 0.0425f;
        distortions[numDistortions].Config.K[0]                          = 1.0102f;
        distortions[numDistortions].Config.K[1]                          = 1.0371f;
        distortions[numDistortions].Config.K[2]                          = 1.0831f;
        distortions[numDistortions].Config.K[3]                          = 1.1353f;
        distortions[numDistortions].Config.K[4]                          = 1.2f;
        distortions[numDistortions].Config.K[5]                          = 1.2851f;
        distortions[numDistortions].Config.K[6]                          = 1.3979f;
        distortions[numDistortions].Config.K[7]                          = 1.56f;
        distortions[numDistortions].Config.K[8]                          = 1.8f;
        distortions[numDistortions].Config.K[9]                          = 2.25f;
        distortions[numDistortions].Config.K[10]                         = 3.0f;
        distortions[numDistortions].MaxRadius                            = 1.0f;
        numDistortions++;
        

        
        // Chromatic aberration doesn't seem to change with eye relief.
        for ( int i = 0; i < numDistortions; i++ )
        {
            distortions[i].Config.ChromaticAberration[0]        = -0.006f;
            distortions[i].Config.ChromaticAberration[1]        =  0.0f;
            distortions[i].Config.ChromaticAberration[2]        =  0.014f;
            distortions[i].Config.ChromaticAberration[3]        =  0.0f;
        }
    }
    else if ( hmd.EyeCups == EyeCup_DKHD2A )
    {
        // Tuned DKHD2 lens
        numDistortions = 0;
       
        distortions[numDistortions].Config.Eqn = Distortion_CatmullRom10;
        distortions[numDistortions].EyeRelief                            = 0.010f;
        distortions[numDistortions].Config.MetersPerTanAngleAtCenter     = 0.0425f;
        distortions[numDistortions].Config.K[0]                          = 1.0f;
        distortions[numDistortions].Config.K[1]                          = 1.0425f;
        distortions[numDistortions].Config.K[2]                          = 1.0826f;
        distortions[numDistortions].Config.K[3]                          = 1.130f;
        distortions[numDistortions].Config.K[4]                          = 1.185f;
        distortions[numDistortions].Config.K[5]                          = 1.250f;
        distortions[numDistortions].Config.K[6]                          = 1.338f;
        distortions[numDistortions].Config.K[7]                          = 1.455f;
        distortions[numDistortions].Config.K[8]                          = 1.620f;
        distortions[numDistortions].Config.K[9]                          = 1.840f;
        distortions[numDistortions].Config.K[10]                         = 2.200f;
        distortions[numDistortions].MaxRadius                            = 1.0f;
        
        defaultDistortion = numDistortions;   // this is the default
        numDistortions++;

        distortions[numDistortions] = distortions[0];
        distortions[numDistortions].EyeRelief = 0.020f;
        numDistortions++;

        // Chromatic aberration doesn't seem to change with eye relief.
        for ( int i = 0; i < numDistortions; i++ )
        {
            distortions[i].Config.ChromaticAberration[0]        = -0.006f;
            distortions[i].Config.ChromaticAberration[1]        =  0.0f;
            distortions[i].Config.ChromaticAberration[2]        =  0.014f;
            distortions[i].Config.ChromaticAberration[3]        =  0.0f;
        }
    }
    else if ( hmd.EyeCups == EyeCup_PinkA || hmd.EyeCups == EyeCup_DK2A )
    {
        // Tuned Crystal Cove & DK2 Lens (CES & GDC)
        numDistortions = 0;
       
        
        distortions[numDistortions].EyeRelief                            = 0.008f;
        distortions[numDistortions].Config.MetersPerTanAngleAtCenter     = 0.036f;
        // TODO: Need to retune this distortion for minimum eye relief
        distortions[numDistortions].Config.Eqn = Distortion_CatmullRom10;
        distortions[numDistortions].Config.K[0]                          = 1.003f;
        distortions[numDistortions].Config.K[1]                          = 1.02f;
        distortions[numDistortions].Config.K[2]                          = 1.042f;
        distortions[numDistortions].Config.K[3]                          = 1.066f;
        distortions[numDistortions].Config.K[4]                          = 1.094f;
        distortions[numDistortions].Config.K[5]                          = 1.126f;
        distortions[numDistortions].Config.K[6]                          = 1.162f;
        distortions[numDistortions].Config.K[7]                          = 1.203f;
        distortions[numDistortions].Config.K[8]                          = 1.25f;
        distortions[numDistortions].Config.K[9]                          = 1.31f;
        distortions[numDistortions].Config.K[10]                         = 1.38f;
        distortions[numDistortions].MaxRadius                            = 1.0f;
        
        distortions[numDistortions].Config.ChromaticAberration[0]        = -0.0112f;
        distortions[numDistortions].Config.ChromaticAberration[1]        = -0.015f;
        distortions[numDistortions].Config.ChromaticAberration[2]        =  0.0187f;
        distortions[numDistortions].Config.ChromaticAberration[3]        =  0.015f;
        
        numDistortions++;





        distortions[numDistortions].EyeRelief                            = 0.018f;
        distortions[numDistortions].Config.MetersPerTanAngleAtCenter     = 0.036f;

        distortions[numDistortions].Config.Eqn = Distortion_CatmullRom10;
        distortions[numDistortions].Config.K[0]                          = 1.003f;
        distortions[numDistortions].Config.K[1]                          = 1.02f;
        distortions[numDistortions].Config.K[2]                          = 1.042f;
        distortions[numDistortions].Config.K[3]                          = 1.066f;
        distortions[numDistortions].Config.K[4]                          = 1.094f;
        distortions[numDistortions].Config.K[5]                          = 1.126f;
        distortions[numDistortions].Config.K[6]                          = 1.162f;
        distortions[numDistortions].Config.K[7]                          = 1.203f;
        distortions[numDistortions].Config.K[8]                          = 1.25f;
        distortions[numDistortions].Config.K[9]                          = 1.31f;
        distortions[numDistortions].Config.K[10]                         = 1.38f;
        distortions[numDistortions].MaxRadius                            = 1.0f;
        
        distortions[numDistortions].Config.ChromaticAberration[0]        = -0.015f;
        distortions[numDistortions].Config.ChromaticAberration[1]        = -0.02f;
        distortions[numDistortions].Config.ChromaticAberration[2]        =  0.025f;
        distortions[numDistortions].Config.ChromaticAberration[3]        =  0.02f;
        
        defaultDistortion = numDistortions;   // this is the default
        numDistortions++;
        
        /*
        // Orange Lens on DK2
        distortions[numDistortions].EyeRelief                            = 0.010f;
        distortions[numDistortions].Config.MetersPerTanAngleAtCenter     = 0.031f;

        distortions[numDistortions].Config.Eqn = Distortion_CatmullRom10;
        distortions[numDistortions].Config.K[0]                          = 1.00f;
        distortions[numDistortions].Config.K[1]                          = 1.0169f;
        distortions[numDistortions].Config.K[2]                          = 1.0378f;
        distortions[numDistortions].Config.K[3]                          = 1.0648f;
        distortions[numDistortions].Config.K[4]                          = 1.0990f;
        distortions[numDistortions].Config.K[5]                          = 1.141f;
        distortions[numDistortions].Config.K[6]                          = 1.192f;
        distortions[numDistortions].Config.K[7]                          = 1.255f;
        distortions[numDistortions].Config.K[8]                          = 1.335f;
        distortions[numDistortions].Config.K[9]                          = 1.435f;
        distortions[numDistortions].Config.K[10]                         = 1.56f;
        distortions[numDistortions].MaxRadius                            = 1.0f;
        */
    }
    else
    {
        // Unknown lens.
        // Use DK1 black lens settings, just so we can continue to run with something.
        distortions[0].EyeRelief = 0.005f;
        distortions[0].Config.MetersPerTanAngleAtCenter = 0.043875f;
        distortions[0].Config.Eqn = Distortion_RecipPoly4;
        distortions[0].Config.K[0] = 1.0f;
        distortions[0].Config.K[1] = -0.3999f;
        distortions[0].Config.K[2] =  0.2408f;
        distortions[0].Config.K[3] = -0.4589f;
        distortions[0].SampleRadius[0] = 0.2f;
        distortions[0].SampleRadius[1] = 0.4f;
        distortions[0].SampleRadius[2] = 0.6f;

        distortions[1] = distortions[0];
        distortions[1].EyeRelief = 0.010f;
        numDistortions = 2;

        // Chromatic aberration doesn't seem to change with eye relief.
        for ( int i = 0; i < numDistortions; i++ )
        {
            // These are placeholder, they have not been tuned!
            distortions[i].Config.ChromaticAberration[0]        =  0.0f;
            distortions[i].Config.ChromaticAberration[1]        =  0.0f;
            distortions[i].Config.ChromaticAberration[2]        =  0.0f;
            distortions[i].Config.ChromaticAberration[3]        =  0.0f;
        }
    }

	OVR_ASSERT(numDistortions < MaxDistortions);

    DistortionDescriptor *pUpper = NULL;
    DistortionDescriptor *pLower = NULL;
    float lerpVal = 0.0f;
    if (eyeReliefInMeters == 0)
    {   // Use a constant default distortion if an invalid eye-relief is supplied
        pLower = &(distortions[defaultDistortion]);
        pUpper = &(distortions[defaultDistortion]);
        lerpVal = 0.0f;
    }
    else
    {
        for ( int i = 0; i < numDistortions-1; i++ )
        {
            OVR_ASSERT ( distortions[i].EyeRelief < distortions[i+1].EyeRelief );
            if ( ( distortions[i].EyeRelief <= eyeReliefInMeters ) && ( distortions[i+1].EyeRelief > eyeReliefInMeters ) )
            {
                pLower = &(distortions[i]);
                pUpper = &(distortions[i+1]);
                lerpVal = ( eyeReliefInMeters - pLower->EyeRelief ) / ( pUpper->EyeRelief - pLower->EyeRelief );
                // No break here - I want the ASSERT to check everything every time!
            }
        }
    }

    if ( pUpper == NULL )
    {
#if 0
        // Outside the range, so extrapolate rather than interpolate.
        if ( distortions[0].EyeRelief > eyeReliefInMeters )
        { 
            pLower = &(distortions[0]);
            pUpper = &(distortions[1]);
        }
        else
        {
            OVR_ASSERT ( distortions[numDistortions-1].EyeRelief <= eyeReliefInMeters );
            pLower = &(distortions[numDistortions-2]);
            pUpper = &(distortions[numDistortions-1]);
        }
        lerpVal = ( eyeReliefInMeters - pLower->EyeRelief ) / ( pUpper->EyeRelief - pLower->EyeRelief );
#else
        // Do not extrapolate, just clamp - slightly worried about people putting in bogus settings.
        if ( distortions[0].EyeRelief > eyeReliefInMeters )
        {
            pLower = &(distortions[0]);
            pUpper = &(distortions[0]);
        }
        else
        {
            OVR_ASSERT ( distortions[numDistortions-1].EyeRelief <= eyeReliefInMeters );
            pLower = &(distortions[numDistortions-1]);
            pUpper = &(distortions[numDistortions-1]);
        }
        lerpVal = 0.0f;
#endif
    }
    float invLerpVal = 1.0f - lerpVal;

    pLower->Config.MaxR = pLower->MaxRadius;
    pUpper->Config.MaxR = pUpper->MaxRadius;

    LensConfig result;
    // Where is the edge of the lens - no point modelling further than this.
    float maxValidRadius = invLerpVal * pLower->MaxRadius + lerpVal * pUpper->MaxRadius;
    result.MaxR = maxValidRadius;

    switch ( distortionType )
    {
    case Distortion_Poly4:
        // Deprecated
        OVR_ASSERT ( false );
        break;
    case Distortion_RecipPoly4:{
        // Lerp control points and fit an equation to them.
        float fitX[4];
        float fitY[4];
        fitX[0] = 0.0f;
        fitY[0] = 1.0f;
        for ( int ctrlPt = 1; ctrlPt < 4; ctrlPt ++ )
        {
            float radiusLerp = invLerpVal * pLower->SampleRadius[ctrlPt-1] + lerpVal * pUpper->SampleRadius[ctrlPt-1];
            float radiusLerpSq = radiusLerp * radiusLerp;
            float fitYLower = pLower->Config.DistortionFnScaleRadiusSquared ( radiusLerpSq );
            float fitYUpper = pUpper->Config.DistortionFnScaleRadiusSquared ( radiusLerpSq );
            fitX[ctrlPt] = radiusLerpSq;
            fitY[ctrlPt] = 1.0f / ( invLerpVal * fitYLower + lerpVal * fitYUpper );
        }

        result.Eqn = Distortion_RecipPoly4;
        bool bSuccess = FitCubicPolynomial ( result.K, fitX, fitY );
        OVR_ASSERT ( bSuccess );
        OVR_UNUSED ( bSuccess );

        // Set up the fast inverse.
        float maxRDist = result.DistortionFn ( maxValidRadius );
        result.MaxInvR = maxRDist;
        result.SetUpInverseApprox();

        }break;

    case Distortion_CatmullRom10:{

        // Evenly sample & lerp points on the curve.
        const int NumSegments = LensConfig::NumCoefficients;
        result.MaxR = maxValidRadius;
        // Directly interpolate the K0 values
        result.K[0] = invLerpVal * pLower->Config.K[0] + lerpVal * pUpper->Config.K[0];

        // Sample and interpolate the distortion curves to derive K[1] ... K[n]
        for ( int ctrlPt = 1; ctrlPt < NumSegments; ctrlPt++ )
        {
            float radiusSq = ( (float)ctrlPt / (float)(NumSegments-1) ) * maxValidRadius * maxValidRadius;
            float fitYLower = pLower->Config.DistortionFnScaleRadiusSquared ( radiusSq );
            float fitYUpper = pUpper->Config.DistortionFnScaleRadiusSquared ( radiusSq );
            float fitLerp = invLerpVal * fitYLower + lerpVal * fitYUpper;
            result.K[ctrlPt] = fitLerp;
        }

        result.Eqn = Distortion_CatmullRom10;

        for ( int ctrlPt = 1; ctrlPt < NumSegments; ctrlPt++ )
        {
            float radiusSq = ( (float)ctrlPt / (float)(NumSegments-1) ) * maxValidRadius * maxValidRadius;
            float val = result.DistortionFnScaleRadiusSquared ( radiusSq );            
            OVR_ASSERT ( Alg::Abs ( val - result.K[ctrlPt] ) < 0.0001f );
            OVR_UNUSED1(val); // For release build.
        }

        // Set up the fast inverse.
        float maxRDist = result.DistortionFn ( maxValidRadius );
        result.MaxInvR = maxRDist;
        result.SetUpInverseApprox();

        }break;

    default: OVR_ASSERT ( false ); break;
    }


    // Chromatic aberration.
    result.ChromaticAberration[0] = invLerpVal * pLower->Config.ChromaticAberration[0] + lerpVal * pUpper->Config.ChromaticAberration[0];
    result.ChromaticAberration[1] = invLerpVal * pLower->Config.ChromaticAberration[1] + lerpVal * pUpper->Config.ChromaticAberration[1];
    result.ChromaticAberration[2] = invLerpVal * pLower->Config.ChromaticAberration[2] + lerpVal * pUpper->Config.ChromaticAberration[2];
    result.ChromaticAberration[3] = invLerpVal * pLower->Config.ChromaticAberration[3] + lerpVal * pUpper->Config.ChromaticAberration[3];

    // Scale.
    result.MetersPerTanAngleAtCenter =  pLower->Config.MetersPerTanAngleAtCenter * invLerpVal +
                                        pUpper->Config.MetersPerTanAngleAtCenter * lerpVal;
    /*
    // Commented out - Causes ASSERT with no HMD plugged in
#ifdef OVR_BUILD_DEBUG
    if ( distortionType == Distortion_CatmullRom10 )
    {
        TestSaveLoadLensConfig ( result );
    }
#endif
    */
    return result;
}


DistortionRenderDesc CalculateDistortionRenderDesc ( StereoEye eyeType, HmdRenderInfo const &hmd,
                                                     const LensConfig *pLensOverride /*= NULL */ )
{
    // From eye relief, IPD and device characteristics, we get the distortion mapping.
    // This distortion does the following things:
    // 1. It undoes the distortion that happens at the edges of the lens.
    // 2. It maps the undistorted field into "retina" space.
    // So the input is a pixel coordinate - the physical pixel on the display itself.
    // The output is the real-world direction of the ray from this pixel as it comes out of the lens and hits the eye.
    // However we typically think of rays "coming from" the eye, so the direction (TanAngleX,TanAngleY,1) is the direction
    //      that the pixel appears to be in real-world space, where AngleX and AngleY are relative to the straight-ahead vector.
    // If your renderer is a raytracer, you can use this vector directly (normalize as appropriate).
    // However in standard rasterisers, we have rendered a 2D image and are putting it in front of the eye,
    //      so we then need a mapping from this space to the [-1,1] UV coordinate space, which depends on exactly
    //      where "in space" the app wants to put that rendertarget.
    //      Where in space, and how large this rendertarget is, is completely up to the app and/or user,
    //      though of course we can provide some useful hints.

    // TODO: Use IPD and eye relief to modify distortion (i.e. non-radial component)
    // TODO: cope with lenses that don't produce collimated light.
    //       This means that IPD relative to the lens separation changes the light vergence,
    //       and so we actually need to change where the image is displayed.

    const HmdRenderInfo::EyeConfig &hmdEyeConfig = ( eyeType == StereoEye_Left ) ? hmd.EyeLeft : hmd.EyeRight;

    DistortionRenderDesc localDistortion;
    localDistortion.Lens = hmdEyeConfig.Distortion;

    if ( pLensOverride != NULL )
    {
        localDistortion.Lens = *pLensOverride;
    }

    Sizef pixelsPerMeter(hmd.ResolutionInPixels.w / ( hmd.ScreenSizeInMeters.w - hmd.ScreenGapSizeInMeters ),
                         hmd.ResolutionInPixels.h / hmd.ScreenSizeInMeters.h);

    localDistortion.PixelsPerTanAngleAtCenter = (pixelsPerMeter * localDistortion.Lens.MetersPerTanAngleAtCenter).ToVector();
    // Same thing, scaled to [-1,1] for each eye, rather than pixels.

    localDistortion.TanEyeAngleScale = Vector2f(0.25f, 0.5f).EntrywiseMultiply(
                                       (hmd.ScreenSizeInMeters / localDistortion.Lens.MetersPerTanAngleAtCenter).ToVector());
    
    // <--------------left eye------------------><-ScreenGapSizeInMeters-><--------------right eye----------------->
    // <------------------------------------------ScreenSizeInMeters.Width----------------------------------------->
    //                            <----------------LensSeparationInMeters--------------->
    // <--centerFromLeftInMeters->
    //                            ^
    //                      Center of lens

    // Find the lens centers in scale of [-1,+1] (NDC) in left eye.
    float visibleWidthOfOneEye = 0.5f * ( hmd.ScreenSizeInMeters.w - hmd.ScreenGapSizeInMeters );
    float centerFromLeftInMeters = ( hmd.ScreenSizeInMeters.w - hmd.LensSeparationInMeters ) * 0.5f;
    localDistortion.LensCenter.x = (     centerFromLeftInMeters / visibleWidthOfOneEye          ) * 2.0f - 1.0f;
    localDistortion.LensCenter.y = ( hmd.CenterFromTopInMeters  / hmd.ScreenSizeInMeters.h ) * 2.0f - 1.0f;
    if ( eyeType == StereoEye_Right )
    {
        localDistortion.LensCenter.x = -localDistortion.LensCenter.x;
    }

    return localDistortion;
}

FovPort CalculateFovFromEyePosition ( float eyeReliefInMeters,
                                      float offsetToRightInMeters,
                                      float offsetDownwardsInMeters,
                                      float lensDiameterInMeters,
                                      float extraEyeRotationInRadians /*= 0.0f*/ )
{
    // 2D view of things:
    //       |-|            <--- offsetToRightInMeters (in this case, it is negative)
    // |=======C=======|    <--- lens surface (C=center)
    //  \    |       _/
    //   \   R     _/
    //    \  |   _/
    //     \ | _/
    //      \|/
    //       O  <--- center of pupil

    // (technically the lens is round rather than square, so it's not correct to
    // separate vertical and horizontal like this, but it's close enough)
    float halfLensDiameter = lensDiameterInMeters * 0.5f;
    FovPort fovPort;
    fovPort.UpTan    = ( halfLensDiameter + offsetDownwardsInMeters ) / eyeReliefInMeters;
    fovPort.DownTan  = ( halfLensDiameter - offsetDownwardsInMeters ) / eyeReliefInMeters;
    fovPort.LeftTan  = ( halfLensDiameter + offsetToRightInMeters   ) / eyeReliefInMeters;
    fovPort.RightTan = ( halfLensDiameter - offsetToRightInMeters   ) / eyeReliefInMeters;

    if ( extraEyeRotationInRadians > 0.0f )
    {
        // That's the basic looking-straight-ahead eye position relative to the lens.
        // But if you look left, the pupil moves left as the eyeball rotates, which
        // means you can see more to the right than this geometry suggests.
        // So add in the bounds for the extra movement of the pupil.

        // Beyond 30 degrees does not increase FOV because the pupil starts moving backwards more than sideways.
        extraEyeRotationInRadians = Alg::Min ( DegreeToRad ( 30.0f ), Alg::Max ( 0.0f, extraEyeRotationInRadians ) );
        
        // The rotation of the eye is a bit more complex than a simple circle.  The center of rotation
        // at 13.5mm from cornea is slightly further back than the actual center of the eye.
        // Additionally the rotation contains a small lateral component as the muscles pull the eye
        const float eyeballCenterToPupil = 0.0135f;  // center of eye rotation
        const float eyeballLateralPull = 0.001f * (extraEyeRotationInRadians / DegreeToRad ( 30.0f));  // lateral motion as linear function 
        float extraTranslation = eyeballCenterToPupil * sinf ( extraEyeRotationInRadians ) + eyeballLateralPull;
        float extraRelief = eyeballCenterToPupil * ( 1.0f - cosf ( extraEyeRotationInRadians ) );

        fovPort.UpTan    = Alg::Max ( fovPort.UpTan   , ( halfLensDiameter + offsetDownwardsInMeters + extraTranslation ) / ( eyeReliefInMeters + extraRelief ) );
        fovPort.DownTan  = Alg::Max ( fovPort.DownTan , ( halfLensDiameter - offsetDownwardsInMeters + extraTranslation ) / ( eyeReliefInMeters + extraRelief ) );
        fovPort.LeftTan  = Alg::Max ( fovPort.LeftTan , ( halfLensDiameter + offsetToRightInMeters   + extraTranslation ) / ( eyeReliefInMeters + extraRelief ) );
        fovPort.RightTan = Alg::Max ( fovPort.RightTan, ( halfLensDiameter - offsetToRightInMeters   + extraTranslation ) / ( eyeReliefInMeters + extraRelief ) );
    }

    return fovPort;
}



FovPort CalculateFovFromHmdInfo ( StereoEye eyeType,
                                  DistortionRenderDesc const &distortion,
                                  HmdRenderInfo const &hmd,
                                  float extraEyeRotationInRadians /*= 0.0f*/ )
{
    FovPort fovPort;
    float eyeReliefInMeters;
    float offsetToRightInMeters;
    if ( eyeType == StereoEye_Right )
    {
        eyeReliefInMeters     = hmd.EyeRight.ReliefInMeters;
        offsetToRightInMeters = hmd.EyeRight.NoseToPupilInMeters - 0.5f * hmd.LensSeparationInMeters;
    }
    else
    {
        eyeReliefInMeters     = hmd.EyeLeft.ReliefInMeters;
        offsetToRightInMeters = -(hmd.EyeLeft.NoseToPupilInMeters - 0.5f * hmd.LensSeparationInMeters);
    }

    // Limit the eye-relief to 6 mm for FOV calculations since this just tends to spread off-screen
    // and get clamped anyways on DK1 (but in Unity it continues to spreads and causes 
    // unnecessarily large render targets)
    eyeReliefInMeters = Alg::Max(eyeReliefInMeters, 0.006f);

    // Central view.
    fovPort = CalculateFovFromEyePosition ( eyeReliefInMeters,
                                            offsetToRightInMeters,
                                            0.0f,
                                            hmd.LensDiameterInMeters,
                                            extraEyeRotationInRadians );

    // clamp to the screen
    fovPort = ClampToPhysicalScreenFov ( eyeType, distortion, fovPort );
       
    return fovPort;
}



FovPort GetPhysicalScreenFov ( StereoEye eyeType, DistortionRenderDesc const &distortion )
{
    OVR_UNUSED1 ( eyeType );

    FovPort resultFovPort;

    // Figure out the boundaries of the screen. We take the middle pixel of the screen,
    // move to each of the four screen edges, and transform those back into TanAngle space.
    Vector2f dmiddle = distortion.LensCenter;

    // The gotcha is that for some distortion functions, the map will "wrap around"
    // for screen pixels that are not actually visible to the user (especially on DK1,
    // which has a lot of invisible pixels), and map to pixels that are close to the middle.
    // This means the edges of the screen will actually be
    // "closer" than the visible bounds, so we'll clip too aggressively.

    // Solution - step gradually towards the boundary, noting the maximum distance.
    struct FunctionHider
    {
        static FovPort FindRange ( Vector2f from, Vector2f to, int numSteps,
                                          DistortionRenderDesc const &distortion )
        {
            FovPort result;
            result.UpTan    = 0.0f;
            result.DownTan  = 0.0f;
            result.LeftTan  = 0.0f;
            result.RightTan = 0.0f;

            float stepScale = 1.0f / ( numSteps - 1 );
            for ( int step = 0; step < numSteps; step++ )
            {
                float    lerpFactor  = stepScale * (float)step;
                Vector2f sample      = from + (to - from) * lerpFactor;
                Vector2f tanEyeAngle = TransformScreenNDCToTanFovSpace ( distortion, sample );

                result.LeftTan  = Alg::Max ( result.LeftTan,  -tanEyeAngle.x );
                result.RightTan = Alg::Max ( result.RightTan,  tanEyeAngle.x );
                result.UpTan    = Alg::Max ( result.UpTan,    -tanEyeAngle.y );
                result.DownTan  = Alg::Max ( result.DownTan,   tanEyeAngle.y );
            }
            return result;
        }
    };

    FovPort leftFovPort  = FunctionHider::FindRange( dmiddle, Vector2f( -1.0f, dmiddle.y ), 10, distortion );
    FovPort rightFovPort = FunctionHider::FindRange( dmiddle, Vector2f( 1.0f, dmiddle.y ),  10, distortion );
    FovPort upFovPort    = FunctionHider::FindRange( dmiddle, Vector2f( dmiddle.x, -1.0f ), 10, distortion );
    FovPort downFovPort  = FunctionHider::FindRange( dmiddle, Vector2f( dmiddle.x, 1.0f ),  10, distortion );
    
    resultFovPort.LeftTan  = leftFovPort.LeftTan;
    resultFovPort.RightTan = rightFovPort.RightTan;
    resultFovPort.UpTan    = upFovPort.UpTan;
    resultFovPort.DownTan  = downFovPort.DownTan;

    return resultFovPort;
}

FovPort ClampToPhysicalScreenFov( StereoEye eyeType, DistortionRenderDesc const &distortion,
                                         FovPort inputFovPort )
{
    FovPort resultFovPort;
    FovPort phsyicalFovPort = GetPhysicalScreenFov ( eyeType, distortion );
    resultFovPort.LeftTan  = Alg::Min ( inputFovPort.LeftTan,  phsyicalFovPort.LeftTan );
    resultFovPort.RightTan = Alg::Min ( inputFovPort.RightTan, phsyicalFovPort.RightTan );
    resultFovPort.UpTan    = Alg::Min ( inputFovPort.UpTan,    phsyicalFovPort.UpTan );
    resultFovPort.DownTan  = Alg::Min ( inputFovPort.DownTan,  phsyicalFovPort.DownTan );

    return resultFovPort;
}

Sizei CalculateIdealPixelSize ( StereoEye eyeType, DistortionRenderDesc const &distortion,
                                FovPort tanHalfFov, float pixelsPerDisplayPixel )
{
    OVR_UNUSED(eyeType);   // might be useful in the future if we do overlapping fovs

    Sizei result;    
    // TODO: if the app passes in a FOV that doesn't cover the centre, use the distortion values for the nearest edge/corner to match pixel size.
    result.w  = (int)(0.5f + pixelsPerDisplayPixel * distortion.PixelsPerTanAngleAtCenter.x * ( tanHalfFov.LeftTan + tanHalfFov.RightTan ) );
    result.h = (int)(0.5f + pixelsPerDisplayPixel * distortion.PixelsPerTanAngleAtCenter.y * ( tanHalfFov.UpTan   + tanHalfFov.DownTan  ) );
    return result;
}

Recti GetFramebufferViewport ( StereoEye eyeType, HmdRenderInfo const &hmd )
{
    Recti result;
    result.w = hmd.ResolutionInPixels.w/2;
    result.h = hmd.ResolutionInPixels.h;
    result.x = 0;
    result.y = 0;
    if ( eyeType == StereoEye_Right )
    {
        result.x = (hmd.ResolutionInPixels.w+1)/2;      // Round up, not down.
    }
    return result;
}


ScaleAndOffset2D CreateNDCScaleAndOffsetFromFov ( FovPort tanHalfFov )
{
    float projXScale = 2.0f / ( tanHalfFov.LeftTan + tanHalfFov.RightTan );
    float projXOffset = ( tanHalfFov.LeftTan - tanHalfFov.RightTan ) * projXScale * 0.5f;
    float projYScale = 2.0f / ( tanHalfFov.UpTan + tanHalfFov.DownTan );
    float projYOffset = ( tanHalfFov.UpTan - tanHalfFov.DownTan ) * projYScale * 0.5f;

    ScaleAndOffset2D result;
    result.Scale    = Vector2f(projXScale, projYScale);
    result.Offset   = Vector2f(projXOffset, projYOffset);
    // Hey - why is that Y.Offset negated?
    // It's because a projection matrix transforms from world coords with Y=up,
    // whereas this is from NDC which is Y=down.

    return result;
}


ScaleAndOffset2D CreateUVScaleAndOffsetfromNDCScaleandOffset ( ScaleAndOffset2D scaleAndOffsetNDC,
                                                               Recti renderedViewport,
                                                               Sizei renderTargetSize )
{
    // scaleAndOffsetNDC takes you to NDC space [-1,+1] within the given viewport on the rendertarget.
    // We want a scale to instead go to actual UV coordinates you can sample with,
    // which need [0,1] and ignore the viewport.
    ScaleAndOffset2D result;
    // Scale [-1,1] to [0,1]
    result.Scale  = scaleAndOffsetNDC.Scale * 0.5f;
    result.Offset = scaleAndOffsetNDC.Offset * 0.5f + Vector2f(0.5f);
    
    // ...but we will have rendered to a subsection of the RT, so scale for that.
    Vector2f scale(  (float)renderedViewport.w / (float)renderTargetSize.w,
                     (float)renderedViewport.h / (float)renderTargetSize.h );
    Vector2f offset( (float)renderedViewport.x / (float)renderTargetSize.w,
                     (float)renderedViewport.y / (float)renderTargetSize.h );

	result.Scale  = result.Scale.EntrywiseMultiply(scale);
    result.Offset  = result.Offset.EntrywiseMultiply(scale) + offset;
    return result;
}



Matrix4f CreateProjection( bool rightHanded, FovPort tanHalfFov,
                           float zNear /*= 0.01f*/, float zFar /*= 10000.0f*/ )
{
    // A projection matrix is very like a scaling from NDC, so we can start with that.
    ScaleAndOffset2D scaleAndOffset = CreateNDCScaleAndOffsetFromFov ( tanHalfFov );

    float handednessScale = 1.0f;
    if ( rightHanded )
    {
        handednessScale = -1.0f;
    }

    Matrix4f projection;
    // Produces X result, mapping clip edges to [-w,+w]
    projection.M[0][0] = scaleAndOffset.Scale.x;
    projection.M[0][1] = 0.0f;
    projection.M[0][2] = handednessScale * scaleAndOffset.Offset.x;
    projection.M[0][3] = 0.0f;

    // Produces Y result, mapping clip edges to [-w,+w]
    // Hey - why is that YOffset negated?
    // It's because a projection matrix transforms from world coords with Y=up,
    // whereas this is derived from an NDC scaling, which is Y=down.
    projection.M[1][0] = 0.0f;
    projection.M[1][1] = scaleAndOffset.Scale.y;
    projection.M[1][2] = handednessScale * -scaleAndOffset.Offset.y;
    projection.M[1][3] = 0.0f;

    // Produces Z-buffer result - app needs to fill this in with whatever Z range it wants.
    // We'll just use some defaults for now.
    projection.M[2][0] = 0.0f;
    projection.M[2][1] = 0.0f;
    projection.M[2][2] = -handednessScale * zFar / (zNear - zFar);
    projection.M[2][3] = (zFar * zNear) / (zNear - zFar);

    // Produces W result (= Z in)
    projection.M[3][0] = 0.0f;
    projection.M[3][1] = 0.0f;
    projection.M[3][2] = handednessScale;
    projection.M[3][3] = 0.0f;

    return projection;
}


Matrix4f CreateOrthoSubProjection ( bool rightHanded, StereoEye eyeType,
                                    float tanHalfFovX, float tanHalfFovY,
                                    float unitsX, float unitsY,
                                    float distanceFromCamera, float interpupillaryDistance,
                                    Matrix4f const &projection,
                                    float zNear /*= 0.0f*/, float zFar /*= 0.0f*/ )
{
    OVR_UNUSED1 ( rightHanded );

    float orthoHorizontalOffset = interpupillaryDistance * 0.5f / distanceFromCamera;
    switch ( eyeType )
    {
    case StereoEye_Center:
        orthoHorizontalOffset = 0.0f;
        break;
    case StereoEye_Left:
        break;
    case StereoEye_Right:
        orthoHorizontalOffset = -orthoHorizontalOffset;
        break;
    default: OVR_ASSERT ( false ); break;
    }

    // Current projection maps real-world vector (x,y,1) to the RT.
    // We want to find the projection that maps the range [-FovPixels/2,FovPixels/2] to
    // the physical [-orthoHalfFov,orthoHalfFov]
    // Note moving the offset from M[0][2]+M[1][2] to M[0][3]+M[1][3] - this means
    // we don't have to feed in Z=1 all the time.
    // The horizontal offset math is a little hinky because the destination is
    // actually [-orthoHalfFov+orthoHorizontalOffset,orthoHalfFov+orthoHorizontalOffset]
    // So we need to first map [-FovPixels/2,FovPixels/2] to
    //                         [-orthoHalfFov+orthoHorizontalOffset,orthoHalfFov+orthoHorizontalOffset]:
    // x1 = x0 * orthoHalfFov/(FovPixels/2) + orthoHorizontalOffset;
    //    = x0 * 2*orthoHalfFov/FovPixels + orthoHorizontalOffset;
    // But then we need the sam mapping as the existing projection matrix, i.e.
    // x2 = x1 * Projection.M[0][0] + Projection.M[0][2];
    //    = x0 * (2*orthoHalfFov/FovPixels + orthoHorizontalOffset) * Projection.M[0][0] + Projection.M[0][2];
    //    = x0 * Projection.M[0][0]*2*orthoHalfFov/FovPixels +
    //      orthoHorizontalOffset*Projection.M[0][0] + Projection.M[0][2];
    // So in the new projection matrix we need to scale by Projection.M[0][0]*2*orthoHalfFov/FovPixels and
    // offset by orthoHorizontalOffset*Projection.M[0][0] + Projection.M[0][2].

    float orthoScaleX = 2.0f * tanHalfFovX / unitsX;
    float orthoScaleY = 2.0f * tanHalfFovY / unitsY;
    Matrix4f ortho;
    ortho.M[0][0] = projection.M[0][0] * orthoScaleX;
    ortho.M[0][1] = 0.0f;
    ortho.M[0][2] = 0.0f;
    ortho.M[0][3] = -projection.M[0][2] + ( orthoHorizontalOffset * projection.M[0][0] );

    ortho.M[1][0] = 0.0f;
    ortho.M[1][1] = -projection.M[1][1] * orthoScaleY;       // Note sign flip (text rendering uses Y=down).
    ortho.M[1][2] = 0.0f;
    ortho.M[1][3] = -projection.M[1][2];

    if ( fabsf ( zNear - zFar ) < 0.001f )
    {
        ortho.M[2][0] = 0.0f;
        ortho.M[2][1] = 0.0f;
        ortho.M[2][2] = 0.0f;
        ortho.M[2][3] = zFar;
    }
    else
    {
        ortho.M[2][0] = 0.0f;
        ortho.M[2][1] = 0.0f;
        ortho.M[2][2] = zFar / (zNear - zFar);
        ortho.M[2][3] = (zFar * zNear) / (zNear - zFar);
    }

    // No perspective correction for ortho.
    ortho.M[3][0] = 0.0f;
    ortho.M[3][1] = 0.0f;
    ortho.M[3][2] = 0.0f;
    ortho.M[3][3] = 1.0f;

    return ortho;
}


//-----------------------------------------------------------------------------------
// A set of "forward-mapping" functions, mapping from framebuffer space to real-world and/or texture space.

// This mimics the first half of the distortion shader's function.
Vector2f TransformScreenNDCToTanFovSpace( DistortionRenderDesc const &distortion,
                                          const Vector2f &framebufferNDC )
{
    // Scale to TanHalfFov space, but still distorted.
    Vector2f tanEyeAngleDistorted;
    tanEyeAngleDistorted.x = ( framebufferNDC.x - distortion.LensCenter.x ) * distortion.TanEyeAngleScale.x;
    tanEyeAngleDistorted.y = ( framebufferNDC.y - distortion.LensCenter.y ) * distortion.TanEyeAngleScale.y;
    // Distort.
    float radiusSquared = ( tanEyeAngleDistorted.x * tanEyeAngleDistorted.x )
                        + ( tanEyeAngleDistorted.y * tanEyeAngleDistorted.y );
    float distortionScale = distortion.Lens.DistortionFnScaleRadiusSquared ( radiusSquared );
    Vector2f tanEyeAngle;
    tanEyeAngle.x = tanEyeAngleDistorted.x * distortionScale;
    tanEyeAngle.y = tanEyeAngleDistorted.y * distortionScale;

    return tanEyeAngle;
}

// Same, with chromatic aberration correction.
void TransformScreenNDCToTanFovSpaceChroma ( Vector2f *resultR, Vector2f *resultG, Vector2f *resultB, 
                                             DistortionRenderDesc const &distortion,
                                             const Vector2f &framebufferNDC )
{
    // Scale to TanHalfFov space, but still distorted.
    Vector2f tanEyeAngleDistorted;
    tanEyeAngleDistorted.x = ( framebufferNDC.x - distortion.LensCenter.x ) * distortion.TanEyeAngleScale.x;
    tanEyeAngleDistorted.y = ( framebufferNDC.y - distortion.LensCenter.y ) * distortion.TanEyeAngleScale.y;
    // Distort.
    float radiusSquared = ( tanEyeAngleDistorted.x * tanEyeAngleDistorted.x )
                        + ( tanEyeAngleDistorted.y * tanEyeAngleDistorted.y );
    Vector3f distortionScales = distortion.Lens.DistortionFnScaleRadiusSquaredChroma ( radiusSquared );
    *resultR = tanEyeAngleDistorted * distortionScales.x;
    *resultG = tanEyeAngleDistorted * distortionScales.y;
    *resultB = tanEyeAngleDistorted * distortionScales.z;
}

// This mimics the second half of the distortion shader's function.
Vector2f TransformTanFovSpaceToRendertargetTexUV( ScaleAndOffset2D const &eyeToSourceUV,
                                                  Vector2f const &tanEyeAngle )
{
    Vector2f textureUV;
    textureUV.x = tanEyeAngle.x * eyeToSourceUV.Scale.x + eyeToSourceUV.Offset.x;
    textureUV.y = tanEyeAngle.y * eyeToSourceUV.Scale.y + eyeToSourceUV.Offset.y;
    return textureUV;
}

Vector2f TransformTanFovSpaceToRendertargetNDC( ScaleAndOffset2D const &eyeToSourceNDC,
                                                Vector2f const &tanEyeAngle )
{
    Vector2f textureNDC;
    textureNDC.x = tanEyeAngle.x * eyeToSourceNDC.Scale.x + eyeToSourceNDC.Offset.x;
    textureNDC.y = tanEyeAngle.y * eyeToSourceNDC.Scale.y + eyeToSourceNDC.Offset.y;
    return textureNDC;
}

Vector2f TransformScreenPixelToScreenNDC( Recti const &distortionViewport,
                                          Vector2f const &pixel )
{
    // Move to [-1,1] NDC coords.
    Vector2f framebufferNDC;
    framebufferNDC.x = -1.0f + 2.0f * ( ( pixel.x - (float)distortionViewport.x ) / (float)distortionViewport.w );
    framebufferNDC.y = -1.0f + 2.0f * ( ( pixel.y - (float)distortionViewport.y ) / (float)distortionViewport.h );
    return framebufferNDC;
}

Vector2f TransformScreenPixelToTanFovSpace( Recti const &distortionViewport,
                                            DistortionRenderDesc const &distortion,
                                            Vector2f const &pixel )
{
    return TransformScreenNDCToTanFovSpace( distortion,
                TransformScreenPixelToScreenNDC( distortionViewport, pixel ) );
}

Vector2f TransformScreenNDCToRendertargetTexUV( DistortionRenderDesc const &distortion,
                                                StereoEyeParams const &eyeParams,
                                                Vector2f const &pixel )
{
    return TransformTanFovSpaceToRendertargetTexUV ( eyeParams,
                TransformScreenNDCToTanFovSpace ( distortion, pixel ) );
}

Vector2f TransformScreenPixelToRendertargetTexUV( Recti const &distortionViewport,
                                                  DistortionRenderDesc const &distortion,
                                                  StereoEyeParams const &eyeParams,
                                                  Vector2f const &pixel )
{
    return TransformTanFovSpaceToRendertargetTexUV ( eyeParams,
                TransformScreenPixelToTanFovSpace ( distortionViewport, distortion, pixel ) );
}


//-----------------------------------------------------------------------------------
// A set of "reverse-mapping" functions, mapping from real-world and/or texture space back to the framebuffer.

Vector2f TransformTanFovSpaceToScreenNDC( DistortionRenderDesc const &distortion,
                                          const Vector2f &tanEyeAngle, bool usePolyApprox /*= false*/ )
{
    float tanEyeAngleRadius = tanEyeAngle.Length();
    float tanEyeAngleDistortedRadius = distortion.Lens.DistortionFnInverseApprox ( tanEyeAngleRadius );
    if ( !usePolyApprox )
    {
        tanEyeAngleDistortedRadius = distortion.Lens.DistortionFnInverse ( tanEyeAngleRadius );
    }
    Vector2f tanEyeAngleDistorted = tanEyeAngle;
    if ( tanEyeAngleRadius > 0.0f )
    {   
        tanEyeAngleDistorted = tanEyeAngle * ( tanEyeAngleDistortedRadius / tanEyeAngleRadius );
    }

    Vector2f framebufferNDC;
    framebufferNDC.x = ( tanEyeAngleDistorted.x / distortion.TanEyeAngleScale.x ) + distortion.LensCenter.x;
    framebufferNDC.y = ( tanEyeAngleDistorted.y / distortion.TanEyeAngleScale.y ) + distortion.LensCenter.y;

    return framebufferNDC;
}

Vector2f TransformRendertargetNDCToTanFovSpace( const ScaleAndOffset2D &eyeToSourceNDC,
                                                const Vector2f &textureNDC )
{
    Vector2f tanEyeAngle = (textureNDC - eyeToSourceNDC.Offset) / eyeToSourceNDC.Scale;
    return tanEyeAngle;
}



} //namespace OVR

//Just want to make a copy disentangled from all these namespaces!
float ExtEvalCatmullRom10Spline ( float const *K, float scaledVal )
{
	return(OVR::EvalCatmullRom10Spline ( K, scaledVal ));
}


