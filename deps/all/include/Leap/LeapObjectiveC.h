/******************************************************************************\
* Copyright (C) 2012-2013 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#import <Foundation/Foundation.h>

/*************************************************************************
Do not be alarmed by the copyright notice above. Please bear with us as we
work with our lawyers to finalize a permissive license for this code.

This wrapper works by doing a deep copy of the bulk of the hand and finger
hierarchy as soon as you the user requests `[controller frame]`. This is
enables us to set up the appropriate linkage between LeapHand and
LeapPointable ObjectiveC objects.

The motions API forced our hand to move the Frame and Hand objects towards
thin wrappers. Each now contains a pointer to its corresponding C++ object.
The screen API brought Pointable objects to wrap and keep around a C++
Leap::Pointable object as well.

Because the wrapped C++ object is kept around, attributes such as position
and velocity now have their ObjectiveC objects created lazily.

Major Leap API features supported in this wrapper today:
* Obtaining data through both polling (LeapController only) as well as
  through callbacks
* Waiting for single-threaded event callbacks through NSNotification objects
  (LeapListener), in addition to ObjectiveC delegates (LeapDelegate)
* Getting lists of hands, fingers, tools, or pointables from a frame or hand
  (e.g. `[frame hands]`, `[frame tools]`, or `[hand fingers]`)
* Getting hands, fingers, tools, or pointables a frame or hand by ID
  (e.g. `[frame hand:ID]`, `[frame tool:ID]`, or `[hand finger:ID]`)
* Querying back up the hierarchy, e.g. `[finger hand]` or `[hand frame]`
* Various hand/finger/tool properties: direction, palmNormal, sphereRadius,
  and more
* LeapVector math helper functions: pitch, roll, yaw, vector add, scalar
  multiply, dot product, cross product, LeapMatrix, and more
* Motions (translation/rotation/scale) from LeapHand and LeapFrame classes
* LeapGesture events, including swipe, circle, screen tap, and key tap
* LeapScreen location/orientation/size as well as intersection/projection
* LeapConfig class for configuring gesture recognition parameters

Notes:
* Class names are prefixed by Leap, although LM and LPM were considered.
  Users may change the prefix locally, for example:
    sed -i '.bak' 's/Leap\([A-NP-Z]\)/LPM\1/g' LeapObjectiveC.*
    # above regexp matches LeapController, LeapVector, not LeapObjectiveC
* Requires XCode 4.2+, relies on Automatic Reference Counting (ARC),
  minimum target OS X 10.7
* Contributions are welcome. Contact us via https://developer.leapmotion.com

*************************************************************************/

//////////////////////////////////////////////////////////////////////////
//VECTOR
/**
 * The LeapVector class represents a three-component mathematical vector or point
 * such as a direction or position in three-dimensional space.
 *
 * The Leap software employs a right-handed Cartesian coordinate system.
 * Values given are in units of real-world millimeters. The origin is centered
 * at the center of the Leap device. The x- and z-axes lie in the horizontal
 * plane, with the x-axis running parallel to the long edge of the device.
 * The y-axis is vertical, with positive values increasing upwards (in contrast
 * to the downward orientation of most computer graphics coordinate systems).
 * The z-axis has positive values increasing away from the computer screen.
 *
 * <img src="../docs/images/Leap_Axes.png"/>
 *
 * @available Since 1.0
 */
@interface LeapVector : NSObject

/**
 * Creates a new LeapVector with the specified component values.
 *
 * @example Vector_Constructor_1.txt
 *
 * @param x The horizontal component.
 * @param y The vertical component.
 * @param z The depth component.
 * @available Since 1.0
 */
- (id)initWithX:(float)x y:(float)y z:(float)z;
/**
 * Copies the specified LeapVector.
 *
 * @example Vector_Constructor_2.txt
 *
 * @param vector The LeapVector to copy.
 * @available Since 1.0
 */
- (id)initWithVector:(const LeapVector *)vector;
- (NSString *)description;
/**
 * The magnitude, or length, of this vector.
 *
 * @example Vector_Magnitude.txt
 *
 * The magnitude is the L2 norm, or Euclidean distance between the origin and
 * the point represented by the (x, y, z) components of this LeapVector object.
 *
 * @returns The length of this vector.
 * @available Since 1.0
 */
- (float)magnitude;
@property (nonatomic, getter = magnitude, readonly)float magnitude;
/**
 * The square of the magnitude, or length, of this vector.
 *
 * @example Vector_Magnitude_Squared.txt
 *
 * @returns The square of the length of this vector.
 * @available Since 1.0
 */
- (float)magnitudeSquared;
@property (nonatomic, getter = magnitudeSquared, readonly)float magnitudeSquared;
/**
 * The distance between the point represented by this LeapVector
 * object and a point represented by the specified LeapVector object.
 *
 * @example Vector_DistanceTo.txt
 *
 * @param vector A LeapVector object.
 * @returns The distance from this point to the specified point.
 * @available Since 1.0
 */
- (float)distanceTo:(const LeapVector *)vector;
/**
 *  The angle between this vector and the specified vector in radians.
 *
 * @example Vector_AngleTo.txt
 *
 * The angle is measured in the plane formed by the two vectors. The
 * angle returned is always the smaller of the two conjugate angles.
 * Thus `[A angleTo:B] == [B angleTo:A]` and is always a positive
 * value less than or equal to pi radians (180 degrees).
 *
 * If either vector has zero length, then this function returns zero.
 *
 * <img src="../docs/images/Math_AngleTo.png"/>
 *
 * @param vector A LeapVector object.
 * @returns The angle between this vector and the specified vector in radians.
 * @available Since 1.0
 */
- (float)angleTo:(const LeapVector *)vector;
/**
 *  The pitch angle in radians.
 *
 * @example Vector_Pitch.txt
 *
 * Pitch is the angle between the negative z-axis and the projection of
 * the vector onto the y-z plane. In other words, pitch represents rotation
 * around the x-axis.
 * If the vector points upward, the returned angle is between 0 and pi radians
 * (180 degrees); if it points downward, the angle is between 0 and -pi radians.
 *
 * <img src="../docs/images/Math_Pitch_Angle.png"/>
 *
 * @returns The angle of this vector above or below the horizon (x-z plane).
 * @available Since 1.0
 */
- (float)pitch;
@property (nonatomic, getter = pitch, readonly)float pitch;
/**
 *  The roll angle in radians.
 *
 * @example Vector_Roll.txt
 *
 * Roll is the angle between the y-axis and the projection of
 * the vector onto the x-y plane. In other words, roll represents rotation
 * around the z-axis. If the vector points to the left of the y-axis,
 * then the returned angle is between 0 and pi radians (180 degrees);
 * if it points to the right, the angle is between 0 and -pi radians.
 *
 * <img src="../docs/images/Math_Roll_Angle.png"/>
 *
 * Use this function to get roll angle of the plane to which this vector is a
 * normal. For example, if this vector represents the normal to the palm,
 * then this function returns the tilt or roll of the palm plane compared
 * to the horizontal (x-z) plane.
 *
 * @returns The angle of this vector to the right or left of the y-axis.
 * @available Since 1.0
 */
- (float)roll;
@property (nonatomic, getter = roll, readonly)float roll;
/**
 *  The yaw angle in radians.
 *
 * @example Vector_Yaw.txt
 *
 * Yaw is the angle between the negative z-axis and the projection of
 * the vector onto the x-z plane. In other words, yaw represents rotation
 * around the y-axis. If the vector points to the right of the negative z-axis,
 * then the returned angle is between 0 and pi radians (180 degrees);
 * if it points to the left, the angle is between 0 and -pi radians.
 *
 * <img src="../docs/images/Math_Yaw_Angle.png"/>
 *
 * @returns The angle of this vector to the right or left of the negative z-axis.
 * @available Since 1.0
 */
- (float)yaw;
@property (nonatomic, getter = yaw, readonly)float yaw;
/**
 * Adds two vectors.
 *
 * @example Vector_Plus.txt
 *
 * @param vector The LeapVector addend.
 * @returns The sum of the two LeapVectors.
 * @available Since 1.0
 */
- (LeapVector *)plus:(const LeapVector *)vector;
/**
 * Subtract a vector from this vector.
 *
 * @example Vector_Minus.txt
 *
 * @param vector the LeapVector subtrahend.
 * @returns the difference between the two LeapVectors.
 * @available Since 1.0
 */
- (LeapVector *)minus:(const LeapVector *)vector;
/**
 * Negate this vector.
 *
 * @example Vector_Negate.txt
 *
 * @returns The negation of this LeapVector.
 * @available Since 1.0
 */
- (LeapVector *)negate;
/**
 * Multiply this vector by a number.
 *
 * @example Vector_Times.txt
 *
 * @param scalar The scalar factor.
 * @returns The product of this LeapVector and a scalar.
 * @available Since 1.0
 */
- (LeapVector *)times:(float)scalar;
/**
 * Divide this vector by a number.
 *
 * @example Vector_Divide.txt
 *
 * @param scalar The scalar divisor;
 * @returns The dividend of this LeapVector divided by a scalar.
 * @available Since 1.0
 */
- (LeapVector *)divide:(float)scalar;
// not provided: unary assignment operators (plus_equals, minus_equals)
// user should emulate with above operators
/**
 * Checks LeapVector equality.
 *
 * @example Vector_Equals.txt
 *
 * Vectors are equal if each corresponding component is equal.
 * @param vector The LeapVector to compare.
 * @returns YES, if the LeapVectors are equal.
 * @available Since 1.0
 */
- (BOOL)equals:(const LeapVector *)vector;
// not provided: not_equals
// user should emulate with !v.equals(...)
/**
 *  The dot product of this vector with another vector.
 *
 * @example Vector_Dot.txt
 *
 * The dot product is the magnitude of the projection of this vector
 * onto the specified vector.
 *
 * <img src="../docs/images/Math_Dot.png"/>
 *
 * @param vector A LeapVector object.
 * @returns The dot product of this vector and the specified vector.
 * @available Since 1.0
 */
- (float)dot:(const LeapVector *)vector;
/**
 *  The cross product of this vector and the specified vector.
 *
 * @example Vector_Cross.txt
 *
 * The cross product is a vector orthogonal to both original vectors.
 * It has a magnitude equal to the area of a parallelogram having the
 * two vectors as sides. The direction of the returned vector is
 * determined by the right-hand rule. Thus `[A cross:B] ==
 * [[B negate] cross:A]`.
 *
 * <img src="../docs/images/Math_Cross.png"/>
 *
 * @param vector A LeapVector object.
 * @returns The cross product of this vector and the specified vector.
 * @available Since 1.0
 */
- (LeapVector *)cross:(const LeapVector *)vector;
/**
 *  A normalized copy of this vector.
 *
 * @example Vector_Normalized.txt
 *
 * A normalized vector has the same direction as the original vector,
 * but with a length of one.
 *
 * @returns A LeapVector object with a length of one, pointing in the same
 * direction as this Vector object.
 * @available Since 1.0
 */
- (LeapVector *)normalized;
@property (nonatomic, getter = normalized, readonly)LeapVector *normalized;
/**
 * Returns an NSArray object containing the vector components in the
 * order: x, y, z.
 *
 * @example Vector_ToNSArray.txt
 *
 * @available Since 1.0
 */
- (NSArray *)toNSArray;
@property (nonatomic, getter = toNSArray, readonly)NSArray *toNSArray;
/**
 * Returns an NSMutableData object containing the vector components as
 * consecutive floating point values.
 *
 * @example Vector_ToFloatPointer.txt
 * @available Since 1.0
 */
- (NSMutableData *)toFloatPointer;
@property (nonatomic, getter = toFloatPointer, readonly)NSMutableData *toFloatPointer;
// not provided: toVector4Type
// no templates, and ObjectiveC does not have a common math vector type
/**
 * The zero vector: (0, 0, 0)
 *
 * @example Vector_Zero.txt
 * @available Since 1.0
 */
+ (LeapVector *)zero;
/**
 * The x-axis unit vector: (1, 0, 0).
 *
 * @example Vector_XAxis.txt
 * @available Since 1.0
 */
+ (LeapVector *)xAxis;
/**
 * The y-axis unit vector: (0, 1, 0).
 *
 * @example Vector_YAxis.txt
 * @available Since 1.0
 */
+ (LeapVector *)yAxis;
/**
 * The z-axis unit vector: (0, 0, 1).
 *
 * @example Vector_ZAxis.txt
 * @available Since 1.0
 */
+ (LeapVector *)zAxis;
/**
 * The unit vector pointing left along the negative x-axis: (-1, 0, 0).
 *
 * @example Vector_Left.txt
 * @available Since 1.0
 */
+ (LeapVector *)left;
/**
 * The unit vector pointing right along the positive x-axis: (1, 0, 0).
 *
 * @example Vector_Right.txt
 * @available Since 1.0
 */
+ (LeapVector *)right;
/**
 * The unit vector pointing down along the negative y-axis: (0, -1, 0).
 *
 * @example Vector_Down.txt
 * @available Since 1.0
 */
+ (LeapVector *)down;
/**
 * The unit vector pointing up along the positive y-axis: (0, 1, 0).
 *
 * @example Vector_Up.txt
 * @available Since 1.0
 */
+ (LeapVector *)up;
/**
 * The unit vector pointing forward along the negative z-axis: (0, 0, -1).
 *
 * @example Vector_Forward.txt
 * @available Since 1.0
 */
+ (LeapVector *)forward;
/**
 * The unit vector pointing backward along the positive z-axis: (0, 0, 1).
 *
 * @example Vector_Backward.txt
 * @available Since 1.0
 */
+ (LeapVector *)backward;

/** 
 * The horizontal component.
 * @available Since 1.0
 */
@property (nonatomic, assign, readonly)float x;
/** 
 * The vertical component. 
 * @available Since 1.0
 */
@property (nonatomic, assign, readonly)float y;
/** 
 * The depth component. 
 * @available Since 1.0
 */
@property (nonatomic, assign, readonly)float z;

@end

//////////////////////////////////////////////////////////////////////////
//MATRIX
/**
 *  The LeapMatrix class represents a transformation matrix.
 *
 * To use this class to transform a <LeapVector>, construct a matrix containing the
 * desired transformation and then use the <[LeapMatrix transformPoint:]> or
 * <[LeapMatrix transformDirection:]> functions to apply the transform.
 *
 * Transforms can be combined by multiplying two or more transform matrices using
 * the <[LeapMatrix times:]> function.
 * @available Since 1.0
 */
@interface LeapMatrix : NSObject

/**
 *  Constructs a transformation matrix from the specified basis and translation vectors.
 *
 * @example Matrix_Constructor_1.txt
 *
 * @param xBasis A <LeapVector> specifying rotation and scale factors for the x-axis.
 * @param yBasis A <LeapVector> specifying rotation and scale factors for the y-axis.
 * @param zBasis A <LeapVector> specifying rotation and scale factors for the z-axis.
 * @param origin A <LeapVector> specifying translation factors on all three axes.
 * @available Since 1.0
 */
- (id)initWithXBasis:(const LeapVector *)xBasis yBasis:(const LeapVector *)yBasis zBasis:(const LeapVector *)zBasis origin:(const LeapVector *)origin;
/**
 * Constructs a copy of the specified Matrix object.
 *
 * @example Matrix_Constructor_2.txt
 *
 * @param matrix the LeapMatrix to copy.
 * @available Since 1.0
 */
- (id)initWithMatrix:(LeapMatrix *)matrix;
/**
 *  Constructs a transformation matrix specifying a rotation around the specified vector.
 *
 * @example Matrix_Constructor_3.txt
 *
 * @param axis A <LeapVector> specifying the axis of rotation.
 * @param angleRadians The amount of rotation in radians.
 * @available Since 1.0
 */
- (id)initWithAxis:(const LeapVector *)axis angleRadians:(float)angleRadians;
/**
 *  Constructs a transformation matrix specifying a rotation around the specified vector
 * and a translation by the specified vector.
 *
 * @example Matrix_Constructor_4.txt
 *
 * @param axis A <LeapVector> specifying the axis of rotation.
 * @param angleRadians The angle of rotation in radians.
 * @param translation A <LeapVector> representing the translation part of the transform.
 * @available Since 1.0
 */
- (id)initWithAxis:(const LeapVector *)axis angleRadians:(float)angleRadians translation:(const LeapVector *)translation;
- (NSString *)description;
// not provided: setRotation
// This was mainly an internal helper function for the above constructors
/**
 *  Transforms a vector with this matrix by transforming its rotation,
 * scale, and translation.
 *
 * Translation is applied after rotation and scale.
 *
 * @example Matrix_TransformPoint.txt
 *
 * @param point A <LeapVector> representing the 3D position to transform.
 * @returns A new <LeapVector> representing the transformed original.
 * @available Since 1.0
 */
- (LeapVector *)transformPoint:(const LeapVector *)point;
/**
 *  Transforms a vector with this matrix by transforming its rotation and
 * scale only.
 *
 * @example Matrix_TransformDirection.txt
 *
 * @param direction The <LeapVector> to transform.
 * @returns A new <LeapVector> representing the transformed original.
 * @available Since 1.0
 */
