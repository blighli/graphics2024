#include "openglwidget.h"
#include "qobjectdefs.h"
#include <QOpenGLFunctions>
#include <qmath.h>
#include <QImage>
#include <QMatrix4x4>
#include <QPainter>
#include <QToolTip>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget{parent},
    VBO(QOpenGLBuffer::VertexBuffer),
    IBO(QOpenGLBuffer::IndexBuffer),
    sunVBO(QOpenGLBuffer::VertexBuffer),
    sunIBO(QOpenGLBuffer::IndexBuffer),
    earth_tex(nullptr),
    sun_tex(nullptr),
    cameraPos(0.0f,0.0f,0.0f),
    cameraTarget(0.0f,0.0f,0.0f),
    cameraDirection(cameraPos-cameraTarget),
    cameraRight(QVector3D::crossProduct({0.0f,1.0f,0.0f},cameraDirection)),
    cameraUp(QVector3D::crossProduct(cameraDirection,cameraRight))
{
    setFocusPolicy(Qt::StrongFocus);
    // setMouseTracking(true);  // 启用鼠标跟踪
}

OpenGLWidget::~OpenGLWidget(){
    if(earth_tex){
        delete earth_tex;
    }
    if(sun_tex){
        delete sun_tex;
    }
    if(moon_tex){
        delete moon_tex;
    }
}

void OpenGLWidget::initializeGL(){
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // 创建球体的顶点数据
    generateSphere(1.0f, 100, 100);  // 半径1，纬线和经线数量100
    // qDebug()<<vertices;
    /**
     * 地球和月球的着色器准备
     */

    // 创建顶点着色器
    QOpenGLShader *vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    if (!vertexShader->compileSourceFile(":shaders/sphere.vert")) {
        qDebug() << "Vertex shader compilation failed:" << vertexShader->log();
        return;
    }

    // 创建片段着色器
    QOpenGLShader *fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    if (!fragmentShader->compileSourceFile(":shaders/sphere.frag")) {
        qDebug() << "Fragment shader compilation failed:" << fragmentShader->log();
        return;
    }
    sphereShaderProgram.addShader(vertexShader);
    sphereShaderProgram.addShader(fragmentShader);
    if(!sphereShaderProgram.link()){                      //链接着色器
        qDebug()<<"Link shader program failed:"<<sphereShaderProgram.log();    //如果链接出错,打印报错信息
    }

    /**
     * 太阳着色器准备
     */
    // 创建顶点着色器
    QOpenGLShader *sunVertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    if (!sunVertexShader->compileSourceFile(":shaders/sun.vert")) {
        qDebug() << "Vertex shader compilation failed:" << sunVertexShader->log();
        return;
    }

    // 创建片段着色器
    QOpenGLShader *sunFragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    if (!sunFragmentShader->compileSourceFile(":shaders/sun.frag")) {
        qDebug() << "Fragment shader compilation failed:" << sunFragmentShader->log();
        return;
    }
    sunShaderProgram.addShader(sunVertexShader);
    sunShaderProgram.addShader(sunFragmentShader);
    if(!sunShaderProgram.link()){                      //链接着色器
        qDebug()<<"Link shader program failed:"<<sunShaderProgram.log();    //如果链接出错,打印报错信息
    }


    /**
     * 物体区分着色器
     */
    // 创建顶点着色器
    QOpenGLShader *pickVertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    if (!pickVertexShader->compileSourceFile(":shaders/pick.vert")) {
        qDebug() << "Vertex shader compilation failed:" << pickVertexShader->log();
        return;
    }

    // 创建片段着色器
    QOpenGLShader *pickFragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    if (!pickFragmentShader->compileSourceFile(":shaders/pick.frag")) {
        qDebug() << "Fragment shader compilation failed:" << pickFragmentShader->log();
        return;
    }
    pickShaderProgram.addShader(pickVertexShader);
    pickShaderProgram.addShader(pickFragmentShader);
    if(!pickShaderProgram.link()){                      //链接着色器
        qDebug()<<"Link shader program failed:"<<pickShaderProgram.log();    //如果链接出错,打印报错信息
    }

    VAO.bind();
    VBO.create();       //生成VBO对象
    VBO.bind();         //将VBO绑定到当前的顶点缓冲对象（QOpenGLBuffer::VertexBuffer）中
    //将顶点数据分配到VBO中，第一个参数为数据指针，第二个参数为数据的字节长度
    VBO.allocate(vertices.data(),sizeof(float)*vertices.size());

    IBO.create();  // 创建IBO
    IBO.bind();    // 绑定IBO
    IBO.allocate(indices.data(), sizeof(unsigned int) * indices.size());  // 为索引分配内

    sphereShaderProgram.setAttributeBuffer(0, GL_FLOAT, 0, 3, 8 * sizeof(float));
    sphereShaderProgram.enableAttributeArray(0); // 顶点位置

    sphereShaderProgram.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, 8 * sizeof(float));
    sphereShaderProgram.enableAttributeArray(1); // 法线

    sphereShaderProgram.setAttributeBuffer(2, GL_FLOAT, 6 * sizeof(float), 2, 8 * sizeof(float));
    sphereShaderProgram.enableAttributeArray(2); // 纹理位置

    pickShaderProgram.setAttributeBuffer(0, GL_FLOAT, 0, 3, 8 * sizeof(float));
    pickShaderProgram.enableAttributeArray(0); // 顶点位置

    VAO.release();

    QVector<float> sunVertices(vertices);
    sunVAO.bind();
    sunVBO.create();       //生成VBO对象
    sunVBO.bind();         //将VBO绑定到当前的顶点缓冲对象（QOpenGLBuffer::VertexBuffer）中
    //将顶点数据分配到VBO中，第一个参数为数据指针，第二个参数为数据的字节长度
    sunVBO.allocate(sunVertices.data(),sizeof(float)*vertices.size());

    sunIBO.create();  // 创建IBO
    sunIBO.bind();    // 绑定IBO
    sunIBO.allocate(indices.data(), sizeof(unsigned int) * indices.size());  // 为索引分配内

    sunShaderProgram.setAttributeBuffer(0, GL_FLOAT, 0, 3, 8 * sizeof(float));
    sunShaderProgram.enableAttributeArray(0); // 顶点位置

    sunShaderProgram.setAttributeBuffer(2, GL_FLOAT, 6 * sizeof(float), 2, 8 * sizeof(float));
    sunShaderProgram.enableAttributeArray(2); // 纹理位置
    sunVAO.release();

    loadTextures();

}

