#include "Window.h"

#include <vector>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>

#include <QDockWidget>
#include <QGroupBox>
#include <QButtonGroup>
#include <QMenuBar>
#include <QApplication>
#include <QLabel>
#include <QProgressBar>
#include <QCheckBox>
#include <QRadioButton>
#include <QColorDialog>
#include <QLCDNumber>
#include <QPixmap>
#include <QFrame>
#include <QSplitter>
#include <QMenu>
#include <QScrollArea>
#include <QCoreApplication>
#include <QFont>
#include <QSizePolicy>
#include <QImageReader>
#include <QStatusBar>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>

#include "RayTracer.h"
#include "Scene.h"

using namespace std;


Window::Window () : QMainWindow (NULL) {
    try {
        viewer = new GLViewer;
    } catch (GLViewer::Exception e) {
        cerr << e.getMessage () << endl;
        exit (1);
    }
    QGroupBox * renderingGroupBox = new QGroupBox (this);
    QHBoxLayout * renderingLayout = new QHBoxLayout (renderingGroupBox);
    
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole (QPalette::Base);
    imageLabel->setSizePolicy (QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents (true);
    imageLabel->setPixmap (QPixmap::fromImage (rayImage));
    
    renderingLayout->addWidget (viewer);
    renderingLayout->addWidget (imageLabel);

    setCentralWidget (renderingGroupBox);
    
    QDockWidget * controlDockWidget = new QDockWidget (this);
    initControlWidget ();
    
    controlDockWidget->setWidget (controlWidget);
    controlDockWidget->adjustSize ();
    addDockWidget (Qt::RightDockWidgetArea, controlDockWidget);
    controlDockWidget->setFeatures (QDockWidget::AllDockWidgetFeatures);

    lightChoice = POINT_LIGHT;
    flags = NONE;

    setMinimumWidth (800);
    setMinimumHeight (400);
}

Window::~Window () {

}

void Window::displayTime (QTime time)
{
	int length = time.elapsed();
	int minutes = length / (60*1000);
	int seconds = (length - (minutes * (60*1000)))/1000;
	int milliseconds = length - (minutes * (60*1000)) - (seconds*1000);
	QVariant min = QVariant(minutes);
	QVariant sec = QVariant(seconds);
	QVariant milli = QVariant(milliseconds);
	emit updateTime("       "+min.toString()+" min "+sec.toString()+" s "+milli.toString());
}

void Window::enableAO (bool ao)
{
    ao? flags |= ENABLE_AO : flags &= ~ENABLE_AO;
}

void Window::updateAO(int value) 
{
    RayTracer::AO_RAY_COUNT = value;
}

void Window::enableAA (bool aa)
{
    aa? flags |= ENABLE_AA : flags &= ~ENABLE_AA;
}

void Window::updateAA(int value)
{
    RayTracer::ANTIALIASING_RES = value;
}

void Window::updateLights (int type)
{
        lightChoice = type;
}

void Window::renderRayImage () {
    qglviewer::Camera * cam = viewer->camera ();
    RayTracer * rayTracer = RayTracer::getInstance ();
    connect(rayTracer,SIGNAL(renderDone(QTime)),this,SLOT(displayTime(QTime)));
    qglviewer::Vec p = cam->position ();
    qglviewer::Vec d = cam->viewDirection ();
    qglviewer::Vec u = cam->upVector ();
    qglviewer::Vec r = cam->rightVector ();
    Vec3Df camPos (p[0], p[1], p[2]);
    Vec3Df viewDirection (d[0], d[1], d[2]);
    Vec3Df upVector (u[0], u[1], u[2]);
    Vec3Df rightVector (r[0], r[1], r[2]);
    float fieldOfView = cam->horizontalFieldOfView ();
    float aspectRatio = cam->aspectRatio ();
    unsigned int screenWidth = cam->screenWidth ();
    unsigned int screenHeight = cam->screenHeight ();
    rayImage = rayTracer->render (camPos, viewDirection, upVector, rightVector,
                              fieldOfView, aspectRatio, screenWidth, screenHeight, lightChoice, flags);
    imageLabel->setPixmap (QPixmap::fromImage (rayImage));
    
}

void Window::setBGColor () {
    QColor c = QColorDialog::getColor (QColor (133, 152, 181), this);
    if (c.isValid () == true) {
        cout << c.red () << endl;
        RayTracer::getInstance ()->setBackgroundColor (Vec3Df (c.red (), c.green (), c.blue ()));
        viewer->setBackgroundColor (c);
        viewer->updateGL ();
    }
}

void Window::exportGLImage () {
    viewer->saveSnapshot (false, false);
}

void Window::exportRayImage () {
    QString filename = QFileDialog::getSaveFileName (this,
                                                     "Save ray-traced image",
                                                     ".",
                                                     "*.jpg *.bmp *.png");
    if (!filename.isNull () && !filename.isEmpty ()) {
        rayImage.save (filename);
    }
}

void Window::about () {
    
    QMessageBox::about (this, 
                        "About This Program", 
                        "<b>RayMini</b> <br> by <i>Daniel Ross / Arnaud Douceur</i>.");
}

void Window::initControlWidget () {
    controlWidget = new QGroupBox ();
    QVBoxLayout * layout = new QVBoxLayout (controlWidget);

    /** PREVIEW BOX
     */

    QGroupBox * previewGroupBox = new QGroupBox ("Preview", controlWidget);
    QVBoxLayout * previewLayout = new QVBoxLayout (previewGroupBox);
    
    QCheckBox * wireframeCheckBox = new QCheckBox ("Wireframe", previewGroupBox);
    connect (wireframeCheckBox, SIGNAL (toggled (bool)), viewer, SLOT (setWireframe (bool)));
    previewLayout->addWidget (wireframeCheckBox);
   
    QButtonGroup * modeButtonGroup = new QButtonGroup (previewGroupBox);
    modeButtonGroup->setExclusive (true);
    QRadioButton * flatButton = new QRadioButton ("Flat", previewGroupBox);
    QRadioButton * smoothButton = new QRadioButton ("Smooth", previewGroupBox);
    modeButtonGroup->addButton (flatButton, static_cast<int>(GLViewer::Flat));
    modeButtonGroup->addButton (smoothButton, static_cast<int>(GLViewer::Smooth));
    connect (modeButtonGroup, SIGNAL (buttonClicked (int)), viewer, SLOT (setRenderingMode (int)));
    previewLayout->addWidget (flatButton);
    previewLayout->addWidget (smoothButton);
    
    QPushButton * snapshotButton  = new QPushButton ("Save preview", previewGroupBox);
    connect (snapshotButton, SIGNAL (clicked ()) , this, SLOT (exportGLImage ()));
    previewLayout->addWidget (snapshotButton);

    layout->addWidget (previewGroupBox);

    /*************************************************************/

    /** RENDER BOX
     */

    QGroupBox * rayGroupBox = new QGroupBox ("Ray Tracing", controlWidget);
    QVBoxLayout * rayLayout = new QVBoxLayout (rayGroupBox);

    /** Render options
     */
    QCheckBox * AOCheckBox = new QCheckBox ("Ambient Occlusion", rayGroupBox);
    connect (AOCheckBox, SIGNAL (toggled (bool)), this, SLOT (enableAO (bool)));
    rayLayout->addWidget (AOCheckBox);
    QSlider* AOSlider = new QSlider(Qt::Horizontal);
	rayLayout->addWidget(AOSlider);
    connect (AOCheckBox, SIGNAL (toggled(bool)), AOSlider, SLOT(setVisible(bool)));
    AOSlider->setRange(0,64);
	AOSlider->setVisible(false);
    connect (AOSlider, SIGNAL (valueChanged(int)), this, SLOT(updateAO(int)));


    QCheckBox * AACheckBox = new QCheckBox ("Anti Aliasing", rayGroupBox);
    connect (AACheckBox, SIGNAL (toggled (bool)), this, SLOT (enableAA (bool)));
    rayLayout->addWidget (AACheckBox);
    QSlider* AASlider = new QSlider(Qt::Horizontal);
	rayLayout->addWidget(AASlider);
    connect (AACheckBox, SIGNAL (toggled(bool)), AASlider, SLOT(setVisible(bool)));
	AASlider->setRange(1,8);
	AASlider->setVisible(false);
    connect (AASlider, SIGNAL (valueChanged(int)), this, SLOT(updateAA(int)));

    
     /** Render Progress Bar
     */
    QLabel * renderBarLabel = new QLabel("Rendering progress bar: ") ;
	rayLayout->addWidget(renderBarLabel);
    renderBar = new QProgressBar (rayGroupBox);
    rayLayout->addWidget (renderBar);
    connect (RayTracer::getInstance(), SIGNAL(init(int, int)), renderBar, SLOT(setRange(int,int)));
    connect (RayTracer::getInstance(), SIGNAL(progress(int)), renderBar, SLOT(setValue(int)));
    
    /** Render Time Label
     */
    QLabel * timeLabel = new QLabel("      Rendering time: ");
	timeDisplayLabel = new QLabel("          0 seconds");
	rayLayout->addWidget(timeLabel);
	rayLayout->addWidget(timeDisplayLabel);	
	connect(this,SIGNAL(updateTime(QString)),timeDisplayLabel,SLOT(setText(QString)));

     /** Render Button
     */
    QPushButton * rayButton = new QPushButton ("Render", rayGroupBox);
    rayLayout->addWidget (rayButton);
    connect (rayButton, SIGNAL (clicked ()), this, SLOT (renderRayImage ()));

    /** Save Button
     */
    QPushButton * saveButton  = new QPushButton ("Save", rayGroupBox);
    connect (saveButton, SIGNAL (clicked ()) , this, SLOT (exportRayImage ()));
    rayLayout->addWidget (saveButton);

    layout->addWidget (rayGroupBox);

    /*************************************************************/

    /** GLOBAL BOX
     */

    QGroupBox * globalGroupBox = new QGroupBox ("Global Settings", controlWidget);
    QVBoxLayout * globalLayout = new QVBoxLayout (globalGroupBox);
    
    /**
     * Lights
     */
	QLabel* lightsLabel = new QLabel("Lights");
    QComboBox* lightsComboBox = new QComboBox;
	lightsComboBox->addItem("Point Light");
	lightsComboBox->addItem("Area Light");
	globalLayout->addWidget(lightsLabel);
	globalLayout->addWidget(lightsComboBox);
	connect(lightsComboBox, SIGNAL(activated(int)),this, SLOT(updateLights(int))); 

    QPushButton * bgColorButton  = new QPushButton ("Background Color", globalGroupBox);
    connect (bgColorButton, SIGNAL (clicked()) , this, SLOT (setBGColor()));
    globalLayout->addWidget (bgColorButton);
    
    QPushButton * aboutButton  = new QPushButton ("About", globalGroupBox);
    connect (aboutButton, SIGNAL (clicked()) , this, SLOT (about()));
    globalLayout->addWidget (aboutButton);
    
    QPushButton * quitButton  = new QPushButton ("Quit", globalGroupBox);
    connect (quitButton, SIGNAL (clicked()) , qApp, SLOT (closeAllWindows()));
    globalLayout->addWidget (quitButton);

    layout->addWidget (globalGroupBox);

    /*************************************************************/
    

    layout->addStretch (0);
}