- (LeapVector *)transformDirection:(const LeapVector *)direction;
/**
 *  Performs a matrix inverse if the matrix consists entirely of rigid
 * transformations (translations and rotations).  If the matrix is not rigid,
 * this operation will not represent an inverse.
 *
 * Note that all matricies that are directly returned by the API are rigid.
 *
 * @returns The rigid inverse of the matrix.
 * @available Since 1.0
 */
- (LeapMatrix *)rigidInverse;
@property (nonatomic, getter = rigidInverse, readonly)LeapMatrix *rigidInverse;
/**
 *  Multiply transform matrices.
 *
 * @example Matrix_Times.txt
 *
 * Combines two transformations into a single equivalent transformation.
 *
 * @param other A LeapMatrix to multiply on the right hand side.
 * @returns A new LeapMatrix representing the transformation equivalent to
 * applying the other transformation followed by this transformation.
 * @available Since 1.0
 */
- (LeapMatrix *)times:(const LeapMatrix *) other;
// not provided: unary assignment operator times_equals
/**
 * Compare LeapMatrix equality component-wise.
 *
 * @example Matrix_Equals.txt
 *
 * @param other The LeapMatrix object to compare.
 * @return YES, if the corresponding elements in the two matrices are equal.
 * @available Since 1.0
 */
- (BOOL)equals:(const LeapMatrix *) other;
// not provided: not_equals
/**
 *  Converts a LeapMatrix object to a 9-element NSArray object.
 *
 * The elements of the matrix are inserted into the array in row-major order.
 *
 * @example Matrix_ToNSArray_1.txt
 *
 * Translation factors are discarded.
 * @available Since 1.0
 */
- (NSMutableArray *)toNSArray3x3;
@property (nonatomic, getter = toNSArray3x3, readonly)NSMutableArray *toNSArray3x3;
/**
 *  Converts a LeapMatrix object to a 16-element NSArray object.
 *
 * The elements of the matrix are inserted into the array in row-major order.
 *
 * @example Matrix_ToNSArray_2.txt
 *
 * @available Since 1.0
 */
- (NSMutableArray *)toNSArray4x4;
@property (nonatomic, getter = toNSArray4x4, readonly)NSMutableArray *toNSArray4x4;
/**
 *  Returns the identity matrix specifying no translation, rotation, and scale.
 *
 * @example Matrix_Identity.txt
 *
 * @returns The identity matrix.
 * @available Since 1.0
 */
+ (LeapMatrix *)identity;

/** 
 * The rotation and scale factors for the x-axis. 
 * @available Since 1.0
 */
@property (nonatomic, strong, readonly)LeapVector *xBasis;
/** 
 * The rotation and scale factors for the y-axis. 
 * @available Since 1.0
 */
@property (nonatomic, strong, readonly)LeapVector *yBasis;
/** 
 * The rotation and scale factors for the z-axis. 
 * @available Since 1.0
 */
@property (nonatomic, strong, readonly)LeapVector *zBasis;
/** 
 * The translation factors for all three axes. 
 * @available Since 1.0
 */
@property (nonatomic, strong, readonly)LeapVector *origin;

@end

//////////////////////////////////////////////////////////////////////////
//CONSTANTS
/** 
 * The constant pi as a single precision floating point number. 
 * @available Since 1.0
 */
extern const float LEAP_PI;
/**
 * The constant ratio to convert an angle measure from degrees to radians.
 * Multiply a value in degrees by this constant to convert to radians.
 * @available Since 1.0
 */
extern const float LEAP_DEG_TO_RAD;
/**
 * The constant ratio to convert an angle measure from radians to degrees.
 * Multiply a value in radians by this constant to convert to degrees.
 * @available Since 1.0
 */
extern const float LEAP_RAD_TO_DEG;

/**
 * The supported types of gestures.
 * @available Since 1.0
 */
typedef enum LeapGestureType {
    LEAP_GESTURE_TYPE_INVALID = -1, /**< An invalid type. */
    LEAP_GESTURE_TYPE_SWIPE = 1, /**< A straight line movement by the hand with fingers extended. */
    LEAP_GESTURE_TYPE_CIRCLE = 4, /**< A circular movement by a finger. */
    LEAP_GESTURE_TYPE_SCREEN_TAP = 5, /**< A forward tapping movement by a finger. */
    LEAP_GESTURE_TYPE_KEY_TAP = 6, /**< A downward tapping movement by a finger. */
} LeapGestureType;

/**
 * The possible gesture states.
 */
typedef enum LeapGestureState {
    LEAP_GESTURE_STATE_INVALID = -1, /**< An invalid state */
    LEAP_GESTURE_STATE_START = 1, /**< The gesture is starting. Just enough has happened to recognize it. */
    LEAP_GESTURE_STATE_UPDATE = 2, /**< The gesture is in progress. (Note: not all gestures have updates). */
    LEAP_GESTURE_STATE_STOP = 3, /**< The gesture has completed or stopped. */
} LeapGestureState;

/**
 * The supported controller policies.
 * @available Since 1.0
 */
typedef enum LeapPolicyFlag {
    LEAP_POLICY_DEFAULT = 0,                 /**< The default policy. */
    LEAP_POLICY_BACKGROUND_FRAMES = (1 << 0) /**< Receive background frames. */
} LeapPolicyFlag;

/**
 * Defines the values for reporting the state of a Pointable object in relation to
 * an adaptive touch plane.
 * @available Since 1.0
 */
typedef enum LeapPointableZone {
    LEAP_POINTABLE_ZONE_NONE       = 0,  /**< The Pointable object is too far from
                                          the plane to be considered hovering or touching.*/
    LEAP_POINTABLE_ZONE_HOVERING   = 1,   /**< The Pointable object is close to, but
                                           not touching the plane.*/
    LEAP_POINTABLE_ZONE_TOUCHING   = 2,  /**< The Pointable has penetrated the plane. */
} LeapPointableZone;

//////////////////////////////////////////////////////////////////////////
//POINTABLE
@class LeapFrame;
@class LeapHand;
@class LeapInteractionBox;

/**
 * The LeapPointable class reports the physical characteristics of a detected finger or tool.
 *
 * Both fingers and tools are classified as LeapPointable objects. Use the
 * <[LeapPointable isFinger]> function to determine whether a pointable object
 * represents a finger. Use the <[LeapPointable isTool]> function to determine
 * whether a pointable object represents a tool. The Leap classifies a detected
 * entity as a tool when it is thinner, straighter, and longer than a typical finger.
 *
 * To provide touch emulation, the Leap Motion software associates a floating touch
 * plane that adapts to the user's finger movement and hand posture. The Leap Motion
 * interprets purposeful movements toward this plane as potential touch points. The
 * logic used by the LeapPointable class is the same as that used by the Leap Motion
 * software for OS-level touch and mouse input emulation. The LeapPointable class reports
 * touch state with the touchZone and touchDistance properties.
 *
 * Note that LeapPointable objects can be invalid, which means that they do not contain
 * valid tracking data and do not correspond to a physical entity. Invalid LeapPointable
 * objects can be the result of asking for a pointable object using an ID from an
 * earlier frame when no pointable objects with that ID exist in the current frame.
 * A pointable object created from the LeapPointable constructor is also invalid.
 * Test for validity with the <[LeapPointable isValid]> function.
 * @available Since 1.0
 */
@interface LeapPointable : NSObject

- (NSString *)description;
/**
 * A unique ID assigned to this LeapPointable object, whose value remains the
 * same across consecutive frames while the tracked finger or tool remains
 * visible. If tracking is lost (for example, when a finger is occluded by
 * another finger or when it is withdrawn from the Leap field of view), the
 * Leap may assign a new ID when it detects the entity in a future frame.
 *
 * Use the ID value with the <[LeapFrame pointable:]> function to find this
 * LeapPointable object in future frames.
 *
 * @returns The ID assigned to this LeapPointable object.
 * @available Since 1.0
 */
- (int32_t)id;
@property (nonatomic, getter = id, readonly)int32_t id;
/**
 * The tip position in millimeters from the Leap origin.
 *
 * @returns The <LeapVector> containing the coordinates of the tip position.
 * @available Since 1.0
 */
- (LeapVector *)tipPosition;
@property (nonatomic, getter = tipPosition, readonly)LeapVector *tipPosition;
/**
 * The rate of change of the tip position in millimeters/second.
 *
 * @returns The <LeapVector> containing the coordinates of the tip velocity.
 * @available Since 1.0
 */
- (LeapVector *)tipVelocity;
@property (nonatomic, getter = tipVelocity, readonly)LeapVector *tipVelocity;
/**
 * The direction in which this finger or tool is pointing.
 *
 * The direction is expressed as a unit vector pointing in the same
 * direction as the tip.
 *
 * <img src="../docs/images/Leap_Finger_Model.png"/>
 *
 * @returns The <LeapVector> pointing in the same direction as the tip of this
 * LeapPointable object.
 * @available Since 1.0
 */
- (LeapVector *)direction;
@property (nonatomic, getter = direction, readonly)LeapVector *direction;
/**
 * The estimated width of the finger or tool in millimeters.
 *
 * The reported width is the average width of the visible portion of the
 * finger or tool from the hand to the tip. If the width isn't known,
 * then a value of 0 is returned.
 *
 * @returns The estimated width of this LeapPointable object.
 * @available Since 1.0
 */
- (float)width;
@property (nonatomic, getter = width, readonly)float width;
/**
 * The estimated length of the finger or tool in millimeters.
 *
 * The reported length is the visible length of the finger or tool from the
 * hand to tip. If the length isn't known, then a value of 0 is returned.
 *
 * @returns The estimated length of this LeapPointable object.
 * @available Since 1.0
 */
- (float)length;
@property (nonatomic, getter = length, readonly)float length;
/**
 * Whether or not the LeapPointable is believed to be a finger.
 * Fingers are generally shorter, thicker, and less straight than tools.
 *
 * @returns YES, if this LeapPointable is classified as a <LeapFinger>.
 * @available Since 1.0
 */
- (BOOL)isFinger;
@property (nonatomic, getter = isFinger, readonly)BOOL isFinger;
/**
 * Whether or not the LeapPointable is believed to be a tool.
 * Tools are generally longer, thinner, and straighter than fingers.
 *
 * @returns YES, if this LeapPointable is classified as a <LeapTool>.
 * @available Since 1.0
 */
- (BOOL)isTool;
@property (nonatomic, getter = isTool, readonly)BOOL isTool;
/**
 * Reports whether this is a valid LeapPointable object.
 *
 * @returns YES, if this LeapPointable object contains valid tracking data.
 * @available Since 1.0
 */
- (BOOL)isValid;
@property (nonatomic, getter = isValid, readonly)BOOL isValid;
/**
 * The current touch zone of this LeapPointable object.
 *
 * The Leap Motion software computes the touch zone based on a floating touch
 * plane that adapts to the user's finger movement and hand posture. The Leap
 * Motion interprets purposeful movements toward this plane as potential touch
 * points. When a LeapPointable moves close to the adaptive touch plane, it enters the
 * "hovering" zone. When a LeapPointable reaches or passes through the plane, it enters
 * the "touching" zone.
 *
 * The possible states are present in the Zone enum:
 *
 * * LEAP_POINTABLE_ZONE_NONE -- The LeapPointable is outside the hovering zone.
 * * LEAP_POINTABLE_ZONE_HOVERING -- The LeapPointable is close to, but not touching the touch plane.
 * * LEAP_POINTABLE_ZONE_TOUCHING -- The LeapPointable has penetrated the touch plane.
 *
 *  <img src="../docs/images/Leap_Touch_Plane.png"/>
 *
 * The touchDistance property provides a normalized indication of the distance to
 * the touch plane when the LeapPointable is in the hovering or touching zones.
 * @available Since 1.0
 */
- (LeapPointableZone)touchZone;
@property (nonatomic, getter = touchZone, readonly)LeapPointableZone touchZone;
/**
 * A value proportional to the distance between this LeapPointable object and the
 * adaptive touch plane.
 *
 * The touch distance is a value in the range [-1, 1]. The value 1.0 indicates the
 * LeapPointable is at the far edge of the hovering zone. The value 0 indicates the
 * LeapPointable is just entering the touching zone. A value of -1.0 indicates the
 * LeapPointable is firmly within the touching zone. Values in between are
 * proportional to the distance from the plane. Thus, the touchDistance of 0.5
 * indicates that the LeapPointable is halfway into the hovering zone.
 *
 * You can use the touchDistance value to modulate visual feedback given to the
 * user as their fingers close in on a touch target, such as a button.
 * @available Since 1.0
 */
- (float)touchDistance;
@property (nonatomic, getter = touchDistance, readonly)float touchDistance;
/**
 * The stabilized tip position of this LeapPointable.
 *
 * Smoothing and stabilization is performed in order to make
 * this value more suitable for interaction with 2D content.
 * @available Since 1.0
 */
- (LeapVector *)stabilizedTipPosition;
@property (nonatomic, getter = stabilizedTipPosition, readonly)LeapVector *stabilizedTipPosition;
/**
 * The duration of time this Pointable has been visible to the Leap Motion Controller.
 *
 * @returns The duration (in seconds) that this Pointable has been tracked.
 * @available Since 1.0
 */
- (float)timeVisible;
@property (nonatomic, getter = timeVisible, readonly)float timeVisible;
/**
 * The <LeapFrame> associated with this LeapPointable object.
 *
 * @returns The associated <LeapFrame> object, if available; otherwise,
 * an invalid LeapFrame object is returned.
 * @available Since 1.0
 */
- (LeapFrame *)frame;
@property (nonatomic, weak, getter = frame, readonly)LeapFrame *frame;
/**
 * The <LeapHand> associated with this finger or tool.
 *
 * @returns The associated <LeapHand> object, if available; otherwise,
 * an invalid LeapHand object is returned.
 * @available Since 1.0
 */
- (LeapHand *)hand;
@property (nonatomic, weak, getter = hand, readonly)LeapHand *hand;
/**
 * Returns an invalid LeapPointable object.
 *
 * You can use the instance returned by this function in comparisons testing
 * whether a given LeapPointable instance is valid or invalid. (You can also use the
 * LeapPointable isValid function.)
 *
 * @returns The invalid LeapPointable instance.
 * @available Since 1.0
 */
+ (LeapPointable *)invalid;

@end

//////////////////////////////////////////////////////////////////////////
//FINGER
/**
 * The LeapFinger class represents a tracked finger.
 *
 * Fingers are pointable objects that the Leap has classified as a finger.
 * Get valid LeapFinger objects from a <LeapFrame> or a <LeapHand> object.
 *
 * Note that LeapFinger objects can be invalid, which means that they do not contain
 * valid tracking data and do not correspond to a physical finger. Invalid LeapFinger
 * objects can be the result of asking for a finger using an ID from an
 * earlier frame when no fingers with that ID exist in the current frame.
 * A LeapFinger object created from the LeapFinger constructor is also invalid.
 * Test for validity with the LeapFinger isValid function.
 * @available Since 1.0
 */
@interface LeapFinger : LeapPointable
@end

//////////////////////////////////////////////////////////////////////////
//TOOL
/**
 * The LeapTool class represents a tracked tool.
 *
 * Tools are pointable objects that the Leap has classified as a tool.
 * Tools are longer, thinner, and straighter than a typical finger.
 * Get valid LeapTool objects from a <LeapFrame> or a <LeapHand> object.
 *
 * <img src="../docs/images/Leap_Tool.png"/>
 *
 * Note that LeapTool objects can be invalid, which means that they do not contain
 * valid tracking data and do not correspond to a physical tool. Invalid LeapTool
 * objects can be the result of asking for a tool object using an ID from an
 * earlier frame when no tools with that ID exist in the current frame.
 * A LeapTool object created from the LeapTool constructor is also invalid.
 * Test for validity with the LeapTool isValid function.
 * @available Since 1.0
 */
@interface LeapTool : LeapPointable
@end

//////////////////////////////////////////////////////////////////////////
//HAND
/**
 * The LeapHand class reports the physical characteristics of a detected hand.
 *
 * Hand tracking data includes a palm position and velocity; vectors for
 * the palm normal and direction to the fingers; properties of a sphere fit
 * to the hand; and lists of the attached fingers and tools.
 *
 * Note that LeapHand objects can be invalid, which means that they do not contain
 * valid tracking data and do not correspond to a physical entity. Invalid LeapHand
 * objects can be the result of asking for a Hand object using an ID from an
 * earlier frame when no hand objects with that ID exist in the current frame.
 * A hand object created from the LeapHand constructor is also invalid.
 * Test for validity with the LeapHand isValid function.
 * @available Since 1.0
 */
@interface LeapHand : NSObject

- (NSString *)description;
/**
 * A unique ID assigned to this LeapHand object, whose value remains the same
 * across consecutive frames while the tracked hand remains visible.
 *
 * If tracking is lost (for example, when a hand is occluded by another hand
 * or when it is withdrawn from or reaches the edge of the Leap field of view),
 * the Leap may assign a new ID when it detects the hand in a future frame.
 *
 * Use the ID value with the <[LeapFrame hand:]> function to find this LeapHand object
 * in future frames.
 *
 * @returns The ID of this hand.
 * @available Since 1.0
 */