void OpenGLWidget::loadTextures(){
    QImage image(":/images/earth.jpg");
    if(image.isNull()){
        qDebug()<<"Failed to load texture";
        QImage dummy(128,128,QImage::Format_RGB32);
        dummy.fill(Qt::blue);
        image=dummy;
    }
    earth_tex=new QOpenGLTexture(image.mirrored());
    earth_tex->setMinificationFilter(QOpenGLTexture::Linear);
    earth_tex->setMagnificationFilter(QOpenGLTexture::Linear);
    earth_tex->setWrapMode(QOpenGLTexture::Repeat);

    QImage sun_image(":/images/sun.jpg");
    if(sun_image.isNull()){
        qDebug()<<"Failed to load texture";
        QImage dummy(128,128,QImage::Format_RGB32);
        dummy.fill(Qt::red);
        sun_image=dummy;
    }
    sun_tex=new QOpenGLTexture(sun_image.mirrored());
    sun_tex->setMinificationFilter(QOpenGLTexture::Linear);
    sun_tex->setMagnificationFilter(QOpenGLTexture::Linear);
    sun_tex->setWrapMode(QOpenGLTexture::Repeat);

    QImage moon_image(":/images/moon.jpg");
    if(moon_image.isNull()){
        qDebug()<<"Failed to load texture";
        QImage dummy(128,128,QImage::Format_RGB32);
        dummy.fill(Qt::red);
        moon_image=dummy;
    }
    moon_tex=new QOpenGLTexture(moon_image.mirrored());
    moon_tex->setMinificationFilter(QOpenGLTexture::Linear);
    moon_tex->setMagnificationFilter(QOpenGLTexture::Linear);
    moon_tex->setWrapMode(QOpenGLTexture::Repeat);

    QImage sky_image(":/images/background.jpg");
    if(sky_image.isNull()){
        qDebug()<<"Failed to load texture";
        QImage dummy(128,128,QImage::Format_RGB32);
        dummy.fill(Qt::black);
        sky_image=dummy;
    }
    sky_tex=new QOpenGLTexture(sky_image.mirrored());
    sky_tex->setMinificationFilter(QOpenGLTexture::Linear);
    sky_tex->setMagnificationFilter(QOpenGLTexture::Linear);
    sky_tex->setWrapMode(QOpenGLTexture::Repeat);
}


