/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef HIPTERMDEFINITION_H
#define HIPTERMDEFINITION_H

// The following header file is generated by CMake and thus it's located in
// the build directory. It provides an export macro for classes and functions
// that you want to be part of the public interface of your module.
#include <MitkOrthopedicsExports.h>

namespace othopedics
{
	
	
	enum class MITKORTHOPEDICS_EXPORT ESide
	{
		right,
		left
	};

	MITKORTHOPEDICS_EXPORT inline const char* to_string(ESide e)
	{
		switch (e)
		{
		case ESide::right: return "right";
		case ESide::left: return "left";
		default: return "unknown";
		}
	}

	enum class ELandMarks
	{
		//==========================Pelvis=======================
		/** \brief [UserInput] A registration point located near the lowermost border of the operative posterior acetabulum.*/
		p_PostHorn = 0,
		/** \brief [UserInput] A registration point located near the lowermost border of the operative anterior acetabulum..*/
		p_AntHorn,
		/** \brief [UserInput] A depressed area located lateral to the anterior margin of the operative acetabulum,
		 * easily palpable or identifiable during surgery.
		 * Used only as an alternative choice for the anterior horn landmark in direct anterior pelvic registration.*/
		p_AntNotch,
		/** \brief [UserInput]Hip Center of Rotation	(operative Side) */
		p_COR,

		/** \brief [UserInput] Left ASIS(Anterior superior iliac spine) */
		p_ASIS_L,
		/** \brief [UserInput] Right ASIS(Anterior superior iliac spine) */
		p_ASIS_R,
		/** \brief [UserInput] Left Pubic tubercle,The rounded eminence located at the upper border of the right pubis near the pubic symphysis. */
		p_PT_L,
		/** \brief [UserInput] Right Pubic tubercle */
		p_PT_R,
		/** \brief [UserInput] Leaving the boneand centrally located in the symphysis pubis, it is used to determine the midlineand plane of segmentation of the pelvis. */
		p_MidLine,
		/** \brief [UserInput]The top line of the sacral plate (S1) defined on the sagittal CT image, used to determine the angle between the sacral plate and the anterior-posterior axis.	 */
		p_SS_A,
		/** \brief [UserInput]The top line of the sacral plate (S1) defined on the sagittal CT image, used to determine the angle between the sacral plate and the anterior-posterior axis.	 */
		p_SS_P,

		/** \brief [UserInput]Femur Assembly Point (Left Side) = FHC_L	*/
		p_FemurAssemblyPoint_L,
		/** \brief [UserInput]Femur Assembly Point (Right Side) = FHC_R */
		p_FemurAssemblyPoint_R,

		/** \brief [ModelOutput] the Origin of Pelvis local coordinates */
		p_O,

		// /** \brief [ModelOutput] the Center of Right Acetabular */
		// p_RAC,
		// /** \brief [ModelOutput] the Center of Left Acetabular */
		// p_LAC,


		//==========================Femur=======================

		/** \brief [UserInput] Femoral Neck Saddle */
		f_FNS,
		/** \brief [UserInput] Femur Landmark 2 */
		f_FL2,
		/** \brief [UserInput] Greater Trochanter */
		f_GT,

		/** \brief [UserInput] Femur Head Center */
		f_FHC,
		/** \brief [UserInput] Femur Neck center */
		f_FNC,
		/** \brief [UserInput] Proximal point of Femoral canal axis */
		f_PFCA,
		/** \brief [UserInput] Distal point of Femoral canal axis  */
		f_DFCA,
		/** \brief [UserInput] Medial Femoral Epicondyle */
		f_ME,
		/** \brief [UserInput] Lateral Femoral Epicondyle  */
		f_LE,
		/** \brief [UserInput] Lesser Trochanter */
		f_LT,
		/** \brief [ModelOutput] Midpoint of Medial and Lateral Epicondyles */
		f_MidEEs,
		/** \brief [ModelOutput] Intersection Point of the Femoral Canal and the Neck shaft  */
		f_NeckCanalIntersectPoint,

		f_O,

