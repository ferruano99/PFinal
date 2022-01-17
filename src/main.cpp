#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shaders.h"
#include "Model.h"
#include "Texture.h"
#include <cmath>

#define PI 3.14159265

void configScene();

void renderScene();


//FUNCIÓN LUCES
void setLights(glm::mat4 P, glm::mat4 V);


//FUNCIONES CALLBACKS
void funFramebufferSize(GLFWwindow *window, int width, int height);

void funKey(GLFWwindow *window, int key, int scancode, int action, int mods);

void funScroll(GLFWwindow *window, double xoffset, double yoffset);

void funCursorPos(GLFWwindow *window, double xpos, double ypos);


//FUNCIONES OBJETOS
void drawObjectMat(Model model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M);

void drawObjectTex(Model model, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M);


//PLANO VERTICAL Y HORIZONTAL
void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M);


//DIBUJO DE LA NORIA




//Sustento noria (patas sustento principal)
void drawPataNoria(glm::mat4 P, glm::mat4 V, glm::mat4 M);

void drawSustento(glm::mat4 P, glm::mat4 V, glm::mat4 M);

//Toroide grande
void drawToroideNoria(glm::mat4 P, glm::mat4 V, glm::mat4 M);

//Interior de la noria
void drawTriangulo(glm::mat4 P, glm::mat4 V, glm::mat4 M);

void drawInterior(glm::mat4 P, glm::mat4 V, glm::mat4 M);


//CABINA
void drawCabina(glm::mat4 P, glm::mat4 V, glm::mat4 M);

void drawCabinasEnToroide(glm::mat4 P, glm::mat4 V, glm::mat4 M);

//Noria final
void juntarPartesToroide(glm::mat4 P, glm::mat4 V, glm::mat4 M);

void drawNoria(glm::mat4 P, glm::mat4 V, glm::mat4 M);


// Shaders
Shaders shaders;

//Materiales y luces
#define   NLD 1
#define   NLP 1
#define   NLF 1
Light lightG;
Light lightD[NLD];
Light lightP[NLP];
Light lightF[NLF];
float RPos = 0;

Material rubyo, rubytrans, cyan, gold, emerald, pbronze;
Textures floor1, heliceTrasera, cuerpo, mluz;


//Imágenes (texturas)
Texture img1, img2, img3, img4, img5, img6, img7;


// Modelos
Model plane, cilindro, esfera, cono, torus, triangulo, cabin;
// Viewport
int w = 500;
int h = 500;


//zoom de la cámara
float fovy = 50.0;
//Movimiento de la cámara
float alphaX = 40.0;
float alphaY = 25.0;