- (int32_t)id;
@property (nonatomic, getter = id, readonly)int32_t id;
/**
 * The list of <LeapPointable> objects (fingers and tools) detected in this frame
 * that are associated with this hand, given in arbitrary order.
 *
 * The list can be empty if no fingers or tools associated with this hand
 * are detected.
 *
 * Use the <[LeapPointable isFinger]> function to determine whether or not an
 * item in the list represents a finger. Use the <[LeapPointable isTool]> function
 * to determine whether or not an item in the list represents a tool.
 * You can also get only fingers using the <[LeapHand fingers]> function or
 * only tools using the <[LeapHand tools]> function.
 *
 * @returns An NSArray containing all <LeapPointable> objects associated with this hand.
 * @available Since 1.0
 */
- (NSArray *)pointables;
@property (nonatomic, getter = pointables, readonly)NSArray *pointables;
/**
 * The list of <LeapFinger> objects detected in this frame that are attached to
 * this hand, given in arbitrary order.
 *
 * The list can be empty if no fingers attached to this hand are detected.
 *
 * @returns An NSArray containing all <LeapFinger> objects attached to this hand.
 * @available Since 1.0
 */
- (NSArray *)fingers;
@property (nonatomic, getter = fingers, readonly)NSArray *fingers;
/**
 * The list of <LeapTool> objects detected in this frame that are held by this
 * hand, given in arbitrary order.
 * The list can be empty if no tools held by this hand are detected.
 *
 * @returns An NSArray containing all <LeapTool> objects held by this hand.
 * @available Since 1.0
 */
- (NSArray *)tools;
@property (nonatomic, getter = tools, readonly)NSArray *tools;
/**
 * The <LeapPointable> object with the specified ID associated with this hand.
 *
 * Use this [LeapHand pointable:] function to retrieve a LeapPointable object
 * associated with this hand using an ID value obtained from a previous frame.
 * This function always returns a LeapPointable object, but if no finger or tool
 * with the specified ID is present, an invalid LeapPointable object is returned.
 *
 * Note that ID values persist across frames, but only until tracking of a
 * particular object is lost. If tracking of a finger or tool is lost and subsequently
 * regained, the new LeapPointable object representing that finger or tool may have a
 * different ID than that representing the finger or tool in an earlier frame.
 *
 * @param pointableId The ID value of a <LeapPointable> object from a previous frame.
 * @returns The <LeapPointable> object with the matching ID if one exists for this
 * hand in this frame; otherwise, an invalid LeapPointable object is returned.
 * @available Since 1.0
 */
- (LeapPointable *)pointable:(int32_t)pointableId;
/**
 * The <LeapFinger> object with the specified ID attached to this hand.
 *
 * Use this [LeapHand finger:] function to retrieve a LeapFinger object attached to
 * this hand using an ID value obtained from a previous frame.
 * This function always returns a LeapFinger object, but if no finger
 * with the specified ID is present, an invalid LeapFinger object is returned.
 *
 * Note that ID values persist across frames, but only until tracking of a
 * particular object is lost. If tracking of a finger is lost and subsequently
 * regained, the new LeapFinger object representing that finger may have a
 * different ID than that representing the finger in an earlier frame.
 *
 * @param fingerId The ID value of a <LeapFinger> object from a previous frame.
 * @returns The <LeapFinger> object with the matching ID if one exists for this
 * hand in this frame; otherwise, an invalid LeapFinger object is returned.
 * @available Since 1.0
 */
- (LeapFinger *)finger:(int32_t)fingerId;
/**
 * The <LeapTool> object with the specified ID held by this hand.
 *
 * Use this [LeapHand tool:] function to retrieve a LeapTool object held by
 * this hand using an ID value obtained from a previous frame.
 * This function always returns a LeapTool object, but if no tool
 * with the specified ID is present, an invalid LeapTool object is returned.
 *
 * Note that ID values persist across frames, but only until tracking of a
 * particular object is lost. If tracking of a tool is lost and subsequently
 * regained, the new LeapTool object representing that tool may have a
 * different ID than that representing the tool in an earlier frame.
 *
 * @param toolId The ID value of a <LeapTool> object from a previous frame.
 * @returns The <LeapTool> object with the matching ID if one exists for this
 * hand in this frame; otherwise, an invalid LeapTool object is returned.
 * @available Since 1.0
 */
- (LeapTool *)tool:(int32_t)toolId;
/**
 * The center position of the palm in millimeters from the Leap origin.
 *
 * @returns The <LeapVector> representing the coordinates of the palm position.
 * @available Since 1.0
 */
- (LeapVector *)palmPosition;
@property (nonatomic, getter = palmPosition, readonly)LeapVector *palmPosition;
/**
 * The stabilized tip position of this Pointable.
 *
 * Smoothing and stabilization is performed in order to make
 * this value more suitable for interaction with 2D content.
 *
 * @returns A modified tip position of this Pointable object
 * with some additional smoothing and stabilization applied.
 * @available Since 1.0
 */
 - (LeapVector *)stabilizedPalmPosition;
@property (nonatomic, getter = stabilizedPalmPosition, readonly)LeapVector *stabilizedPalmPosition;
/**
 * The rate of change of the palm position in millimeters/second.
 *
 * @returns The <LeapVector> representing the coordinates of the palm velocity.
 * @available Since 1.0
 */
- (LeapVector *)palmVelocity;
@property (nonatomic, getter = palmVelocity, readonly)LeapVector *palmVelocity;
/**
 * The normal vector to the palm. If your hand is flat, this vector will
 * point downward, or "out" of the front surface of your palm.
 *
 * <img src="../docs/images/Leap_Palm_Vectors.png"/>
 *
 * The direction is expressed as a unit vector pointing in the same
 * direction as the palm normal (that is, a vector orthogonal to the palm).
 *
 * @returns The <LeapVector> normal to the plane formed by the palm.
 * @available Since 1.0
 */
- (LeapVector *)palmNormal;
@property (nonatomic, getter = palmNormal, readonly)LeapVector *palmNormal;
/**
 * The direction from the palm position toward the fingers.
 *
 * The direction is expressed as a unit vector pointing in the same
 * direction as the directed line from the palm position to the fingers.
 *
 * @returns The <LeapVector> pointing from the palm position toward the fingers.
 * @available Since 1.0
 */
- (LeapVector *)direction;
@property (nonatomic, getter = direction, readonly)LeapVector *direction;
/**
 * The center of a sphere fit to the curvature of this hand.
 *
 * This sphere is placed roughly as if the hand were holding a ball.
 *
 * <img src="../docs/images/Leap_Hand_Ball.png"/>
 *
 * @returns The <LeapVector> representing the center position of the sphere.
 * @available Since 1.0
 */
- (LeapVector *)sphereCenter;
@property (nonatomic, getter = sphereCenter, readonly)LeapVector *sphereCenter;
/**
 * The radius of a sphere fit to the curvature of this hand.
 *
 * This sphere is placed roughly as if the hand were holding a ball. Thus the
 * size of the sphere decreases as the fingers are curled into a fist.
 * @returns The radius of the sphere in millimeters.
 * @available Since 1.0
 */
- (float)sphereRadius;
@property (nonatomic, getter = sphereRadius, readonly)float sphereRadius;
/**
 * Reports whether this is a valid LeapHand object.
 *
 * @returns YES, if this LeapHand object contains valid tracking data.
 * @available Since 1.0
 */
- (BOOL)isValid;
@property (nonatomic, getter = isValid, readonly)BOOL isValid;
/**
 * The <LeapFrame> associated with this Hand.
 *
 * @returns The associated <LeapFrame> object, if available; otherwise,
 * an invalid LeapFrame object is returned.
 * @available Since 1.0
 */
- (LeapFrame *)frame;
@property (nonatomic, weak, getter = frame, readonly)LeapFrame *frame;
/**
 * The change of position of this hand between the current <LeapFrame> and
 * the specified LeapFrame.
 *
 * The returned translation vector provides the magnitude and direction of
 * the movement in millimeters.
 *
 * If a corresponding LeapHand object is not found in sinceFrame, or if either
 * this frame or sinceFrame are invalid LeapFrame objects, then this method
 * returns a zero vector.
 *
 * @param sinceFrame The starting <LeapFrame> for computing the translation.
 * @returns A <LeapVector> representing the heuristically determined change in
 * hand position between the current frame and that specified in the
 * sinceFrame parameter.
 * @available Since 1.0
 */
- (LeapVector *)translation:(const LeapFrame *)sinceFrame;
/**
 * The estimated probability that the hand motion between the current
 * frame and the specified <LeapFrame> is intended to be a translating motion.
 *
 * If a corresponding LeapHand object is not found in sinceFrame, or if either
 * this frame or sinceFrame are invalid LeapFrame objects, then this method
 * returns zero.
 *
 * @param sinceFrame The starting <LeapFrame> for computing the translation.
 * @returns A value between 0 and 1 representing the estimated probability
 * that the hand motion between the current frame and the specified frame
 * is intended to be a translating motion.
 * @available Since 1.0
 */
- (float)translationProbability:(const LeapFrame *)sinceFrame;
/**
 * The axis of rotation derived from the change in orientation of this
 * hand, and any associated fingers and tools, between the current <LeapFrame>
 * and the specified LeapFrame.
 *
 * The returned direction vector is normalized.
 *
 * If a corresponding LeapHand object is not found in sinceFrame, or if either
 * this frame or sinceFrame are invalid LeapFrame objects, then this method
 * returns a zero vector.
 *
 * @param sinceFrame The starting <LeapFrame> for computing the relative rotation.
 * @returns A <LeapVector> containing the normalized direction vector representing the heuristically
 * determined axis of rotational change of the hand between the current
 * frame and that specified in the sinceFrame parameter.
 * @available Since 1.0
 */
- (LeapVector *)rotationAxis:(const LeapFrame *)sinceFrame;
/**
 * The angle of rotation around the rotation axis derived from the change
 * in orientation of this hand, and any associated fingers and tools,
 * between the current <LeapFrame> and the specified LeapFrame.
 *
 * The returned angle is expressed in radians measured clockwise around the
 * rotation axis (using the right-hand rule) between the start and end frames.
 * The value is always between 0 and pi radians (0 and 180 degrees).
 *
 * If a corresponding LeapHand object is not found in sinceFrame, or if either
 * this frame or sinceFrame are invalid LeapFrame objects, then the angle of
 * rotation is zero.
 *
 * @param sinceFrame The starting <LeapFrame> for computing the relative rotation.
 * @returns A positive value representing the heuristically determined
 * rotational change of the hand between the current frame and that
 * specified in the sinceFrame parameter.
 * @available Since 1.0
 */
- (float)rotationAngle:(const LeapFrame *)sinceFrame;
/**
 * The angle of rotation around the specified axis derived from the change
 * in orientation of this hand, and any associated fingers and tools,
 * between the current <LeapFrame> and the specified LeapFrame.
 *
 * The returned angle is expressed in radians measured clockwise around the
 * rotation axis (using the right-hand rule) between the start and end frames.
 * The value is always between -pi and pi radians (-180 and 180 degrees).
 *
 * If a corresponding LeapHand object is not found in sinceFrame, or if either
 * this frame or sinceFrame are invalid LeapFrame objects, then the angle of
 * rotation is zero.
 *
 * @param sinceFrame The starting <LeapFrame> for computing the relative rotation.
 * @param axis A <LeapVector> representing the axis to measure rotation around.
 * @returns A value representing the heuristically determined rotational
 * change of the hand between the current frame and that specified in the
 * sinceFrame parameter around the specified axis.
 * @available Since 1.0
 */
- (float)rotationAngle:(const LeapFrame *)sinceFrame axis:(const LeapVector *)axis;
/**
 * The transform matrix expressing the rotation derived from the change
 * in orientation of this hand, and any associated fingers and tools,
 * between the current <LeapFrame> and the specified LeapFrame.
 *
 * If a corresponding LeapHand object is not found in sinceFrame, or if either
 * this frame or sinceFrame are invalid LeapFrame objects, then this method
 * returns an identity matrix.
 *
 * @param sinceFrame The starting <LeapFrame> for computing the relative rotation.
 * @returns A transformation <LeapMatrix> representing the heuristically determined
 * rotational change of the hand between the current frame and that specified
 * in the sinceFrame parameter.
 * @available Since 1.0
 */
- (LeapMatrix *)rotationMatrix:(const LeapFrame *)sinceFrame;
/**
 * The estimated probability that the hand motion between the current
 * <LeapFrame> and the specified LeapFrame is intended to be a rotating motion.
 *
 * If a corresponding LeapHand object is not found in sinceFrame, or if either
 * this frame or sinceFrame are invalid LeapFrame objects, then this method
 * returns zero.
 *
 * @param sinceFrame The starting <LeapFrame> for computing the relative rotation.
 * @returns A value between 0 and 1 representing the estimated probability
 * that the hand motion between the current frame and the specified frame
 * is intended to be a rotating motion.
 * @available Since 1.0
 */
- (float)rotationProbability:(const LeapFrame *)sinceFrame;
/**
 * The scale factor derived from this hand's motion between the current
 * <LeapFrame> and the specified LeapFrame.
 *
 * The scale factor is always positive. A value of 1.0 indicates no
 * scaling took place. Values between 0.0 and 1.0 indicate contraction
 * and values greater than 1.0 indicate expansion.
 *
 * The Leap derives scaling from the relative inward or outward motion of
 * a hand and its associated fingers and tools (independent of translation
 * and rotation).
 *
 * If a corresponding LeapHand object is not found in sinceFrame, or if either
 * this frame or sinceFrame are invalid LeapFrame objects, then this method
 * returns 1.0.
 *
 * @param sinceFrame The starting <LeapFrame> for computing the relative scaling.
 * @returns A positive value representing the heuristically determined
 * scaling change ratio of the hand between the current frame and that
 * specified in the sinceFrame parameter.
 * @available Since 1.0
 */
- (float)scaleFactor:(const LeapFrame *)sinceFrame;
/**
 * The estimated probability that the hand motion between the current
 * <LeapFrame> and the specified LeapFrame is intended to be a scaling motion.
 *
 * If a corresponding LeapHand object is not found in sinceFrame, or if either
 * this frame or sinceFrame are invalid LeapFrame objects, then this method
 * returns zero.
 *
 * @param sinceFrame The starting <LeapFrame> for computing the relative scaling.
 * @returns A value between 0 and 1 representing the estimated probability
 * that the hand motion between the current frame and the specified frame
 * is intended to be a scaling motion.
 * @available Since 1.0
 */
- (float)scaleProbability:(const LeapFrame *)sinceFrame;
/**
 * Returns an invalid LeapHand object.
 *
 * You can use the instance returned by this function in comparisons testing
 * whether a given LeapHand instance is valid or invalid. (You can also use the
 * LeapHand isValid: function.)
 *
 * @returns The invalid LeapHand instance.
 * @available Since 1.0
 */
+ (LeapHand *)invalid;

@end

//////////////////////////////////////////////////////////////////////////
//POINTABLEorHANDLIST CATEGORY
/**
 * The LeapPointableOrHandList category provides methods for getting objects
 * from an NSArray containing <LeapPointable>, <LeapFinger>, <LeapTool>, or
 * <LeapHand> objects based on their physical position within the Leap
 * coordinate system.
 * @available Since 1.0
 */
@interface NSArray (LeapPointableOrHandList)
/**
 * The member of the list that is farthest to the left within the standard
 * Leap frame of reference (i.e has the smallest X coordinate).
 *
 * Returns nil if the NSArray is empty.
 * @available Since 1.0
 */
- (id)leftmost;
/**
 * The member of the list that is farthest to the right within the standard
 * Leap frame of reference (i.e has the largest X coordinate).
 *
 * Returns nil if the NSArray is empty.
 * @available Since 1.0
 */
- (id)rightmost;
/**
 * The member of the list that is farthest to the front within the standard
 * Leap frame of reference (i.e has the smallest Z coordinate).
 *
 * Returns nil if the NSArray is empty.
 * @available Since 1.0
 */
- (id)frontmost;

@end

//////////////////////////////////////////////////////////////////////////
//SCREEN
/**
 * The LeapScreen class represents a computer monitor screen.
 *
 * *Important: The Screen Locator class used to define the position of monitor
 * screens in relation to the Leap Motion controller has been removed. Without this
 * tool, the Screen class is of extremely limited utility. We are re-evaluating this 
 * feature due to the difficulty in performing the location procedure and continuing, 
 * wide-spread confusion about its purpose. The Screen class still exists in the API, 
 * but should not be used.*
 *
 * The LeapScreen class reports characteristics describing the position and
 * orientation of the monitor screen within the Leap coordinate system. These
 * characteristics include the bottom-left corner position of the screen,
 * direction vectors for the horizontal and vertical axes of the screen, and
 * the screen's normal vector. The screen must be properly registered with the
 * Screen Locator for the Leap to report these characteristics accurately.
 * The LeapScreen class also reports the size of the screen in pixels, using
 * information obtained from the operating system. (Run the Screen Locator
 * from the Leap Application Settings dialog, on the Screen page.)
 *
 * Get an array containing the available LeapScreen objects from a connected
 * LeapController object:
 *
 * @example Screen_Closest_1.txt
 *
 * You can get the point of intersection between the screen and a ray
 * projected from a <LeapPointable> object using the LeapScreen
 * intersect:normalize:clampRatio function.
 * Likewise, you can get the distance to the closest point on the screen to a point in space
 * using the LeapScreen distanceToPoint: function. Again, the screen location
 * must be registered with the Screen Locator for these functions to
 * return accurate values.
 *
 * Note that LeapScreen objects can be invalid, which means that they do not contain
 * valid screen coordinate data and do not correspond to a physical entity.
 * Test for validity with the LeapScreen isValid: function.
 * @available Since 1.0
 */
