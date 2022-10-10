#include "lancetRobotTrackingTool.h"

lancet::RobotEndTool::RobotEndTool()
  : TrackingTool(),
    m_TCP(mitk::AffineTransform3D::New()), m_LoadData(0)
{
}

lancet::RobotEndTool::~RobotEndTool()
{
}
