#pragma once

#include "DianaAPIDef.h"

DIANA_API int moveJToTarget(/*IN[7]*/ double *joints, /*IN*/ double v, /*IN*/ double a, /*IN*/ int zv_shaper_order = 0,
                            /*IN*/ double zv_shaper_frequency = 0, /*IN*/ double zv_shaper_damping_ratio = 0,
                            /*IN*/ const char *strIpAddress = "");

DIANA_API int moveJToPose(/*IN[6]*/ double *pose, /*IN*/ double v, /*IN*/ double a,
                          /*IN[6]*/ double *active_tcp = nullptr, /*IN*/ int zv_shaper_order = 0,
                          /*IN*/ double zv_shaper_frequency = 0, /*IN*/ double zv_shaper_damping_ratio = 0,
                          /*IN*/ const char *strIpAddress = "");

DIANA_API int moveLToTarget(/*IN[7]*/ double *joints, /*IN*/ double v, /*IN*/ double a, /*IN*/ int zv_shaper_order = 0,
                            /*IN*/ double zv_shaper_frequency = 0, /*IN*/ double zv_shaper_damping_ratio = 0,
                            /*IN*/ bool avoid_singular = false,
                            /*IN*/ const char *strIpAddress = "");

DIANA_API int moveLToPose(/*IN[6]*/ double *pose, /*IN*/ double v, /*IN*/ double a,
                          /*IN[6]*/ double *active_tcp = nullptr, /*IN*/ int zv_shaper_order = 0,
                          /*IN*/ double zv_shaper_frequency = 0, /*IN*/ double zv_shaper_damping_ratio = 0,
                          /*IN*/ bool avoid_singular = false,
                          /*IN*/ const char *strIpAddress = "");

DIANA_API int freeDriving(unsigned char mode, /*IN*/ const char *strIpAddress = "");

DIANA_API int stop(const char *strIpAddress = "");

DIANA_API int forward(/*IN[7]*/ double *joints, /*OUT[6]*/ double *pose, /*IN[6]*/ double *active_tcp = nullptr,
                      /*IN*/ const char *strIpAddress = "");

DIANA_API int inverse(/*IN[6]*/ double *pose, /*OUT[7]*/ double *joints, /*IN[6]*/ double *active_tcp = nullptr,
                      /*IN*/ const char *strIpAddress = "");

DIANA_API int releaseBrake(const char *strIpAddress = "");  // powerOn

// setDefaultActiveTcp:该函数将会改变moveTCP，rotationTCP，moveJToPose，moveLToPose，speedJ，speedL，forward，inverse，getTcpPos，getTcpExternalForce的默认行为。
DIANA_API int setDefaultActiveTcp(/*IN[16]*/ double *default_tcp, /*IN*/ const char *strIpAddress = "");

DIANA_API int setDefaultActiveTcpPose(/*IN[6]*/ double *arrPose, /*IN*/ const char *strIpAddress = "");

DIANA_API int homogeneous2Pose(/*IN[16]*/ double *matrix, /*OUT[6]*/ double *pose);

DIANA_API int pose2Homogeneous(/*IN[6]*/ double *pose, /*OUT[16]*/ double *matrix);

DIANA_API int inverse_ext(/*IN[7]*/ double *ref_joints, /*IN[6]*/ double *pose, /*OUT[7]*/ double *joints,
                          /*IN[6]*/ double *active_tcp = nullptr, /*IN*/ const char *strIpAddress = "");

DIANA_API int addMoveLByTarget(/*IN*/ unsigned int complex_path_id, /*IN[7]*/ double *target_joints, /*IN*/ double vel,
                               /*IN*/ double acc, /*IN*/ double blendradius, /*IN*/ int zv_shaper_order = 0,
                               /*IN*/ double zv_shaper_frequency = 0, /*IN*/ double zv_shaper_damping_ratio = 0,
                               /*IN*/ bool avoid_singular = false,
                               /*IN*/ const char *strIpAddress = "");