@interface LeapScreen : NSObject

- (NSString *)description;
/**
 * A unique identifier for this screen based on the screen
 * information in the configuration. A default screen with ID, *0*,
 * always exists and contains default characteristics, even if no screens
 * have been located.
 * @available Since 1.0
 */
- (int32_t)id;
@property (nonatomic, getter = id, readonly)int32_t id;
/**
 * Returns the intersection between this screen and a ray projecting from a
 * Pointable object.
 *
 * The projected ray emanates from the <[LeapPointable tipPosition]> along the
 * pointable's direction vector.
 *
 * @example Screen_Normalized_1.txt
 *
 * Set the normalize parameter to true to request the intersection point in
 * normalized screen coordinates. Normalized screen coordinates are usually
 * values between 0 and 1, where 0 represents the screen's origin at the
 * bottom-left corner and 1 represents the opposite edge (either top or
 * right). When you request normalized coordinates, the z-component of the
 * returned vector is zero. Multiply a normalized coordinate by the values
 * returned by <[LeapScreen widthPixels]> or <[LeapScreen heightPixels]> to calculate
 * the screen position in pixels (remembering that many other computer
 * graphics coordinate systems place the origin in the top-left corner).
 *
 * Set the normalize parameter to false to request the intersection point
 * in Leap coordinates (millimeters from the Leap origin).
 *
 * If the LeapPointable object points outside the screen's border (but still
 * intersects the plane in which the screen lies), the returned intersection
 * point is clamped to the nearest point on the edge of the screen.
 *
 * You can use the clampRatio parameter to contract or expand the area in
 * which you can point. For example, if you set the clampRatio parameter to
 * 0.5, then the positions reported for intersection points outside the
 * central 50% of the screen are moved to the border of this smaller area.
 * If, on the other hand, you expanded the area by setting clampRatio to
 * a value such as 3.0, then you could point well outside screen's physical
 * boundary before the intersection points would be clamped. The positions
 * for any points clamped would also be placed on this larger outer border.
 * The positions reported for any intersection points inside the clamping
 * border are unaffected by clamping.
 *
 * If the LeapPointable object does not point toward the plane of the screen
 * (i.e. it is pointing parallel to or away from the screen), then the
 * components of the returned vector are all set to NaN (not-a-number).
 *
 * To get the physical distance from the tip of a pointable object to the
 * screen intersection point, set the normalize parameter to NO to get the
 * intersection coordinates in terms of the Leap coordinate system and then
 * calculate the distance between the two points:
 *
 * @example Screen_DistanceTo_1.txt
 *
 * @param pointable The <LeapPointable> object to check for screen intersection.
 *
 * @param normalize If true, return normalized coordinates representing
 * the intersection point as a percentage of the screen's width and height.
 * If false, return Leap coordinates (millimeters from the Leap origin,
 * which is located at the center of the top surface of the Leap device).
 * If true and the clampRatio parameter is set to 1.0, coordinates will be
 * of the form (0..1, 0..1, 0). Setting the clampRatio to a different value
 * changes the range for normalized coordinates. For example, a clampRatio
 * of 5.0 changes the range of values to be of the form (-2..3, -2..3, 0).
 *
 * @param clampRatio Adjusts the clamping border around this screen.
 * By default this ratio is 1.0, and the border corresponds to the actual
 * boundaries of the screen. Setting clampRatio to 0.5 would reduce the
 * interaction area. Likewise, setting the ratio to 2.0 would increase the
 * interaction area, adding 50% around each edge of the physical monitor.
 * Intersection points outside the interaction area are repositioned to
 * the closest point on the clamping border before the vector is returned.
 *
 * @returns A <LeapVector> containing the coordinates of the intersection between
 * this screen and a ray projecting from the specified Pointable object.
 * @available Since 1.0
 */
- (LeapVector *)intersect:(LeapPointable *)pointable normalize:(BOOL)normalize clampRatio:(float)clampRatio;

/**
 * Returns the intersection between this screen and a ray projecting from
 * the specified position along the specified direction.
 *
 * Set the normalize parameter to true to request the intersection point in
 * normalized screen coordinates. Normalized screen coordinates are usually
 * values between 0 and 1, where 0 represents the screen's origin at the
 * bottom-left corner and 1 represents the opposite edge (either top or
 * right). When you request normalized coordinates, the z-component of the
 * returned vector is zero. Multiply a normalized coordinate by the values
 * returned by <[LeapScreen widthPixels]> or <[LeapScreen heightPixels]> to calculate
 * the screen position in pixels (remembering that many other computer
 * graphics coordinate systems place the origin in the top-left corner).
 *
 * Set the normalize parameter to false to request the intersection point
 * in Leap coordinates (millimeters from the Leap origin).
 *
 * If the specified ray points outside the screen's border (but still
 * intersects the plane in which the screen lies), the returned intersection
 * point is clamped to the nearest point on the edge of the screen.
 *
 * You can use the clampRatio parameter to contract or expand the area in
 * which you can point. For example, if you set the clampRatio parameter to
 * 0.5, then the positions reported for intersection points outside the
 * central 50% of the screen are moved to the border of this smaller area.
 * If, on the other hand, you expanded the area by setting clampRatio to
 * a value such as 3.0, then you could point well outside screen's physical
 * boundary before the intersection points would be clamped. The positions
 * for any points clamped would also be placed on this larger outer border.
 * The positions reported for any intersection points inside the clamping
 * border are unaffected by clamping.
 *
 * If the specified ray does not point toward the plane of the screen
 * (i.e. it is pointing parallel to or away from the screen), then the
 * components of the returned vector are all set to NaN (not-a-number).
 *
 * @param position The position from which to check for screen intersection.
 * @param direction The direction in which to check for screen intersection.
 *
 * @param normalize If true, return normalized coordinates representing
 * the intersection point as a percentage of the screen's width and height.
 * If false, return Leap coordinates (millimeters from the Leap origin,
 * which is located at the center of the top surface of the Leap device).
 * If true and the clampRatio parameter is set to 1.0, coordinates will be
 * of the form (0..1, 0..1, 0). Setting the clampRatio to a different value
 * changes the range for normalized coordinates. For example, a clampRatio
 * of 5.0 changes the range of values to be of the form (-2..3, -2..3, 0).
 *
 * @param clampRatio Adjusts the clamping border around this screen.
 * By default this ratio is 1.0, and the border corresponds to the actual
 * boundaries of the screen. Setting clampRatio to 0.5 would reduce the
 * interaction area. Likewise, setting the ratio to 2.0 would increase the
 * interaction area, adding 50% around each edge of the physical monitor.
 * Intersection points outside the interaction area are repositioned to
 * the closest point on the clamping border before the vector is returned.
 *
 * @returns A Vector containing the coordinates of the intersection between
 * this screen and a ray projecting from the specified position in the
 * specified direction.
 * @available Since 1.0
 */
- (LeapVector *)intersect:(const LeapVector *)position direction:(const LeapVector *)direction normalize:(BOOL)normalize clampRatio:(float)clampRatio;

/**
 * Returns the projection from the specified position onto this screen.
 *
 * @example Screen_Project_1.txt
 *
 * Set the normalize parameter to true to request the projection point in
 * normalized screen coordinates. Normalized screen coordinates are usually
 * values between 0 and 1, where 0 represents the screen's origin at the
 * bottom-left corner and 1 represents the opposite edge (either top or
 * right). When you request normalized coordinates, the z-component of the
 * returned vector is zero. Multiply a normalized coordinate by the values
 * returned by <[LeapScreen widthPixels]> or <[LeapScreen heightPixels]> to calculate
 * the screen position in pixels (remembering that many other computer
 * graphics coordinate systems place the origin in the top-left corner).
 *
 * Set the normalize parameter to false to request the projection point
 * in Leap coordinates (millimeters from the Leap origin).
 *
 * If the specified point projects outside the screen's border, the returned
 * projection point is clamped to the nearest point on the edge of the screen.
 *
 * You can use the clampRatio parameter to contract or expand the area in
 * which you can point. For example, if you set the clampRatio parameter to
 * 0.5, then the positions reported for projection points outside the
 * central 50% of the screen are moved to the border of this smaller area.
 * If, on the other hand, you expanded the area by setting clampRatio to
 * a value such as 3.0, then you could point well outside screen's physical
 * boundary before the projection points would be clamped. The positions
 * for any points clamped would also be placed on this larger outer border.
 * The positions reported for any projection points inside the clamping
 * border are unaffected by clamping.
 *
 * @param position The position from which to project onto this screen.
 *
 * @param normalize If true, return normalized coordinates representing
 * the projection point as a percentage of the screen's width and height.
 * If false, return Leap coordinates (millimeters from the Leap origin,
 * which is located at the center of the top surface of the Leap device).
 * If true and the clampRatio parameter is set to 1.0, coordinates will be
 * of the form (0..1, 0..1, 0). Setting the clampRatio to a different value
 * changes the range for normalized coordinates. For example, a clampRatio
 * of 5.0 changes the range of values to be of the form (-2..3, -2..3, 0).
 *
 * @param clampRatio Adjusts the clamping border around this screen.
 * By default this ratio is 1.0, and the border corresponds to the actual
 * boundaries of the screen. Setting clampRatio to 0.5 would reduce the
 * interaction area. Likewise, setting the ratio to 2.0 would increase the
 * interaction area, adding 50% around each edge of the physical monitor.
 * Projection points outside the interaction area are repositioned to
 * the closest point on the clamping border before the vector is returned.
 *
 * @returns A Vector containing the coordinates of the projection between
 * this screen and a ray projecting from the specified position onto the
 * screen along its normal vector.
 * @available Since 1.0
 */
- (LeapVector *)project:(LeapVector *)position normalize:(BOOL)normalize clampRatio:(float)clampRatio;
/**
 * A <LeapVector> representing the horizontal axis of this LeapScreen within the
 * Leap coordinate system.
 *
 * The magnitude of this vector estimates the physical width of this LeapScreen
 * in millimeters. The direction of this vector is parallel to the bottom
 * edge of the screen and points toward the right edge of the screen.
 *
 * Together, horizontalAxis, verticalAxis, and bottomLeftCorner
 * describe the physical position, size and orientation of this LeapScreen.
 *
 * @returns A <LeapVector> representing the bottom, horizontal edge of this LeapScreen.
 * @available Since 1.0
 */
- (LeapVector *)horizontalAxis;
@property (nonatomic, getter = horizontalAxis, readonly)LeapVector *horizontalAxis;
/**
 * A <LeapVector> representing the vertical axis of this LeapScreen within the
 * Leap coordinate system.
 *
 * The magnitude of this vector estimates the physical height of this LeapScreen
 * in millimeters. The direction of this vector is parallel to the left
 * edge of the screen and points toward the top edge of the screen.
 *
 * Together, horizontalAxis, verticalAxis, and bottomLeftCorner
 * describe the physical position, size and orientation of this screen.
 *
 * @returns A <LeapVector> representing the left, vertical edge of this LeapScreen.
 * @available Since 1.0
 */
- (LeapVector *)verticalAxis;
@property (nonatomic, getter = verticalAxis, readonly)LeapVector *verticalAxis;
/**
 * A <LeapVector> representing the bottom left corner of this LeapScreen within the
 * Leap coordinate system.
 *
 * The point represented by this vector defines the origin of the screen
 * in the Leap coordinate system.
 *
 * Together, horizontalAxis, verticalAxis, and bottomLeftCorner
 * describe the physical position, size and orientation of this LeapScreen.
 *
 * @returns A <LeapVector> containing the coordinates of the bottom-left corner
 * of this LeapScreen.
 * @available Since 1.0
 */
- (LeapVector *)bottomLeftCorner;
@property (nonatomic, getter = bottomLeftCorner, readonly)LeapVector *bottomLeftCorner;
/**
 * A <LeapVector> normal to the plane in which this LeapScreen lies.
 *
 * The normal vector is a unit direction vector orthogonal to the screen's
 * surface plane. It points toward a viewer positioned for typical use of
 * the monitor.
 *
 * @returns A <LeapVector> representing this LeapScreen's normal vector.
 * @available Since 1.0
 */
- (LeapVector *)normal;
@property (nonatomic, getter = normal, readonly)LeapVector *normal;
/**
 * The horizontal resolution of this screen, in pixels.
 *
 * @returns The width of this LeapScreen in pixels.
 * @available Since 1.0
 */
- (int)widthPixels;
@property (nonatomic, getter = widthPixels, readonly)int widthPixels;
/**
 * The vertical resolution of this screen, in pixels.
 *
 * @returns The height of this LeapScreen in pixels.
 * @available Since 1.0
 */
- (int)heightPixels;
@property (nonatomic, getter = heightPixels, readonly)int heightPixels;
/**
 * The shortest distance from the specified point to the plane in which this
 * LeapScreen lies.
 *
 * @example Screen_DistanceTo_2.txt
 *
 * @param point The point of interest.
 * @returns The length of the perpendicular line segment extending from
 * the plane this LeapScreen lies in to the specified point.
 * @available Since 1.0
 */
- (float)distanceToPoint:(const LeapVector *)point;
/**
 * Reports whether this is a valid LeapScreen object.
 *
 * **Important:** A valid LeapScreen object does not necessarily contain
 * up-to-date screen location information. Location information is only
 * accurate until the Leap device or the monitor are moved. In addition, the
 * primary screen always contains default location information even if the
 * user has never run the screen location utility. This default location
 * information will not return accurate results.
 *
 * @returns YES, if this LeapScreen object contains valid data.
 * @available Since 1.0
 */
- (BOOL)isValid;
@property (nonatomic, getter = isValid, readonly)BOOL isValid;
/**
 * Compare LeapScreen object equality.
 * Two LeapScreen objects are equal if and only if both objects represent the
 * exact same screens and both LeapScreen objects are valid.
 * @param other The LeapScreen object to compare.
 * @available Since 1.0
 */
- (BOOL)equals:(const LeapScreen *)other;
// not provided: not_equals
// user should emulate with !scr.equals(...)
/**
 * Returns an invalid LeapScreen object.
 *
 * You can use the instance returned by this function in comparisons testing
 * whether a given LeapScreen instance is valid or invalid. (You can also use the
 * LeapScreen isValid function.)
 *
 * @returns The invalid LeapScreen instance.
 * @available Since 1.0
 */
+ (LeapScreen *)invalid;

@end

//////////////////////////////////////////////////////////////////////////
// SCREENLIST Category
/**
 * The LeapScreenList category supplies methods for getting a screen from
 * an NSArray containing <LeapScreen> objects based on the relationship between
 * the screens and a LeapPointable object or point.
 * @available Since 1.0
 */
@interface NSArray (LeapScreenList)
/**
 * Gets the closest <LeapScreen> intercepting a ray projecting from the specified
 * Pointable object.
 *
 * The projected ray emanates from the Pointable tipPosition along the
 * Pointable's direction vector. If the projected ray does not intersect
 * any screen surface directly, then the Leap checks for intersection with
 * the planes extending from the surfaces of the known screens
 * and returns the LeapScreen with the closest intersection.
 *
 * @example Screen_Closest_2.txt
 *
 * If no intersections are found (i.e. the ray is directed parallel to or
 * away from all known screens), then an invalid LeapScreen object is returned.
 *
 * *Note:* Be sure to test whether the LeapScreen object returned by this method
 * is valid. Attempting to use an invalid LeapScreen object will lead to
 * incorrect results.
 *
 * @param pointable The <LeapPointable> object to check for screen intersection.
 * @returns The closest <LeapScreen> toward which the specified LeapPointable object
 * is pointing, or, if the pointable is not pointing in the direction of
 * any known screen, an invalid LeapScreen object.
 * @available Since 1.0
 */
- (LeapScreen *)closestScreenHit:(LeapPointable *)pointable;
/**
 * Gets the closest <LeapScreen> intercepting a ray projecting from the specified
 * position in the specified direction.
 *
 * The projected ray emanates from the position along the direction vector.
 * If the projected ray does not intersect any screen surface directly,
 * then the Leap checks for intersection with the planes extending from the
 * surfaces of the known screens and returns the LeapScreen with the closest
 * intersection.
 *
 * If no intersections are found (i.e. the ray is directed parallel to or
 * away from all known screens), then an invalid LeapScreen object is returned.
 *
 * *Note:* Be sure to test whether the LeapScreen object returned by this method
 * is valid. Attempting to use an invalid LeapScreen object will lead to
 * incorrect results.
 *
 * @param position The position from which to check for screen intersection.
 * @param direction The direction in which to check for screen intersection.
 * @returns The closest <LeapScreen> toward which the specified ray is pointing,
 * or, if the ray is not pointing in the direction of any known screen,
 * an invalid LeapScreen object.
 * @available Since 1.0
 */
- (LeapScreen *)closestScreenHit:(const LeapVector *)position direction:(const LeapVector *)direction;
/**
 * Gets the <LeapScreen> closest to the specified position.
 *
 * @example Screen_Closest_3.txt
 *
 * The specified position is projected along each screen's normal vector
 * onto the screen's plane. The screen whose projected point is closest to
 * the specified position is returned. Call <[LeapScreen project:position normalize:NO clampRatio:1.0f]>
 * on the returned LeapScreen object to find the projected point.
 *
 * @param position The position from which to check for screen projection.
 * @returns The closest <LeapScreen> onto which the specified position is projected.
 * @available Since 1.0
 */
