#define GLEW_STATIC

#include <glew.h>
#include <glfw3.h>
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include<glm.hpp>
#include<vec3.hpp>
#include<mat4x4.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include "shader.h"
#include "Model.h"
#include <cmath>   


#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>



const string yourpath = "C:/Users/Asus/Desktop/KabanovD/";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3  lightNimbusPos = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3  lightNimbuscolor = glm::vec3(1.0f, 0.8745f, 0.0f);
glm::vec3  suncolor = glm::vec3(1.0f, 1.0f, 1.0f);


bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 1200.0f / 2.0;
float lastY = 900.0 / 2.0;
float fov = 45.0f;
float angleinputlr = 0.0f;
float angleinputud = 0.0f;
float anglespeed = 2.0f;
float heightScale = 0.00006;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
const GLint numv = 20;
float cameraSpeed;

glm::vec3 lightPosst(3.0f, 5.0f, 2.0f);
glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

const int N1 = 500; //"точность" круга


std::vector<float> torvert; //вершины и индексы для тора
std::vector<int> torind;



const double Pi = 3.14159265358979323846;

const int NT1 = 100; //"точность" тора
const int NT2 = 100;//"точность" тора

void GenTorus(double r1, double r2) {  //генератор вершин тора 
    int iidx = 0;
    for (int i = 0; i < NT1; i++) {
        int i2 = (i < NT1 - 1) ? (i + 1) : (0);
        double phi = i * Pi * 2 / NT1;
        for (int j = 0; j < NT2; j++) {
            int j2 = (j < NT2 - 1) ? (j + 1) : (0);
            double psi = j * Pi * 2 / NT2;

            double nx = cos(phi) * cos(psi);
            double ny = sin(psi);
            double nz = sin(phi) * cos(psi);
            // координаты
            torvert.push_back(r1 * cos(phi) + r2 * nx);
            torvert.push_back(r2 * ny);
            torvert.push_back(r1 * sin(phi) + r2 * nz);
            //нормали
            torvert.push_back(nx);
            torvert.push_back(ny);
            torvert.push_back(nz);

            torind.push_back(i * NT2 + j);
            torind.push_back(i * NT2 + j2);
            torind.push_back(i2 * NT2 + j2);
            torind.push_back(i2 * NT2 + j);


        }
    }
}
const int NSV = 250;//"точность" сферы
const int NSH = 250;//"точность" сферы
std::vector<GLfloat> sfvert;
std::vector<int> sfind;

void GenSphere() { // генератор сферы
    float x, y, z, xy;
    int k1, k2;
    int indv = 0, indi = 0;
    for (int i = 0; i <= NSV;i++) {
        k1 = i * (NSH + 1);
        k2 = k1 + NSH + 1;
        double phi = Pi / 2 - Pi * i / NSV;
        xy = cos(phi);
        z = sin(phi);

        for (int j = 0; j <= NSH; j++, k1++, k2++) {
            double psi = 2 * Pi * j / NSH;
            x = xy * cos(psi);
            y = xy * sin(psi);
            if (abs(x) < 0.001f)
                x = 0;
            if (abs(y) < 0.001f)
                y = 0;
            if (abs(z) < 0.001f)
                z = 0;
            //координаты
            sfvert.push_back(x);
            sfvert.push_back(y);
            sfvert.push_back(z);
            //нормали
            sfvert.push_back(x);
            sfvert.push_back(y);
            sfvert.push_back(z);
            if (i == NSV || j == NSH)
                continue;
            if (i != 0) {
                sfind.push_back(k1);
                sfind.push_back(k2);
                sfind.push_back(k1 + 1);
            }
            if (i != (NSV - 1)) {
                sfind.push_back(k1 + 1);
                sfind.push_back(k2);
                sfind.push_back(k2 + 1);
            }
        }
    }

}
std::vector<int> cirind;
std::vector<float> cirvert;
void Gencircle3() {
    //pos
    cirvert.push_back(0.0f);
    cirvert.push_back(0.0f);
    cirvert.push_back(0.0f);
    //normal
    cirvert.push_back(0.0f);
    cirvert.push_back(0.0f);
    cirvert.push_back(1.0f);
    //texcoord
    cirvert.push_back(0.5f);
    cirvert.push_back(0.5f);
    //tangent
    cirvert.push_back(1.0f);
    cirvert.push_back(0.0f);
    cirvert.push_back(0.0f);
    //bitangent
    cirvert.push_back(0.0f);
    cirvert.push_back(1.0f);
    cirvert.push_back(0.0f);

    int i = 0;
    int j = 0;
    for (i = 0;i < N1;i++) {
        double phi = (i)*Pi * 2 / N1;
        double px = cos(phi);
        double py = sin(phi);
        double pz = 0;
        if (abs(py) < 0.001f)
            py = 0;
        if (abs(px) < 0.001f)
            px = 0;
        cirvert.push_back(px);
        cirvert.push_back(py);
        cirvert.push_back(pz);

        cirvert.push_back(0.0f);
        cirvert.push_back(0.0f);
        cirvert.push_back(1.0f);
        px = px * 0.5f + 0.5f;
        py = py * 0.5f + 0.5f;
        cirvert.push_back(px);
        cirvert.push_back(py);

        cirvert.push_back(1.0f);
        cirvert.push_back(0.0f);
        cirvert.push_back(0.0f);

        cirvert.push_back(0.0f);
        cirvert.push_back(1.0f);
        cirvert.push_back(0.0f);

        cirind.push_back(0);
        cirind.push_back(i + 1);
        cirind.push_back(i + 2);
    }
    cirind.pop_back();
    cirind.push_back(1);
}


