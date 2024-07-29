/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "Mitk_AimooeToolCoordinateTransform.h"
#include <mitkBaseRenderer.h>
#include <mitkGeometry3D.h>
#include <mitkPlaneGeometry.h>
#include <mitkVector.h>
#include <mitkLine.h>
#include <mitkColorProperty.h>
#include <mitkPointSet.h>
#include <mitkDataStorage.h>
#include <mitkCommon.h>
#include <mitkAnnotation.h>
#include <mitkTextAnnotation3D.h>
#include <mitkRenderingManager.h>
#include <mitkVtkPropRenderer.h>
#include <mitkIOUtil.h>
#include <vtkMatrix4x4.h>
#include <mitkMatrixConvert.h>
#include <mitkBaseGeometry.h>


//VTK
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLineSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkConeSource.h>
#include <vtkAppendPolyData.h>
#include <vtkCylinderSource.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

// Qt
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QInputDialog>
#include <Qtimer>
#include <QMessageBox>
#include <QInputDialog>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QLineEdit>
#include <Qstring>


// mitk image
#include <mitkImage.h>
#include <QmitkStdMultiWidget.h>
#include <mitkDataStorage.h>

//C++
#include <algorithm>
#include <fstream>

const std::string Mitk_AimooeToolCoordinateTransform::VIEW_ID = "org.mitk.views.mitk_aimooetoolcoordinatetransform";

void Mitk_AimooeToolCoordinateTransform::SetFocus()
{
  
}

void Mitk_AimooeToolCoordinateTransform::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);

  connect(m_Controls.Openfile_PB, &QPushButton::clicked, this, &Mitk_AimooeToolCoordinateTransform::OpenFile);

  connect(m_Controls.show_points_pB, &QPushButton::clicked, this, &Mitk_AimooeToolCoordinateTransform::ShowPoints);

  connect(m_Controls.compute_pB, &QPushButton::clicked, this, &Mitk_AimooeToolCoordinateTransform::ComputeTransform);
  connect(m_Controls.apply_pB, &QPushButton::clicked, this, &Mitk_AimooeToolCoordinateTransform::ApplyTransform);
  connect(m_Controls.pushButton, &QPushButton::clicked, this, &Mitk_AimooeToolCoordinateTransform::Undo);

  connect(m_Controls.save_new_pB, &QPushButton::clicked, this, &Mitk_AimooeToolCoordinateTransform::SaveAtNewFile);
  connect(m_Controls.save_pB, &QPushButton::clicked, this, &Mitk_AimooeToolCoordinateTransform::SaveAtOldFile);

  connect(m_Controls.add_second_pB_3, &QPushButton::clicked, this, &Mitk_AimooeToolCoordinateTransform::SetSecond);
 
  connect(m_Controls.primer_axes_cb, QOverload<const QString&>::of(&QComboBox::activated), this, &Mitk_AimooeToolCoordinateTransform::ChangePrimerLabel);
  connect(m_Controls.primer_axes_cb, QOverload<const QString&>::of(&QComboBox::activated), this, &Mitk_AimooeToolCoordinateTransform::ChangeSeconderyCB);
  connect(m_Controls.secondery_axes_cb, QOverload<const QString&>::of(&QComboBox::activated), this, &Mitk_AimooeToolCoordinateTransform::ChangeSeconderyLabel);

}

void Mitk_AimooeToolCoordinateTransform::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{
 
}


//open file ,read file
void Mitk_AimooeToolCoordinateTransform::OpenFile() {

    // open a dialog choose dir
    filePath = QFileDialog::getOpenFileName(nullptr, "Choose your file", QDir::homePath(), "(*.aimtool)");

    if (!filePath.isEmpty()) {
        // open the file
       QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            MITK_INFO << "not can to open this file." << file.errorString(); 
            return;
        }
        else {
            file.close();
            MITK_INFO << "open the file succsess.";
        }
    }
    else {
        MITK_INFO << "you must to choose one file.";
        return;
    }

    Readfileinfo = true;

    //read file info
    ReadFIleinfo();

}