- (LeapScreen *)closestScreen:(LeapVector *)position;
@end

//////////////////////////////////////////////////////////////////////////
//DEVICE
/**
 * The LeapDevice class represents a physically connected device.
 *
 * The LeapDevice class contains information related to a particular connected
 * device such as field of view, device id, and calibrated positions.
 *
 * Note that Device objects can be invalid, which means that they do not contain
 * valid device information and do not correspond to a physical device.
 * Test for validity with the <[LeapDevice isValid]> function.
 * @available Since 1.0
 */
@interface LeapDevice : NSObject

- (NSString *)description;
/**
 * The angle of view along the x axis of this device, in radians.
 *
 * <img src="../docs/images/Leap_horizontalViewAngle.png"/>
 *
 * The Leap Motion controller scans a volume in the shape of an inverted pyramid
 * centered at the device's center and extending upwards. The horizontalViewAngle
 * reports the view angle along the long dimension of the device.
 * @available Since 1.0
 */
- (float)horizontalViewAngle;
@property (nonatomic, getter = horizontalViewAngle, readonly)float horizontalViewAngle;
/**
 * The angle of view along the z axis of this device, in radians.
 *
 * <img src="../docs/images/Leap_verticalViewAngle.png"/>
 *
 * The Leap Motion controller scans a region in the shape of an inverted pyramid
 * centered at the device's center and extending upwards. The verticalViewAngle
 * reports the view angle along the short dimension of the device.
 * @available Since 1.0
 */
- (float)verticalViewAngle;
@property (nonatomic, getter = verticalViewAngle, readonly)float verticalViewAngle;
/**
 * The maximum reliable tracking range, in millimeters.
 *
 * The range reports the maximum recommended distance from the device center
 * for which tracking is expected to be reliable. This distance is not a hard limit.
 * Tracking may be still be functional above this distance or begin to degrade slightly
 * before this distance depending on calibration and extreme environmental conditions.
 * @available Since 1.0
 */
- (float)range;
@property (nonatomic, getter = range, readonly)float range;
/**
 * The distance to the nearest edge of the Leap Motion controller's view volume.
 *
 * The view volume is an axis-aligned, inverted pyramid centered on the device origin
 * and extending upward to the range limit. The walls of the pyramid are described
 * by the horizontalViewAngle and verticalViewAngle properties and the roof by the range.
 * This function estimates the distance between the specified input position and the
 * nearest wall or roof of the view volume.
 *
 * @param position The point to use for the distance calculation.
 * @returns The distance in millimeters from the input position to the nearest boundary.
 * @available Since 1.0
 */
- (float)distanceToBoundary:(const LeapVector *)position;
/**
 * Reports whether this is a valid LeapDevice object.
 *
 * @returns True, if this LeapDevice object contains valid data.
 * @available Since 1.0
 */
- (BOOL)isValid;
@property (nonatomic, getter = isValid, readonly)BOOL isValid;
/**
 * Compare LeapDevice object inequality.
 * Two LeapDevice objects are equal if and only if both LeapDevice objects represent the
 * exact same LeapDevice and both Devices are valid.
 * @available Since 1.0
 */
- (BOOL)equals:(const LeapDevice *)other;
// not provided: not_equals
// user should emulate with !scr.equals(...)
/**
 * Returns an invalid LeapDevice object.
 *
 * You can use the instance returned by this function in comparisons testing
 * whether a given LeapDevice instance is valid or invalid. (You can also use the
 * <[LeapDevice isValid]> function.)
 *
 * @returns The invalid LeapDevice instance.
 * @available Since 1.0
 */
+ (LeapDevice *)invalid;

@end

//////////////////////////////////////////////////////////////////////////
//INTERACTIONBOX
/**
 * The LeapInteractionBox class represents a box-shaped region completely
 * within the field of view of the Leap Motion controller.
 *
 * The interaction box is an axis-aligned rectangular prism and provides normalized
 * coordinates for hands, fingers, and tools within this box. The InteractionBox class
 * can make it easier to map positions in the Leap Motion coordinate system to 2D or
 * 3D coordinate systems used for application drawing.
 *
 * <img src="../docs/images/Leap_InteractionBox.png"/>
 *
 * The LeapInteractionBox region is defined by a center and dimensions along the x, y,
 * and z axes.
 * @available Since 1.0
 */
@interface LeapInteractionBox : NSObject

- (NSString *)description;
/**
 * Normalizes the coordinates of a point using the interaction box.
 *
 * Coordinates from the Leap frame of reference (millimeters) are converted
 * to a range of [0..1] such that the minimum value of the LeapInteractionBox maps to 0
 * and the maximum value of the LeapInteractionBox maps to 1.
 *
 * @param position The input position in device coordinates.
 * @param clamp Whether or not to limit the output value to the range [0,1] when the
 * input position is outside the LeapInteractionBox. Defaults to true.
 * @returns The normalized position.
 * @available Since 1.0
 */
- (LeapVector *)normalizePoint:(const LeapVector *)position clamp:(BOOL)clamp;
/**
 * Converts a position defined by normalized LeapInteractionBox coordinates into device
 * coordinates in millimeters.
 *
 * This function performs the inverse of [LeapInteractionBox normalizePoint:].
 *
 * @param normalizedPosition The input position in LeapInteractionBox coordinates.
 * @returns The corresponding denormalized position in device coordinates.
 * @available Since 1.0
 */
- (LeapVector *)denormalizePoint:(const LeapVector *)position;
/**
 * The center of the LeapInteractionBox in device coordinates (millimeters). This point
 * is equidistant from all sides of the box.
 *
 * @returns The LeapInteractionBox center in device coordinates.
 * @available Since 1.0
 */
- (LeapVector *)center;
@property (nonatomic, getter = center, readonly)LeapVector *center;
/**
 * The width of the LeapInteractionBox in millimeters, measured along the x-axis.
 *
 * @returns The LeapInteractionBox width in millimeters.
 * @available Since 1.0
 */
- (float)width;
@property (nonatomic, getter = width, readonly)float width;
/**
 * The height of the LeapInteractionBox in millimeters, measured along the y-axis.
 *
 * @returns The LeapInteractionBox height in millimeters.
 * @available Since 1.0
 */
- (float)height;
@property (nonatomic, getter = width, readonly)float height;
/**
 * The depth of the LeapInteractionBox in millimeters, measured along the z-axis.
 *
 * @returns The LeapInteractionBox depth in millimeters.
 * @available Since 1.0
 */
- (float)depth;
@property (nonatomic, getter = width, readonly)float depth;
/**
 * Reports whether this is a valid LeapInteractionBox object.
 *
 * @returns True, if this LeapInteractionBox object contains valid data.
 * @available Since 1.0
 */
- (BOOL)isValid;
@property (nonatomic, getter = isValid, readonly)BOOL isValid;
/**
 * Reports whether this is a valid LeapInteractionBox object.
 *
 * @returns True, if this LeapInteractionBox object contains valid data.
 * @available Since 1.0
 */
- (BOOL)equals:(const LeapInteractionBox *)other;
// not provided: not_equals
// user should emulate with !scr.equals(...)
/**
 * Returns an invalid LeapInteractionBox object.
 *
 * You can use the instance returned by this function in comparisons testing
 * whether a given LeapInteractionBox instance is valid or invalid. (You can also use the
 * <[LeapInterationBox isValid]> function.)
 *
 * @returns The invalid InteractionBox instance.
 * @available Since 1.0
 */
+ (LeapInteractionBox *)invalid;

@end

//////////////////////////////////////////////////////////////////////////
//GESTURE
/**
 * The LeapGesture class represents a recognized movement by the user.
 *
 * The Leap watches the activity within its field of view for certain movement
 * patterns typical of a user gesture or command. For example, a movement from side to
 * side with the hand can indicate a swipe gesture, while a finger poking forward
 * can indicate a screen tap gesture.
 *
 * When the Leap recognizes a gesture, it assigns an ID and adds a
 * LeapGesture object to the frame gesture list. For continuous gestures, which
 * occur over many frames, the Leap updates the gesture by adding
 * a LeapGesture object having the same ID and updated properties in each
 * subsequent frame.
 *
 * **Important:** Recognition for each type of gesture must be enabled using the
 * <[LeapController enableGesture:enable:]> function; otherwise **no gestures are
 * recognized or reported**.
 *
 * Subclasses of LeapGesture define the properties for the specific movement patterns
 * recognized by the Leap.
 *
 * The LeapGesture subclasses for include:
 *
 * * <LeapCircleGesture> -- A circular movement by a finger.
 * * <LeapSwipeGesture> -- A straight line movement by the hand with fingers extended.
 * * <LeapScreenTapGesture> -- A forward tapping movement by a finger.
 * * <LeapKeyTapGesture> -- A downward tapping movement by a finger.
 *
 * Circle and swipe gestures are continuous and these objects can have a
 * state of start, update, and stop.
 *
 * The tap gestures are discrete. The Leap only creates a single
 * LeapScreenTapGesture or LeapKeyTapGesture object appears for each tap and that
 * object is always assigned the stop state.
 *
 * Get valid LeapGesture instances from a <LeapFrame> object. You can get a list of gestures
 * with the <[LeapFrame gestures:]> method. You can also
 * use the <[LeapFrame gesture:]> method to find a gesture in the current frame using
 * an ID value obtained in a previous frame.
 *
 * LeapGesture objects can be invalid. For example, when you get a gesture by ID
 * using `[LeapFrame gesture:]`, and there is no gesture with that ID in the current
 * frame, then `gesture:` returns an Invalid LeapGesture object (rather than a null
 * value). Always check object validity in situations where a gesture might be
 * invalid.
 *
 * The following keys can be used with the LeapConfig class to configure the gesture
 * recognizer:
 *
 * Key string | Value type | Default value | Units
 * -----------|------------|---------------|------
 * Gesture.Circle.MinRadius | float | 5.0 | mm
 * Gesture.Circle.MinArc | float | 1.5*pi | radians
 * Gesture.Swipe.MinLength | float | 150 | mm
 * Gesture.Swipe.MinVelocity | float | 1000 | mm/s
 * Gesture.KeyTap.MinDownVelocity | float | 50 | mm/s
 * Gesture.KeyTap.HistorySeconds | float | 0.1 | s
 * Gesture.KeyTap.MinDistance | float | 5.0 | mm
 * Gesture.ScreenTap.MinForwardVelocity  | float | 50 | mm/s
 * Gesture.ScreenTap.HistorySeconds | float | 0.1 | s
 * Gesture.ScreenTap.MinDistance | float | 3.0 | mm
 * @available Since 1.0
 */
@interface LeapGesture : NSObject
/**
 * The <LeapFrame> containing this LeapGesture instance.
 *
 * @return The parent <LeapFrame> object.
 * @available Since 1.0
 */
@property (nonatomic, strong, readonly)LeapFrame *frame;
/**
 * The list of hands associated with this LeapGesture, if any.
 *
 * If no hands are related to this gesture, the list is empty.
 *
 * @return NSArray the list of related <LeapHand> objects.
 * @available Since 1.0
 */
@property (nonatomic, strong, readonly)NSArray *hands;
/**
 * The list of fingers and tools associated with this LeapGesture, if any.
 *
 * If no <LeapPointable> objects are related to this gesture, the list is empty.
 *
 * @return NSArray the list of related <LeapPointable> objects.
 * @available Since 1.0
 */
@property (nonatomic, strong, readonly)NSArray *pointables;

- (NSString *)description;
/**
 * The gesture type.
 *
 *
 * The supported types of gestures are defined by the LeapGestureType enumeration:
 *
 * * LEAP_GESTURE_TYPE_INVALID -- An invalid type.
 * * LEAP_GESTURE_TYPE_SWIPE  -- A straight line movement by the hand with fingers extended.
 * * LEAP_GESTURE_TYPE_CIRCLE -- A circular movement by a finger.
 * * LEAP_GESTURE_TYPE_SCREEN_TAP -- A forward tapping movement by a finger.
 * * LEAP_GESTURE_TYPE_KEY_TAP -- A downward tapping movement by a finger.
 *
 * @returns LeapGestureType A value from the LeapGestureType enumeration.
 * @available Since 1.0
 */
- (LeapGestureType)type;
@property (nonatomic, getter = type, readonly)LeapGestureType type;
/**
 * The gesture state.
 *
 * Recognized movements occur over time and have a beginning, a middle,
 * and an end. The 'state' attribute reports where in that sequence this
 * LeapGesture object falls.
 *
 * The possible gesture states are defined by the LeapGestureState enumeration:
 *
 * * LEAP_GESTURE_STATE_INVALID -- An invalid state.
 * * LEAP_GESTURE_STATE_START -- The gesture is starting. Just enough has happened to recognize it.
 * * LEAP_GESTURE_STATE_UPDATE -- The gesture is in progress. (Note: not all gestures have updates).
 * * LEAP_GESTURE_STATE_STOP -- The gesture has completed or stopped.
 *
 * @returns LeapGestureState A value from the LeapGestureState enumeration.
 * @available Since 1.0
 */
- (LeapGestureState)state;
@property (nonatomic, getter = state, readonly)LeapGestureState state;
/**
 * The gesture ID.
 *
 * All LeapGesture objects belonging to the same recognized movement share the
 * same ID value. Use the ID value with the <[LeapFrame gesture:]> method to
 * find updates related to this LeapGesture object in subsequent frames.
 *
 * @returns int32_t the ID of this LeapGesture.
 * @available Since 1.0
 */
- (int32_t)id;
@property (nonatomic, getter = id, readonly)int32_t id;
/**
 * The elapsed duration of the recognized movement up to the
 * frame containing this LeapGesture object, in microseconds.
 *
 * The duration reported for the first LeapGesture in the sequence (with the
 * LEAP_GESTURE_STATE_START state) will typically be a small positive number since
 * the movement must progress far enough for the Leap to recognize it as
 * an intentional gesture.
 *
 * @return int64_t the elapsed duration in microseconds.
 * @available Since 1.0
 */
- (int64_t)duration;
@property (nonatomic, getter = duration, readonly)int64_t duration;
/**
 * The elapsed duration in seconds.
 * @see duration
 * @return float the elapsed duration in seconds.
 * @available Since 1.0
 */
- (float)durationSeconds;
@property (nonatomic, getter = durationSeconds, readonly)float durationSeconds;
/**
 * Reports whether this LeapGesture instance represents a valid gesture.
 *
 * An invalid LeapGesture object does not represent a snapshot of a recognized
 * movement. Invalid LeapGesture objects are returned when a valid object cannot
 * be provided. For example, when you get an gesture by ID
 * using <[LeapFrame gesture:]>, and there is no gesture with that ID in the current
 * frame, then `gesture:` returns an Invalid LeapGesture object (rather than a null
 * value). Always check object validity in situations where an gesture might be
 * invalid.
 *
 * @returns bool Yes, if this is a valid LeapGesture instance; NO, otherwise.
 * @available Since 1.0
 */
- (BOOL)isValid;
@property (nonatomic, getter = isValid, readonly)BOOL isValid;
/**
 * Compare LeapGesture object equality.
 *
 * Two LeapGestures are equal if they represent the same snapshot of the same
 * recognized movement.
 * @param other The LeapGesture object to compare.
 * @available Since 1.0
 */
- (BOOL)equals:(const LeapGesture *)other;
// not provided: not_equals
// user should emulate with !scr.equals(...)
/**
 * Returns an invalid LeapGesture object.
 *
 * You can use the instance returned by this function in comparisons testing
 * whether a given LeapGesture instance is valid or invalid. (You can also use the
 * <[LeapGesture isValid]> function.)
 *
 * @returns The invalid LeapGesture instance.
 * @available Since 1.0
 */
+ (LeapGesture *)invalid;

@end

////////////////////////////////////////////////////////////////////////
//SWIPE GESTURE
/**
 * The LeapSwipeGesture class represents a swiping motion of a finger or tool.
 *
 * <img src="../docs/images/Leap_Gesture_Swipe.png"/>
 *
 * **Important:** To use swipe gestures in your application, you must enable
 * recognition of the swipe gesture. You can enable recognition with:
 *
 * @example Gesture_Swipe_enable.txt
 *
 * Swipe gestures are continuous. The LeapSwipeGesture objects for the gesture have
 * three possible states:
 *
 * * LEAP_GESTURE_STATE_START -- The gesture has just started.
 * * LEAP_GESTURE_STATE_UPDATE -- The swipe gesture is continuing.
 * * LEAP_GESTURE_STATE_STOP -- The swipe gesture is finished.
 *
 * You can set the minimum length and velocity required for a movement
 * to be recognized as a swipe using the config attribute of a connected
 * LeapController object. Use the following keys to configure swipe recognition:
 *
 * Key string | Value type | Default value | Units
 * -----------|------------|---------------|------
 * Gesture.Swipe.MinLength | float | 150 | mm
 * Gesture.Swipe.MinVelocity | float | 1000 | mm/s
 *
 * The following example demonstrates how to set the swipe configuration
 * parameters:
 *
 * @example Gesture_Swipe_Params.txt
 * @available Since 1.0
 */
@interface LeapSwipeGesture : LeapGesture