int main() {

    // Inicializamos GLFW
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Creamos la ventana
    GLFWwindow *window;
    window = glfwCreateWindow(w, h, "Sesion 5", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); //Para las hélices y las animaciones

    // Inicializamos GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        return false;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    const GLubyte *oglVersion = glGetString(GL_VERSION);
    std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;

    //
    // usamos los CallBacks
    glfwSetFramebufferSizeCallback(window, funFramebufferSize); //OBLIGATORIO
    glfwSetKeyCallback(window, funKey);
    glfwSetScrollCallback(window, funScroll);
    glfwSetCursorPosCallback(window, funCursorPos);


    // Entramos en el bucle de renderizado
    configScene();
    while (!glfwWindowShouldClose(window)) {
        renderScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void configScene() {

    // Test de profundidad
    glEnable(GL_DEPTH_TEST);
    glPolygonOffset(1.0, 1.0);

    //Transparecias
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Shaders
    shaders.initShaders("resources/shaders/vshader.glsl", "resources/shaders/fshader.glsl");

    // Modelos
    plane.initModel("resources/models/plane.obj");
    cilindro.initModel("resources/models/cylinder.obj");
    esfera.initModel("resources/models/sphere.obj");
    cono.initModel("resources/models/cone.obj");
    torus.initModel("resources/models/thin_torus.obj");
    triangulo.initModel("resources/models/triangle.obj");
    cabin.initModel("resources/models/cabinaMod.obj");


    // Texturas (imágenes) QUITABLE
    img1.initTexture("resources/textures/img1.png");
    img2.initTexture("resources/textures/img2.png");
    img3.initTexture("resources/textures/img3.png");
    img4.initTexture("resources/textures/img4.png");
    img5.initTexture("resources/textures/img5.png");
    img6.initTexture("resources/textures/img6.png");
    img7.initTexture("resources/textures/img7.png");

    // Luz de ambiente global AÑADIBLE
    lightG.ambient = glm::vec3(0.5, 0.5, 0.5);

    //Luces direccionales AÑADIBLES
    lightD[0].direction = glm::vec3(0, -1.0, 0.0);
    lightD[0].ambient = glm::vec3(0.1, 0.1, 0.1);
    lightD[0].diffuse = glm::vec3(0.7, 0.7, 0.7);
    lightD[0].specular = glm::vec3(0.7, 0.7, 0.7);

    //Luces posicionales AÑADIBLES
    lightP[0].position = glm::vec3(-1.5, 0.2, 0.0);
    lightP[0].ambient = glm::vec3(0.5, 0.5, 0.5);
    lightP[0].diffuse = glm::vec3(0.9, 0.9, 0.9);
    lightP[0].specular = glm::vec3(0.9, 0.9, 0.9);
    lightP[0].c0 = 1.00;
    lightP[0].c1 = 0.22;
    lightP[0].c2 = 0.20;

    // Luces focales AÑADIBLES

    lightF[0].position = glm::vec3(3.0, 3.0, 4.0);
    lightF[0].direction = glm::vec3(0.0, 0.0, 0.0);
    lightF[0].ambient = glm::vec3(0.2, 0.2, 0.2);
    lightF[0].diffuse = glm::vec3(0.9, 0.9, 0.9);
    lightF[0].specular = glm::vec3(0.9, 0.9, 0.9);
    lightF[0].innerCutOff = 0.4;
    lightF[0].outerCutOff = lightF[0].innerCutOff + 0.8;
    lightF[0].c0 = 1.000;
    lightF[0].c1 = 0.090;
    lightF[0].c2 = 0.032;

    // Materiales AÑADIBLES y QUITABLES
    rubyo.ambient = glm::vec4(0.1745f, 0.01175f, 0.01175f, 1.0f);
    rubyo.diffuse = glm::vec4(0.61424f, 0.04136f, 0.04136f, 1.0f);
    rubyo.specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
    rubyo.shininess = 76.8f;
    rubyo.emissive = glm::vec4(0, 0, 0, 1);

    rubytrans.ambient = glm::vec4(0.1745f, 0.01175f, 0.01175f, 0.55f);
    rubytrans.diffuse = glm::vec4(0.61424f, 0.04136f, 0.04136f, 0.55f);
    rubytrans.specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 0.55f);
    rubytrans.shininess = 76.8f;
    rubytrans.emissive = glm::vec4(0, 0, 0, 1);

    pbronze.ambient = glm::vec4(0.25f, 0.148f, 0.06475f, 1.0f);
    pbronze.diffuse = glm::vec4(0.4f, 0.2368f, 0.1036f, 1.0f);
    pbronze.specular = glm::vec4(0.774597f, 0.458561f, 0.200621f, 1.0f);
    pbronze.shininess = 76.8f;
    pbronze.emissive = glm::vec4(0, 0, 0, 1);


    gold.ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
    gold.diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0);
    gold.specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
    gold.shininess = 51.2f;
    gold.emissive = glm::vec4(0, 0, 0, 1);
    //comit
    emerald.ambient = glm::vec4(0.0215f, 0.1745f, 0.0215f, 0.55f);
    emerald.diffuse = glm::vec4(0.07568f, 0.61424f, 0.07568f, 0.55f);
    emerald.specular = glm::vec4(0.633f, 0.727811f, 0.633f, 0.55f);
    emerald.shininess = 76.8f;
    emerald.emissive = glm::vec4(0, 0, 0, 1);

    cyan.ambient = glm::vec4(0.0f, 0.05f, 0.05f, 1.0f);
    cyan.diffuse = glm::vec4(0.4f, 0.5f, 0.5f, 1.0f);
    cyan.specular = glm::vec4(0.04f, 0.7f, 0.7f, 1.0f);
    cyan.shininess = 10.0f;
    cyan.emissive = glm::vec4(0, 0, 0, 1);


    //Textura (material) AÑADIBLE Y QUITABLE


    mluz.diffuse = img4.getTexture();
    mluz.specular = img4.getTexture();
    mluz.emissive = img1.getTexture();
    mluz.normal = 0;
    mluz.shininess = 10;

    cuerpo.diffuse = img6.getTexture();
    cuerpo.specular = img6.getTexture();
    cuerpo.emissive = img1.getTexture();
    cuerpo.normal = img7.getTexture();
    cuerpo.shininess = 10;

    floor1.diffuse = img5.getTexture();
    floor1.specular = img3.getTexture();
    floor1.emissive = img2.getTexture();
    floor1.normal = 0;
    floor1.shininess = 10;

    heliceTrasera.diffuse = img4.getTexture();
    heliceTrasera.specular = img4.getTexture();
    heliceTrasera.emissive = img4.getTexture();
    heliceTrasera.normal = 0;
    heliceTrasera.shininess = 10;


}