		/** \brief [UserInput] Proximal Checkpoint,The position before the operation   */
		f_CheckPointP_pre,
		/** \brief [UserInput] Distal Checkpoint,The position before the operation    */
		f_CheckPointD_pre,
		/** \brief [UserInput] Proximal Checkpoint,The position after the operation   */
		f_CheckPointP_post,
		/** \brief [UserInput] Distal Checkpoint,The position after the operation    */
		f_CheckPointD_post,
		/** \brief [UserInput] Femur Head Center During the operation, the surgeon will place four points on the cup to verify its placement.  */
		f_FHC_inOp,

		//==========================Cup=======================
		cup_COR,
		cup_LinerAssemblyPoint,

		//==========================Stem=======================
		stem_O,

		stem_HeadAssemblyPoint_M,
		stem_HeadAssemblyPoint_S,
		stem_HeadAssemblyPoint_L,

		stem_CutPoint,
		stem_NeckCanalIntersectPoint,
		stem_EndPoint,
	};

	inline const char* to_String(ELandMarks e)
	{
		switch (e)
		{
		case ELandMarks::p_PostHorn: return "p_PostHorn";
		case ELandMarks::p_AntHorn: return "p_AntHorn";
		case ELandMarks::p_AntNotch: return "p_AntNotch";
		case ELandMarks::p_COR: return "p_COR";
		case ELandMarks::p_ASIS_R: return "p_ASIS_R";
		case ELandMarks::p_ASIS_L: return "p_ASIS_L";
		case ELandMarks::p_PT_R: return "p_PT_R";
		case ELandMarks::p_PT_L: return "p_PT_L";
		case ELandMarks::p_MidLine: return "p_MidLine";
		case ELandMarks::p_SS_A: return "p_SS_A";
		case ELandMarks::p_SS_P: return "p_SS_P";
		case ELandMarks::p_FemurAssemblyPoint_L: return "p_FemurAssemblyPoint_L";
		case ELandMarks::p_FemurAssemblyPoint_R: return "p_FemurAssemblyPoint_R";
		case ELandMarks::p_O: return "p_O";
		case ELandMarks::f_FNS: return "f_FNS";
		case ELandMarks::f_FL2: return "f_FL2";
		case ELandMarks::f_GT: return "f_GT";
		case ELandMarks::f_FHC: return "f_FHC";
		case ELandMarks::f_FNC: return "f_FNC";
		case ELandMarks::f_PFCA: return "f_PFCA";
		case ELandMarks::f_DFCA: return "f_DFCA";
		case ELandMarks::f_ME: return "f_ME";
		case ELandMarks::f_LE: return "f_LE";
		case ELandMarks::f_LT: return "f_LT";
		case ELandMarks::f_CheckPointP_pre: return "f_CheckPointP_pre";
		case ELandMarks::f_CheckPointD_pre: return "f_CheckPointD_pre";
		case ELandMarks::f_CheckPointP_post: return "f_CheckPointP_post";
		case ELandMarks::f_CheckPointD_post: return "f_CheckPointD_post";
		case ELandMarks::f_FHC_inOp: return "f_FHC_inOp";
		case ELandMarks::f_MidEEs: return "f_MidEEs";
		case ELandMarks::f_O: return "f_O";
		default: return "unknown";
		}
	}

	enum class EAxes
	{
		/** \brief [ModelOutput] Pelvic local coords X axis,
		 * pointing to anterior,Perpendicular to the plane defined by the bilateral anterior superior iliac spines and midpoint, perpendicular to the Z-axis, . */
		p_X = 0,
		/** \brief [ModelOutput] Pelvic local coords Y axis,
		 * pointing to head,Perpendicular to both the X-axis and Z-axis. */
		p_Y,
		/** \brief [ModelOutput] Pelvic local coords Z axis,
		 * pointing to the right. Parallel to the line connecting the left and right anterior superior iliac spines */
		p_Z,
		/** \brief [ModelOutput] Pelvic Horizontal Axis, The line connecting LASI and RASI */