void OpenGLWidget::paintGL()
{
    static float sun_rot_angle=0.0f;
    static float earth_revolve_angle=0.0f;
    static float earth_rot_angle=0.0f;
    static float moon_revolve_angle=0.0f;
    static float moon_rot_angle=0.0f;
    QVector3D  light_pos(0.0f, 0.0f,1.0f);
    QVector3D  light_color(sun_lux*0.44, sun_lux*0.33, sun_lux*0.22); // 44% 红光，33 绿光，22% 蓝光
    QVector3D  sun_emissive_color(EMISSIVE_RATIO*sun_lux*0.44, EMISSIVE_RATIO*sun_lux*0.33, EMISSIVE_RATIO*sun_lux*0.22);
    QVector3D  sky_emissive_color(0.0f, 0.0f, 0.0f);

    projection.setToIdentity();
    view.setToIdentity();

    sun_rot_angle+=sun_rotation_angular_velocity;
    if(sun_rot_angle>360.0f){
        sun_rot_angle=0.0f;
    }

    earth_revolve_angle+=sun_rotation_angular_velocity*earth_revolution_sun_rotation_ratio;
    if(earth_revolve_angle>360.0f){
        earth_revolve_angle=0.0f;
    }

    earth_rot_angle+=sun_rotation_angular_velocity*earth_rotation_sun_rotation_ratio;
    if(earth_rot_angle>360.0f){
        earth_rot_angle=0.0f;
    }

    moon_revolve_angle+=sun_rotation_angular_velocity*moon_revolution_sun_rotation_ratio;
    if(moon_revolve_angle>360.0f){
        moon_revolve_angle=0.0f;
    }

    moon_rot_angle+=sun_rotation_angular_velocity*moon_rotation_sun_rotation_ratio;
    if(moon_rot_angle>360.0f){
        moon_rot_angle=0.0f;
    }

    updateCamera();
    // 设置投影矩阵（透视投影）
    projection.perspective(camera_fov, float(width()) / float(height()), 0.1f, 100.0f);
    // 设置视图矩阵
    view.lookAt(cameraPos, cameraTarget, cameraUp);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // 默认帧缓冲是 0
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


    sphereShaderProgram.bind();

    sphereShaderProgram.setUniformValue("projection", projection);
    sphereShaderProgram.setUniformValue("view", view);
    sphereShaderProgram.setUniformValue("lightPos", light_pos);
    sphereShaderProgram.setUniformValue("viewPos", cameraPos);
    sphereShaderProgram.setUniformValue("lightColor", light_color);

    earth_model.setToIdentity();
    earth_model.scale(earth_sun_radius_ratio);
    earth_model.rotate(solar_axial_tilt,0,1,0);
    earth_model.rotate(earth_revolve_angle,0,0,1);
    earth_model.translate(sun_earth_distance/earth_sun_radius_ratio,0,0);
    earth_model.rotate(-earth_revolve_angle,0,0,1); //消除公转角速度
    earth_model.rotate(earth_obliquity_of_the_ecliptic,1,0,0); // 地球黄道夹角，可能不太对,这里只是简单模拟一下
    earth_model.rotate(earth_rot_angle,0,0,1);


    earth_tex->bind(0);
    sphereShaderProgram.setUniformValue("ourTexture",0);
    sphereShaderProgram.setUniformValue("model", earth_model);

    VAO.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,0);  // 使用索引绘制
    VAO.release();

    moon_model.setToIdentity();
    moon_model.scale(earth_sun_radius_ratio*moon_earth_radius_ratio);
    moon_model.rotate(solar_axial_tilt,0,1,0);
    moon_model.rotate(earth_revolve_angle,0,0,1);
    moon_model.translate(sun_earth_distance/(earth_sun_radius_ratio*moon_earth_radius_ratio),0,0); //移动到地球的位置
    moon_model.rotate(-earth_revolve_angle,0,0,1);
    moon_model.rotate(earth_obliquity_of_the_ecliptic,1,0,0);
    moon_model.rotate(moon_earth_obliquity_of_the_ecliptic,1,0,0); // 月球轨道和地球自转平面夹角
    moon_model.rotate(moon_revolve_angle,0,0,1);
    moon_model.translate(earth_moon_distance/(earth_sun_radius_ratio*moon_earth_radius_ratio),0,0);
    moon_model.rotate(-moon_revolve_angle,0,0,1); // 消去公转
    moon_model.rotate(moon_obliquity_of_the_ecliptic,1,0,0);
    moon_model.rotate(moon_rot_angle,0,0,1);

    moon_tex->bind(0);
    sphereShaderProgram.setUniformValue("ourTexture",0);
    sphereShaderProgram.setUniformValue("model", moon_model);

    VAO.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,0);  // 使用索引绘制
    VAO.release();

    sphereShaderProgram.release();

    sunShaderProgram.bind();
    sunShaderProgram.setUniformValue("projection", projection);
    sunShaderProgram.setUniformValue("view", view);

    // 太阳
    sun_model.setToIdentity();
    sun_model.rotate(sun_rot_angle,0,0,1);

    sun_tex->bind(0);
    sunShaderProgram.setUniformValue("ourTexture",0);
    sunShaderProgram.setUniformValue("model", sun_model);
    sunShaderProgram.setUniformValue("emissiveColor",sun_emissive_color);
    sunVAO.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,0);  // 使用索引绘制
    sunVAO.release();

    // 背景
    sky_model.setToIdentity();
    sky_model.scale(SKY_SCALE);
    sky_tex->bind(0);
    sunShaderProgram.setUniformValue("ourTexture",0);
    sunShaderProgram.setUniformValue("model", sky_model);
    sunShaderProgram.setUniformValue("emissiveColor",sky_emissive_color);
    sunVAO.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,0);  // 使用索引绘制
    sunVAO.release();

    sunShaderProgram.release();

    // 初始化选择物体帧缓冲
    // 记得删除原来的帧缓冲对象，不然会显存泄露
    glDeleteFramebuffers(1, &pickFBO);
    glDeleteRenderbuffers(1, &pickRenderBuffer);

    glGenFramebuffers(1, &pickFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, pickFBO);
    // 创建渲染缓冲对象
    glGenRenderbuffers(1, &pickRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, pickRenderBuffer);
    // 为渲染缓冲分配存储空间，使用 GL_RGBA 格式，动态调整缓冲区大小
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, screen()->devicePixelRatio()*width(), screen()->devicePixelRatio()*height());
    // 将渲染缓冲对象附加到 FBO（颜色附件）
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, pickRenderBuffer);
    // 检查 FBO 是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        qDebug()<< "Framebuffer not complete!";
    }
    // 绘制物体选择
    glBindFramebuffer(GL_FRAMEBUFFER, pickFBO);  //  物体选择帧缓冲
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 清除屏幕
    pickShaderProgram.bind();
    pickShaderProgram.setUniformValue("projection", projection);
    pickShaderProgram.setUniformValue("view", view);
    pickShaderProgram.setUniformValue("code",SUN_CODE);
    pickShaderProgram.setUniformValue("model", sun_model);

    VAO.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,0);  // 使用索引绘制
    VAO.release();

    pickShaderProgram.setUniformValue("code",EARTH_CODE);
    pickShaderProgram.setUniformValue("model", earth_model);
    VAO.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,0);  // 使用索引绘制
    VAO.release();

    pickShaderProgram.setUniformValue("code",MOON_CODE);
    pickShaderProgram.setUniformValue("model", moon_model);
    VAO.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,0);  // 使用索引绘制
    VAO.release();
    pickShaderProgram.release();


    update();
}