void setLights(glm::mat4 P, glm::mat4 V) { //OBLIGATORIO
    shaders.setLight("ulightG", lightG);
    for (int i = 0; i < NLD; i++) shaders.setLight("ulightD[" + toString(i) + "]", lightD[i]);
    for (int i = 0; i < NLP; i++) shaders.setLight("ulightP[" + toString(i) + "]", lightP[i]);
    for (int i = 0; i < NLF; i++) shaders.setLight("ulightF[" + toString(i) + "]", lightF[i]);

    for (int i = 0; i < NLP; i++) {

        glm::mat4 R = glm::rotate(I, glm::radians(RPos), glm::vec3(0, 1, 0));
        glm::mat4 M = glm::translate(I, lightP[i].position) * glm::scale(I, glm::vec3(0.5 * 0.1));
        drawObjectTex(esfera, mluz, P, V, R * M);
    }

    for (int i = 0; i < NLF; i++) {
        glm::mat4 M = glm::translate(I, lightF[i].position) * glm::scale(I, glm::vec3(0.5 * 0.1));
        drawObjectTex(esfera, mluz, P, V, M);
    }
}

void renderScene() {

    // Borramos el buffer de color
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Indicamos los shaders a utilizar
    shaders.useShaders();

    // Matriz P
    float nplane = 0.1;
    float fplane = 50.0;
    float aspect = (float) w / (float) h;
    glm::mat4 P = glm::perspective(glm::radians(fovy), aspect, nplane, fplane);

    // Matriz V
    float x = 10.0f * glm::cos(glm::radians(alphaY)) * glm::sin(glm::radians(alphaX));
    float y = 10.0f * glm::sin(glm::radians(alphaY));
    float z = 10.0f * glm::cos(glm::radians(alphaY)) * glm::cos(glm::radians(alphaX));

    glm::vec3 eye(x, y, z);
    glm::vec3 center(0, 0, 0.0);
    glm::vec3 up(0, 1, 0);
    glm::mat4 V = glm::lookAt(eye, center, up);

    //Fijamos las luces
    setLights(P, V);

    // Dibujamos la escena
    drawSuelo(P, V, I);
    drawNoria(P, V, I);
}

void drawObjectMat(Model model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M) { //OBLIGATORIO

    shaders.setMat4("uN", glm::transpose(glm::inverse(M)));
    shaders.setMat4("uM", M);
    shaders.setMat4("uPVM", P * V * M);
    shaders.setBool("uWithMaterials", true);
    shaders.setMaterial("umaterial", material);
    model.renderModel(GL_FILL);


}

void drawObjectTex(Model model, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M) { //OBLIGATORIO

    shaders.setMat4("uN", glm::transpose(glm::inverse(M)));
    shaders.setMat4("uM", M);
    shaders.setMat4("uPVM", P * V * M);
    shaders.setBool("uWithMaterials", false);
    shaders.setTextures("utextures", textures);
    if (textures.normal != 0) shaders.setBool("uWithNormals", true);
    else shaders.setBool("uWithNormals", false);
    model.renderModel(GL_FILL);

}

