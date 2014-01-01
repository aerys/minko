LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := bullet-dynamics
LOCAL_CFLAGS :=
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../

LOCAL_SRC_FILES := \
ConstraintSolver/btSequentialImpulseConstraintSolver.cpp \
ConstraintSolver/btUniversalConstraint.cpp \
ConstraintSolver/btHingeConstraint.cpp \
ConstraintSolver/btTypedConstraint.cpp \
ConstraintSolver/btSolve2LinearConstraint.cpp \
ConstraintSolver/btContactConstraint.cpp \
ConstraintSolver/btSliderConstraint.cpp \
ConstraintSolver/btGearConstraint.cpp \
ConstraintSolver/btGeneric6DofSpringConstraint.cpp \
ConstraintSolver/btPoint2PointConstraint.cpp \
ConstraintSolver/btGeneric6DofConstraint.cpp \
ConstraintSolver/btHinge2Constraint.cpp \
ConstraintSolver/btConeTwistConstraint.cpp \
Vehicle/btRaycastVehicle.cpp \
Vehicle/btWheelInfo.cpp \
Character/btKinematicCharacterController.cpp \
Dynamics/btDiscreteDynamicsWorld.cpp \
Dynamics/btSimpleDynamicsWorld.cpp \
Dynamics/btRigidBody.cpp \
Dynamics/Bullet-C-API.cpp

include $(BUILD_STATIC_LIBRARY)