void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void OpenGLWidget::generateSphere(float radius, int numLatitudes, int numLongitudes) {
    vertices.clear();
    indices.clear();


    float dYaw = 2.0f * M_PI / float(numLongitudes);
    float dPitch = M_PI / float(numLatitudes);
    for (int i = 0; i <= numLongitudes; ++i) {
        float yaw = i * dYaw;
        float U = (float)i / (float)numLongitudes;
        for (int j = 0; j <= numLatitudes; ++j) {
            float pitch = -M_PI/2 - j * dPitch;             // 为了正确计算UV坐标，维度从南极开始计算
            float V = (float)j / (float)numLatitudes;
            float x = radius * cos(pitch) * cos(yaw);
            float y = radius * cos(pitch) * sin(yaw);
            float z = radius * sin(pitch);
            QVector3D normal(x, y, z);
            vertices.append(x);
            vertices.append(y);
            vertices.append(z);
            vertices.append(normal.x());
            vertices.append(normal.y());
            vertices.append(normal.z());
            vertices.append(U);
            vertices.append(V);

        }
    }

    for (int i = 0; i < numLongitudes; ++i) {
        for (int j = 0; j < numLatitudes; ++j) {
            // 经纬网格面片的左下三角
            indices.push_back(i * (numLatitudes + 1) + j);
            indices.push_back((i + 1) * (numLatitudes + 1) + j);
            indices.push_back(i * (numLatitudes + 1) + j + 1);
            // 经纬网格面片的右上三角
            indices.push_back(i * (numLatitudes + 1) + j + 1);
            indices.push_back((i + 1) * (numLatitudes + 1) + j);
            indices.push_back((i + 1) * (numLatitudes + 1) + j + 1);
        }
    }
}