void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M) {


    glm::mat4 S = glm::scale(I, glm::vec3(7.2, 1.0, 7.2));
    drawObjectMat(plane, gold, P, V, M * S);

}

void funFramebufferSize(GLFWwindow *window, int width, int height) {

    // Configuracion del Viewport
    glViewport(0, 0, width, height);

    // Actualizacion de w y h
    w = width;
    h = height;


}


void funKey(GLFWwindow *window, int key, int scancode, int action, int mods) {

}

void funScroll(GLFWwindow *window, double xoffset, double yoffset) {
    if (yoffset < 0) { //Movimiento de la rueda del ratón
        if (fovy < 90)
            fovy += 5;
    } else {
        if (fovy > 10)
            fovy -= 5;
    }
}

void funCursorPos(GLFWwindow *window, double xpos, double ypos) {

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        return; //para que funcione cada vez que estemos pulsando el click izq
    }
    float limY = 89.0;
    alphaX = 90.0 * (2.0 * xpos / (float) w - 1.0);
    alphaY = 90.0 * (1.0 - 2.0 * ypos / (float) h);
    if (alphaY < -limY) alphaY = -limY;
    if (alphaY > limY) alphaY = limY;

}

void drawToroideNoria(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    glm::mat4 Rx = glm::rotate(I, glm::radians(90.0f), glm::vec3(1, 0, 0));

    glm::mat4 S = glm::scale(I, glm::vec3(3, 1, 3));

    drawObjectMat(torus, rubyo, P, V, M * Rx * S);
}

void drawPataNoria(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    glm::mat4 S = glm::scale(I, glm::vec3(0.1, 3, 0.1));
    glm::mat4 T = glm::translate(I, glm::vec3(0, -3, 0));
    glm::mat4 Rx = glm::rotate(I, glm::radians(25.0f), glm::vec3(1, 0, 0));
    drawObjectMat(cilindro, cyan, P, V, M * Rx * T * S);
}

void drawSustento(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    //Rotación de -50º para hacer la pata simétrica (en la original se rota 25º)
    glm::mat4 R = glm::rotate(I, glm::radians(-50.0f), glm::vec3(1, 0, 0));
    drawPataNoria(P, V, M * R);
    drawPataNoria(P, V, M);
}

void drawTriangulo(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    glm::mat4 S = glm::scale(I, glm::vec3(0.1, 1.5, 0.1));
    glm::mat4 T = glm::translate(I, glm::vec3(0, -1.5, 0));
    drawObjectMat(triangulo, emerald, P, V, M * T * S);
}

void drawInterior(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    for (int i = 0; i < 25; ++i) {
        glm::mat4 R = glm::rotate(I, glm::radians(i * 360.0f / 25), glm::vec3(0, 0, 1));
        drawTriangulo(P, V, M * R);
    }
}

void drawCabina(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    glm::mat4 S = glm::scale(I, glm::vec3(0.2, 0.2, 0.2));
    glm::mat4 R = glm::rotate(I, glm::radians(90.0f), glm::vec3(1, 0, 0));
    drawObjectMat(cabin, pbronze, P, V, M * R * S);

}

void drawCabinasEnToroide(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    glm::mat4 T = glm::translate(I, glm::vec3(0, 3 + 0.2, 0));

    for (int i = 0; i < 15; ++i) {
        glm::mat4 R = glm::rotate(I, glm::radians(i * 360.0f / 15), glm::vec3(0, 0, 1));
        drawCabina(P, V, M * R * T);
    }
}

void juntarPartesToroide(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    drawToroideNoria(P, V, M);
    drawCabinasEnToroide(P, V, M);
    drawInterior(P, V, M);
}

void drawNoria(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    //Calculamos con pitágoras la longitud en el eje y de la pata para ponerla encima del plano
    float longpata = 6.0f * cos(25.0f * PI / 180.0f);
    glm::mat4 T = glm::translate(I, glm::vec3(0, longpata, 0));
    drawSustento(P, V, M * T);
    juntarPartesToroide(P, V, M * T);
}