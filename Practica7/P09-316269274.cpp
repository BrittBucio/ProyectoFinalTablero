/*
Semestre 2024-1
Práctica 7: Iluminación 1 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;



//variables para animación básica del dado
float movDado;
float movDadoOffset;
float movDadoZ;
float movDadoZOffset;
float rotDado;
float rotDadoOffset;
bool avanzaDado;
bool avanzaDadoZ;

//variables para animación básica del helicoptero
float movHelicoptero; //avanzar sobre y
float movHelicopteroOffset;
float movHelicopteroZ; //avanzar sobre z
float movHelicopteroZOffset;
float rotHelicoptero; //rotar sobre x
float rotHelicopteroOffset;
float rotHelicopteroHelices; //rotar helices
float rotHelicopteroHelicesOffset;
bool avanzaHelicoptero;
bool avanzaHelicopteroZ;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture dadoTexture;

Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M; //Modelo del helicoptero
Model Dado_M;  //Modelo del octaedro

Model helicoptero; //Modelo del helicoptero
Model heliceP;
Model heliceG;
Model helipuerto;


//Modelos de coches
Model cocheCuerpo;
Model llantasTraseras;
Model llantasDelanteras;
Model llantaDelantera1;
Model llantaDelantera2;
Model cofreTex;

//Modelos de la lampara
Model lamparaBase;
Model lamparaCubierta;

//Modelo de la street lamp
Model street_lamp;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
PointLight pointLights2[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
SpotLight spotLights2[MAX_SPOT_LIGHTS];

//Bandera para el movimiento del coche
bool frente = false;
float comparaz = 0.0f;
//variables para el manejo de luz por tiempo del helicóptero
int tiempoLuz = 0;
bool apagadoLuz = false;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
														//qué cara se va a iluminar
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, -1.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		-1.0f, 0.0f, 0.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	//calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5 );

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearOctaedro()
{
	unsigned int indices[] = {
		// front up
		0, 1, 2,
		// right up
		3,4,5,
		// back up
		6,7,8,
		// left up
		9,10,11,
		// front down
		12,13,14,
		// right down
		15,16,17,
		// back down
		18,19,20,
		// left down
		21,22,23

	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
	// average normals
	GLfloat cubo_vertices[] = {
		// front up
		//x       y       z       S       T			NX    NY     NZ
	   -0.3535f, 0.0f, 0.3535f, 0.26f,  0.51f, 0.0f, 0.0f, -1.0f,  //Num 8
		0.3535f, 0.0f, 0.3535f, 0.73f,  0.51f, 0.0f, 0.0f, -1.0f,
		0.0f,    0.5f,    0.0f, 0.5f,   0.74f, 0.0f, 0.0f, -1.0f,
		// right up
		0.3535f, 0.0f, -0.3535f, 0.95f,  0.74f, -1.0f, 0.0f, 0.0f, //Num 5
		0.3535f, 0.0f,  0.3535f, 0.74f,  0.51f, -1.0f, 0.0f,0.0f,
		0.0f,    0.5f,     0.0f, 0.51f,  0.74f, -1.0f, 0.0f, 0.0f,
		// back up
	   -0.3535f, 0.0f, -0.3535f, 0.02f, 0.76f, 1.0f, 0.0f, 0.0f,  //Num 4
		0.3535f,  0.0f, -0.3535f, 0.26f, 0.99f,1.0f, 0.0f, 0.0f,
		0.0f,     0.5f,     0.0f, 0.49f, 0.75f,1.0f, 0.0f, 0.0f,
		// left up
	   -0.3535f, 0.0f, -0.3535f, 0.06f, 0.71f, 1.0f, 0.0f, 0.0f, //Num 1 
	   -0.3535f, 0.0f,  0.3535f, 0.27f,0.53f,  1.0f, 0.0f, 0.0f,
		0.0f,     0.5f,    0.0f, 0.49f, 0.75f, 1.0f, 0.0f, 0.0f,
		// front down
	   -0.3535f,  0.0f, 0.3535f, 0.26f, 0.5f,  0.0f, 0.0f, -1.0f,
	    0.3535f, 0.0f,  0.3535f, 0.74f,  0.5f, 0.0f, 0.0f, -1.0f, //Num 3
		0.0f,    -0.5f,    0.0f, 0.49f, 0.26f, 0.0f, 0.0f, -1.0f,
		// right down
		0.3535f, 0.0f, -0.3535f, 0.95f, 0.28f, -1.0f, 0.0f, 0.0f, //Num 2
		0.3535f, 0.0f, 0.3535f,  0.75f, 0.48f, -1.0f, 0.0f,0.0f,
		0.0f,   -0.5f,    0.0f,  0.5f,  0.26f, -1.0f, 0.0f, 0.0f,
		// back down
	   -0.3535f,0.0f,  -0.3535f, 0.02f, 0.25f, 1.0f, 0.0f, 0.0f, //Num 7 
		0.3535f, 0.0f, -0.3535f, 0.24f, 0.02f, 1.0f, 0.0f, 0.0f,
		0.0f,   -0.5f,     0.0f, 0.49f, 0.25f, 1.0f, 0.0f, 0.0f,
		// left down
	   -0.3535f, 0.0f, -0.3535f, 0.0f, 0.25f,  1.0f, 0.0f, 0.0f, //Num 6 
	   -0.3535f, 0.0f,  0.3535f, 0.26f, 0.479f,1.0f, 0.0f, 0.0f,
		0.0f,    -0.5f,    0.0f, 0.479f, 0.26f,1.0f, 0.0f, 0.0f,



	};

	Mesh* octaedro = new Mesh();
	octaedro->CreateMesh(cubo_vertices, indices, 192, 24);
	meshList.push_back(octaedro);

	calcAverageNormals(indices, 12, cubo_vertices, 32, 8, 5);

}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearOctaedro();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	//Texturas del dado
	dadoTexture = Texture("Textures/dado_octaedro.tga");
	dadoTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");

	//Cargar modelos de coche
	cocheCuerpo = Model();
	cocheCuerpo.LoadModel("Models/ModeloCoche/coche_cuerpo.obj");
	llantasTraseras = Model();
	llantasTraseras.LoadModel("Models/ModeloCoche/llantas_traserasT.obj");
	llantaDelantera1 = Model();
	llantaDelantera1.LoadModel("Models/ModeloCoche/llanta_delanteraT_izq.obj");
	llantaDelantera2 = Model();
	llantaDelantera2.LoadModel("Models/ModeloCoche/llanta_delanteraT_der.obj");
	cofreTex = Model();
	cofreTex.LoadModel("Models/ModeloCoche/cofre_texturizado.obj");

	//Cargar modelos de la lampara
	lamparaBase = Model();
	lamparaBase.LoadModel("Models/lamparaBase.obj");
	lamparaCubierta = Model();
	lamparaCubierta.LoadModel("Models/cubiertaLamparaTexturizado.obj");

	//Modelos de la street lamp
	street_lamp = Model();
	street_lamp.LoadModel("Models/street light.obj");

	//Helicoptero
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	helicoptero = Model();
	helicoptero.LoadModel("Models/helicoptero1.obj");
	heliceP = Model();
	heliceP.LoadModel("Models/heliceP.obj");
	heliceG = Model();
	heliceG.LoadModel("Models/heliceG.obj");
	helipuerto = Model();
	helipuerto.LoadModel("Models/helipuerto.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,  //componente difusa, color de la luz
		0.0f, 0.0f, -1.0f); //qué tan intenso es el color del objeto
	//contador de luces puntuales

//----------------------------------------- LUCES PUNTUALES ---------------------------------------------------------------------------------
	unsigned int pointLightCount = 0;

	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 0.3f, //intensidad ambiental en 0, color de la luz 
		-6.0f, 1.5f, 1.5f, //vector de posición
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado, esto se va a investigar en el previo
	pointLightCount++;

	//Declaración de segunda luz de la lampara
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 5.0f, //intensidad ambiental en 0, color de la luz 
		60.0f, 6.5f, 0.5f, //vector de posición
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado, esto se va a investigar en el previo
	pointLightCount++;

	//Street_lamp luz puntual
	pointLights[2] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 5.0f, //intensidad ambiental en 0, color de la luz 
		66.0f, 12.5f, -5.0f, //vector de posición
		0.3f, 0.2f, 0.0f); //ecuación de segundo grado, esto se va a investigar en el previo
	pointLightCount++;

	pointLights2[0]= pointLights[0]; 
	pointLights2[1] = pointLights[2]; //luz street_lamp
	pointLights2[2] = pointLights[1]; //luz lampara

	//------------------------------------- LUCES SPOTLIGHT ---------------------------------------------------------------------------------

	unsigned int spotLightCount = 0;
	
	//linterna
	spotLights[0] = SpotLight(1.0f, 0.0f, 1.0f,  //no están atenuadas
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija --> segunda luz del coche (trasera)
	spotLights[1] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f, //ambiental, difuso
		10.0f, 0.0f, 0.0f, //posicion
		0.0f, 0.5f, -3.5f, //dirección a la que apunta
		1.0f, 0.3f, 0.1f, //
		5.0f); //angulo de apertura del cono
	spotLightCount++;

	//se crean mas luces puntuales y spotlight 

	//luz del faro
	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f, //ambiental, difuso
		0.0f, 10.0f, -15.0f, //posicion
		0.0f, -0.5f, 3.5f, //dirección a la que apunta
		1.0f, 0.0f, 0.0f, //
		25.0f); //angulo de apertura del cono
	spotLightCount++;

	//luz del helicoptero
	spotLights[3] = SpotLight(1.0f, 1.0f, 0.0f,
		1.0f, 2.0f, //ambiental, difuso
		5.0f, 40.0f, 6.0f, //posicion
		0.0f, -1.0f, 0.0f, //dirección a la que apunta
		1.0f, 0.0f, 0.0f, //
		30.0f); //angulo de apertura del cono
	spotLightCount++;

	/*for (int i = 0; i <= spotLightCount; i++) {
		spotLights2[i] = spotLights[i+1];
	}
	spotLights2[3] = spotLights[0];*/

	//spotLights2[0] = spotLights[1]; //luz fija
	//spotLights2[1] = spotLights[2]; //luz faro
	//spotLights2[2] = spotLights[3]; //luz helicóptero
	//spotLights2[3] = spotLights[0]; //luz linterna
	

	
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	
	movDado = 0.0f;
	movDadoOffset = 0.05f;
	movDadoZ = 0.0f;
	movDadoZOffset = 0.10f;
	rotDado = 0.0f;
	rotDadoOffset = 3.0f;
	avanzaDado = true;
	avanzaDadoZ = true;

	movHelicoptero = 0.0f;
	movHelicopteroOffset = 0.05f;
	movHelicopteroZ = 0.0f;
	movHelicopteroZOffset = 0.05f;
	rotHelicoptero = 0.0f;
	rotHelicopteroOffset = 5.0f;
	rotHelicopteroHelices = 0.0f;
	rotHelicopteroHelicesOffset = 5.0f;
	avanzaHelicoptero = true;	
	avanzaHelicopteroZ = true;


	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		

		// Animación del dado, primero se debe apretar una tecla (8) luego el dado debe caer al suelo y luego rotar hacia z 
		if (mainWindow.getTirarDado()) {
			if (avanzaDado)
			{
				if (movDado > -3.0f)
				{
					movDado -= movDadoOffset * deltaTime;
					
					if (movDado <= -2.999f) {
						movDado = -2.999f;
						if (avanzaDadoZ)
						{
							if (movDadoZ > -4.0f)
							{
								movDadoZ -= movDadoZOffset * deltaTime;
								rotDado += rotDadoOffset * deltaTime;
							}
							else
							{
								avanzaDadoZ = false;
							}
						}
						else
						{
							
						}
					}
				}
				else
				{
					avanzaDado = false;
				}
			}
			else
			{
				
			}
		}
		else {
			movDado = 0.0f;
			movDadoZ = 0.0f;
			rotDado = 0.0f - 1.0;
			avanzaDado = true;
			avanzaDadoZ = true;
		}
		
		//Animación para el helicoptero, primero despega, sube 7 unidades, avanza una distancia de 20 unidades, da vuelta sobre su eje y aterriza
		if (mainWindow.getDespegaHelicoptero()) {
			if (avanzaHelicoptero)
			{
				if (movHelicoptero < 8.0f)
				{
					movHelicoptero += movHelicopteroOffset * deltaTime;
					rotHelicopteroHelices -= rotHelicopteroHelicesOffset * deltaTime;
					if (movHelicoptero >= 7.0f) {
						if (avanzaHelicopteroZ)
						{
							movHelicoptero = 7.0f;
							if (movHelicopteroZ < 20.0f)
							{
								movHelicopteroZ += movHelicopteroZOffset * deltaTime;
								printf("avanza %f \n ", movHelicopteroZ);
								if (movHelicopteroZ >= 20.0f) {
					
									if (avanzaHelicoptero)
									{
										if (movHelicoptero > -3.0f)
										{
											movHelicoptero -= movHelicopteroOffset * deltaTime;
											
										}
										else
										{
											avanzaHelicoptero = false;
										}
									}

								}
								
							}
							else
							{
								avanzaHelicopteroZ = false;
							}
						}
						
					}
					
				}
				else
				{
					avanzaHelicoptero = false;
				}
			}
			else
			{
				if (movHelicoptero > 0.0f)
				{
					movHelicoptero -= movHelicopteroOffset * deltaTime;
					rotHelicopteroHelices += rotHelicopteroHelicesOffset * deltaTime;
					rotHelicoptero += rotHelicopteroOffset * deltaTime;
				}
				
			}
		}

		


		
		



		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f; //resta .3 unidades en y para que no quede la luz en el centro de la cámara
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		//arreglo auxiliar
		spotLights2[3].SetFlash(lowerLight, camera.getCameraDirection());
		//para el arreglo principal
		//spotLights[2].SetPos(glm::vec3(0.0f, 1.5f, mainWindow.getposicionx() + 18.0f));  //para el coche
		spotLights[3].SetPos(glm::vec3(0.0f, 40.0f, mainWindow.getmuevex() + 10.5f));	//para el helicoptero
		
		//para el arreglo auxiliar de luces
		//spotLights2[1].SetPos(glm::vec3(0.0f, 1.5f, mainWindow.getposicionx() + 18.0f)); //para el coche
		//spotLights2[2].SetPos(glm::vec3(0.0f, 40.0f, mainWindow.getmuevex() + 10.5f));   //para el helicoptero

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);

		//Apagar las luces puntuales
		if (!mainWindow.getApagaPointLight() && mainWindow.getApagaPointLight2()) {
			shaderList[0].SetPointLights(pointLights, pointLightCount - 1);
		}
		else if (!mainWindow.getApagaPointLight2() && mainWindow.getApagaPointLight()) {
			shaderList[0].SetPointLights(pointLights2, pointLightCount - 1);
		}
		else if (!mainWindow.getApagaPointLight2() && !mainWindow.getApagaPointLight()) {
			shaderList[0].SetPointLights(pointLights2, pointLightCount - 2);
		}
		else
			shaderList[0].SetPointLights(pointLights, pointLightCount);

		//Apagar luces de las spotlights, arreglo spotlights --> apagar helicoptero, arreg
		
		tiempoLuz++;
		if (tiempoLuz == 60) {
			apagadoLuz = !apagadoLuz ;
			tiempoLuz = 0;
		}
		if (apagadoLuz) {
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
		}
		else
			shaderList[0].SetSpotLights(spotLights, spotLightCount);

		//if (!mainWindow.getApagaSpotLight() && mainWindow.getApagaSpotLight2()) {
		//	shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
		//}
		//else if (!mainWindow.getApagaSpotLight2() && mainWindow.getApagaSpotLight()) {
		//	shaderList[0].SetSpotLights(spotLights2, spotLightCount - 1);
		//}
		//else if (!mainWindow.getApagaSpotLight2() && !mainWindow.getApagaSpotLight()) {
		//	shaderList[0].SetSpotLights(spotLights2, spotLightCount-2);
		//}
		//else
		//	shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Luces del coche según su movimiento
		if (comparaz > mainWindow.getposicionx() or (frente and comparaz == mainWindow.getposicionx())) {
			frente = true;
			//luz del faro
			spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
				1.0f, 2.0f, //ambiental, difuso
				0.0f, 10.0f, mainWindow.getposicionx(), //posicion
				0.0f, 0.5f, -3.5f, //dirección a la que apunta
				1.0f, 0.0f, 0.0f, //
				25.0f); //angulo de apertura del cono
			//spotLightCount++;
		}
		else if (comparaz < mainWindow.getposicionx() or  (!frente and comparaz == mainWindow.getposicionx())){
			frente = false;
			//luz del faro
			spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
				1.0f, 2.0f, //ambiental, difuso
				0.0f, 10.0f, mainWindow.getposicionx(), //posicion
				0.0f, -0.5f, 3.5f, //dirección a la que apunta
				1.0f, 0.0f, 0.0f, //
				25.0f); //angulo de apertura del cono
			//spotLightCount++;
		}
			comparaz = mainWindow.getposicionx();
		

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelAuxHel(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		
		//-------------------------- INICIA MODELO DEL OCTAEDRO -----------------------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 19.5f, 0.0f));
		model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
		model = glm::translate(model, glm::vec3(0.0, movDado, 0.0f));
		model = glm::translate(model, glm::vec3(0.0, 0.0, movDadoZ));
		//model = glm::rotate(model, glm::radians(rotDado), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotDado * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));  //prueba
		model = glm::rotate(model, glm::radians(rotDado), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotDado), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshList[4]->RenderMesh();

		//Instancia de la lampara
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(60.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lamparaBase.RenderModel();
		lamparaCubierta.RenderModel();

		//Instancia de la street lamp
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(60.0f, -2.0f, 40.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//street_lamp.RenderModel();


		// *      INICIA MODELO DE COCHE     *
		//Cuerpo del coche, sin llantas ni cofre
		// Teclas: R y T para mover el coche, 1 y 2 para abrir y cerrar el cofre, V y B para mover las llantas

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -1.5f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, mainWindow.getposicionx()));
		modelaux = model;
		color = glm::vec3(0.1f, 0.3f, 0.73f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cocheCuerpo.RenderModel();

		//modelaux = model;
		modelaux = translate(modelaux, glm::vec3(0.0f, 0.0f, 3.5f));

		//Llantas traseras
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.036f, 5.384f, 15.479f));
		model = glm::rotate(model, glm::radians(mainWindow.getAnguloLlanta()), glm::vec3(1.0f, 0.0f, 0.0f));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantasTraseras.RenderModel();

		//Llanta del izq
		model = modelaux;
		model = glm::translate(model, glm::vec3(-10.284f, 5.312f, -18.739f));
		model = glm::rotate(model, glm::radians(mainWindow.getAnguloLlanta()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaDelantera1.RenderModel();

		//Lanta del der
		model = modelaux;
		model = glm::translate(model, glm::vec3(10.361f, 5.312f, -18.739f));
		model = glm::rotate(model, glm::radians(mainWindow.getAnguloLlanta()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaDelantera2.RenderModel();


		modelaux = translate(modelaux, glm::vec3(0.0f, 0.0f, -3.5f));
		//Cofre
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.298f, 11.897f, 10.884f));
		model = glm::rotate(model, glm::radians(mainWindow.getAnguloCofre()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-4.298f, -11.897f, -10.884f));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cofreTex.RenderModel();
	
		//Instancia del helicoptero (para mover con Y y U)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 40.0f, 6.0));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, mainWindow.getmuevex(), 0.0f));
		model = glm::scale(model, glm::vec3(3.5f, 3.5f, 3.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel(); 

		//-------------------------------------- HELICOPTERO ESTRUCTURA HEREDADA ------------------------------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0));
		model = glm::scale(model, glm::vec3(3.5f, 3.5f, 3.5f));
		model = glm::translate(model, glm::vec3(0.0, movHelicoptero, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, movHelicopteroZ));
		model = glm::translate(model, glm::vec3(30.02f, 1.605f, -3.31f));
		model = glm::rotate(model, rotHelicoptero * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-30.02f, -1.605f, 3.31f));
		modelAuxHel = model;
		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		helicoptero.RenderModel();

		//Helice principal
		
		model = modelAuxHel;
		model = glm::translate(model, glm::vec3(30.017f, 2.797f, 3.209f));
		model = glm::rotate(model, rotHelicopteroHelices * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		heliceG.RenderModel();

		//Helice de cola
		model = modelAuxHel;
		model = glm::translate(model, glm::vec3(29.819f, 3.251f, -7.361f));
		model = glm::rotate(model, rotHelicopteroHelices * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		heliceP.RenderModel();

		//Helipuerto
		model = glm::mat4(1.0); 
		model = glm::scale(model, glm::vec3(3.5f, 3.5f, 3.5f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		helipuerto.RenderModel();
		
		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35.0f, 4.0f, 4.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