DIANA_API int addMoveLByPose(/*IN*/ unsigned int complex_path_id, /*IN[6]*/ double *target_pose, /*IN*/ double vel,
                             /*IN*/ double acc, /*IN*/ double blendradius, /*IN*/ int zv_shaper_order = 0,
                             /*IN*/ double zv_shaper_frequency = 0, /*IN*/ double zv_shaper_damping_ratio = 0,
                             /*IN*/ bool avoid_singular = false,
                             /*IN*/ const char *strIpAddress = "");

DIANA_API int addMoveJByTarget(/*IN*/ unsigned int complex_path_id, /*IN[7]*/ double *target_joints,
                               /*IN*/ double vel_percent, /*IN*/ double acc_percent, /*IN*/ double blendradius_percent,
                               /*IN*/ int zv_shaper_order = 0, /*IN*/ double zv_shaper_frequency = 0,
                               /*IN*/ double zv_shaper_damping_ratio = 0,
                               /*IN*/ const char *strIpAddress = "");

DIANA_API int addMoveJByPose(/*IN*/ unsigned int complex_path_id, /*IN[6]*/ double *target_pose,
                             /*IN*/ double vel_percent, /*IN*/ double acc_percent, /*IN*/ double blendradius_percent,
                             /*IN*/ int zv_shaper_order = 0, /*IN*/ double zv_shaper_frequency = 0,
                             /*IN*/ double zv_shaper_damping_ratio = 0,
                             /*IN*/ const char *strIpAddress = "");

DIANA_API int addMoveCByTarget(/*IN*/ unsigned int complex_path_id, /*IN[7]*/ double *pass_joints,
                               /*IN[7]*/ double *target_joints, /*IN*/ double vel, /*IN*/ double acc,
                               /*IN*/ double blendradius, /*IN*/ bool ignore_rotation, /*IN*/ int zv_shaper_order = 0,
                               /*IN*/ double zv_shaper_frequency = 0, /*IN*/ double zv_shaper_damping_ratio = 0,
                               /*IN*/ bool avoid_singular = false,
                               /*IN*/ const char *strIpAddress = "");

DIANA_API int addMoveCByPose(/*IN*/ unsigned int complex_path_id, /*IN[6]*/ double *pass_pose,
                             /*IN[6]*/ double *target_pose, /*IN*/ double vel, /*IN*/ double acc,
                             /*IN*/ double blendradius, /*IN*/ bool ignore_rotation, /*IN*/ int zv_shaper_order = 0,
                             /*IN*/ double zv_shaper_frequency = 0, /*IN*/ double zv_shaper_damping_ratio = 0,
                             /*IN*/ bool avoid_singular = false,
                             /*IN*/ const char *strIpAddress = "");

DIANA_API int readDO(/*IN*/ const char *groupName, /*IN*/ const char *IOName, /*OUT*/ int &value,
                     /*IN*/ const char *strIpAddress = "");

DIANA_API int readAO(/*IN*/ const char *groupName, /*IN*/ const char *IOName, /*OUT*/ int &mode, /*OUT*/ double &value,
                     /*IN*/ const char *strIpAddress = "");

DIANA_API int getDefaultActiveTcpPose(/*OUT[6]*/ double *arrPose, /*IN*/ const char *strIpAddress = "");

DIANA_API int getActiveTcpPayload(/*OUT[10]*/ double *payload, /*IN*/ const char *strIpAddress = "");

DIANA_API int moveTcp_ex(/*IN*/ coordinate_e c, /*IN*/ tcp_direction_e d, /*IN*/ double v, /*IN*/ double a,
                         /*IN*/ const char *strIpAddress = "");

DIANA_API int rotationTCP_ex(/*IN*/ coordinate_e c, /*IN*/ tcp_direction_e d, /*IN*/ double v, /*IN*/ double a,
                             /*IN*/ const char *strIpAddress = "");