void OpenGLWidget::updateCamera(){
    const float epi=0.001f;
    cameraPos=QVector3D(
        cameraTarget.x()+camera_radius*qCos(camera_theta)*qSin(camera_alpha),
        cameraTarget.y()-camera_radius*qCos(camera_theta)*qCos(camera_alpha),
        cameraTarget.z()+camera_radius*qSin(camera_theta));
    cameraDirection=cameraPos-cameraTarget;
    // 注意当 camera的朝向和水平重合会出现无法计算叉积的情况，那么就要另外计算
    cameraDirection.normalize();
    if(abs(camera_theta-M_PI)<epi||abs(camera_theta)<epi||abs(camera_theta-2*M_PI)<epi){
        // theta 接近 0 PI 2PI 三种情况
        // 0, 2PI 算一组
        // PI 算一组
        if(abs(camera_theta-M_PI)>M_PI/2){
            cameraRight=QVector3D::crossProduct({0.0F,0.0F,-1.0f},cameraDirection);
        }else{
            cameraRight=QVector3D::crossProduct({0.0F,0.0F,1.0f},cameraDirection);
        }
    }else{
        QVector3D normal(qSin(camera_alpha),-qCos(camera_alpha),0.0f);
        if(camera_theta>M_PI) normal=-normal;
        cameraRight=QVector3D::crossProduct(normal,cameraDirection);

    }
    cameraUp=QVector3D::crossProduct(cameraDirection,cameraRight);
    cameraRight.normalize();
    cameraUp.normalize();
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event){
    switch(event->key()){
    case Qt::Key_W:
    case Qt::Key_Up:  //上
        cameraTarget+=cameraUp*CAMERA_VEL;
        break;
    case Qt::Key_S:
    case Qt::Key_Down: // 下
        cameraTarget+=-cameraUp*CAMERA_VEL;
        break;
    case Qt::Key_D:
    case Qt::Key_Right: // 右
        cameraTarget+=cameraRight*CAMERA_VEL;
        break;
    case Qt::Key_A:
    case Qt::Key_Left: // 左
        cameraTarget+=-cameraRight*CAMERA_VEL;
        break;
    case Qt::Key_Q: // 拉近
        cameraTarget+=-cameraDirection*CAMERA_VEL;
        break;
    case Qt::Key_E: // 远离
        cameraTarget+=cameraDirection*CAMERA_VEL;
        break;
    }
}