/**
 * The current position of the swipe.
 *
 * @returns <LeapVector> The current swipe position within the Leap frame of
 * reference, in mm.
 * @available Since 1.0
 */
- (LeapVector *)position;
@property (nonatomic, getter = position, readonly)LeapVector *position;
/**
 * The position where the swipe began.
 *
 * @returns <LeapVector> The starting position within the Leap frame of
 * reference, in mm.
 * @available Since 1.0
 */
- (LeapVector *)startPosition;
@property (nonatomic, getter = startPosition, readonly)LeapVector *startPosition;
/**
 * The unit direction vector parallel to the swipe motion.
 *
 * You can compare the components of the vector to classify the swipe as
 * appropriate for your application. For example, if you are using swipes
 * for two dimensional scrolling, you can compare the x and y values to
 * determine if the swipe is primarily horizontal or vertical.
 *
 * @returns <LeapVector> The unit direction vector representing the swipe
 * motion.
 * @available Since 1.0
 */
- (LeapVector *)direction;
@property (nonatomic, getter = direction, readonly)LeapVector *direction;
/**
 * The swipe speed in mm/second.
 *
 * @returns float The speed of the finger performing the swipe gesture in
 * millimeters per second.
 * @available Since 1.0
 */
- (float)speed;
@property (nonatomic, getter = speed, readonly)float speed;
/**
 * The finger or tool performing the swipe gesture.
 *
 * @returns A <LeapPointable> object representing the swiping finger
 * or tool.
 * @available Since 1.0
 */
- (LeapPointable *)pointable;

@end

//////////////////////////////////////////////////////////////////////////
//CIRCLE GESTURE
/**
 * The LeapCircleGesture classes represents a circular finger movement.
 *
 * A circle movement is recognized when the tip of a finger draws a circle
 * within the Leap field of view.
 *
 * <img src="../docs/images/Leap_Gesture_Circle.png"/>
 *
 * **Important:** To use circle gestures in your application, you must enable
 * recognition of the circle gesture. You can enable recognition with:
 *
 * @example Gesture_Circle_Enable.txt
 *
 * Circle gestures are continuous. The LeapCircleGesture objects for the gesture have
 * three possible states:
 *
 * * LEAP_GESTURE_STATE_START -- The circle gesture has just started. The movement has
 *  progressed far enough for the recognizer to classify it as a circle.
 * * LEAP_GESTURE_STATE_UPDATE -- The circle gesture is continuing.
 * * LEAP_GESTURE_STATE_STOP -- The circle gesture is finished.
 *
 * You can set the minimum radius and minimum arc length required for a movement
 * to be recognized as a circle using the config attribute of a connected
 * LeapController object. Use the following keys to configure circle recognition:
 *
 * Key string | Value type | Default value | Units
 * -----------|------------|---------------|------
 * Gesture.Circle.MinRadius | float | 5.0 | mm
 * Gesture.Circle.MinArc | float | 1.5*pi | radians
 *
 * The following example demonstrates how to set the circle configuration
 * parameters:
 *
 * @example Gesture_Circle_Params.txt
 * @available Since 1.0
 */
@interface LeapCircleGesture : LeapGesture

- (float)progress;
@property (nonatomic, getter = progress, readonly)float progress;
/**
 * The center point of the circle within the Leap frame of reference.
 *
 * @returns <LeapVector> The center of the circle in mm from the Leap origin.
 * @available Since 1.0
 */
- (LeapVector *)center;
@property (nonatomic, getter = center, readonly)LeapVector *center;

/**
 * Returns the normal vector for the circle being traced.
 *
 * If you draw the circle clockwise, the normal vector points in the same
 * general direction as the pointable object drawing the circle. If you draw
 * the circle counterclockwise, the normal points back toward the
 * pointable. If the angle between the normal and the pointable object
 * drawing the circle is less than 90 degrees, then the circle is clockwise.
 *
 *
 *     `NSString* clockwiseness;`
 *     `if ([[[circleGesture pointable] direction] angleTo:[circleGesture normal]] <= LEAP_PI/4) {`
 *     `    clockwiseness = @"clockwise";`
 *     `}`
 *     `else {`
 *     `    clockwiseness = @"counterclockwise";`
 *     `}`
 *
 *
 * @return <LeapVector> the normal vector for the circle being traced
 * @available Since 1.0
 */

- (LeapVector *)normal;
@property (nonatomic, getter = normal, readonly)LeapVector *normal;
/**
 * The radius of the circle.
 *
 * @returns The circle radius in mm.
 * @available Since 1.0
 */
- (float)radius;
@property (nonatomic, getter = radius, readonly)float radius;
/**
 * The finger performing the circle gesture.
 *
 * @returns A <LeapPointable> object representing the circling finger.
 * @available Since 1.0
 */
- (LeapPointable *)pointable;
@property (nonatomic, getter = pointable, readonly)LeapPointable *pointable;

@end

//////////////////////////////////////////////////////////////////////////
//SCREEN TAP GESTURE
/**
 * The LeapScreenTapGesture class represents a tapping gesture by a finger or tool.
 *
 * A screen tap gesture is recognized when the tip of a finger pokes forward
 * and then springs back to approximately the original postion, as if
 * tapping a vertical screen. The tapping finger must pause briefly before beginning the tap.
 *
 * <img src="../docs/images/Leap_Gesture_Tap2.png"/>
 *
 * **Important:** To use screen tap gestures in your application, you must enable
 * recognition of the screen tap gesture. You can enable recognition with:
 *
 * @example Gesture_ScreenTap_Enable.txt
 *
 * LeapScreenTap gestures are discrete. The LeapScreenTapGesture object
 * representing a tap always has the state, LEAP_GESTURE_STATE_STOP. Only one
 * LeapScreenTapGesture object is created for each screen tap gesture recognized.
 *
 * You can set the minimum finger movement and velocity required for a movement
 * to be recognized as a screen tap as well as adjust the detection window for
 * evaluating the movement using the config attribute of a connected
 * LeapController object. Use the following keys to configure screen tap recognition:
 *
 * Key string | Value type | Default value | Units
 * -----------|------------|---------------|------
 * Gesture.ScreenTap.MinForwardVelocity  | float | 50 | mm/s
 * Gesture.ScreenTap.HistorySeconds | float | 0.1 | s
 * Gesture.ScreenTap.MinDistance | float | 3.0 | mm
 *
 * The following example demonstrates how to set the screen tap configuration
 * parameters:
 *
 * @example Gesture_ScreenTap_Params.txt
 * @available Since 1.0
 */
@interface LeapScreenTapGesture : LeapGesture

/**
 * The position where the screen tap is registered.
 *
 * @return A <LeapVector> containing the coordinates of screen tap location.
 * @available Since 1.0
 */
- (LeapVector *)position;
@property (nonatomic, getter = position, readonly)LeapVector *position;
/**
 * The direction of finger tip motion.
 *
 * @returns <LeapVector> A unit direction vector.
 * @available Since 1.0
 */
- (LeapVector *)direction;
@property (nonatomic, getter = direction, readonly)LeapVector *direction;
/**
 * The progess value is always 1.0 for a screen tap gesture.
 *
 * @returns float The value 1.0.
 * @available Since 1.0
 */
- (float)progress;
@property (nonatomic, getter = progress, readonly)float progress;
/**
 * The finger performing the screen tap gesture.
 *
 * @returns A <LeapPointable> object representing the tapping finger.
 * @available Since 1.0
 */
- (LeapPointable *)pointable;
@property (nonatomic, getter = pointable, readonly)LeapPointable *pointable;

@end

//////////////////////////////////////////////////////////////////////////
//KEY TAP GESTURE
/**
 * The LeapKeyTapGesture class represents a tapping gesture by a finger or tool.
 *
 * A key tap gesture is recognized when the tip of a finger rotates down toward the
 * palm and then springs back to approximately the original postion, as if
 * tapping. The tapping finger must pause briefly before beginning the tap.
 *
 * <img src="../docs/images/Leap_Gesture_Tap.png"/>
 *
 * **Important:** To use key tap gestures in your application, you must enable
 * recognition of the key tap gesture. You can enable recognition with:
 *
 * @example Gesture_KeyTap_Enable.txt
 *
 * Key tap gestures are discrete. The LeapKeyTapGesture object representing a tap always
 * has the state, LEAP_GESTURE_STATE_STOP. Only one LeapKeyTapGesture object is
 * created for each key tap gesture recognized.
 *
 * You can set the minimum finger movement and velocity required for a movement
 * to be recognized as a key tap as well as adjust the detection window for
 * evaluating the movement using the config attribute of a connected
 * LeapController object. Use the following configuration keys to configure key tap
 * recognition:
 *
 * Key string | Value type | Default value | Units
 * -----------|------------|---------------|------
 * Gesture.KeyTap.MinDownVelocity | float | 50 | mm/s
 * Gesture.KeyTap.HistorySeconds | float | 0.1 | s
 * Gesture.KeyTap.MinDistance | float | 5.0 | mm
 *
 * The following example demonstrates how to set the key tap configuration
 * parameters:
 *
 * @example Gesture_KeyTap_Params.txt
 * @available Since 1.0
 */
@interface LeapKeyTapGesture : LeapGesture

/**
 * The position where the key tap is registered.
 *
 * @return A <LeapVector> containing the coordinates of key tap location.
 * @available Since 1.0
 */
- (LeapVector *)position;
@property (nonatomic, getter = position, readonly)LeapVector *position;
/**
 * The direction of finger tip motion.
 *
 * @returns <LeapVector> A unit direction vector.
 * @available Since 1.0
 */
- (LeapVector *)direction;
@property (nonatomic, getter = direction, readonly)LeapVector *direction;
/**
 * The progess value is always 1.0 for a key tap gesture.
 *
 * @returns float The value 1.0.
 * @available Since 1.0
 */
- (float)progress;
@property (nonatomic, getter = progress, readonly)float progress;
/**
 * The finger performing the key tap gesture.
 *
 * @returns A <LeapPointable> object representing the tapping finger.
 * @available Since 1.0
 */
- (LeapPointable *)pointable;
@property (nonatomic, getter = pointable, readonly)LeapPointable *pointable;

@end

//////////////////////////////////////////////////////////////////////////
//FRAME
/**
 * The LeapFrame class represents a set of hand and finger tracking data detected
 * in a single frame.
 *
 * The Leap detects hands, fingers and tools within the tracking area, reporting
 * their positions, orientations and motions in frames at the Leap frame rate.
 *
 * Access LeapFrame objects through an instance of a <LeapController>. Implement a
 * <LeapListener> subclass to receive a callback event when a new LeapFrame is available.
 * @available Since 1.0
 */
@interface LeapFrame : NSObject

/**
 * The list of <LeapHand> objects detected in this frame, given in arbitrary order.
 * The list can be empty if no hands are detected.
 *
 * @returns NSArray containing all <LeapHand> objects detected in this frame.
 * @available Since 1.0
 */
@property (nonatomic, strong, readonly)NSArray *hands;
/**
 * The list of <LeapPointable> objects (fingers and tools) detected in this frame,
 * given in arbitrary order. The list can be empty if no fingers or tools are detected.
 *
 * @returns NSArray containing all <LeapPointable> objects detected in this frame.
 * @available Since 1.0
 */
@property (nonatomic, strong, readonly)NSArray *pointables;
/**
 * The list of <LeapFinger> objects detected in this frame, given in arbitrary order.
 * The list can be empty if no fingers are detected.
 *
 * @returns NSArray containing all <LeapFinger> objects detected in this frame.
 * @available Since 1.0
 */
@property (nonatomic, strong, readonly)NSArray *fingers;
/**
 * The list of <LeapTool> objects detected in this frame, given in arbitrary order.
 * The list can be empty if no tools are detected.
 *
 * @returns NSArray containing all <LeapTool> objects detected in this frame.
 * @available Since 1.0
 */
@property (nonatomic, strong, readonly)NSArray *tools;

- (NSString *)description;
- (void *)interfaceFrame;
/**
 * A unique ID for this LeapFrame. Consecutive frames processed by the Leap
 * have consecutive increasing values.
 *
 * @returns The frame ID.
 * @available Since 1.0
 */
- (int64_t)id;
@property (nonatomic, getter = id, readonly)int64_t id;
/**
 * The frame capture time in microseconds elapsed since the Leap started.
 *
 * @returns The timestamp in microseconds.
 * @available Since 1.0
 */
- (int64_t)timestamp;
@property (nonatomic, getter = timestamp, readonly)int64_t timestamp;
/**
 * The <LeapHand> object with the specified ID in this frame.
 *
 * Use the [LeapFrame hand:] function to retrieve the LeapHand object from
 * this frame using an ID value obtained from a previous frame.
 * This function always returns a LeapHand object, but if no hand
 * with the specified ID is present, an invalid LeapHand object is returned.
 *
 * Note that ID values persist across frames, but only until tracking of a
 * particular object is lost. If tracking of a hand is lost and subsequently
 * regained, the new LeapHand object representing that physical hand may have
 * a different ID than that representing the physical hand in an earlier frame.
 *
 * @param handId The ID value of a <LeapHand> object from a previous frame.
 * @returns The <LeapHand> object with the matching ID if one exists in this frame;
 * otherwise, an invalid LeapHand object is returned.
 * @available Since 1.0
 */
- (LeapHand *)hand:(int32_t)handId;
/**
 * The <LeapPointable> object with the specified ID in this frame.
 *
 * Use the [LeapFrame pointable:] function to retrieve the LeapPointable object from
 * this frame using an ID value obtained from a previous frame.
 * This function always returns a LeapPointable object, but if no finger or tool
 * with the specified ID is present, an invalid LeapPointable object is returned.
 *
 * Note that ID values persist across frames, but only until tracking of a
 * particular object is lost. If tracking of a finger or tool is lost and subsequently
 * regained, the new LeapPointable object representing that finger or tool may have
 * a different ID than that representing the finger or tool in an earlier frame.
 *
 * @param pointableId The ID value of a <LeapPointable> object from a previous frame.
 * @returns The <LeapPointable> object with the matching ID if one exists in this frame;
 * otherwise, an invalid LeapPointable object is returned.
 * @available Since 1.0
 */
- (LeapPointable *)pointable:(int32_t)pointableId;
/**
 * The <LeapFinger> object with the specified ID in this frame.
 *
 * Use the [LeapFrame finger:] function to retrieve the LeapFinger object from
 * this frame using an ID value obtained from a previous frame.
 * This function always returns a LeapFinger object, but if no finger
 * with the specified ID is present, an invalid LeapFinger object is returned.
 *
 * Note that ID values persist across frames, but only until tracking of a
 * particular object is lost. If tracking of a finger is lost and subsequently
 * regained, the new LeapFinger object representing that physical finger may have
 * a different ID than that representing the finger in an earlier frame.
 *
 * @param fingerId The ID value of a <LeapFinger> object from a previous frame.
 * @returns The <LeapFinger> object with the matching ID if one exists in this frame;
 * otherwise, an invalid LeapFinger object is returned.
 * @available Since 1.0
 */
- (LeapFinger *)finger:(int32_t)fingerId;
/**
 * The <LeapTool> object with the specified ID in this frame.
 *
 * Use the [LeapFrame tool:] function to retrieve the LeapTool object from
 * this frame using an ID value obtained from a previous frame.
 * This function always returns a LeapTool object, but if no tool
 * with the specified ID is present, an invalid LeapTool object is returned.
 *
 * Note that ID values persist across frames, but only until tracking of a
 * particular object is lost. If tracking of a tool is lost and subsequently
 * regained, the new LeapTool object representing that tool may have a
 * different ID than that representing the tool in an earlier frame.
 *
 * @param toolId The ID value of a <LeapTool> object from a previous frame.
 * @returns The <LeapTool> object with the matching ID if one exists in this frame;
 * otherwise, an invalid LeapTool object is returned.
 * @available Since 1.0
 */
- (LeapTool *)tool:(int32_t)toolId;
/**
 * The gestures recognized or continuing since the specified frame.
 *
 * Circle and swipe gestures are updated every frame. Tap gestures
 * only appear in the list for a single frame.
 *
 * @param sinceFrame An earlier LeapFrame. Set to nil to get the gestures for
 * the current LeapFrame only.
 * @return NSArray containing the list of gestures.
 * @available Since 1.0
 */
- (NSArray *)gestures:(const LeapFrame *)sinceFrame;
/**
 * The <LeapGesture> object with the specified ID in this frame.
 *
 * Use the [LeapFrame gesture:] function to return a Gesture object in this
 * frame using an ID obtained in an earlier frame. The function always
 * returns a LeapGesture object, but if there was no update for the gesture in
 * this frame, then an invalid LeapGesture object is returned.
 *
 * All LeapGesture objects representing the same recognized movement share the
 * same ID.
 * @param gestureId The ID of a <LeapGesture> object from a previous frame.
 * @returns The <LeapGesture> object in the frame with the specified ID if one
 * exists; Otherwise, an Invalid LeapGesture object.
 * @available Since 1.0
 */
- (LeapGesture *)gesture:(int32_t)gestureId;
/**
 * The change of position derived from the overall linear motion between
 * the current frame and the specified frame.
 *
 * The returned translation vector provides the magnitude and direction of
 * the movement in millimeters.
 *
 * The Leap derives frame translation from the linear motion of
 * all objects detected in the field of view.
 *
 * If either this frame or sinceFrame is an invalid LeapFrame object, then this
 * method returns a zero vector.
 *
 * @param sinceFrame The starting frame for computing the relative translation.
 * @returns A <LeapVector> representing the heuristically determined change in
 * position of all objects between the current frame and that specified
 * in the sinceFrame parameter.
 * @available Since 1.0
 */