void Mitk_AimooeToolCoordinateTransform::ReadFIleinfo() {

    if (!Readfileinfo) {
        MITK_INFO << "Please perform the previous operation.";
        return;
    }

    QFile file(filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream in(&file);

    int lineCount = 0;
    
    for (int i = 0; i < PositionStart; ++i) {
        QString line = in.readLine();

        if (lineCount == PositionNumRows) {
            PointNum = line.toInt();//jump the front/top two rows and get the point num
        }

        lineCount ++;
    }

    pointSet = mitk::PointSet::New();
    pointSet->Clear();
    
    // go to read point info
    for (int j = 0; j < PointNum; ++j) {

        if (!in.atEnd()) {
            QString line2 = in.readLine();
            QStringList parts = line2.split(" ", QString::SkipEmptyParts);

            mitk::Point3D point;
            for (int k = 0; k < 3; ++k) {

                point[k] = parts[k].toDouble();

            }
            pointSet->InsertPoint(j, point);        

            MITK_INFO << "load point data....";
            MITK_INFO << point;
            lineCount++;
        }
    }

    file.close();
    MITK_INFO<< "the point num: "<< PointNum;

    //flag
    ShowPoint = true;
    Compute = true;
   
}

// show our points
void Mitk_AimooeToolCoordinateTransform::ShowPoints() {

    if (!ShowPoint) {
        MITK_INFO << "Please perform the previous operation.";
        return;
    }
   
    // set only 3D window can visibility
    GetDataStorage()->GetNamedNode("stdmulti.widget0.plane")->SetVisibility(false);
    GetDataStorage()->GetNamedNode("stdmulti.widget1.plane")->SetVisibility(false);
    GetDataStorage()->GetNamedNode("stdmulti.widget2.plane")->SetVisibility(false);

    // remove two  DataNode
    if (pointSetNode.IsNotNull()||AxesNode.IsNotNull()) {
        GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("PointSet"));
        GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Axes"));
    }
    else {
        MITK_INFO << "no node to remove";
    }


    
    pointSetNode = mitk::DataNode::New();
    
    //set point size
    float newPointSize = 10.0; // Example size
    pointSet->SetProperty("pointsize", mitk::FloatProperty::New(newPointSize));

    pointSetNode->SetData(pointSet);
    pointSetNode->SetName("PointSet");

    // already inut point interaction
    GetDataStorage()->Add(pointSetNode);

    //add a coordinate
    LoadCoordinate();
   
    //ShowOne = false;
}


