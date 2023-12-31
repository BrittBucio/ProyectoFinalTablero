#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	GLfloat getposicionx() { return posicionx; }
	GLfloat getAnguloLlanta() { return anguloLlanta; }
	GLfloat getAnguloCofre() { return anguloCofre; }
	GLboolean getApagaPointLight() { return apagaPointLight; }
	GLboolean getApagaPointLight2() { return apagaPointLight2; }
	GLboolean getApagaSpotLight() { return apagaSpotLight; }
	GLboolean getApagaSpotLight2() { return apagaSpotLight2; }
	GLboolean getMeterMoneda() { return meterMoneda; }
	GLboolean getTirarDado() { return tirarDado; }
	GLboolean getDespegaHelicoptero() { return despegaHelicoptero; }
	GLboolean getMoverFlipperInferiorIzq() { return moverFlipperInferiorIzq; }
	GLboolean getMoverFlipperInferiorDer() { return moverFlipperInferiorDer; }
	GLboolean getMoverFlipperSuperiorIzq() { return moverFlipperSuperiorIzq; }
	GLboolean getMoverFlipperSuperiorDer() { return moverFlipperSuperiorDer; }
	GLboolean getMouseFirstMoved() { return mouseFirstMoved; }
	GLboolean getAction() { return action; }
	
	int getnumCamara() { return numCamara; }

	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat posicionx = 0.0f;
	GLfloat anguloLlanta = 0.0f;
	GLfloat anguloCofre = 0.0f;
	GLboolean apagaPointLight;
	GLboolean apagaPointLight2;
	GLboolean apagaSpotLight;
	GLboolean apagaSpotLight2;
	GLboolean meterMoneda;
	GLboolean tirarDado;
	GLboolean despegaHelicoptero;
	GLboolean moverFlipperInferiorIzq;
	GLboolean moverFlipperInferiorDer;
	GLboolean moverFlipperSuperiorIzq;
	GLboolean moverFlipperSuperiorDer;

	GLboolean animacionPelota;
	GLboolean animacionPalanca;

	GLboolean action = false;
	int numCamara = 0;

	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
	static void ManejaMouseClick(GLFWwindow* window, int button, int action, int mods); //para mover la palanca

};