void OpenGLWidget::wheelEvent(QWheelEvent * event){
    int delta = event->angleDelta().y();  // 获取垂直滚动增量，单位是1/8度
    if (delta > 0) {
        camera_fov+=CAMERA_FOV_VEL;
        if(camera_fov>MAX_CAMERA_FOV) camera_fov=MAX_CAMERA_FOV;
    } else if (delta < 0) {
        camera_fov-=CAMERA_FOV_VEL;
        if(camera_fov<MIN_CAMERA_FOV) camera_fov=MIN_CAMERA_FOV;
    }
}

void OpenGLWidget::mousePressEvent(QMouseEvent * event){
    unsigned char res[4];
    float screen_pixel_ratio=screen()->devicePixelRatio(); // 屏幕的分辨率缩放
    QPoint mousePos = event->pos();
    glBindFramebuffer(GL_FRAMEBUFFER, pickFBO);  //  物体选择帧缓冲
    glReadPixels(mousePos.x()*screen_pixel_ratio,(height()-mousePos.y())*screen_pixel_ratio,1,1,GL_RGBA,GL_UNSIGNED_BYTE,&res);
    switch(res[0]) {
        case 0:
            qDebug()<<"Nothing Picked";
            break;
        case SUN_CODE:
            QToolTip::showText(event->globalPosition().toPoint(), "太阳");
            break;
        case EARTH_CODE:
            QToolTip::showText(event->globalPosition().toPoint(), "地球");
            break;
        case MOON_CODE:
            QToolTip::showText(event->globalPosition().toPoint(), "月亮");
            break;
        default:
            qDebug()<<res[0] <<"unrecognized";
            break;
    }
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent * event){
    unsigned char res[4];
    float screen_pixel_ratio=screen()->devicePixelRatio(); // 屏幕的分辨率缩放
    QPoint mousePos = event->pos();
    glBindFramebuffer(GL_FRAMEBUFFER, pickFBO);  //  物体选择帧缓冲
    glReadPixels(mousePos.x()*screen_pixel_ratio,(height()-mousePos.y())*screen_pixel_ratio,1,1,GL_RGBA,GL_UNSIGNED_BYTE,&res);
    switch(res[0]) {
    case 0:
        break;
    case SUN_CODE:
        QToolTip::showText(event->globalPosition().toPoint(), "太阳");
        break;
    case EARTH_CODE:
        QToolTip::showText(event->globalPosition().toPoint(), "地球");
        break;
    case MOON_CODE:
        QToolTip::showText(event->globalPosition().toPoint(), "月亮");
        break;
    default:
        qDebug()<<res[0] <<"unrecognized";
        break;
    }
    QToolTip::hideText();
}

void OpenGLWidget::onSunRotVelChange(int value){
    sun_rotation_angular_velocity=MAX_VEL*(float)value/100;
}

void OpenGLWidget::onEarthSunRatioChanged(int value){
    earth_sun_radius_ratio=(float)value/100;
}

void OpenGLWidget::onMoonEarthRatioChanged(int value){
    moon_earth_radius_ratio=(float)value/100;
}

void OpenGLWidget::onSolarAxialTiltChanged(int value){
    solar_axial_tilt=MAX_TILT_DEGREE*(float)value/360;
}

void OpenGLWidget::onEarthObliquityChanged(int value){
    earth_obliquity_of_the_ecliptic=MAX_TILT_DEGREE*(float)value/360;
}

void OpenGLWidget::onMoonEarthObliquityChanged(int value){
    moon_earth_obliquity_of_the_ecliptic=MAX_TILT_DEGREE*(float)value/360;
}

void OpenGLWidget::onMoonObliquityChanged(int value){
    moon_obliquity_of_the_ecliptic=MAX_TILT_DEGREE*(float)value/360;
}

