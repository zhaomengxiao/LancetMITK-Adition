/**
 * @file VegaThread.h
 * @author WUZHUOBIN jiejin2022.163.com
 * @since Mar.29.2020
 * @version 1.0
 * @copyright WUZHUOBIN All rights reserved.
 * VegaThread class.
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
#ifndef VEGA_THREAD_H
#define VEGA_THREAD_H
#pragma once
#include "Common.h"
#include "ToolData.h"
#include "PortHandleInfo.h"
// qt
#include <QThread>

class vtkMatrix4x4;
class Vega;

namespace VegaSpace
{
	void writeSettingFile();
}

/**
 * @class VegaThread
 * @brief A Vega Thread
 * 
 */
class VegaThread: public QThread
{
  Q_OBJECT;
public:
  /**
   * @brief 
   * 
   */
  enum PART_NUMBER {
    PELVIS = 0,
    PROBE,
    ROBOT,
    TOOL
  };
  /**
   * @brief Construct a new Vega Thread object
   * 
   * @param fps Frame per second.
   * @param parent Parent Object.
   */
  explicit VegaThread(unsigned int fps = 20, QObject *parent = nullptr);
  /**
   * @brief Construct a new Vega Thread object
   * 
   * @param parent 
   */
  explicit VegaThread(QObject *parent);
  /**
   * @brief Destroy the Vega Thread object
   * 
   */
  virtual ~VegaThread() override;
  /**
   * @brief Initialize the vega thread.
   * 
   */
  void initialize() const;
  /**
   * @brief Start the tracking.
   * 
   */
  void startTracking() const;
  /**
   * @brief Stop the tracking.
   * 
   */
  void stopTracking() const;
  /**
   * @brief Get the Api Revision information.
   * 
   * @return The Api revision information.
   */
  QString getApiRevision() const;
  bool isConnected() const;
  /**
   * @brief Get the Tool Data object of the No.i handle.
   * 
   * @param i 
   * @return ToolData 
   */
  ToolData getToolData(unsigned short handle);
  PortHandleInfo getPortHandleInfo(unsigned short handle);
  const QList<unsigned short> getHandles();
  void getMatrix4x4(unsigned short handle, vtkMatrix4x4 *matrix);
  void setReferenceHandle(unsigned short handle = 0);
  unsigned short getReferenceHandle() const;
  void getReferenceMatrix4x4(unsigned short handle, vtkMatrix4x4 *matrix);
  unsigned short partNumberToHandle(int partNumber);
  int handleToPartNumber(unsigned short handle);

Q_SIGNALS:
  void trackerUpdated();

private:
  Vega *vega; ///< The worker object to do communication with vega through socket.
  unsigned int fps; ///< Frame per second
  unsigned short referenceHandle;
};

#endif //!VEGA_THREAD_H