void Mitk_AimooeToolCoordinateTransform::LoadCoordinate() {

    //initial Axes surface and AxesNode
    Axes = mitk::Surface::New();
    AxesNode = mitk::DataNode::New();

    //the axis length
    double axisLength = 5.0;

    //the Axes element initial
    vtkSmartPointer<vtkSphereSource> vtkSphere = vtkSmartPointer<vtkSphereSource>::New();
    vtkSmartPointer<vtkConeSource> vtkCone = vtkSmartPointer<vtkConeSource>::New();
    vtkSmartPointer<vtkCylinderSource> vtkCylinder = vtkSmartPointer<vtkCylinderSource>::New();
    vtkSmartPointer<vtkPolyData> axis = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkLineSource> vtkLine = vtkSmartPointer<vtkLineSource>::New();
    vtkSmartPointer<vtkLineSource> vtkLine2 = vtkSmartPointer<vtkLineSource>::New();
    vtkSmartPointer<vtkLineSource> vtkLine3 = vtkSmartPointer<vtkLineSource>::New();
    vtkSmartPointer<vtkAppendPolyData> appendPolyData = vtkSmartPointer<vtkAppendPolyData>::New();
    vtkSmartPointer<vtkPolyData> surface = vtkSmartPointer<vtkPolyData>::New();

    //Y-Axis (start with y, cause cylinder is oriented in y by vtk default...)
    vtkCone->SetDirection(0, 1, 0);
    vtkCone->SetHeight(1.0);
    vtkCone->SetRadius(0.4f);
    vtkCone->SetResolution(16);
    vtkCone->SetCenter(0.0, axisLength, 0.0);
    vtkCone->Update();

    vtkCylinder->SetRadius(0.05);
    vtkCylinder->SetHeight(axisLength);
    vtkCylinder->SetCenter(0.0, 0.5 * axisLength, 0.0);
    vtkCylinder->Update();

    appendPolyData->AddInputData(vtkCone->GetOutput());
    appendPolyData->AddInputData(vtkCylinder->GetOutput());
    appendPolyData->Update();
    axis->DeepCopy(appendPolyData->GetOutput());

    //y symbol
    vtkLine->SetPoint1(-0.5, axisLength + 2., 0.0);
    vtkLine->SetPoint2(0.0, axisLength + 1.5, 0.0);
    vtkLine->Update();

    vtkLine2->SetPoint1(0.5, axisLength + 2., 0.0);
    vtkLine2->SetPoint2(-0.5, axisLength + 1., 0.0);
    vtkLine2->Update();

    appendPolyData->AddInputData(vtkLine->GetOutput());
    appendPolyData->AddInputData(vtkLine2->GetOutput());
    appendPolyData->AddInputData(axis);
    appendPolyData->Update();
    surface->DeepCopy(appendPolyData->GetOutput());

    //X-axis
    vtkSmartPointer<vtkTransform> XTransform = vtkSmartPointer<vtkTransform>::New();
    XTransform->RotateZ(-90);
    vtkSmartPointer<vtkTransformPolyDataFilter> TrafoFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    TrafoFilter->SetTransform(XTransform);
    TrafoFilter->SetInputData(axis);
    TrafoFilter->Update();

    //x symbol
    vtkLine->SetPoint1(axisLength + 2., -0.5, 0.0);
    vtkLine->SetPoint2(axisLength + 1., 0.5, 0.0);
    vtkLine->Update();

    vtkLine2->SetPoint1(axisLength + 2., 0.5, 0.0);
    vtkLine2->SetPoint2(axisLength + 1., -0.5, 0.0);
    vtkLine2->Update();

    appendPolyData->AddInputData(vtkLine->GetOutput());
    appendPolyData->AddInputData(vtkLine2->GetOutput());
    appendPolyData->AddInputData(TrafoFilter->GetOutput());
    appendPolyData->AddInputData(surface);
    appendPolyData->Update();
    surface->DeepCopy(appendPolyData->GetOutput());

    //Z-axis
    vtkSmartPointer<vtkTransform> ZTransform = vtkSmartPointer<vtkTransform>::New();
    ZTransform->RotateX(90);
    TrafoFilter->SetTransform(ZTransform);
    TrafoFilter->SetInputData(axis);
    TrafoFilter->Update();

    //z symbol
    vtkLine->SetPoint1(-0.5, 0.0, axisLength + 2.);
    vtkLine->SetPoint2(0.5, 0.0, axisLength + 2.);
    vtkLine->Update();

    vtkLine2->SetPoint1(-0.5, 0.0, axisLength + 2.);
    vtkLine2->SetPoint2(0.5, 0.0, axisLength + 1.);
    vtkLine2->Update();

    vtkLine3->SetPoint1(0.5, 0.0, axisLength + 1.);
    vtkLine3->SetPoint2(-0.5, 0.0, axisLength + 1.);
    vtkLine3->Update();

    appendPolyData->AddInputData(vtkLine->GetOutput());
    appendPolyData->AddInputData(vtkLine2->GetOutput());
    appendPolyData->AddInputData(vtkLine3->GetOutput());
    appendPolyData->AddInputData(TrafoFilter->GetOutput());
    appendPolyData->AddInputData(surface);
    appendPolyData->Update();
    surface->DeepCopy(appendPolyData->GetOutput());

    //Center
    vtkSphere->SetRadius(0.2f);
    vtkSphere->SetCenter(0.0, 0.0, 0.0);
    vtkSphere->Update();

    appendPolyData->AddInputData(vtkSphere->GetOutput());
    appendPolyData->AddInputData(surface);
    appendPolyData->Update();
    surface->DeepCopy(appendPolyData->GetOutput());

    //Scale
    vtkSmartPointer<vtkTransform> ScaleTransform = vtkSmartPointer<vtkTransform>::New();
    ScaleTransform->Scale(20., 20., 20.);

    TrafoFilter->SetTransform(ScaleTransform);
    TrafoFilter->SetInputData(surface);
    TrafoFilter->Update();

    Axes->SetVtkPolyData(TrafoFilter->GetOutput());

    // get surface data in node
    if (Axes.IsNotNull()){  
    
        AxesNode->SetData(Axes);
        AxesNode->SetName("Axes");
        GetDataStorage()->Add(AxesNode);
}
    else{
        MITK_INFO << "Axes not create " << filePath;
    }
 
}