		/** \brief [ModelOutput] Femur Mechanical Axis*/
		f_Mechanics,
		/** \brief [ModelOutput] Femur Canal Axis*/
		f_Canal,
		/** \brief [ModelOutput] Femur Epicondylar Axis*/
		f_Epicondyla,
		/** \brief [ModelOutput] Femur Neck Axis*/
		f_Neck,

		f_X_canal,
		f_Y_canal,
		f_Z_canal,

		f_X_mechanical,
		f_Y_mechanical,
		f_Z_mechanical,


		//prothesis
		cup_X,
		cup_Y,
		cup_Z,

		stem_X,
		stem_Y,
		stem_Z
	};

	inline const char* to_string(EAxes e)
	{
		switch (e)
		{
		case EAxes::p_X: return "p_X";
		case EAxes::p_Y: return "p_Y";
		case EAxes::p_Z: return "p_Z";
		case EAxes::f_Mechanics: return "f_Mechanics";
		case EAxes::f_Canal: return "f_Canal";
		case EAxes::f_Epicondyla: return "f_Epicondyla";
		case EAxes::f_Neck: return "f_Neck";
		case EAxes::f_X_canal: return "f_X_canal";
		case EAxes::f_Y_canal: return "f_Y_canal";
		case EAxes::f_Z_canal: return "f_Z_canal";
		case EAxes::f_X_mechanical: return "f_X_mechanical";
		case EAxes::f_Y_mechanical: return "f_Y_mechanical";
		case EAxes::f_Z_mechanical: return "f_Z_mechanical";
		default: return "unknown";
		}
	}

	enum class EPlanes
	{
		MIDPLANE = 0,
	};

	inline const char* to_String(EPlanes e)
	{
		switch (e)
		{
		case EPlanes::MIDPLANE: return "MIDPLANE";
		default: return "unknown";
		}
	}

	enum class EResult
	{
		/** \brief [ModelOutput] ����ǰ���*/
		p_PelvicTilt = 0,
		/** \brief [ModelOutput] ����ɹ����*/
		f_OpVersion,

		/** \brief [ModelOutput] �Ҳ�ʱ�ǰ���*/
		r_RightAnteversion,
		/** \brief [ModelOutput] �Ҳ�ʱ���չ��*/
		r_RightInclination,
		/** \brief [ModelOutput] ���ʱ�ǰ���*/
		r_LeftAnteversion,
		/** \brief [ModelOutput] ���ʱ���չ��*/
		r_LeftInclination,


		/** \brief [ModelOutput] Contralateral Side HipLength*/
		r_HipLength_contra,
		/** \brief [ModelOutput] Preoperative HipLength(Operation Side)*/
		r_HipLength_preop,
		/** \brief [ModelOutput] Planed Hip Length(Operation Side)*/
		r_HipLength_planed,
		/** \brief [ModelOutput] Reduced Hip Length(Operation Side)*/
		r_HipLength_reduced,


		/** \brief [ModelOutput] Contralateral Side Combined Offset*/
		r_Offset_contra,
		/** \brief [ModelOutput] Preoperative Combined Offset(Operation Side)*/
		r_Offset_preop,
		/** \brief [ModelOutput] Planed Combined Offset(Operation Side)*/
		r_Offset_planed,
		/** \brief [ModelOutput] Reduced Combined Offset(Operation Side)*/
		r_Offset_reduced,

		/** \brief [ModelOutput] Preoperative HipLength Difference, Operation Side vs.Contralateral Side
		 *
		 * r_HipLength_preop - r_HipLength_contra
		 */
		r_HipLengthDiff_preop_vs_contra,
		/** \brief [ModelOutput] Preoperative Combined Offset Diff, Operation Side vs. Contralateral Side
		 *
		 * r_Offset_preop - r_Offset_contra
		 */
		r_OffsetDiff_preop_vs_contra,


		/** \brief [ModelOutput] Planed HipLength Diff,  vs. Contralateral Side
		 *
		 * r_HipLength_planed - r_HipLength_contra
		 */
		r_HipLengthDiff_planed_vs_contra,
		/** \brief [ModelOutput] Planed Combined Offset Diff, vs. Contralateral Side
		 *
		 * r_Offset_planed - r_Offset_contra
		 */
		r_OffsetDiff_planed_vs_contra,

