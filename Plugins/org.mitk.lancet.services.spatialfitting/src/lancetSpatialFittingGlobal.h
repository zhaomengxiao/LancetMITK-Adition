/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief Global macro definition header file.
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \version V1.0.0
 * \date 2022-11-18 10:32:42
 * 
 * \par Modify History
 *		-# Sun initialization the version for 2022-11-18 10:32:42.
 *
 * \remark Non
 */
#ifndef LancetSpatialFittingGlobal_H
#define LancetSpatialFittingGlobal_H

// Declare namespace generation macro.
#ifndef LancetSpatialFittingNameSpace
# define LancetSpatialFittingNameSpace

// CXX Standard Namespace Keyword Declaration.
# ifndef CXX_NAMESPACE
#  define CXX_NAMESPACE namespace
# endif // !CXX_NAMESPACE

// Namespace Scope Start Tab.
# define NAMESPACE_START_SCOPE {
// Namespace Scope Exit Tab.
# define NAMESPACE_CLOSE_SCOPE }

// Namespace Scope Template Generate Macro Definition.
# ifndef NAMSPEACE_DEFINE 
# define NAMSPEACE_DEFINE
#  define BEGIN_NAMSPEACE_DEFINE(name) CXX_NAMESPACE name NAMESPACE_START_SCOPE
#  define END_NAMSPEACE_DEFINE(name) NAMESPACE_CLOSE_SCOPE // ##name
# endif // !NAMSPEACE_DEFINE

// Macro definition of declaration space fitting namespace.
# define BEGIN_SPATIAL_FITTING_NAMESPACE BEGIN_NAMSPEACE_DEFINE(lancet::spatial_fitting)
# define END_SPATIAL_FITTING_NAMESPACE END_NAMSPEACE_DEFINE(lancet::spatial_fitting)
#endif // !LancetSpatialFittingNameSpace

// Import action macro of dynamic library.
#ifndef ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
# include <org_mitk_lancet_services_spatialfitting_Export.h>
#endif

// Import CXX standard memory processing module.
#ifndef _MEMORY_
#  include <memory>
#endif

#ifndef itkObject_h
#	 include <itkObject.h>
#endif

#ifndef itkSmartPointer_h
#  include <itkSmartPointer.h>
#endif

#ifndef itkObjectFactory_h
#  include <itkObjectFactory.h>
#endif

#ifndef MITK_COMMON_H_DEFINED
#	 include <mitkCommon.h>
#endif

#endif // !LancetSpatialFittingGlobal_H