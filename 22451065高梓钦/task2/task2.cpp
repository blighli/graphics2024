#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glut.h>

#include "Sphere.h"

// �ӿڵĻص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// �������
void processInput(GLFWwindow* window);

// ����
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

// phong����ģ��
// ���嶥����ɫ��
const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"            // ����λ��
"layout (location = 1) in vec3 aNormal;\n"          // ���㷨��
"layout (location = 2) in vec3 aTexCoord;\n"        // ��������
"out vec3 FragPos;\n"                               // ���Ƭ��λ��
"out vec3 Normal;\n"                               // �������
"out vec3 TexCoord;\n"                             // �����������
"uniform mat4 model;\n"                            // ģ�;���
"uniform mat4 view;\n"                             // ��ͼ����
"uniform mat4 projection;\n"                       // ͶӰ����
"void main()\n"
"{\n"
"   // ��������ռ��еĶ���λ��\n"
"   FragPos = vec3(model * vec4(aPos, 1.0));\n"
"   // ���㷨�߲��任������ռ�\n"
"   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"   // ����ü��ռ��еĶ���λ��\n"
"   gl_Position = projection * view * vec4(FragPos, 1.0);\n"
"   // �����������굽Ƭ����ɫ��\n"
"   TexCoord = aTexCoord;\n"
"}\0";

// ����Ƭ����ɫ��
const char* fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"uniform sampler2D texture1;\n"  // ������������󶨵�����Ԫ
"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"in vec3 TexCoord;\n"  // �������������
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 objectColor;\n"
"void main()\n"
"{\n"
"   // ��������\n"
"   float ambientStrength = 0.1;\n"
"   vec3 ambient = ambientStrength * lightColor;\n"
"   // ���������\n"
"   vec3 norm = normalize(Normal);\n"
"   vec3 lightDir = normalize(lightPos - FragPos);\n"
"   float diff = max(dot(norm, lightDir), 0.0);\n"
"   vec3 diffuse = diff * lightColor;\n"
"   // �������\n"
"   float specularStrength = 0.5;\n"
"   vec3 viewDir = normalize(viewPos - FragPos);\n"
"   vec3 reflectDir = reflect(-lightDir, norm);\n"
"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
"   vec3 specular = specularStrength * spec * lightColor;\n"
"   // ������ɫ\n"
"   vec3 textureColor = texture(texture1, TexCoord.xy).rgb;\n"  // ��������TexCoord.xy ����������
"   // ���\n"
"   vec3 result = (ambient + diffuse + specular) * objectColor;\n"
"   // ʹ��������ɫ��ϣ�����ֱ�Ӹ��ǻ���м�Ȩ���\n"
"   FragColor = vec4(result * textureColor, 1.0);\n"  // ������Ĺ��ս����������ɫ��ˣ��õ�������ɫ
"}\0";


// ����
glm::vec3 lightPos(0.6f, 0.5f, 1.0f);

void MySphere(float r = 0.5f, float* sphere_vertices = nullptr, float* sphere_normals = nullptr, float* sphere_texCoords = nullptr, int* sphere_indices = nullptr, int nSlices = 50, int nStacks = 50) {
    int nVerts = (nStacks + 1) * (nSlices + 1);
    float theta, phi;
    float thetaFac = 3.14f * 2.0f / nSlices;
    float phiFac = 3.14f * 1.0f / nStacks;
    float nx, ny, nz;
    int idx = 0;

    // ��������Ķ��㣬���ߺ���������
    for (int i = 0; i <= nSlices; i++) {
        theta = i * thetaFac;
        for (int j = 0; j <= nStacks; j++) {
            phi = j * phiFac;

            // ���㶥��λ��
            nx = sinf(phi) * cosf(theta);
            ny = sinf(phi) * sinf(theta);
            nz = cosf(phi);
            sphere_vertices[idx * 3] = r * nx;
            sphere_vertices[idx * 3 + 1] = r * ny;
            sphere_vertices[idx * 3 + 2] = r * nz;

            // ���㷨�ߣ���λ������ָ�����ģ�
            sphere_normals[idx * 3] = nx;
            sphere_normals[idx * 3 + 1] = ny;
            sphere_normals[idx * 3 + 2] = nz;

            // ������������
            // s = theta / (2 * pi), t = phi / pi
            sphere_texCoords[idx * 2] = (float)i / 50; // [0, 1]
            sphere_texCoords[idx * 2 + 1] = (float)j / 50;        // [0, 1]

            idx++;
        }
    }

    // ����������������
    int indx = 0;
    for (int i = 0; i < nStacks; i++) {
        for (int j = 0; j < nSlices; j++) {
            int i0 = i * (nSlices + 1) + j;
            int i1 = i0 + 1;
            int i2 = i0 + (nSlices + 1);
            int i3 = i2 + 1;
            if ((j + i) % 2 == 1) {
                sphere_indices[indx] = i0;
                sphere_indices[indx + 1] = i2;
                sphere_indices[indx + 2] = i1;
                sphere_indices[indx + 3] = i1;
                sphere_indices[indx + 4] = i2;
                sphere_indices[indx + 5] = i3;
                indx += 6;
            }
            else {
                sphere_indices[indx] = i0;
                sphere_indices[indx + 1] = i2;
                sphere_indices[indx + 2] = i3;
                sphere_indices[indx + 3] = i0;
                sphere_indices[indx + 4] = i3;
                sphere_indices[indx + 5] = i1;
                indx += 6;
            }
        }
    }
}