int main(void)
{
    glfwInit();

    const int WINDOW_WIDTH = SCR_WIDTH;
    const int WINDOW_HEIGHT = SCR_HEIGHT;
    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Window", NULL, NULL);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    float cubevertices[] = {
    -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   1.0f, 1.0f,   1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   1.0f, 1.0f,   1.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,    0.0f,  0.0f, 1.0f,    0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,    0.0f,  0.0f, 1.0f,    1.0f, 0.0f,   1.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    0.0f,  0.0f, 1.0f,    1.0f, 1.0f,   1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    0.0f,  0.0f, 1.0f,    1.0f, 1.0f,   1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,    0.0f,  0.0f, 1.0f,    0.0f, 1.0f,   0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,    0.0f,  0.0f, 1.0f,    0.0f, 0.0f,   0.0f, 0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 1.0f, 1.0f,

     0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   1.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   1.0f, 1.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,   1.0f, 0.0f,   1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,   1.0f, 0.0f,   1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,   0.0f, 1.0f,   1.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,   1.0f, 1.0f,   1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,   1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,   1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,   0.0f, 0.0f,   0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,   0.0f, 1.0f,   1.0f, 0.0f, 1.0f
    };
    GLint GlewInitResult = glewInit();
    if (GLEW_OK != GlewInitResult)
    {
        printf("ERROR: %s", glewGetErrorString(GlewInitResult));
        exit(EXIT_FAILURE);
    }
    //shader for skycube
    Shader myShader(yourpath + "vs.txt", yourpath + "fs.txt");
    //shader for lightsource
    Shader lightCubeShader(yourpath + "vsLight.txt", yourpath + "fsLight.txt");
    //SHREK    
    Shader modelshader(yourpath + "vsmodel.txt", yourpath + "fsmodel.txt");
    Model Shrek(yourpath + "imag/CHARACTER_Shrek.obj");
    //shader for torus and shpere 
    Shader shader2(yourpath + "vsmodel2.txt", yourpath + "fsmodel2.txt");

    //shader for shadows
    Shader simpleDepthShader(yourpath + "vsshadowmap.txt", yourpath + "fsshadowmap.txt");
    //shader for circle 
    Shader waveshader(yourpath + "vswave.txt", yourpath + "fswave.txt");
    Shader outline(yourpath + "vsmodel.txt", yourpath + "fsOutlineGold.txt");

    stbi_set_flip_vertically_on_load(true);

    unsigned int texture1, normalmap, HightMap;
    // texture 1//
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char* data = stbi_load((yourpath + "imag/universe.jpg").c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture1" << std::endl;
    }
    stbi_image_free(data);

    // normalmap // 
    glGenTextures(1, &normalmap);
    glBindTexture(GL_TEXTURE_2D, normalmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load((yourpath + "imag/norm4.png").c_str(), &width, &height, &nrChannels, 0);
    if (data) {

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture2" << std::endl;
    }
    stbi_image_free(data);

    // highmap// 
    glGenTextures(1, &HightMap);
    glBindTexture(GL_TEXTURE_2D, HightMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load((yourpath + "imag/high4.png").c_str(), &width, &height, &nrChannels, 0);
    if (data) {

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture2" << std::endl;
    }
    stbi_image_free(data);



    GLuint VAO[2], VBO[2], EBO, EBO2, EBOtor, lightVAO, lightVBO;
    glGenVertexArrays(2, VAO);//skycube
    glGenBuffers(2, VBO);
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubevertices), cubevertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glGenVertexArrays(1, &lightVAO);//light
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubevertices), cubevertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GenTorus(0.5, 0.1);
    GLuint vertexBuffertor;
    GLuint vertexArraytor;

    shader2.use();
    glGenVertexArrays(1, &vertexArraytor);//torus
    glBindVertexArray(vertexArraytor);
    glGenBuffers(1, &vertexBuffertor);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffertor);
    glBufferData(GL_ARRAY_BUFFER, sizeof(torvert[0]) * torvert.size(), &torvert[0], GL_STATIC_DRAW);
    glGenBuffers(1, &EBOtor);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOtor);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(torind[0]) * torind.size(), &torind[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Gencircle3();
    GLuint vertexBuffer;
    GLuint vertexArray;
    shader2.use();
    glGenVertexArrays(1, &vertexArray);//circle
    glBindVertexArray(vertexArray);
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cirvert[0]) * cirvert.size(), &cirvert[0], GL_STATIC_DRAW);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cirind[0]) * cirind.size(), &cirind[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);



    GenSphere();
    GLuint vertexBuffer2;
    GLuint vertexArray2;
    shader2.use();
    glGenVertexArrays(1, &vertexArray2);//sphere
    glBindVertexArray(vertexArray2);
    glGenBuffers(1, &vertexBuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer2);
    glBufferData(GL_ARRAY_BUFFER, sfvert.size() * sizeof(sfvert[0]), &sfvert[0], GL_STATIC_DRAW);
    glGenBuffers(1, &EBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sfind.size() * sizeof(sfind[0]), &sfind[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);





    //  depth map 
    const unsigned int SHADOW_WIDTH = 10240, SHADOW_HEIGHT = 10240;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    modelshader.use();
    modelshader.setInt("texture_diffusel", 0);
    modelshader.setInt("shadowMap", 1);

    shader2.use();
    shader2.setInt("shadowMap", 0);

    waveshader.use();
    waveshader.setInt("shadowMap", 0);
    waveshader.setInt("normalMap", 1);
    waveshader.setInt("HightMap", 2);


    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 model = glm::mat4(1.0f);
    float angle;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


    /// start render looooooooooooooooooooop
    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;



        processInput(window);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //first shadows

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 0.1f, far_plane = 20.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 13.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPosst, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        //===================================+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //Shrek
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        angle = 0.0f + angleinputlr;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        angle = 0.0f + angleinputud;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        simpleDepthShader.use();
        simpleDepthShader.setMat4("model", model);
        Shrek.Draw(simpleDepthShader);
        //Sphere
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
        angle = 0.0f + angleinputlr;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        angle = 0.0f + angleinputud;
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        simpleDepthShader.setMat4("model", model);
        glBindVertexArray(vertexArray2);
        glDrawElements(GL_TRIANGLES, sfind.size() * 3, GL_UNSIGNED_INT, 0);
        //Circle
        simpleDepthShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -8.0f, 0.0f));
        angle = 0.0f + angleinputlr;
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        angle = 0.0f + angleinputud;
        model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
        simpleDepthShader.setMat4("model", model);
        glBindVertexArray(vertexArray);
        glDrawElements(GL_TRIANGLES, cirind.size() * 3, GL_UNSIGNED_INT, 0);
        //torus1
        simpleDepthShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -4.0f, 0.0f));
        angle = 0.0f + angleinputlr;
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        angle = 0.0f + angleinputud;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        simpleDepthShader.setMat4("model", model);
        glBindVertexArray(vertexArraytor);
        glDrawElements(GL_QUADS, torind.size() * 4, GL_UNSIGNED_INT, 0);
        //torus2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        angle = 0.0f + angleinputlr;
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        angle = 0.0f + angleinputud;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        simpleDepthShader.setMat4("model", model);
        glBindVertexArray(vertexArraytor);
        glDrawElements(GL_QUADS, torind.size() * 4, GL_UNSIGNED_INT, 0);
        //===============================-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //second objects themselves 

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        projection = glm::perspective(glm::radians(fov), 1200.0f / 900.0f, 0.1f, 200.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ///  skyblock +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        myShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        myShader.setMat4("model", model);
        myShader.setVec3("objectColor", 0.0f, 1.0f, 0.8f);
        myShader.setVec3("lightColor", lightColor);
        myShader.setVec3("lightPos", lightPos);
        myShader.setVec3("viewPos", cameraPos);
        myShader.setInt("texture1", 0);
        myShader.setInt("texture2", 1);
        myShader.setVec3("material.ambient", 1.0f, 1.0f, 1.0f);
        myShader.setVec3("material.diffuse", 0.50754f, 0.50754f, 0.50754f);
        myShader.setVec3("material.specular", 0.508273f, 0.508273f, 0.508273f);
        myShader.setFloat("material.shininess", 1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -7.0f, 0.0f));
        angle = 0.0f + angleinputlr;
        angle = 0.0f + angleinputud;
        model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
        myShader.setMat4("model", model);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        ///  |||skyblock -------------------------------------------------------------------------------

        ///  outline sphere  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        outline.use();
        outline.setMat4("view", view);
        outline.setMat4("projection", projection);
        shader2.use();
        shader2.setMat4("projection", projection);
        shader2.setMat4("view", view);

        glStencilMask(0x00);

        //Circle +++++
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -8.0f, 0.0f));
        angle = 0.0f + angleinputlr;
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        angle = 0.0f + angleinputud;
        model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
        waveshader.use();
        waveshader.setMat4("projection", projection);
        waveshader.setMat4("view", view);
        waveshader.setMat4("model", model);
        waveshader.setVec3("objectColor", 0.828f, 0.71f, 0.5f);
        waveshader.setVec3("lightPos", lightPos);
        waveshader.setVec3("viewPos", cameraPos);
        waveshader.setVec3("material.ambient", 0.5f, 0.5f, 0.5f);
        waveshader.setVec3("material.diffuse", 0.50754f, 0.50754f, 0.50754f);
        waveshader.setVec3("material.specular", 0.508273f, 0.508273f, 0.508273f);
        waveshader.setFloat("material.shininess", 256.0f);
        waveshader.setVec3("light[0].lightColor", lightNimbuscolor);
        waveshader.setVec3("light[0].lightPos", lightNimbusPos);
        waveshader.setFloat("light[0].consta", 1.0f);
        waveshader.setFloat("light[0].line", 0.9f);
        waveshader.setFloat("light[0].sqrish", 0.32f);
        waveshader.setVec3("lightsun.direction", -lightPosst);
        waveshader.setVec3("lightsun.lightColor", suncolor);
        waveshader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        waveshader.setFloat("heightScale", heightScale);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalmap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, HightMap);

        glBindVertexArray(vertexArray);
        glDrawElements(GL_TRIANGLES, cirind.size() * 3, GL_UNSIGNED_INT, 0);
        //Circle -----------

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        //Sphere ++++++++++
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
        angle = 0.0f + angleinputlr;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        angle = 0.0f + angleinputud;
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        shader2.use();
        shader2.setMat4("model", model);
        shader2.setVec3("objectColor", 1.0f, 0.0f, 0.0f);
        shader2.setVec3("viewPos", cameraPos);
        shader2.setVec3("material.ambient", 0.3f, 0.3f, 0.3f);
        shader2.setVec3("material.diffuse", 0.5f, 0.4f, 0.4f);
        shader2.setVec3("material.specular", 0.7f, 0.04f, 0.04f);
        shader2.setFloat("material.shininess", 200.0f);
        shader2.setVec3("light[0].lightColor", lightNimbuscolor);
        shader2.setVec3("light[0].lightPos", lightNimbusPos);
        shader2.setFloat("light[0].consta", 1.0f);
        shader2.setFloat("light[0].line", 0.9f);
        shader2.setFloat("light[0].sqrish", 0.32f);
        shader2.setVec3("lightsun.direction", -lightPosst);
        shader2.setVec3("lightsun.lightColor", suncolor);
        shader2.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glBindVertexArray(vertexArray2);
        glDrawElements(GL_TRIANGLES, sfind.size() * 3, GL_UNSIGNED_INT, 0);
        //Sphere ----------------

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        outline.use();
        float scale = 1.05;
        glBindVertexArray(vertexArray2);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
        angle = 0.0f + angleinputlr;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        angle = 0.0f + angleinputud;
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        outline.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, sfind.size() * 3, GL_UNSIGNED_INT, 0);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);
        /// |||outline ---------------------------------------------------------------------------


        /// shrek and nimbus ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        angle = 0.0f + angleinputlr;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        angle = 0.0f + angleinputud;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ///  Nimbus (torus) +++++++++++++++++++++

        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glm::mat4 modeln = glm::translate(model, glm::vec3(0.0f, 1.2f, 0.15f));
        modeln = glm::scale(modeln, glm::vec3(0.1f, 0.1f, 0.1f));
        lightNimbusPos = glm::vec3(modeln[3].x, modeln[3].y, modeln[3].z);
        lightNimbuscolor = glm::vec3(1.0f, 0.8745f, 0.0f);
        suncolor = glm::vec3(1.0f, 1.0f, 1.0f);
        lightCubeShader.setMat4("model", modeln);
        lightCubeShader.setVec3("lightColor", lightNimbuscolor);

        glBindVertexArray(vertexArraytor);
        glDrawElements(GL_QUADS, torind.size() * 4, GL_UNSIGNED_INT, 0);
        ///  Nimbus (torus) ----------------------

        modelshader.use();
        modelshader.setMat4("projection", projection);
        modelshader.setMat4("view", view);

        modelshader.setMat4("model", model);
        modelshader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);

        modelshader.setVec3("viewPos", cameraPos);
        modelshader.setVec3("material.ambient", 0.3f, 0.3f, 0.3f);
        modelshader.setVec3("material.diffuse", 0.50754f, 0.50754f, 0.50754f);
        modelshader.setVec3("material.specular", 0.508273f, 0.508273f, 0.508273f);
        modelshader.setFloat("material.shininess", 2.0f);
        modelshader.setVec3("light[0].lightColor", lightNimbuscolor);
        modelshader.setVec3("light[0].lightPos", lightNimbusPos);
        modelshader.setFloat("light[0].consta", 1.0f);
        modelshader.setFloat("light[0].line", 0.9f);
        modelshader.setFloat("light[0].sqrish", 0.32f);
        modelshader.setVec3("lightsun.direction", -lightPosst);
        modelshader.setVec3("lightsun.lightColor", suncolor);
        modelshader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        Shrek.Draw(modelshader);
        ///  ||| shrek and nimbus --------------------------------------------------------

        /// torus 1 and 2 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        shader2.use();
        shader2.setMat4("projection", projection);
        shader2.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -4.0f, 0.0f)); // translate it down so it's at the center of the scene
        angle = 0.0f + angleinputlr;
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        angle = 0.0f + angleinputud;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

        shader2.setMat4("model", model);
        shader2.setVec3("objectColor", 0.0f, 1.0f, 1.0f);
        shader2.setVec3("viewPos", cameraPos);
        shader2.setVec3("material.ambient", 0.4f, 0.4f, 0.4f);
        shader2.setVec3("material.diffuse", 0.50754f, 0.50754f, 0.50754f);
        shader2.setVec3("material.specular", 0.508273f, 0.508273f, 0.508273f);
        shader2.setFloat("material.shininess", 128.0f);
        shader2.setVec3("light[0].lightColor", lightNimbuscolor);
        shader2.setVec3("light[0].lightPos", lightNimbusPos);
        shader2.setFloat("light[0].consta", 1.0f);
        shader2.setFloat("light[0].line", 0.9f);
        shader2.setFloat("light[0].sqrish", 0.32f);
        shader2.setVec3("lightsun.direction", -lightPosst);
        shader2.setVec3("lightsun.lightColor", suncolor);
        shader2.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glBindVertexArray(vertexArraytor);
        glDrawElements(GL_QUADS, torind.size() * 4, GL_UNSIGNED_INT, 0);

        shader2.setVec3("objectColor", 1.0f, 0.0f, 1.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        angle = 0.0f + angleinputlr;
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        angle = 0.0f + angleinputud;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        shader2.setMat4("model", model);
        glBindVertexArray(vertexArraytor);
        glDrawElements(GL_QUADS, torind.size() * 4, GL_UNSIGNED_INT, 0);
        ///Torus ---------------------------------------------------------------------


        /// shpere in general direction of directional light, beams are parallel but sphere is close
        lightCubeShader.use();
        lightCubeShader.setVec3("lightColor", lightColor);
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);;
        model = glm::translate(model, lightPosst);
        model = glm::scale(model, glm::vec3(0.2f));
        lightCubeShader.setMat4("model", model);
        glBindVertexArray(vertexArray2);
        glDrawElements(GL_TRIANGLES, sfind.size() * 3, GL_UNSIGNED_INT, 0);
        /// spere light ------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;

}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraSpeed = 10.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        angleinputud += anglespeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        angleinputud -= anglespeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        angleinputlr += anglespeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        angleinputlr -= anglespeed;
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
        anglespeed += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
        anglespeed -= 0.1f;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        heightScale = 0.00006;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        heightScale = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}