		/** \brief [ModelOutput] Planed HipLength Diff,  vs. Preoperative
		 *
		 * r_HipLength_planed - r_HipLength_preop
		 */
		r_HipLengthDiff_planed_vs_preop,
		/** \brief [ModelOutput] Planed  Combined Offset Diff,  vs. Preoperative
		 *
		 * r_Offset_planed - r_Offset_preop
		 */
		r_OffsetDiff_planed_vs_preop,


		/** \brief [ModelOutput] Reduced HipLength Diff,  vs. Contralateral Side
		 *
		 * r_HipLength_reduced - r_HipLength_contra
		 */
		r_HipLengthDiff_reduced_vs_contra,
		/** \brief [ModelOutput] Reduced  Combined Offset Diff,  vs. Contralateral Side
		 *
		 * r_Offset_reduced - r_Offset_contra
		 */
		r_OffsetDiff_reduced_vs_contra,

		/** \brief [ModelOutput] Reduced HipLength Diff,  vs. Preoperative
		 *
		 * r_HipLength_reduced - r_HipLength_preop
		 */
		r_HipLengthDiff_reduced_vs_preop,
		/** \brief [ModelOutput] Reduced  Combined Offset Diff,  vs. Preoperative
		 *
		 * r_Offset_reduced - r_Offset_preop
		 */
		r_OffsetDiff_reduced_vs_preop,
	};

	inline const char* to_string(EResult e)
	{
		switch (e)
		{
		case EResult::p_PelvicTilt: return "p_PelvicTilt";
		case EResult::f_OpVersion: return "f_OpVersion";
		case EResult::r_RightAnteversion: return "r_RightAnteversion";
		case EResult::r_RightInclination: return "r_RightInclination";
		case EResult::r_LeftAnteversion: return "r_LeftAnteversion";
		case EResult::r_LeftInclination: return "r_LeftInclination";
		case EResult::r_HipLength_contra: return "r_HipLength_contra";
		case EResult::r_HipLength_preop: return "r_HipLength_preop";
		case EResult::r_HipLength_planed: return "r_HipLength_planed";
		case EResult::r_HipLength_reduced: return "r_HipLength_reduced";
		case EResult::r_Offset_contra: return "r_Offset_contra";
		case EResult::r_Offset_preop: return "r_Offset_preop";
		case EResult::r_Offset_planed: return "r_Offset_planed";
		case EResult::r_Offset_reduced: return "r_Offset_reduced";
		case EResult::r_HipLengthDiff_preop_vs_contra: return "r_HipLengthDiff_preop_vs_contra";
		case EResult::r_OffsetDiff_preop_vs_contra: return "r_OffsetDiff_preop_vs_contra";
		case EResult::r_HipLengthDiff_planed_vs_contra: return "r_HipLengthDiff_planed_vs_contra";
		case EResult::r_OffsetDiff_planed_vs_contra: return "r_OffsetDiff_planed_vs_contra";
		case EResult::r_HipLengthDiff_planed_vs_preop: return "r_HipLengthDiff_planed_vs_preop";
		case EResult::r_OffsetDiff_planed_vs_preop: return "r_OffsetDiff_planed_vs_preop";
		case EResult::r_HipLengthDiff_reduced_vs_contra: return "r_HipLengthDiff_reduced_vs_contra";
		case EResult::r_OffsetDiff_reduced_vs_contra: return "r_OffsetDiff_reduced_vs_contra";
		case EResult::r_HipLengthDiff_reduced_vs_preop: return "r_HipLengthDiff_reduced_vs_preop";
		case EResult::r_OffsetDiff_reduced_vs_preop: return "r_OffsetDiff_reduced_vs_preop";
		default: return "unknown";
		}
	}

	

	enum class ECupAngleType
	{
		RADIO_GRAPHIC,//���ã�Ӱ��ѧ����
		OPERATIVE, //��е��Ķ���
		ANATOMICAL //����ѧ�Ķ���
	};
}


#endif