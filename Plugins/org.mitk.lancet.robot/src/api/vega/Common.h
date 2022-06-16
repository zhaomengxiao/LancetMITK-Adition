/**
 * @file Common.h
 * @author WUZHUOBIN jiejin2022.163.com
 * @since Mar.29.2020
 * @version 1.0
 * @copyright WUZHUOBIN All rights reserved.
 * Some useful fuctions to print out some object of vega.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the LICENSE for more detail.
 Copyright (c) WUZHUOBIN. All rights reserved.
 See COPYRIGHT for more detail.
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 Internal usage only, without the permission of the author, please DO
 NOT publish and distribute without the author's permission.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
#ifndef COMMON_H
#define COMMON_H
// std
#include <ostream>
class SystemAlert;
class PortHandleInfo;
class MarkerData;
class Transform;
class ToolData;

/**
 * @brief Print SystemAlert object.
 * 
 * @param out 
 * @param alert 
 * @return std::ostream& 
 * @see SystemAlert
 */
std::ostream &operator<<(std::ostream &out, const SystemAlert &alert);

/**
 * @brief Print PortHandleInfo object.
 * 
 * @param out 
 * @param info 
 * @return std::ostream& 
 * @see PortHandleInfo
 */
std::ostream &operator<<(std::ostream &out, const PortHandleInfo &info);

/**
 * @brief Print MarkerData object.
 * 
 * @param out 
 * @param markerData 
 * @return std::ostream& 
 * @see MarkerData
 */
std::ostream &operator<<(std::ostream &out, const MarkerData &markerData);

/**
 * @brief Print Transform object.
 * 
 * @param out 
 * @param transform 
 * @return std::ostream& 
 * @see Transform
 */
std::ostream &operator<<(std::ostream &out, const Transform &transform);

/**
 * @brief Print ToolData Object
 * 
 * @param out 
 * @param toolData 
 * @return std::ostream& 
 * @see ToolData
 */
std::ostream &operator<<(std::ostream &out, const ToolData &toolData);
#endif //!COMMON_H