- (LeapVector *)translation:(const LeapFrame *)sinceFrame;
/**
 * The estimated probability that the overall motion between the current
 * frame and the specified frame is intended to be a translating motion.
 *
 * If either this frame or sinceFrame is an invalid LeapFrame object, then this
 * method returns zero.
 *
 * @param sinceFrame The starting frame for computing the translation.
 * @returns A value between 0 and 1 representing the estimated probability
 * that the overall motion between the current frame and the specified frame
 * is intended to be a translating motion.
 * @available Since 1.0
 */
- (float)translationProbability:(const LeapFrame *)sinceFrame;
/**
 * The axis of rotation derived from the overall rotational motion between
 * the current frame and the specified frame.
 *
 * The returned direction vector is normalized.
 *
 * The Leap derives frame rotation from the relative change in position and
 * orientation of all objects detected in the field of view.
 *
 * If either this frame or sinceFrame is an invalid LeapFrame object, or if no
 * rotation is detected between the two frames, a zero vector is returned.
 *
 * @param sinceFrame The starting frame for computing the relative rotation.
 * @returns A <LeapVector> containing the normalized direction vector representing the axis of the
 * heuristically determined rotational change between the current frame
 * and that specified in the sinceFrame parameter.
 * @available Since 1.0
 */
- (LeapVector *)rotationAxis:(const LeapFrame *)sinceFrame;
/**
 * The angle of rotation around the rotation axis derived from the overall
 * rotational motion between the current frame and the specified frame.
 *
 * The returned angle is expressed in radians measured clockwise around the
 * rotation axis (using the right-hand rule) between the start and end frames.
 * The value is always between 0 and pi radians (0 and 180 degrees).
 *
 * The Leap derives frame rotation from the relative change in position and
 * orientation of all objects detected in the field of view.
 *
 * If either this frame or sinceFrame is an invalid LeapFrame object, then the
 * angle of rotation is zero.
 *
 * @param sinceFrame The starting frame for computing the relative rotation.
 * @returns A positive value containing the heuristically determined
 * rotational change between the current frame and that specified in the
 * sinceFrame parameter.
 * @available Since 1.0
 */
- (float)rotationAngle:(const LeapFrame *)sinceFrame;
/**
 * The angle of rotation around the specified axis derived from the overall
 * rotational motion between the current frame and the specified frame.
 *
 * The returned angle is expressed in radians measured clockwise around the
 * rotation axis (using the right-hand rule) between the start and end frames.
 * The value is always between -pi and pi radians (-180 and 180 degrees).
 *
 * The Leap derives frame rotation from the relative change in position and
 * orientation of all objects detected in the field of view.
 *
 * If either this frame or sinceFrame is an invalid LeapFrame object, then the
 * angle of rotation is zero.
 *
 * @param sinceFrame The starting frame for computing the relative rotation.
 * @param axis The <LeapVector> representing the direction of the axis to measure rotation around.
 * @returns A value containing the heuristically determined rotational
 * change between the current frame and that specified in the sinceFrame
 * parameter around the given axis.
 * @available Since 1.0
 */
- (float)rotationAngle:(const LeapFrame *)sinceFrame axis:(const LeapVector *)axis;
/**
 * The transform matrix expressing the rotation derived from the overall
 * rotational motion between the current frame and the specified frame.
 *
 * The Leap derives frame rotation from the relative change in position and
 * orientation of all objects detected in the field of view.
 *
 * If either this frame or sinceFrame is an invalid LeapFrame object, then this
 * method returns an identity matrix.
 *
 * @param sinceFrame The starting frame for computing the relative rotation.
 * @returns A <LeapMatrix> containing the heuristically determined
 * rotational change between the current frame and that specified in the
 * sinceFrame parameter.
 * @available Since 1.0
 */
- (LeapMatrix *)rotationMatrix:(const LeapFrame *)sinceFrame;
/**
 * The estimated probability that the overall motion between the current
 * frame and the specified frame is intended to be a rotating motion.
 *
 * If either this frame or sinceFrame is an invalid LeapFrame object, then this
 * method returns zero.
 *
 * @param sinceFrame The starting frame for computing the relative rotation.
 * @returns A value between 0 and 1 representing the estimated probability
 * that the overall motion between the current frame and the specified frame
 * is intended to be a rotating motion.
 * @available Since 1.0
 */
- (float)rotationProbability:(const LeapFrame *)sinceFrame;
/**
 * The scale factor derived from the overall motion between the current frame
 * and the specified frame.
 *
 * The scale factor is always positive. A value of 1.0 indicates no
 * scaling took place. Values between 0.0 and 1.0 indicate contraction
 * and values greater than 1.0 indicate expansion.
 *
 * The Leap derives scaling from the relative inward or outward motion of
 * all objects detected in the field of view (independent of translation
 * and rotation).
 *
 * If either this frame or sinceFrame is an invalid LeapFrame object, then this
 * method returns 1.0.
 *
 * @param sinceFrame The starting frame for computing the relative scaling.
 * @returns A positive value representing the heuristically determined
 * scaling change ratio between the current frame and that specified in the
 * sinceFrame parameter.
 * @available Since 1.0
 */
- (float)scaleFactor:(const LeapFrame *)sinceFrame;
/**
 * The estimated probability that the overall motion between the current
 * frame and the specified frame is intended to be a scaling motion.
 *
 * If either this frame or sinceFrame is an invalid LeapFrame object, then this
 * method returns zero.
 *
 * @param sinceFrame The starting frame for computing the relative scaling.
 * @returns A value between 0 and 1 representing the estimated probability
 * that the overall motion between the current frame and the specified frame
 * is intended to be a scaling motion.
 * @available Since 1.0
 */
- (float)scaleProbability:(const LeapFrame *)sinceFrame;
/**
 * Reports whether this LeapFrame instance is valid.
 *
 * A valid LeapFrame is one generated by the <LeapController> object that contains
 * tracking data for all detected entities. An invalid LeapFrame contains no
 * actual tracking data, but you can call its functions without risk of a
 * null pointer exception. The invalid LeapFrame mechanism makes it more
 * convenient to track individual data across the frame history. For example,
 * you can invoke:
 *
 *     `LeapFinger finger = [[controller frame:n] finger:fingerID];`
 *
 * for an arbitrary LeapFrame history value, "n", without first checking whether
 * frame: returned a null object. (You should still check that the
 * returned LeapFinger instance is valid.)
 *
 * @returns YES, if this is a valid LeapFrame object; false otherwise.
 * @available Since 1.0
 */
- (BOOL)isValid;
@property (nonatomic, getter = isValid, readonly)BOOL isValid;
/**
 * The current LeapInteractionBox for the frame. See the LeapInteractionBox class
 * documentation for more details on how this class should be used.
 *
 * @returns The current LeapInteractionBox object.
 * @available Since 1.0
 */
- (LeapInteractionBox *)interactionBox;
/**
 * The instantaneous framerate.
 *
 * The rate at which the Leap Motion software is providing frames of data 
 * (in frames per second). The framerate can fluctuate depending on available computing 
 * resources, activity within the device field of view, software tracking settings, 
 * and other factors.
 *
 * @returns An estimate of frames per second of the Leap Motion Controller.
 * @available Since 1.0
 */
- (float)currentFramesPerSecond;
@property (nonatomic, getter = currentFramesPerSecond, readonly)float currentFramesPerSecond;
/**
 * Returns an invalid LeapFrame object.
 *
 * You can use the instance returned by this function in comparisons testing
 * whether a given LeapFrame instance is valid or invalid. (You can also use the
 * <[LeapFrame isValid]> function.)
 *
 * @returns The invalid LeapFrame instance.
 * @available Since 1.0
 */
+ (LeapFrame *)invalid;

@end

//////////////////////////////////////////////////////////////////////////
//CONFIG

/**
 * Enumerates the possible data types for configuration values.
 *
 * The LeapConfig::type() function returns an item from the LeapValueType enumeration.
 * @available Since 1.0
 */
typedef enum {
    TYPE_UNKNOWN = 0,
    TYPE_BOOLEAN = 1,
    TYPE_INT32 = 2,
    TYPE_FLOAT = 6,
    TYPE_STRING = 8
} LeapValueType;

/**
 * The LeapConfig class provides access to Leap system configuration information.
 *
 * You can get and set gesture configuration parameters using the LeapConfig object
 * obtained from a connected <LeapController> object. The key strings required to
 * identify a configuration parameter include:
 *
 * Key string | Value type | Default value | Units
 * -----------|------------|---------------|------
 * Gesture.Circle.MinRadius | float | 5.0 | mm
 * Gesture.Circle.MinArc | float | 1.5*pi | radians
 * Gesture.Swipe.MinLength | float | 150 | mm
 * Gesture.Swipe.MinVelocity | float | 1000 | mm/s
 * Gesture.KeyTap.MinDownVelocity | float | 50 | mm/s
 * Gesture.KeyTap.HistorySeconds | float | 0.1 | s
 * Gesture.KeyTap.MinDistance | float | 5.0 | mm
 * Gesture.ScreenTap.MinForwardVelocity  | float | 50 | mm/s
 * Gesture.ScreenTap.HistorySeconds | float | 0.1 | s
 * Gesture.ScreenTap.MinDistance | float | 3.0 | mm
 *
 * After setting a configuration value, you must call the <[Config save]> method
 * to commit the changes. The configuration value changes are not persistent;
 * your application needs to set the values everytime it runs.
 *
 * @see <LeapCircleGesture>
 * @see <LeapKeyTapGesture>
 * @see <LeapScreenTapGesture>
 * @see <LeapSwipeGesture>
 * @available Since 1.0
 */
@interface LeapConfig : NSObject

/**
 * Reports the natural data type for the value related to the specified key.
 *
 * The supported data types defined by the members of the LeapValueType
 * enumeration:
 *
 * * TYPE_BOOLEAN = 1
 * * TYPE_INT32 = 2
 * * TYPE_FLOAT = 6
 * * TYPE_STRING = 8
 * * TYPE_UNKNOWN = 0 (usually indicates that an error occured)
 *
 * @param key The key for the looking up the value in the configuration dictionary.
 * @returns The native data type of the value, that is, the type that does not
 * require a data conversion.
 * @available Since 1.0
 */
- (LeapValueType)type:(NSString *)key;
/** 
 * Gets the boolean representation for the specified key. 
 * @available Since 1.0
 */
- (BOOL)getBool:(NSString *)key;
/** 
 * Sets the boolean representation for the specified key.
 * @returns YES on success, NO on failure.
 * @available Since 1.0
 */
- (BOOL)setBool:(NSString *)key value:(BOOL)value;
/** 
 * Gets the 32-bit integer representation for the specified key. 
 * @available Since 1.0
 */
- (int32_t)getInt32:(NSString *)key;
/** 
 * Sets the 32-bit integer representation for the specified key.
 * @returns YES on success, NO on failure.
 * @available Since 1.0
 */
- (BOOL)setInt32:(NSString *)key value:(int32_t)value;
/** 
 * Gets the floating point representation for the specified key. 
 * @available Since 1.0
 */
- (float)getFloat:(NSString *)key;
/** 
 * Sets the floating point representation for the specified key.
 * @returns YES on success, NO on failure.
 * @available Since 1.0
 */
- (BOOL)setFloat:(NSString *)key value:(float)value;
/** 
 * Gets the NSString representation for the specified key. 
 * @available Since 1.0
 */
- (NSString *)getString:(NSString *)key;
/** 
 * Sets the string representation for the specified key.
 * @returns YES on success, NO on failure.
 * @available Since 1.0
 */
- (BOOL)setString:(NSString *)key value:(NSString *)value;
/**
 * Saves the current state of the config.
 *
 * Call [LeapConfig save:] after making a set of configuration changes. The
 * [LeapConfig save:] function transfers the configuration changes to the Leap
 * application. The configuration value changes are not persistent; your
 * application needs to set the values everytime it runs.
 *
 * @returns TRUE on success, NO on failure.
 * @available Since 1.0
 */
- (BOOL)save;

@end

//////////////////////////////////////////////////////////////////////////
//CONTROLLER
/**
 * The LeapController class is your main interface to the Leap Motion Controller
 * hardware.
 *
 * Create an instance of this LeapController class to access frames of tracking
 * data and configuration information. Frame data can be polled at any time
 * using the <[LeapController frame:]> function. Set the `frame:` parameter to 0
 * to get the most recent frame. Set the parameter to a positive integer
 * to access previous frames. For example, `[controller frame:10]` returns the
 * frame that occured ten frames ago. A controller stores up to 60 frames in its
 * frame history.
 *
 * Polling is an appropriate strategy for applications which already have an
 * intrinsic update loop, such as a game. You can also add a listener object
 * or delegate to the controller to handle events as they occur.
 * The Leap dispatches events to the listener upon initialization and exiting,
 * on connection changes, and when a new frame of tracking data is available.
 * When these events occur, the controller object invokes the appropriate
 * callback function.
 *
 * **Polling**
 *
 * Create an instance of the LeapController class using the default initializer:
 *
 *     `LeapController *controller = [[LeapController alloc] init];`
 *
 * Access the frame data at regular intervals:
 *
 *     `LeapFrame *frame = [controller frame:0];`
 *
 * You can check <[LeapController isConnected]> to determine if the controller
 * is connected to the Leap software.
 *
 * **LeapListener protocol**
 *
 * Implement a class adopting the <LeapListener> protocol.
 *
 * Create an instance of the LeapController class and assign your LeapListener object to it:
 *
 *     `MYListener *listener = [[MYListener alloc] init];`
 *     `LeapController *controller = [[LeapController alloc] initWithListener:listener];`
 *
 * The controller subscribes the LeapListener instance to the appropriate NSNotifications
 * for the Leap events. When a new frame of data is ready, the controller dispatches an
 * NSNotification on the main application thread, which is handled by your
 * <[LeapListener onFrame:]> implementation.
 *
 * **LeapDelegate protocol**
 *
 * Implement a class adopting the <LeapDelegate> protocol.
 *
 * Create an instance of the LeapController class and assign your LeapListener object to it:
 *
 *     `MYDelegate *delegate = [[MYDelegate alloc] init];`
 *     `LeapController *controller = [[LeapController alloc] init];`
 *     `[controller addDelegate:delegate];`
 *
 * When a new frame of data is ready, the controller calls the
 * <[LeapDelegate onFrame:]> method. The Controller object is multithreaded and
 * calls the LeapDelegate functions on its own thread, not on an application thread.
 *
 *
 * You can handle the other Leap events, `onInit`, `onConnect`, `onDisconnect`,
 * and `onExit` in the same manner.
 *
 * @since Version 1.0
 * @available Since 1.0
 */
@interface LeapController : NSObject

/**
 * Initializes a LeapController instance.
 * @available Since 1.0
 */
- (id)init;
/**
 * Initializes a LeapController instance and assigns a listener.
 *
 * * *Note:* You can use either a listener or a delegate, but not both.
 *
 * @param listener An object adopting the <LeapListener> protocol.
 * @available Since 1.0
 */
- (id)initWithListener:(id)listener;
/**
 * Gets the active policy settings.
 *
 * Use this function to determine the current policy state.
 * Keep in mind that setting a policy flag is asynchronous, so changes are
 * not effective immediately after calling <[LeapController setPolicyFlag]>. In addition, a
 * policy request can be declined by the user. You should always set the
 * policy flags required by your application at startup and check that the
 * policy change request was successful after an appropriate interval.
 *
 * If the controller object is not connected to the Leap, then the default
 * policy state is returned.
 *
 * @returns The current policy flags.
 * @available Since 1.0
 */
- (LeapPolicyFlag)policyFlags;
/**
 * Requests a change in policy.
 *
 * A request to change a policy is subject to user approval and a policy
 * can be changed by the user at any time (using the Leap settings window).
 * The desired policy flags must be set every time an application runs.
 *
 * Policy changes are completed asynchronously and, because they are subject
 * to user approval, may not complete successfully. Call
 * <[LeapController policyFlags]> after a suitable interval to test whether
 * the change was accepted.
 *
 * Currently, the background frames policy is the only policy supported.
 * The background frames policy determines whether an application
 * receives frames of tracking data while in the background. By
 * default, the Leap only sends tracking data to the foreground application.
 * Only applications that need this ability should request the background
 * frames policy.
 *
 * At this time, you can use the Leap applications Settings window to
 * globally enable or disable the background frames policy. However,
 * each application that needs tracking data while in the background
 * must also set the policy flag using this function.
 *
 * This function can be called before the LeapController object is connected,
 * but the request will be sent to the Leap after the controller connects.
 *
 * @param flags A PolicyFlag value indicating the policies to request. Must be
 * a member of the LeapPolicyFlags enumeration:
 *
 * * LEAP_POLICY_DEFAULT -- restore the default policy configuration for this
 * application (do not receive frames while in the background).
 * * LEAP_POLICY_BACKGROUND_FRAMES -- the application should receive frames of
 * motion tracking data while in the background.
 * @available Since 1.0
 */