GLuint loadTexture(const char* path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);  // ����һ������ID
    glBindTexture(GL_TEXTURE_2D, textureID);  // ������

    // ��������Ĺ��˷�ʽ�Ͱ�����ʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // ˮƽ����İ�����ʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  // ��ֱ����İ�����ʽ

    // ����ͼ��
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        // ��������
        if (nrChannels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else if (nrChannels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);  // ��������� mipmaps

        // �ͷ�ͼ������
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


int main()
{
    // ʵ����GLFW����
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // �������ڶ���
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "lexiaoyuan", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // ��ʼ��glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);

    // ������������
    float r = 0.5;
    int nSlices = 50; // ����
    int nStacks = 50; // γ��
    int nVerts = (nStacks + 1) * (nSlices + 1);
    int elements = nSlices * nStacks * 6;
    const int space = nVerts * 3;
    float sphere_vertices[51 * 51 * 3];
    float sphere_normals[51 * 51 * 3];
    float sphere_texcoord[51 * 51 * 2];
    int sphere_indices[50 * 50 * 6];
    
    MySphere(0.5f, sphere_vertices, sphere_normals, sphere_texcoord, sphere_indices ,nSlices, nStacks);


    float interleavedData[51 * 51 * (3 + 3 + 2)]; // ����ÿ�������� 3 ��λ�ã�3 �����ߣ�2 ����������

    for (int i = 0; i < nVerts; i++) {
        interleavedData[i * 8] = sphere_vertices[i * 3];        // ����λ�� x
        interleavedData[i * 8 + 1] = sphere_vertices[i * 3 + 1]; // ����λ�� y
        interleavedData[i * 8 + 2] = sphere_vertices[i * 3 + 2]; // ����λ�� z

        interleavedData[i * 8 + 3] = sphere_normals[i * 3];     // ���� x
        interleavedData[i * 8 + 4] = sphere_normals[i * 3 + 1]; // ���� y
        interleavedData[i * 8 + 5] = sphere_normals[i * 3 + 2]; // ���� z

        interleavedData[i * 8 + 6] = sphere_texcoord[i * 2];   // �������� s
        interleavedData[i * 8 + 7] = sphere_texcoord[i * 2 + 1]; // �������� t
    }

    // ���嶥�㻺�����
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // ���嶥���������
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // ���������������
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // �󶨶��㻺�����
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // ���ƶ������ݵ������ڴ�
    //glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(interleavedData), interleavedData, GL_STATIC_DRAW);

    // �󶨶����������
    glBindVertexArray(VAO);

    // �������������
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_indices), sphere_indices, GL_STATIC_DRAW);


    // ����������ɫ������
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // ���Ӷ�����ɫ�����뵽������ɫ������
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    // ���붥����ɫ��
    glCompileShader(vertexShader);

    // ����Ƭ����ɫ������
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // ����Ƭ����ɫ�����뵽Ƭ����ɫ������
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    // ����Ƭ����ɫ��
    glCompileShader(fragmentShader);

    // ����һ����ɫ���������
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // ����ɫ�����󸽼ӵ�������
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // ������ɫ������
    glLinkProgram(shaderProgram);

    // ��ɫ���������ӳɹ������ɾ����
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // λ������
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(1);
    // ����λ��
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // ���㷨��
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ������������
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLuint textureID = loadTexture("1.jpg");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLuint textureLoc = glGetUniformLocation(shaderProgram, "texture1");
    glUniform1i(textureLoc, 0);  // ������Ԫ 0 �󶨵�����


    // �����ӿ�
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // ע��ص�����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ��Ⱦѭ��
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ������ɫ���������
        glUseProgram(shaderProgram);

        // �󶨶�������
        glBindVertexArray(VAO);

        // �������������
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        // ʹ���߿�ģʽ
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // ʹ�����ģʽ��Ĭ�ϣ�
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        /*lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;*/

        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &glm::vec3(0.0f, 0.0f, 3.0f)[0]);

        // ����ģ�;���
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(30.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // ����۲����
        glm::mat4 view = glm::mat4(1.0f);
        // ע�⣬���ǽ�����������Ҫ�����ƶ������ķ������ƶ���
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // ����ͶӰ����
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // ���������ͳһλ��
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        // �����Ǵ��ݸ���ɫ��
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);


        // ����
        glDrawElements(GL_TRIANGLES, elements, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, elements);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // �ͷ���Դ
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // �ͷ�֮ǰ�������Դ
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