// Draw a line
void Mitk_AimooeToolCoordinateTransform::SetSecond() {

    origin_point_index = (m_Controls.origin_lE->text()).toInt();
    primer_axis_point_index = (m_Controls.primer_point_lE->text()).toInt();

    

    if ((origin_point_index == primer_axis_point_index) || (origin_point_index < 0) || (primer_axis_point_index < 0)) {

        MITK_INFO << "please put in proper point";
        return;
    }

    int n = pointSet->GetSize();
    if ((origin_point_index >= n) || (primer_axis_point_index >= n)) {
        MITK_INFO << "please put in proper point";
        return;
    }


    Eigen::Vector3d origin(pointSet->GetPoint(origin_point_index).GetDataPointer());
    Eigen::Vector3d p(pointSet->GetPoint(primer_axis_point_index).GetDataPointer());

    double point1[3] = { origin(0),origin(1),origin(2)};
    double point2[3] = { p(0),p(1),p(2) };
    //draw a line
    vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
    lineSource->SetPoint1(point1);
    lineSource->SetPoint2(point2);
    lineSource->Update();

    // from VTK data invert to  MITK Surface data
    linesurface = mitk::Surface::New();
    linesurface->SetVtkPolyData(lineSource->GetOutput());

    // if linenode is notNull ,remove it
    if (linenode.IsNotNull())
    {
        GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("primer axis"));
    }

    //create new line data node
    linenode = mitk::DataNode::New();
    linenode->SetData(nullptr); //clear  the same as remove
    linenode->SetData(linesurface);
    linenode->SetName("primer axis");

    //set color for linenode ,you can try another color
    //mitk::ColorProperty::Pointer colorProperty = mitk::ColorProperty::New((1.0, 0.0, 0.0));
    //linenode->SetProperty("color", colorProperty);

    GetDataStorage()->Add(linenode);
}

void Mitk_AimooeToolCoordinateTransform::ComputeTransform() {
    //get primer_axis and secondery_axis
    QString primer_axis_str = m_Controls.primer_axes_cb->currentText();
    QString secondery_axis_str = m_Controls.secondery_axes_cb->currentText();

    //get three point to ensure coordinate
    origin_point_index = (m_Controls.origin_lE->text()).toInt();
    primer_axis_point_index = (m_Controls.primer_point_lE->text()).toInt();
    quadrant_point_index = (m_Controls.secondery_lE->text()).toInt();
    


    if ( (primer_axis_point_index < 0)||(origin_point_index < 0)|| (quadrant_point_index < 0)
        || (quadrant_point_index == primer_axis_point_index)|| (quadrant_point_index == origin_point_index)
        ||(origin_point_index == primer_axis_point_index)) {

        MITK_INFO << "wrong:not set point or two points are same or point index beyond the range";
        return;
    }

    int n = pointSet->GetSize();
    if ((primer_axis_point_index >= n) || (origin_point_index >= n) || (quadrant_point_index >= n)) {

        MITK_INFO << "wrong:not set point or two points are same or point index beyond the range";
        return;
    }


    Eigen::Vector3d origin(pointSet->GetPoint(origin_point_index).GetDataPointer());
    Eigen::Vector3d p(pointSet->GetPoint(primer_axis_point_index).GetDataPointer());
    Eigen::Vector3d q(pointSet->GetPoint(quadrant_point_index).GetDataPointer());

    axis_0 = p - origin; //primer_axis vector
    if (primer_axis_str.contains("-")) {
        axis_0 = -axis_0;
    }

    axis_0.normalize();

    Eigen::Vector3d tmp = q - origin;
    if (secondery_axis_str.contains("-")) {
        tmp = -tmp;
    }
    tmp.normalize();

    axis_2 = axis_0.cross(tmp);  // cross product is third axes
    axis_2.normalize();

    axis_1 = axis_2.cross(axis_0);
    axis_1.normalize();

    //updata axis_index vector
    if (primer_axis_str.contains("X")) {
        if (secondery_axis_str.contains("Z")) {
            std::swap(axis_index[2], axis_index[1]);
        }
    }
    else if (primer_axis_str.contains("Y")) {
        std::swap(axis_index[0], axis_index[1]);
        if (secondery_axis_str.contains("Z")) {
            std::swap(axis_index[2], axis_index[1]);
        }
    }
    else if (primer_axis_str.contains("Z")) {
        std::swap(axis_index[0], axis_index[2]);
        if (secondery_axis_str.contains("X")) {
            std::swap(axis_index[1], axis_index[2]);
        }
    }

    Transform = Transform.Identity(); //initial

    std::vector<Eigen::Vector3d> axes = { axis_0, axis_1, axis_2 };

    //find x，y，z 
    int x_index = 0;
    int y_index = 0;
    int z_index = 0;

    auto it1 = std::find(axis_index.begin(), axis_index.end(), 0);
    if (it1 != axis_index.end()) {
        x_index = std::distance(axis_index.begin(), it1);
    }
    else {
        MITK_INFO << "not find index!\n";
    }
    auto it2 = std::find(axis_index.begin(), axis_index.end(), 1);
    if (it2 != axis_index.end()) {
        y_index = std::distance(axis_index.begin(), it2);
    }
    else {
        MITK_INFO << "not find index !\n" ;
    }
    auto it3 = std::find(axis_index.begin(), axis_index.end(), 2);
    if (it3 != axis_index.end()) {
        z_index = std::distance(axis_index.begin(), it3);
    }
    else {
        MITK_INFO << "not find index!\n";
    }

    axis_index = { 0,1,2 };//set to fault value

    for (int i = 0; i < Transform.rows() - 1; i++) {

        Transform(i, 0) = axes[x_index][i];
        Transform(i, 1) = axes[y_index][i];
        Transform(i, 2) = axes[z_index][i];
        Transform(i, 3) = origin[i];
    }

    MITK_INFO << "T Matrix:\n" << Transform;

    Apply = true;


    //remove line node
    if (linenode.IsNotNull()) {

        GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("primer axis"));
    }

    Coordinate_Transform(Transform);

}

