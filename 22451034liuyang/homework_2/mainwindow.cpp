#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->sunRotVelSlider->setValue((int)(100*DEFAULT_VEL/MAX_VEL));
    ui->earthSunRatioSlider->setValue((int)(100*DEFAULT_EARTH_SUN_RATIO));
    ui->moonEarthRatioSlider->setValue((int)(100*DEFAULT_MOON_EARTH_RATIO));
    ui->sunLuxSlider->setValue((int)(100*DEFAULT_SUN_LUX/MAX_SUN_LUX));
    ui->sunEarthDistanceSlider->setValue((int)(100*DEFAULT_SUN_EARTH_DISTANCE/MAX_SUN_EARTH_DISTANCE));
    ui->earthMoonDistanceSlider->setValue((int)(100*DEFAULT_EARTH_MOON_DISTANCE/MAX_EARTH_MOON_DISTANCE));
    ui->radiusSlider->setValue((int)(100*DEFAULT_CAMERA_RADIUS/MAX_CAMERA_RADIUS));
    ui->earthRevolveVelSlider->setValue((int)(100*DEFAULT_EARTH_REVOLVE_SUN_ROT_RATIO)/MAX_EARTH_REVOLVE_SUN_ROT_RATIO);
    ui->earthRotVelSlider->setValue((int)(100*DEFAULT_EARTH_ROT_SUN_ROT_RATIO)/MAX_EARTH_ROT_SUN_ROT_RATIO);
    ui->moonRevolveSlider->setValue((int)(100*DEFAULT_MOON_REVOLVE_SUN_ROT_RATIO)/MAX_MOON_REVOLE_SUN_ROT_RATIO);
    ui->moonRotSlider->setValue((int)(100*DEFAULT_MOON_ROT_SUN_ROT_RATIO)/MAX_MOON_ROT_SUN_ROT_RATIO);
    ui->solarTiltSlider->setMaximum(360);
    ui->solarTiltSlider->setValue((int)(360*DEFAULT_SOLAR_TILT/MAX_TILT_DEGREE));
    ui->earthTiltSlider->setMaximum(360);
    ui->earthTiltSlider->setValue((int)(360*DEFAULT_EARTH_OBLIQUITY/MAX_TILT_DEGREE));
    ui->moonEarthTiltSlider->setMaximum(360);
    ui->moonEarthTiltSlider->setValue((int)(360*DEFAULT_MOON_EARTH_OBLIQUITY/MAX_TILT_DEGREE));
    ui->moonTiltSlider->setMaximum(360);
    ui->moonTiltSlider->setValue((int)(360*DEFAULT_MOON_OBLIQUITY/MAX_TILT_DEGREE));
    ui->alphaSlider->setMaximum(360);
    ui->alphaSlider->setValue((int)360*DEFAULT_CAMERA_ALPHA/(2*M_PI));
    ui->thetaSlider->setMaximum(360);
    ui->thetaSlider->setValue((int)360*DEFAULT_CAMERA_THETA/(2*M_PI));
    connect(ui->sunRotVelSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onSunRotVelChange);
    connect(ui->earthSunRatioSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onEarthSunRatioChanged);
    connect(ui->moonEarthRatioSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onMoonEarthRatioChanged);
    connect(ui->sunLuxSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onSunLuxChanged);
    connect(ui->solarTiltSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onSolarAxialTiltChanged);
    connect(ui->earthTiltSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onEarthObliquityChanged);
    connect(ui->moonEarthTiltSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onMoonEarthObliquityChanged);
    connect(ui->moonTiltSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onMoonObliquityChanged);
    connect(ui->earthMoonDistanceSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onEarthMoonDistanceChanged);
    connect(ui->sunEarthDistanceSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onSunEarthDistanceChanged);
    connect(ui->alphaSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onCameraAlphaChanged);
    connect(ui->thetaSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onCameraThetaChanged);
    connect(ui->radiusSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onCameraRadiusChanged);
    connect(ui->earthRevolveVelSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onEarthRevolveSunRotRatioChanged);
    connect(ui->earthRotVelSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onEarthRotSunRotRatioChanged);
    connect(ui->moonRevolveSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onMoonRevolveSunRotRatioChanged);
    connect(ui->moonRotSlider,&QSlider::valueChanged,ui->openGLWidget,&OpenGLWidget::onMoonRotSunRotRatioChanged);
    connect(ui->resetCameraBtn,&QPushButton::clicked,ui->openGLWidget,&OpenGLWidget::onCameraReset);
    connect(ui->resetParamsBtn,&QPushButton::clicked,ui->openGLWidget,&OpenGLWidget::onResetParams);
    connect(ui->openGLWidget,&OpenGLWidget::cameraRested,this,&MainWindow::onCameraReset);
    connect(ui->openGLWidget,&OpenGLWidget::paramsRested,this,&MainWindow::onParamsReset);
    ui->statusbar->showMessage(" Tips: 点击图像后WASD或者方向键控制相机上下左右移动\n"
                               "QE控制相机前后移动\n"
                               "鼠标按到天体上显示天体名称—— by liuyang :)");
    this->setWindowTitle("另一个太阳系");
    this->setWindowIcon(QIcon(":/images/icon.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCameraReset(){
    ui->alphaSlider->setValue((int)360*DEFAULT_CAMERA_ALPHA/(2*M_PI));
    ui->thetaSlider->setValue((int)360*DEFAULT_CAMERA_THETA/(2*M_PI));
    ui->radiusSlider->setValue((int)(100*DEFAULT_CAMERA_RADIUS/MAX_CAMERA_RADIUS));
}

void MainWindow::onParamsReset(){
    ui->sunRotVelSlider->setValue((int)(100*DEFAULT_VEL/MAX_VEL));
    ui->earthSunRatioSlider->setValue((int)(100*DEFAULT_EARTH_SUN_RATIO));
    ui->moonEarthRatioSlider->setValue((int)(100*DEFAULT_MOON_EARTH_RATIO));
    ui->sunLuxSlider->setValue((int)(100*DEFAULT_SUN_LUX/MAX_SUN_LUX));
    ui->sunEarthDistanceSlider->setValue((int)(100*DEFAULT_SUN_EARTH_DISTANCE/MAX_SUN_EARTH_DISTANCE));
    ui->earthMoonDistanceSlider->setValue((int)(100*DEFAULT_EARTH_MOON_DISTANCE/MAX_EARTH_MOON_DISTANCE));
    ui->earthRevolveVelSlider->setValue((int)(100*DEFAULT_EARTH_REVOLVE_SUN_ROT_RATIO)/MAX_EARTH_REVOLVE_SUN_ROT_RATIO);
    ui->earthRotVelSlider->setValue((int)(100*DEFAULT_EARTH_ROT_SUN_ROT_RATIO)/MAX_EARTH_ROT_SUN_ROT_RATIO);
    ui->moonRevolveSlider->setValue((int)(100*DEFAULT_MOON_REVOLVE_SUN_ROT_RATIO)/MAX_MOON_REVOLE_SUN_ROT_RATIO);
    ui->moonRotSlider->setValue((int)(100*DEFAULT_MOON_ROT_SUN_ROT_RATIO)/MAX_MOON_ROT_SUN_ROT_RATIO);
    ui->solarTiltSlider->setValue((int)(360*DEFAULT_SOLAR_TILT/MAX_TILT_DEGREE));
    ui->earthTiltSlider->setValue((int)(360*DEFAULT_EARTH_OBLIQUITY/MAX_TILT_DEGREE));
    ui->moonEarthTiltSlider->setValue((int)(360*DEFAULT_MOON_EARTH_OBLIQUITY/MAX_TILT_DEGREE));
    ui->moonTiltSlider->setValue((int)(360*DEFAULT_MOON_OBLIQUITY/MAX_TILT_DEGREE));
}