- (void)setPolicyFlags:(LeapPolicyFlag)flags;
/**
 * Adds a listener to this LeapController.
 *
 * When you add an object adopting the <LeapListener> protocol to a LeapController,
 * the controller automatically subscribes the listener to NSNotifications
 * dispatched for the Leap events.
 *
 * *Note:* You cannot add a listener when there is already a delegate assigned.
 *
 * @param listener An object adopting the <LeapListener> protocol.
 * @returns BOOL Whether or not the listener was successfully added to the list
 * of listeners.
 * @available Since 1.0
 */
- (BOOL)addListener:(id)listener;
/**
 * Unsubscribes the listener object from receiving Leap NSNotifications.
 *
 * @param listener The listener to unsubscribe.
 * @returns BOOL Whether or not the listener was successfully removed.
 * @available Since 1.0
 */
- (BOOL)removeListener:(id)listener;
/**
 * Initializes a LeapController instance and assigns a delegate.
 *
 * * *Note:* You can use either a delegate or a listener, but not both.
 *
 * @param delegate An object adopting the <LeapDelegate> protocol.
 * @available Since 1.0
 */
- (id)initWithDelegate:(id)delegate;
/**
 * Adds a delegate to this LeapController.
 *
 * *Note:* You cannot add a delegate when there is already a listener assigned.
 *
 * @param delegate An object adopting the <LeapDelegate> protocol.
 * @returns BOOL Whether or not the delegate was successfully added.
 * @available Since 1.0
 */
- (BOOL)addDelegate:(id)delegate;
/**
 * Removes the delegate assigned to this LeapController.
 *
 * @returns BOOL Whether or not the delegate was successfully removed.
 * @available Since 1.0
 */
- (BOOL)removeDelegate;
/**
 * Returns a <LeapFrame> containing a frame of tracking data from the Leap. Use the optional
 * history parameter to specify which frame to retrieve. Call
 * `[controller frame:0]` to access the most recent frame; call
 * `[controller frame:1]` to access the previous frame, and so on. If you use a
 * history value greater than the number of stored frames, then the controller
 * returns an invalid frame.
 *
 * @param history The age of the <LeapFrame> to return, counting backwards from
 * the most recent frame (0) into the past and up to the maximum age (59).
 * @returns The specified <LeapFrame>; or, if no history parameter is specified,
 * the newest frame. If a frame is not available at the specified history
 * position, an invalid LeapFrame is returned.
 * @available Since 1.0
 */
- (LeapFrame *)frame:(int)history;
/**
 * Returns a Config object, which you can use to query the Leap system for
 * configuration information.
 * @available Since 1.0
 */
- (LeapConfig *)config;
@property (nonatomic, getter = config, readonly)LeapConfig *config;
/**
 * Returns a list of currently attached devices.  Devices can be queried for
 * device-specific information such as field of view.
 *
 * Currently the Leap controller only recognizes a single device at a time.
 * @available Since 1.0
 */
- (NSArray *)devices;
@property (nonatomic, getter = devices, readonly)NSArray *devices;
/**
 * Reports whether this LeapController is connected to the Leap device.
 *
 * When you first create a LeapController object, isConnected returns false.
 * After the controller finishes initializing and connects to the Leap,
 * isConnected will return true.
 *
 * You can either handle the onConnect event using a <LeapListener> or <LeapDelegate>
 * instance or poll the isConnected function if you need to wait for your
 * application to be connected to the Leap before performing some other
 * operation.
 *
 * @returns True, if connected; false otherwise.
 * @available Since 1.0
 */
- (BOOL)isConnected;
@property (nonatomic, getter = isConnected, readonly)BOOL isConnected;
/**
 * Reports whether this application is the focused, foreground application.
 *
 * Only the foreground application receives tracking information from
 * the Leap Motion Controller.
 *
 * @returns True, if application has focus; false otherwise.
 * @available Since 1.0
 */
- (BOOL)hasFocus;
@property (nonatomic, getter = hasFocus, readonly)BOOL hasFocus;
/**
 * Enables or disables reporting of a specified gesture type.
 *
 * By default, all gesture types are disabled. When disabled, gestures of the
 * disabled type are never reported and will not appear in the frame
 * gesture list.
 *
 * As a performance optimization, only enable recognition for the types
 * of movements that you use in your application.
 *
 * @param gestureType The type of gesture to enable or disable. Must be a
 * member of the LeapGestureType enumeration:
 *
 * * LEAP_GESTURE_TYPE_SWIPE  -- A straight line movement by the hand with fingers extended.
 * * LEAP_GESTURE_TYPE_CIRCLE -- A circular movement by a finger.
 * * LEAP_GESTURE_TYPE_SCREEN_TAP -- A forward tapping movement by a finger.
 * * LEAP_GESTURE_TYPE_KEY_TAP -- A downward tapping movement by a finger.
 *
 * @param enable YES, to enable the specified gesture type; NO,
 * to disable.
 * @see [LeapController isGestureEnabled:]
 * @available Since 1.0
 */
- (void)enableGesture:(LeapGestureType)gestureType enable:(BOOL)enable;
/**
 * Reports whether the specified gesture type is enabled.
 *
 * @param gestureType The type of gesture to check.  Must be a
 * member of the LeapGestureType enumeration:
 *
 * * LEAP_GESTURE_TYPE_SWIPE  -- A straight line movement by the hand with fingers extended.
 * * LEAP_GESTURE_TYPE_CIRCLE -- A circular movement by a finger.
 * * LEAP_GESTURE_TYPE_SCREEN_TAP -- A forward tapping movement by a finger.
 * * LEAP_GESTURE_TYPE_KEY_TAP -- A downward tapping movement by a finger.
 *
 * @return YES, if the specified type is enabled; NO, otherwise.
 * @see [LeapController enableGesture:enable:]
 * @available Since 1.0
 */
- (BOOL)isGestureEnabled:(LeapGestureType)gestureType;
/**
 * The list of <LeapScreen> objects representing the computer display screens
 * whose positions have been identified by using the Leap application's
 * Screen Locator utility.
 *
 * The list always contains at least one entry representing the default
 * screen. If the user has not registered the location of this default
 * screen, then the coordinates, directions, and other values reported by
 * the functions in its <LeapScreen> object will not be accurate. Other monitor
 * screens only appear in the list if their positions have been registered
 * using the Leap Screen Locator.
 *
 * A LeapScreen object represents the position and orientation of a display
 * monitor screen within the Leap coordinate system.
 * For example, if the screen location is known, you can get Leap coordinates
 * for the bottom-left corner of the screen. Registering the screen
 * location also allows the Leap to calculate the point on the screen at
 * which a finger or tool is pointing.
 *
 * A user can run the Screen Locator tool from the Leap application
 * Settings window. Avoid assuming that a screen location is known or that
 * an existing position is still correct. The registered position is only
 * valid as long as the relative position of the Leap device and the
 * monitor screen remain constant.
 *
 * @returns NSArray An array containing the screens whose positions have
 * been registered by the user using the Screen Locator tool.
 * The list always contains at least one entry representing the default
 * monitor. If the user has not run the Screen Locator or has moved the Leap
 * device or screen since running it, the <LeapScreen> object for this entry
 * only contains default values.
 * @available Since 1.0
 */
- (NSArray *)locatedScreens;
@property (nonatomic, getter = locatedScreens, readonly)NSArray *locatedScreens;

@end

//////////////////////////////////////////////////////////////////////////
//LISTENER
/**
 * The LeapListener protocol defines a set of methods that you can
 * implement to respond to NSNotification messages dispatched by a LeapController object.
 *
 * To use the LeapListener protocol, implement a class adopting the protocol
 * and assign an instance of that class to a <LeapController> instance:
 *
 *     `MYListener *listener = [[MYListener alloc] init];`
 *     `LeapController *controller = [[LeapController alloc] initWithListener:listener];`
 *
 * The controller subscribes the LeapListener instance to the appropriate NSNotifications
 * for the Leap events. When a new frame of data is ready, the controller dispatches an
 * NSNotification on the main application thread, which is handled by your
 * <[LeapListener onFrame:]> implementation.
 *
 * You can handle the other Leap events, `onInit`, `onConnect`, `onDisconnect`,
 * `onExit`, `onFocusGained`, and `onFocusLost` in the same manner.
 *
 * You must have a running NSRunLoop to receive NSNotification objects.
 * This is usually present and running by default in a Cocoa application.
 * Calling <[LeapController addListener:]> takes care subscribing the listener object
 * to the appropriate notifications. The LeapListener object is the notification observer,
 * while the LeapController object is the notification sender. You can also subscribe to
 * notifications manually. For example, to subscribe to the OnFrame message, call:
 *
 *     `[[NSNotificationCenter defaultCenter] selector:@selector(onFrame:) name:@"OnFrame" object:controller]]`
 *
 * However, at least one listener must be added to the controller with [LeapController addListener:]
 * or the controller does not bother to dispatch notification messages.
 *
 * Using the LeapListener protocol is not mandatory. You can also use
 * a delegate implementing the <LeapDelegate> protocol or simply poll the
 * controller object (as described in the <LeapController> class overview).
 * @available Since 1.0
 */
@protocol LeapListener<NSObject>

@optional
/**
 * Dispatched once, when the <LeapController> has finished initializing.
 *
 * Only the first LeapListener added to the controller receives this notification.
 *
 *    `- (void)onInit:(NSNotification *)notification`
 *    `{
 *    `    NSLog(@"Initialized");`
 *    `    //...`
 *    `}`
 *
 * @param notification The <LeapController> object dispatching the notification.
 * @available Since 1.0
 */
- (void)onInit:(NSNotification *)notification;
/**
 * Dispatched when the <LeapController> object connects to the Leap software, or when
 * this ListenerListener object is added to a controller that is already connected.
 *
 *     `- (void)onConnect:(NSNotification *)notification`
 *     `{`
 *     `   NSLog(@"Connected");`
 *     `    LeapController *aController = (LeapController *)[notification object];`
 *     `    [aController enableGesture:LEAP_GESTURE_TYPE_CIRCLE enable:YES];`
 *     `    //...`
 *     `}`
 *
 * @param notification The <LeapController> object dispatching the notification.
 * @available Since 1.0
 */
- (void)onConnect:(NSNotification *)notification;
/**
 * Dispatched when the <LeapController> object disconnects from the Leap software.
 * The controller can disconnect when the Leap device is unplugged, the
 * user shuts the Leap software down, or the Leap software encounters an
 * unrecoverable error.
 *
 *     `- (void)onDisconnect:(NSNotification *)notification`
 *     `{`
 *     `    NSLog(@"Disconnected");`
 *     `}`
 *
 * Note: When you launch a Leap-enabled application in a debugger, the
 * Leap library does not disconnect from the application. This is to allow
 * you to step through code without losing the connection because of time outs.
 *
 * @param notification The <LeapController> object dispatching the notification.
 * @available Since 1.0
 */
- (void)onDisconnect:(NSNotification *)notification;
/**
 * Dispatched when this LeapListener object is removed from the <LeapController>
 * or the controller instance is destroyed.
 *
 *     `- (void)onExit:(NSNotification *)notification`
 *     `{`
 *     `    NSLog(@"Exited");`
 *     `}`
 *
 * @param notification The <LeapController> object dispatching the notification.
 * @available Since 1.0
 */
- (void)onExit:(NSNotification *)notification;
/**
 * Dispatched when a new <LeapFrame> containing hand and finger tracking data is available.
 * Access the new frame data using the <[LeapController frame:]> function.
 *
 *    `- (void)onFrame:(NSNotification *)notification`
 *    `{`
 *    `     NSLog(@"New LeapFrame");`
 *    `     LeapController *controller = (LeapController *)[notification object];`
 *    `     LeapFrame *frame = [controller frame:0];`
 *    `     //...`
 *    `}`
 *
 * Note, the <LeapController> skips any pending onFrame notifications while your
 * onFrame handler executes. If your implementation takes too long to return,
 * one or more frames can be skipped. The controller still inserts the skipped
 * frames into the frame history. You can access recent frames by setting
 * the history parameter when calling the [LeapController frame:] function.
 * You can determine if any pending onFrame events were skipped by comparing
 * the ID of the most recent frame with the ID of the last received frame.
 *
 * @param notification The <LeapController> object dispatching the notification.
 * @available Since 1.0
 */
- (void)onFrame:(NSNotification *)notification;

@end

//////////////////////////////////////////////////////////////////////////
//DELEGATE
/**
 * The LeapDelegate protocol defines a set of methods that you can
 * implement in a delegate object for a <LeapController>. The
 * LeapController calls the delegate methods when Leap events occur,
 * such as when a new frame of data is available.
 *
 * To use the LeapDelegate protocol, implement a class adopting the <LeapDelegate> protocol
 * and assign it to a LeapController instance:
 *
 *     `MYDelegate *delegate = [[MYDelegate alloc] init];`
 *     `LeapController *controller = [[LeapController alloc] init];`
 *     `[controller addDelegate:delegate];`
 *
 * When a new frame of data is ready, the controller calls the
 * <[LeapDelegate onFrame:]> method. The other Leap events, `onInit`, `onConnect`, `onDisconnect`,
 * `onExit`, `onFocusGained`, and `onFocusLost` are handled in the same
 * manner. The Controller object is multithreaded and calls the LeapDelegate
 * functions on its own threads, not on an application thread.
 *
 * Using the LeapDelegate protocol is not mandatory. You can also use
 * NSNotifications with a <LeapListener> object or simply poll the
 * controller object (as described in the <LeapController> class overview).
 * @available Since 1.0
 */
@protocol LeapDelegate<NSObject>

@optional
/**
 * Called once, when the <LeapController> has finished initializing.
 *
 *
 *    `- (void)onInit:(LeapController *)controller`
 *    `{`
 *    `    NSLog(@"Initialized");`
 *    `    //...`
 *    `}`
 *
 * @param controller The parent <LeapController> object.
 * @available Since 1.0
 */
- (void)onInit:(LeapController *)controller;
/**
 * Called when the <LeapController> object connects to the Leap software, or when
 * this ListenerDelegate object is added to a controller that is already connected.
 *
 *     `- (void)onConnect:(LeapController *)controller`
 *     `{`
 *     `    NSLog(@"Connected");`
 *     `    [controller enableGesture:LEAP_GESTURE_TYPE_CIRCLE enable:YES];`
 *     `    //...`
 *     `}`
 *
 * @param controller The parent <LeapController> object.
 * @available Since 1.0
 */
- (void)onConnect:(LeapController *)controller;
/**
 * Called when the <LeapController> object disconnects from the Leap software.
 * The controller can disconnect when the Leap device is unplugged, the
 * user shuts the Leap software down, or the Leap software encounters an
 * unrecoverable error.
 *
 *     `- (void)onDisconnect:(LeapController *)controller`
 *     `{`
 *     `    NSLog(@"Disconnected");`
 *     `}`
 *
 * @param controller The parent <LeapController> object.
 * @available Since 1.0
 */
- (void)onDisconnect:(LeapController *)controller;
/**
 * Called when this LeapDelegate object is removed from the <LeapController>
 * or the controller instance is destroyed.
 *
 *     `- (void)onExit:(LeapController *)controller`
 *     `{`
 *     `    NSLog(@"Exited");`
 *     `}`
 *
 * Note: When you launch a Leap-enabled application in a debugger, the
 * Leap library does not disconnect from the application. This is to allow
 * you to step through code without losing the connection because of time outs.
 *
 * @param controller The parent <LeapController> object.
 * @available Since 1.0
 */
- (void)onExit:(LeapController *)controller;
/**
 * Called when a new frame of hand and finger tracking data is available.
 * Access the new frame data using the <[LeapController frame:]> function.
 *
 *    `- (void)onFrame:(LeapController *)controller`
 *    `{`
 *    `     NSLog(@"New LeapFrame");`
 *    `     LeapFrame *frame = [controller frame:0];`
 *    `     //...`
 *    `}`
 *
 * Note, the LeapController skips any pending frames while your
 * onFrame handler executes. If your implementation takes too long to return,
 * one or more frames can be skipped. The controller still inserts the skipped
 * frames into the frame history. You can access recent frames by setting
 * the history parameter when calling the <[LeapController frame:]> function.
 * You can determine if any pending frames were skipped by comparing
 * the ID of the current frame with the ID of the previous received frame.
 *
 * @param controller The parent <LeapController> object.
 * @available Since 1.0
 */
- (void)onFrame:(LeapController *)controller;
/**
 * Called when this application becomes the foreground application.
 *
 * Only the foreground application receives tracking data from the Leap
 * Motion Controller. This function is only called when the controller
 * object is in a connected state.
 *
 *     `- (void)onFocusGained:(LeapController *)controller`
 *     `{`
 *     `    NSLog(@"Focus Gained");`
 *     `}`
 *
 * @param controller The parent <LeapController> object.
 * @available Since 1.0
 */
- (void)onFocusGained:(LeapController *)controller;
/**
 * Called when this application loses the foreground focus.
 *
 * Only the foreground application receives tracking data from the Leap
 * Motion Controller. This function is only called when the controller
 * object is in a connected state.
 *
 *     `- (void)onFocuslost:(LeapController *)controller`
 *     `{`
 *     `    NSLog(@"Focus Lost");`
 *     `}`
 *
 * @param controller The parent <LeapController> object.
 * @available Since 1.0
 */
- (void)onFocusLost:(LeapController *)controller;

@end



//////////////////////////////////////////////////////////////////////////
