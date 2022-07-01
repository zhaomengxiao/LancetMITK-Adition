/**
 * @file Common.cpp
 * @author WUZHUOBIN jiejin2022.163.com
 * @since Mar.29.2020
 * @version 1.0
 * @copyright WUZHUOBIN All rights reserved.
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
#include "Transform.h"
#include "PortHandleInfo.h"
#include "ToolData.h"
#include "SystemAlert.h"
#include "Common.h"

std::ostream &operator<<(std::ostream &out, const SystemAlert &alert)
{
  out << "SystemAlert:" << '\n';
  out << '\t' << alert.toString() << '\n';
  return out;
}

std::ostream &operator<<(std::ostream &out, const PortHandleInfo &info)
{
  out << "PortHandleInfo:" << '\n';
  out << '\t' << "Port Handle: " << info.getPortHandle() << '\n';
  out << '\t' << "Tool Id: " << info.getToolId() << '\n';
  out << '\t' << "Revision: " << info.getRevision() << '\n';
  out << '\t' << "Serial Number: " << info.getSerialNumber() << '\n';
  out << '\t' << "Status: " << info.getStatus() << '\n';
  return out;
}

std::ostream &operator<<(std::ostream &out, const MarkerData &markerData)
{
  out << '\t' << "MarkerData:" << '\n';
  out << '\t' << '\t' << "status: " << MarkerStatus::toString(markerData.status) << '\n';
  out << '\t' << '\t' << "Marker Index: " << markerData.markerIndex << '\n';
  out << '\t' << '\t' << "Marker Position: " << '\n';
  out << '\t' << '\t' << '\t' << "x: " << markerData.x << '\n';
  out << '\t' << '\t' << '\t' << "y: " << markerData.y << '\n';
  out << '\t' << '\t' << '\t' << "z: " << markerData.z << '\n';
  return out;
}

std::ostream &operator<<(std::ostream &out, const Transform &transform)
{
  out << '\t' << "Transform: " << '\n';
  out << '\t' << '\t' << "Face Number: " << transform.getFaceNumber() << '\n';
  out << '\t' << '\t' << "Error Code: " << TransformStatus::toString(transform.getErrorCode()) << '\n';
  out << '\t' << '\t' << "Is Missing: " << (transform.isMissing() ? "true" : "false") << '\n';
  out << '\t' << '\t' << "Tool Handle: " << transform.toolHandle << '\n';
  out << '\t' << '\t' << "Status: " << transform.status << '\n';
  out << '\t' << '\t' << "Quaterion: " << '\n';
  out << '\t' << '\t' << '\t' << "q0: " << transform.q0 << '\n';
  out << '\t' << '\t' << '\t' << "qx: " << transform.qx << '\n';
  out << '\t' << '\t' << '\t' << "qy: " << transform.qy << '\n';
  out << '\t' << '\t' << '\t' << "qz: " << transform.qz << '\n';
  out << '\t' << '\t' << "Transformation: " << '\n';
  out << '\t' << '\t' << '\t' << "tx: " << transform.tx << '\n';
  out << '\t' << '\t' << '\t' << "ty: " << transform.ty << '\n';
  out << '\t' << '\t' << '\t' << "tz: " << transform.tz << '\n';
  out << '\t' << '\t' << "Error: " << transform.error << '\n';
  return out;
}

std::ostream &operator<<(std::ostream &out, const ToolData &toolData)
{
  out << "ToolData: " << '\n';
  out << '\t' << "Frame Number: " << toolData.frameNumber << '\n';
  out << '\t' << "Transform: " << toolData.transform << '\n';
  // out << '\t' << "System Status: " << (toolData.systemStatus != 0 ? SystemStatus::toString(toolData.systemStatus) : 0) << '\n';
  out << '\t' << "System Status: " << toolData.systemStatus << '\n';
  out << '\t' << "Port Status: " << toolData.portStatus << '\n';
  out << '\t' << "Frame Type:" << FrameType::toString(toolData.frameType) << '\n';
  out << '\t' << "Frame Sequence Index: " << toolData.frameSequenceIndex << '\n';
  out << '\t' << "Frame Status: " << toolData.frameStatus << '\n';
  out << '\t' << "Timestamp(seconds): " << toolData.timespec_s << '\n';
  out << '\t' << "Timestamp(nanoseconds): " << toolData.timespec_ns << '\n';
  for (std::size_t i = 0; i < toolData.markers.size(); ++i)
  {
    out << '\t' << "Marker " << i << ": " << toolData.markers[i] << '\n';
  }
  for (std::size_t i = 0; i < toolData.buttons.size(); ++i)
  {
    out << '\t' << "Button " << i << ": " << toolData.buttons[i] << '\n';
  }
  for (std::size_t i = 0; i < toolData.systemAlerts.size(); ++i)
  {
    out << '\t' << "System Alerts: " << i << ": " << toolData.systemAlerts[i] << '\n';
  }
  out << '\t' << "Data Is New: " << (toolData.dataIsNew ? "true" : "false") << '\n';
  return out;
}
