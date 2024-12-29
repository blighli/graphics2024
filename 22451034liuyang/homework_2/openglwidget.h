#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QVector3D>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QWheelEvent>
#include <qmath.h>

#define MAX_VEL 4.0f
#define DEFAULT_VEL 0.5f
#define DEFAULT_EARTH_SUN_RATIO 0.3f
#define DEFAULT_MOON_EARTH_RATIO 0.2f
#define MAX_TILT_DEGREE 360.0f
#define DEFAULT_SOLAR_TILT 7.25f
#define DEFAULT_EARTH_OBLIQUITY 23.5f
#define DEFAULT_MOON_EARTH_OBLIQUITY 5.145f
#define DEFAULT_MOON_OBLIQUITY 1.5f
#define MAX_SUN_EARTH_DISTANCE  9.5f
#define DEFAULT_SUN_EARTH_DISTANCE 4.0f
#define MAX_EARTH_MOON_DISTANCE  4.5f
#define DEFAULT_EARTH_MOON_DISTANCE 1.0f
#define SKY_SCALE 20.0f
#define MAX_SUN_LUX 30.0f
#define DEFAULT_SUN_LUX 6.0f
#define EMISSIVE_RATIO 0.1333f
#define DEFAULT_CAMERA_ALPHA 0.0f
#define DEFAULT_CAMERA_THETA 6*M_PI/7
#define MAX_CAMERA_RADIUS 19.5f
#define DEFAULT_CAMERA_RADIUS 10.0f

#define MAX_EARTH_REVOLVE_SUN_ROT_RATIO 3.0F
#define DEFAULT_EARTH_REVOLVE_SUN_ROT_RATIO 0.0705F
#define MAX_EARTH_ROT_SUN_ROT_RATIO 50.0F
#define DEFAULT_EARTH_ROT_SUN_ROT_RATIO 25.0F
#define MAX_MOON_REVOLE_SUN_ROT_RATIO 10.0F
#define DEFAULT_MOON_REVOLVE_SUN_ROT_RATIO  0.926F
#define MAX_MOON_ROT_SUN_ROT_RATIO 10.0F
#define DEFAULT_MOON_ROT_SUN_ROT_RATIO  0.926F

#define CAMERA_VEL 0.09F
#define MAX_CAMERA_FOV  180.0F
#define DEFAULT_CAMERA_FOV 45.0F
#define MIN_CAMERA_FOV  1.0F
#define CAMERA_FOV_VEL  1.0F


class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();
protected:
    void initializeGL() override; // 初始化OpenGL
    void paintGL() override;     // OpenGL绘制
    void resizeGL(int w, int h) override; // 调整视口大小
    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
private:
    void updateCamera();
    void loadTextures();
    void generateSphere(float radius, int numLatitudes, int numLongitudes);
    QVector<float> vertices;
    QVector<unsigned int> indices;
    QVector<QVector3D> spherePositions;
    QOpenGLShaderProgram sphereShaderProgram;
    QOpenGLShaderProgram sunShaderProgram;
    QOpenGLShaderProgram pickShaderProgram;
    QOpenGLBuffer VBO;
    QOpenGLBuffer IBO;
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer sunVBO;
    QOpenGLBuffer sunIBO;
    QOpenGLVertexArrayObject sunVAO;
    QOpenGLTexture * earth_tex;
    QOpenGLTexture * sun_tex;
    QOpenGLTexture * moon_tex;
    QOpenGLTexture * sky_tex;
    float camera_alpha=DEFAULT_CAMERA_ALPHA;
    float camera_theta=DEFAULT_CAMERA_THETA;
    float camera_radius=DEFAULT_CAMERA_RADIUS;
    float camera_fov=DEFAULT_CAMERA_FOV;
    QVector3D cameraPos;
    QVector3D cameraTarget;
    QVector3D cameraDirection;
    QVector3D cameraRight;
    QVector3D cameraUp;
    float sun_rotation_angular_velocity=DEFAULT_VEL; // 太阳自转速度
    float earth_revolution_sun_rotation_ratio=DEFAULT_EARTH_REVOLVE_SUN_ROT_RATIO; // 地球公转速度与太阳自转速度比值
    float earth_rotation_sun_rotation_ratio=DEFAULT_EARTH_ROT_SUN_ROT_RATIO; // 地球自转与太阳自转速度比值
    float moon_revolution_sun_rotation_ratio=DEFAULT_MOON_REVOLVE_SUN_ROT_RATIO; // 月球公转与太阳自转速度比值
    float moon_rotation_sun_rotation_ratio=DEFAULT_MOON_ROT_SUN_ROT_RATIO; // 月球自转与太阳自转速度比值
    float solar_axial_tilt=DEFAULT_SOLAR_TILT; // 地球公转轴与太阳自转轴的夹角
    float earth_obliquity_of_the_ecliptic=DEFAULT_EARTH_OBLIQUITY; //黄道夹角
    float moon_earth_obliquity_of_the_ecliptic=DEFAULT_MOON_EARTH_OBLIQUITY; // 月球公转与地球赤道夹角
    float moon_obliquity_of_the_ecliptic=DEFAULT_MOON_OBLIQUITY; // 月球自转与公转平面夹角
    float sun_earth_distance=DEFAULT_SUN_EARTH_DISTANCE; // 日地距离
    float earth_sun_radius_ratio=DEFAULT_EARTH_SUN_RATIO; // 地球太阳大小比例
    float moon_earth_radius_ratio=DEFAULT_MOON_EARTH_RATIO; // 月球地球大小比例
    float earth_moon_distance=DEFAULT_EARTH_MOON_DISTANCE; // 地月距离
    float sun_lux=DEFAULT_SUN_LUX; // 太阳光强（照射）
    QMatrix4x4 projection;
    QMatrix4x4 view;
    QMatrix4x4 sun_model;
    QMatrix4x4 earth_model;
    QMatrix4x4 moon_model;
    QMatrix4x4 sky_model;
    GLuint pickFBO;
    GLuint pickRenderBuffer;
    enum{
        SUN_CODE=1,
        EARTH_CODE=2,
        MOON_CODE=3
    };
public slots:
    void onSunRotVelChange(int);
    void onEarthSunRatioChanged(int);
    void onMoonEarthRatioChanged(int);
    void onSolarAxialTiltChanged(int);
    void onEarthObliquityChanged(int);
    void onMoonEarthObliquityChanged(int);
    void onMoonObliquityChanged(int);
    void onSunEarthDistanceChanged(int);
    void onEarthMoonDistanceChanged(int);
    void onSunLuxChanged(int);
    void onCameraAlphaChanged(int);
    void onCameraThetaChanged(int);
    void onCameraRadiusChanged(int);
    void onEarthRevolveSunRotRatioChanged(int);
    void onEarthRotSunRotRatioChanged(int);
    void onMoonRevolveSunRotRatioChanged(int);
    void onMoonRotSunRotRatioChanged(int);
    void onCameraReset();
    void onResetParams();
signals:
    void cameraRested();
    void paramsRested();
};

#endif // OPENGLWIDGET_H
