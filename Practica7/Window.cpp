#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	muevex = 2.0f;
	apagaPointLight = true;
	apagaPointLight2 = true;
	apagaSpotLight = true;
	apagaSpotLight2 = true;
	meterMoneda = false;
	tirarDado = false;
	despegaHelicoptero = false;
	moverFlipperInferiorIzq = false;
	moverFlipperInferiorDer = false;
	moverFlipperSuperiorIzq = false;
	moverFlipperSuperiorDer = false;


	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Practica09:Animación básica", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}




void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow-> muevex += 10.0;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow-> muevex -= 10.0;
	}

	if (key == GLFW_KEY_R)
	{
		theWindow->posicionx += 10.0;
	}
	if (key == GLFW_KEY_T)
	{
		theWindow->posicionx -= 10.0;
	}

	if (key == GLFW_KEY_1 && theWindow->anguloCofre < 0.0)
	{
		theWindow->anguloCofre += 10.0;
	}
	else if (key == GLFW_KEY_2 && theWindow->anguloCofre > -40.0)
	{
		theWindow->anguloCofre -= 10.0;
	}


	if (key == GLFW_KEY_B)
	{
		theWindow->anguloLlanta -= 10.0;
	}

	if (theWindow->anguloLlanta >= 360 || theWindow->anguloLlanta <= -360)
	{
		theWindow->anguloLlanta = 0.0;
	}

	//Nuevas condicionales (apagado de luces)
	if (key == GLFW_KEY_3 && theWindow->apagaPointLight == false && action == GLFW_PRESS)
	{
		theWindow->apagaPointLight = true;
	}
	else if (key == GLFW_KEY_3 && theWindow->apagaPointLight == true && action == GLFW_PRESS)
	{
		theWindow->apagaPointLight = false;
	}

	if (key == GLFW_KEY_4 && theWindow->apagaPointLight2 == false && action == GLFW_PRESS)
	{
		theWindow->apagaPointLight2 = true;
	}
	else if (key == GLFW_KEY_4 && theWindow->apagaPointLight2 == true && action == GLFW_PRESS)
	{
		theWindow->apagaPointLight2 = false;
	}

	//Interruptor de luz spot

	if (key == GLFW_KEY_5 && theWindow->apagaSpotLight == false && action == GLFW_PRESS)
	{
		theWindow->apagaSpotLight = true;
	}
	else if (key == GLFW_KEY_5 && theWindow->apagaSpotLight == true && action == GLFW_PRESS)
	{
		theWindow->apagaSpotLight = false;
	}

	if (key == GLFW_KEY_6 && theWindow->apagaSpotLight2 == false && action == GLFW_PRESS)
	{
		theWindow->apagaSpotLight2 = true;
	}
	else if (key == GLFW_KEY_6 && theWindow->apagaSpotLight2 == true && action == GLFW_PRESS)
	{
		theWindow->apagaSpotLight2 = false;
	}

	//Condicionales para meter moneda y tirar dado
	if (key == GLFW_KEY_H && theWindow->meterMoneda == false && action == GLFW_PRESS)
	{
		theWindow->meterMoneda = true;
	}
	else if (key == GLFW_KEY_H && theWindow->meterMoneda == true && action == GLFW_PRESS)
	{
		theWindow->meterMoneda = false;
	}

	if (key == GLFW_KEY_8 && theWindow->tirarDado == false && action == GLFW_PRESS)
	{
		theWindow->tirarDado = true;
	}
	else if (key == GLFW_KEY_8 && theWindow->tirarDado == true && action == GLFW_PRESS)
	{
		theWindow->tirarDado = false;
	}

	//Condicionales para despegar helicoptero
	if (key == GLFW_KEY_9 && theWindow->despegaHelicoptero == false && action == GLFW_PRESS)
	{
		theWindow->despegaHelicoptero = true;
	}
	else if (key == GLFW_KEY_9 && theWindow->despegaHelicoptero == true && action == GLFW_PRESS)
	{
		theWindow->despegaHelicoptero = false;
	}

	


	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	//Agregar funcionalidad para hacer que la palanca se mueva con el mouse

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