DIANA_API int poseTransform(/*IN[6]*/ double *srcPose, /*IN[6]*/ double *srcMatrixPose, /*IN[6]*/ double *dstMartixPose,
                            /*OUT[6]*/ double *dstPose);

DIANA_API int inverseClosedFull(/*IN[6]*/ const double *pose, /*IN*/ const int lock_joint_index,
                                /*IN*/ const double lock_joint_position, /*IN[7]*/ double *ref_joints,
                                /*IN[6]*/ double *active_tcp = nullptr, /*IN*/ const char *strIpAddress = "");

DIANA_API int getGravInfo(/*IN[3]*/ double *grav, /*IN*/ const char *strIpAddress = "");

DIANA_API int setGravInfo(/*IN[3]*/ const double *grav, /*IN*/ const char *strIpAddress = "");

DIANA_API int getGravAxis(/*IN[3]*/ double *grav_axis, /*IN*/ const char *strIpAddress = "");

DIANA_API int setGravAxis(/*IN[3]*/ const double *grav_axis, /*IN*/ const char *strIpAddress = "");

// industry-v2.8 api
DIANA_API int requireHandlingError(/*OUT*/ int *error, /*IN*/ const char *strIpAddress = "");

DIANA_API int enterRescueMode(const char *strIpAddress = "");

DIANA_API int switchRescueMode(const int scuWorkMode, const char *strIpAddress = "");

DIANA_API int leaveRescueMode(const char *strIpAddress = "");

DIANA_API int getCartImpedanceCoordinateType(const char *strIpAddress = "");

DIANA_API int setCartImpedanceCoordinateType(const int intCoordinateType, const char *strIpAddress = "");

DIANA_API int getJointLockedInCartImpedanceMode(/*OUT*/ bool &isLocked, /*IN*/ const char *strIpAddress = "");

DIANA_API int setJointLockedInCartImpedanceMode(/*IN*/ const bool bLock, /*IN*/ const int intLockedJointIndex = 2,
                                                /*IN*/ const char *strIpAddress = "");

DIANA_API int setHeartbeatParam(/*IN*/ int disconnectTimeout, /*IN*/ int stopRobotTimeout,
                                /*IN*/ const char *strIpAddress = "");

DIANA_API int getHeartbeatParam(/*OUT*/ int *disconnectTimeout, /*OUT*/ int *stopRobotTimeout,
                                /*IN*/ const char *strIpAddress = "");

DIANA_API int customRobotState(/*IN*/ int action, /*IN*/ unsigned long long customBits,
                               /*IN*/ const char *strIpAddress = "");

DIANA_API int getCustomRobotState(/*OUT*/ unsigned long long *customBits, /*IN*/ const char *strIpAddress = "");

DIANA_API int getTcpPoseByTcpName(/*IN*/ const char *tcpName, /*OUT[6]*/ double *coordinate,
                                  /*IN*/ const char *strIpAddress = "");

DIANA_API int getTcpPoseByWorkPieceName(/*IN*/ const char *workPieceName, /*OUT[6]*/ double *coordinate,
                                        /*IN*/ const char *strIpAddress = "");

DIANA_API int getPayLoadByTcpName(/*IN*/ const char *tcpName, /*OUT[10]*/ double *payload,
                                  /*IN*/ const char *strIpAddress = "");

DIANA_API int setDefaultToolTcpCoordinate(/*IN*/ const char *tcpName, /*IN*/ const char *strIpAddress = "");

DIANA_API int setDefaultWorkPieceTcpCoordinate(/*IN*/ const char *workPieceName, /*IN*/ const char *strIpAddress = "");

DIANA_API int getDefaultTcpCoordinate(/* OUT */ char *tcpName, /* IN */ const char *strIpAddress = "");

DIANA_API int getDefaultWorkPieceCoordinate(/* OUT */ char *workpieceName, /* IN */ const char *strIpAddress = "");

DIANA_API int setVelocityPercentValue(/* IN */ int value, /* IN */ const char *strIpAddress = "");