void Mitk_AimooeToolCoordinateTransform::Coordinate_Transform(Eigen::Matrix4d EigenTransform) {

    if (Axes.IsNull())
    {
        MITK_ERROR << "The DataNode does not contain valid surface data.";
        return;
    }

    // vtkMatrix4x4 invert to mitk::AffineTransform3D
    mitk::AffineTransform3D::Pointer mitkTransform = mitk::AffineTransform3D::New();   
    mitk::AffineTransform3D::MatrixType matrix;
    mitk::AffineTransform3D::OffsetType offset;

    //EIgen to mitk
    for (unsigned int i = 0; i < 3; ++i)
    {
        for (unsigned int j = 0; j < 3; ++j)
        {
            matrix(i, j) = EigenTransform(i, j);
        }
        offset[i] = EigenTransform(i, 3);
    }

    mitkTransform->SetMatrix(matrix);
    mitkTransform->SetOffset(offset);

    Axes->GetGeometry()->SetIndexToWorldTransform(mitkTransform);

    AxesNode->Modified();

    mitk::RenderingManager::GetInstance()->RequestUpdateAll(); //updata display

}

//Undo
void Mitk_AimooeToolCoordinateTransform::Undo() {
      
    Eigen::Matrix4d Transform_inverse = Transform.Identity();//记下原来的姿态即可
    MITK_INFO << "Transform_inverse  Matrinx:\n " << Transform_inverse;

    Coordinate_Transform(Transform_inverse);

    // set back to initial value
    origin_point_index = 0;
    primer_axis_point_index = 0;
    quadrant_point_index = 0;

    Apply = false;
    SaveNew = false;
    SaveOld = false;

    Transform = Transform.Identity();

    axis_index = { 0,1,2 };

}

//appaly Transform
void Mitk_AimooeToolCoordinateTransform::ApplyTransform() {

    if (!Apply){
        MITK_INFO << "Please perform the previous operation.";
        return;
    }

    int n = pointSet->GetSize();

    t_pointSet.clear();//initial to clear output point_Set

    MITK_INFO << "n:" << n;
    for (int i = 0; i < n; ++i){ 

        Eigen::Vector3d p3(pointSet->GetPoint(i).GetDataPointer());

        Eigen::Vector4d point4(p3[0], p3[1], p3[2], 1.0);

        Eigen::Vector4d t_point4 = Transform.inverse() * point4;

        Eigen::Vector3d t_point3(
            t_point4[0] / t_point4[3],
            t_point4[1] / t_point4[3],
            t_point4[2] / t_point4[3]
        );

        t_pointSet.push_back(t_point3);

        mitk::Point3D mitkPoint;
        mitkPoint[0] = t_point3[0];
        mitkPoint[1] = t_point3[1];
        mitkPoint[2] = t_point3[2];
        
        MITK_INFO << mitkPoint;     
    }

    SaveNew = true;
    SaveOld = true;

}