void OpenGLWidget::onSunEarthDistanceChanged(int value){
    sun_earth_distance=MAX_SUN_EARTH_DISTANCE*(float)value/100;
}

void OpenGLWidget::onEarthMoonDistanceChanged(int value){
    earth_moon_distance=MAX_EARTH_MOON_DISTANCE*(float)value/100;
}

void OpenGLWidget::onSunLuxChanged(int value){
    sun_lux=MAX_SUN_LUX*(float)value/100;
}

void OpenGLWidget::onCameraAlphaChanged(int value){
    camera_alpha=2*M_PI*(float)value/360;
}

void OpenGLWidget::onCameraThetaChanged(int value){
    camera_theta=2*M_PI*(float)value/360;
}

void OpenGLWidget::onCameraRadiusChanged(int value){
    camera_radius=MAX_CAMERA_RADIUS*(float)value/100;
}

void OpenGLWidget::onEarthRevolveSunRotRatioChanged(int v){
    earth_revolution_sun_rotation_ratio=MAX_EARTH_REVOLVE_SUN_ROT_RATIO*(float)v/100;
}

void OpenGLWidget::onEarthRotSunRotRatioChanged(int v){
    earth_rotation_sun_rotation_ratio=MAX_EARTH_ROT_SUN_ROT_RATIO*(float)v/100;
}

void OpenGLWidget::onMoonRevolveSunRotRatioChanged(int v){
    moon_revolution_sun_rotation_ratio=MAX_MOON_REVOLE_SUN_ROT_RATIO*(float)v/100;
}

void OpenGLWidget::onMoonRotSunRotRatioChanged(int v){
    moon_rotation_sun_rotation_ratio=MAX_MOON_ROT_SUN_ROT_RATIO*(float)v/100;
}

void OpenGLWidget::onCameraReset(){
    cameraTarget=QVector3D(0.0f,0.0f,0.0f);
    camera_alpha=DEFAULT_CAMERA_ALPHA;
    camera_theta=DEFAULT_CAMERA_THETA;
    camera_radius=DEFAULT_CAMERA_RADIUS;
    camera_fov=DEFAULT_CAMERA_FOV;
    emit cameraRested();
}

void OpenGLWidget::onResetParams(){
    sun_rotation_angular_velocity=DEFAULT_VEL; // 太阳自转速度
    earth_revolution_sun_rotation_ratio=DEFAULT_EARTH_REVOLVE_SUN_ROT_RATIO; // 地球公转速度与太阳自转速度比值
    earth_rotation_sun_rotation_ratio=DEFAULT_EARTH_ROT_SUN_ROT_RATIO; // 地球自转与太阳自转速度比值
    moon_revolution_sun_rotation_ratio=DEFAULT_MOON_REVOLVE_SUN_ROT_RATIO; // 月球公转与太阳自转速度比值
    moon_rotation_sun_rotation_ratio=DEFAULT_MOON_ROT_SUN_ROT_RATIO; // 月球自转与太阳自转速度比值
    solar_axial_tilt=DEFAULT_SOLAR_TILT; // 地球公转轴与太阳自转轴的夹角
    earth_obliquity_of_the_ecliptic=DEFAULT_EARTH_OBLIQUITY; //黄道夹角
    moon_earth_obliquity_of_the_ecliptic=DEFAULT_MOON_EARTH_OBLIQUITY; // 月球公转与地球赤道夹角
    moon_obliquity_of_the_ecliptic=DEFAULT_MOON_OBLIQUITY; // 月球自转与公转平面夹角
    sun_earth_distance=DEFAULT_SUN_EARTH_DISTANCE; // 日地距离
    earth_sun_radius_ratio=DEFAULT_EARTH_SUN_RATIO; // 地球太阳大小比例
    moon_earth_radius_ratio=DEFAULT_MOON_EARTH_RATIO; // 月球地球大小比例
    earth_moon_distance=DEFAULT_EARTH_MOON_DISTANCE; // 地月距离
    sun_lux=DEFAULT_SUN_LUX; // 太阳光强（照射）
    emit paramsRested();
}