//save after trasform point
void Mitk_AimooeToolCoordinateTransform::SaveAtNewFile() {
    
    if (!SaveNew) {
        MITK_INFO << "Please perform the previous operation.";
        return;
    }

    //open old file and read context
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        MITK_INFO << "not can open file:" << filePath;
        return;
    }

    // read old file content
    QStringList lines;
    lines.clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }

    // check rows 
    int n = PositionStart;  //start rows
    int m = PositionStart + PointNum;  //end rows
    if (PositionStart < 0 || (PositionStart + PointNum) >= lines.size() || n > m) {
        MITK_INFO << "write is failed,please check.";
        file.close();
        return;
    }
    // refine context
    for (int i = n; i < m; ++i) {
        int index = i - n;
        QString refine_context = QString::number(t_pointSet[index][0], 'f', precision) + " "
            + QString::number(t_pointSet[index][1], 'f', precision) + " "
            + QString::number(t_pointSet[index][2], 'f', precision) + " "
            + QString::number((1 << PointNum) - 1);
        lines[i] = refine_context;
        
    }
    //open a dialog to new aimtool file 
    QString NewfileName = QFileDialog::getSaveFileName(nullptr, "Save your .aimtool file", "", ".aimtool (*.aimtool)");


    if (!NewfileName.isEmpty()) {
        QFile file1(NewfileName);
        if (file1.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file1);
            for (const QString& line : lines) {
                out << line << "\n";             
            }
            file1.close();
        }
        else {
            MITK_INFO << "file is save failed...,please check";
        }
    }

}


void Mitk_AimooeToolCoordinateTransform::SaveAtOldFile() {

    if (!SaveOld){
        MITK_INFO << "Please perform the previous operation.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        MITK_INFO << "can not  open  file:" << filePath;
        return;
    }

    // read file content
    QStringList lines;
    lines.clear();//for rewrite a new file 
    QTextStream in(&file);
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }

    // check rows 
    int n = PositionStart;  //start rows
    int m = PositionStart + PointNum;  //end rows
    if (PositionStart < 0 || (PositionStart + PointNum) >= lines.size() || n > m) {
        MITK_INFO << "write is failed,please check file ";
        file.close();
        return;
    }

    // refine point position
    for (int i = n; i < m ; ++i) {
        int index = i - n;
        QString refine_context = QString::number(t_pointSet[index][0] , 'f', precision) + " "
                             + QString::number(t_pointSet[index][1], 'f', precision) + " "
                             + QString::number(t_pointSet[index][2], 'f', precision) + " "
                             + QString::number((1 << PointNum) - 1);
        lines[i] = refine_context;
    }

    // Clear and rewrite context
    file.resize(0);
    QTextStream out(&file);
    for (const QString& line : lines) {
        out << line << "\n";
    }
    file.close();

    MITK_INFO << " save success, check context.";

}

//change ui label or combox
void Mitk_AimooeToolCoordinateTransform::ChangePrimerLabel(const QString& text){

    m_Controls.primer_lB->setText("Point use to " + text +" axis:");
    QString text_1 = m_Controls.secondery_axes_cb->currentText();
    m_Controls.secondery_lB->setText("Point use to " + text + "-" + text_1 + " quadrant:");
}

void Mitk_AimooeToolCoordinateTransform::ChangeSeconderyLabel(const QString& text){

    QString text_1 = m_Controls.primer_axes_cb->currentText();
    m_Controls.secondery_lB->setText("Point use to "+ text_1+"-"+text + " quadrant:");
}

void Mitk_AimooeToolCoordinateTransform::ChangeSeconderyCB(const QString& text){

    m_Controls.secondery_axes_cb->clear();

    m_Controls.secondery_axes_cb->addItem("X+");
    m_Controls.secondery_axes_cb->addItem("Y+");
    m_Controls.secondery_axes_cb->addItem("Z+");
    m_Controls.secondery_axes_cb->addItem("X-");
    m_Controls.secondery_axes_cb->addItem("Y-");
    m_Controls.secondery_axes_cb->addItem("Z-");

    QStringList itemsToRemove;
    if ((text == "X+") || (text == "X-")){
    
        itemsToRemove << "X+" << "X-";
    }
    else if((text == "Y+") || (text == "Y-")){
        itemsToRemove << "Y+" << "Y-";
    }
    else if ((text == "Z+") || (text == "Z-")){
        itemsToRemove << "Z+" << "Z-";
    }

    for (const QString& item : itemsToRemove){
        int itemIndex = m_Controls.secondery_axes_cb->findText(item);
        if (itemIndex != -1) { 
            // if find proper choose
            m_Controls.secondery_axes_cb->removeItem(itemIndex);
        }
    }

}

//std print point   
std::ostream& operator<<(std::ostream& os, const mitk::Point3D& point) {
    os << "Point(x=" << point[0] << ", y=" << point[1] << ", z=" << point[2] << ")";
    return os;
}