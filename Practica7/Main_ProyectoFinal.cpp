/* Semestre 2024-1
Animación por keyframes
La textura del skybox fue conseguida desde la página https ://opengameart.org/content/elyvisions-skyboxes?page=1
y edité en Gimp rotando 90 grados en sentido antihorario la imagen  sp2_up.png para poder ver continuidad.
Fuentes :
	https ://www.khronos.org/opengl/wiki/Keyframe_Animation
	http ://what-when-how.com/wp-content/uploads/2012/07/tmpcd0074_thumb.png
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

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;

//variables para keyframes
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;

//variables para animación simple de la pelota
float movPelota;
float movPelotaOffset;
float rotPelota;
float rotPelotaOffset;
bool avanzaPelota;

//variables para animación simple de la moneda
float movMoneda;
float movMonedaOffset;
float rotMoneda;
float rotMonedaOffset;
bool avanzaMoneda;

//variables para animación simple de la palanca
float movPalanca;
float movPalancaOffset;
float rotPalanca;
float rotPalancaOffset;
bool avanzaPalanca;



int tiempoLuz = 0;
bool apagadoLuz = false;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera * camera;
Camera cameraLibre;
Camera cameraPinball;
Camera cameraIsometrica;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;



Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;

Skybox skybox;

//Modelos del tablero de pinball
Model tablero_pinball;
Model cuerpo;
Model frente;
Model pantalla_marco;
Model base_monedas;
Model base_monedas_amarilla;
Model moneda_entrada;
Model boton_rojo;
Model vidrio;
Model patas;
Model resorte;
Model soporte_resorte;
Model palanca;
Model base_tablero;
Model base_segunda_capa;

//Obstáculos del tablero de pinball
Model objeto_animado_ins1;
Model objeto_animado_ins2;
Model objeto_animado_ins3;
Model obstaculo_medio_tubo;
Model obstaculo_rejilla;
Model paleta_superior_der;
Model paleta_superior_izq;
Model paleta_inferior_der;
Model paleta_inferior_izq;
Model rampa;
Model tornillos_morados;
Model tornillos_morados2;


Model canica;
Model canica2;
Model moneda;

//Modelos del Avatar
Model anya_cabeza;
Model anya_cuerpo;
Model anya_manos;
Model anya_pierna_der;
Model anya_pierna_izq;


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
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//función para teclado de keyframes 
void inputKeyframesPelota(bool* keys);
void inputKeyframesAnya(bool* keys);

//cálculo del promedio de las normales para sombreado de Phong
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
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;
bool animacion2 = false;


//NEW// Keyframes


float posXpelota = 0.0 , posYpelota = 0.0, posZpelota = 0.0;
float	movPelota_x = 0.0f, movPelota_y = 0.0f, movPelota_z = 0.0f;
float rotacionPelota = 0.0f;

// Para Camara
int numCamara = 0;

//Para animación del avatar

float posXanya = 0.0, posYanya = 0.0, posZanya = 0.0;
float	movAnya_x = 0.0f, movAnya_y = 0.0f, movAnya_z = 0.0f;
float rotacionAnya = 0.0f;


#define MAX_FRAMES 100
int i_max_steps = 90;
int i_curr_stepsPelota = 4, i_curr_stepsAnya = 4;

//Ruta de la pelota: moverse hacia z negativa (subir), hacer una curva para poder salir del carril y entrar al tablero de pinball, caer hacia el hueco debajo de las paletas
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movPelota_x;		//Variable para PosicionX
	float movPelota_y;		//Variable para PosicionY
	float movPelota_z;		//Variable para PosicionZ
	float movPelota_xInc;	//Variable para IncrementoX
	float movPelota_yInc;	//Variable para IncrementoY
	float movPelota_zInc;	//Variable para IncrementoZ
	float rotacionPelota;	//Variable para Rotacion
	float rotacionPelotaInc;//Variable para Incremento de Rotacion

	float movAnya_x;		//Variable para PosicionX
	float movAnya_y;		//Variable para PosicionY
	float movAnya_z;		//Variable para PosicionZ
	float movAnya_xInc;	//Variable para IncrementoX
    float movAnya_yInc;	//Variable para IncrementoY
	float movAnya_zInc;	//Variable para IncrementoZ
	float rotacionAnya;		//Variable para Rotacion
	float rotacionAnyaInc;	//Variable para Incremento de Rotacion

}FRAME;

FRAME KeyFramePelota[MAX_FRAMES];
FRAME KeyFrameAnya[MAX_FRAMES];
int FrameIndexPelota = 10, FrameIndexAnya = 5;			//introducir datos
bool playPelota = false, playAnya = false;
int playIndexPelota = 0, playIndexAnya = 0;

void saveFramePelota(void) //tecla L
{

	printf("frameindexPelota %d\n", FrameIndexPelota);


	KeyFramePelota[FrameIndexPelota].movPelota_x = movPelota_x;
	KeyFramePelota[FrameIndexPelota].movPelota_y = movPelota_y;
	KeyFramePelota[FrameIndexPelota].movPelota_z = movPelota_z;
	KeyFramePelota[FrameIndexPelota].rotacionPelota;//completar
	 
	FrameIndexPelota++;
}

void saveFrameAnya(void) //tecla L
{

	printf("frameindexAnya %d\n", FrameIndexAnya);

	KeyFrameAnya[FrameIndexAnya].movAnya_x = movAnya_x;
	KeyFrameAnya[FrameIndexAnya].movAnya_y = movAnya_y;
	KeyFrameAnya[FrameIndexAnya].movAnya_z = movAnya_z;
	KeyFrameAnya[FrameIndexAnya].rotacionAnya;//completar

	FrameIndexAnya++;
}

void resetElementsPelota(void) //Tecla 0
{
	movPelota_x = KeyFramePelota[0].movPelota_x;
	movPelota_y = KeyFramePelota[0].movPelota_y;
	movPelota_z = KeyFramePelota[0].movPelota_z;
	rotacionPelota = KeyFramePelota[0].rotacionPelota;
}

void resetElementsAnya(void) {
	movAnya_x = KeyFrameAnya[0].movAnya_x;
	movAnya_y = KeyFrameAnya[0].movAnya_y;
	movAnya_z = KeyFrameAnya[0].movAnya_z;
	rotacionAnya = KeyFrameAnya[0].rotacionAnya;
}

void interpolationPelota(void)
{
	KeyFramePelota[playIndexPelota].movPelota_xInc = (KeyFramePelota[playIndexPelota + 1].movPelota_x - KeyFramePelota[playIndexPelota].movPelota_x) / i_max_steps;
	KeyFramePelota[playIndexPelota].movPelota_yInc = (KeyFramePelota[playIndexPelota + 1].movPelota_y - KeyFramePelota[playIndexPelota].movPelota_y) / i_max_steps;
	KeyFramePelota[playIndexPelota].movPelota_zInc = (KeyFramePelota[playIndexPelota + 1].movPelota_z - KeyFramePelota[playIndexPelota].movPelota_z) / i_max_steps;
	KeyFramePelota[playIndexPelota].rotacionPelotaInc = (KeyFramePelota[playIndexPelota + 1].rotacionPelota - KeyFramePelota[playIndexPelota].rotacionPelota) / i_max_steps;
}

void interpolationAnya(void) {
	KeyFrameAnya[playIndexAnya].movAnya_xInc = (KeyFrameAnya[playIndexAnya + 1].movAnya_x - KeyFrameAnya[playIndexAnya].movAnya_x) / i_max_steps;
	KeyFrameAnya[playIndexAnya].movAnya_yInc = (KeyFrameAnya[playIndexAnya + 1].movAnya_y - KeyFrameAnya[playIndexAnya].movAnya_y) / i_max_steps;
	KeyFrameAnya[playIndexAnya].movAnya_zInc = (KeyFrameAnya[playIndexAnya + 1].movAnya_z - KeyFrameAnya[playIndexAnya].movAnya_z) / i_max_steps;
	KeyFrameAnya[playIndexAnya].rotacionAnyaInc = (KeyFrameAnya[playIndexAnya + 1].rotacionAnya - KeyFrameAnya[playIndexAnya].rotacionAnya) / i_max_steps;
}


void animatePelota(void)
{
	//Movimiento del objeto con barra espaciadora
	if (playPelota)
	{
		if (i_curr_stepsPelota >= i_max_steps) //fin de animación entre frames?
		{
			playIndexPelota++;
			printf("playindexPelota : %d\n", playIndexPelota);
			if (playIndexPelota > FrameIndexPelota - 2)	//Fin de toda la animación con último frame?
			{
				/*printf("Frame index= %d\n", FrameIndex);
				printf("termino la animacion\n");*/
				playIndexPelota = 0;
				playPelota = false;
			}
			else //Interpolación del próximo cuadro
			{
				
				i_curr_stepsPelota = 0; //Resetea contador
				//Interpolar
				interpolationPelota();
			}
		}
		else
		{
			//Dibujar Animación
			movPelota_x += KeyFramePelota[playIndexPelota].movPelota_xInc;
			movPelota_y += KeyFramePelota[playIndexPelota].movPelota_yInc;
			movPelota_z += KeyFramePelota[playIndexPelota].movPelota_zInc;
			rotacionPelota += KeyFramePelota[playIndexPelota].rotacionPelotaInc;
			i_curr_stepsPelota++;
		}
	

	}
}

void animateAnya(void)
{
	//Movimiento del objeto con barra espaciadora
	if (playAnya)
	{
		
		if (i_curr_stepsAnya >= i_max_steps) //fin de animación entre frames?
		{
			playIndexAnya++;
			printf("playindexAnya : %d\n", playIndexAnya);
			if (playIndexAnya > FrameIndexAnya - 2)	//Fin de toda la animación con último frame?
			{
				/*printf("Frame index= %d\n", FrameIndex);
				printf("termino la animacion\n");*/
				playIndexAnya = 0;
				playAnya = false;
			}
			else //Interpolación del próximo cuadro
			{

				i_curr_stepsAnya = 0; //Resetea contador
				//Interpolar
				interpolationAnya();
			}
		}
		else
		{
			//Dibujar Animación

			movAnya_x += KeyFrameAnya[playIndexAnya].movAnya_xInc;
			movAnya_y += KeyFrameAnya[playIndexAnya].movAnya_yInc;
			movAnya_z += KeyFrameAnya[playIndexAnya].movAnya_zInc;
			rotacionAnya += KeyFrameAnya[playIndexAnya].rotacionAnyaInc;
			i_curr_stepsAnya++;
		}

	}
}

///////////////* FIN KEYFRAMES*////////////////////////////



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	cameraLibre = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);
	cameraIsometrica = Camera(glm::vec3(-9.728f, 31.789f, 3.15f), glm::vec3(0.0, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);
	cameraPinball = Camera(glm::vec3(posXpelota + movPelota_x + 5.652f, posYpelota + movPelota_y + 17.172f + 0.25f, posZpelota + movPelota_z + 4.064f - 0.25f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

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
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();

	//Cargar modelos del tablero de pinball
	tablero_pinball = Model();
	tablero_pinball.LoadModel("Models/TableroPinball/tablero_pinball.obj");
	cuerpo = Model();
	cuerpo.LoadModel("Models/TableroPinball/cuerpo.obj");
	frente = Model();
	frente.LoadModel("Models/TableroPinball/frente.obj");
	pantalla_marco = Model();
	pantalla_marco.LoadModel("Models/TableroPinball/pantalla_marco.obj");
	base_monedas = Model();
	base_monedas.LoadModel("Models/TableroPinball/base_monedas.obj");
	base_monedas_amarilla = Model();
	base_monedas_amarilla.LoadModel("Models/TableroPinball/base_monedas_amarilla.obj");
	moneda_entrada = Model();
	moneda_entrada.LoadModel("Models/TableroPinball/moneda_entrada.obj");
	boton_rojo = Model();
	boton_rojo.LoadModel("Models/TableroPinball/boton_rojo.obj");
	vidrio = Model();
	vidrio.LoadModel("Models/TableroPinball/vidrio.obj");
	canica = Model();
	canica.LoadModel("Models/TableroPinball/pelota.obj");
	canica2 = Model();
	canica2.LoadModel("Models/TableroPinball/canica1.obj");
	moneda = Model();
	moneda.LoadModel("Models/TableroPinball/Sandbox.obj");
	patas = Model();
	patas.LoadModel("Models/TableroPinball/patas.obj");
	resorte = Model();
	resorte.LoadModel("Models/TableroPinball/resorte.obj");
	soporte_resorte = Model();
	soporte_resorte.LoadModel("Models/TableroPinball/soporte_resorte.obj");
	base_tablero = Model();
	base_tablero.LoadModel("Models/TableroPinball/base_tablero.obj");
	base_segunda_capa = Model();
	base_segunda_capa.LoadModel("Models/TableroPinball/base_segunda_capa.obj");
	palanca = Model();
	palanca.LoadModel("Models/TableroPinball/palanca.obj");

	//Obstáculos
	objeto_animado_ins1 = Model();
	objeto_animado_ins1.LoadModel("Models/TableroPinball/objeto_animado_ins1.obj");
	objeto_animado_ins2 = Model();
	objeto_animado_ins2.LoadModel("Models/TableroPinball/objeto_animado_ins2.obj");
	objeto_animado_ins3 = Model();
	objeto_animado_ins3.LoadModel("Models/TableroPinball/objeto_animado_ins3.obj");
	obstaculo_medio_tubo = Model();
	obstaculo_medio_tubo.LoadModel("Models/TableroPinball/obstaculo_medio_tubo.obj");
	obstaculo_rejilla = Model();
	obstaculo_rejilla.LoadModel("Models/TableroPinball/obstaculo_rejilla.obj");
	paleta_superior_der = Model();
	paleta_superior_der.LoadModel("Models/TableroPinball/paleta_superior_der.obj");
	paleta_superior_izq = Model();
	paleta_superior_izq.LoadModel("Models/TableroPinball/paleta_superior_izq.obj");
	paleta_inferior_der = Model();
	paleta_inferior_der.LoadModel("Models/TableroPinball/paleta_inferior_der.obj");
	paleta_inferior_izq = Model();
	paleta_inferior_izq.LoadModel("Models/TableroPinball/paleta_inferior_izq.obj");
	palanca = Model();
	palanca.LoadModel("Models/TableroPinball/palanca.obj");
	rampa = Model();
	rampa.LoadModel("Models/TableroPinball/rampa.obj");
	tornillos_morados = Model();
	tornillos_morados.LoadModel("Models/TableroPinball/tornillos_morados.obj");
	tornillos_morados2 = Model();
	tornillos_morados2.LoadModel("Models/TableroPinball/tornillos_morados_2.obj");

	//Modelos del avatar (ANYA)
	anya_cabeza = Model();
	anya_cabeza.LoadModel("Models/TableroPinball/anya_cabeza.obj");
	anya_cuerpo = Model();
	anya_cuerpo.LoadModel("Models/TableroPinball/anya_cuerpo.obj");
	anya_manos = Model();
	anya_manos.LoadModel("Models/TableroPinball/anya_manos.obj");
	anya_pierna_der = Model();
	anya_pierna_der.LoadModel("Models/TableroPinball/anya_pierna_der.obj");
	anya_pierna_izq = Model();
	anya_pierna_izq.LoadModel("Models/TableroPinball/anya_pierna_izq.obj");


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");



	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/px.png");
	skyboxFaces.push_back("Textures/Skybox/nx.png");
	skyboxFaces.push_back("Textures/Skybox/ny.png");
	skyboxFaces.push_back("Textures/Skybox/py.png");
	skyboxFaces.push_back("Textures/Skybox/nz.png");
	skyboxFaces.push_back("Textures/Skybox/pz.png");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Luz ambiental para todo el escenario

	pointLights[0] = PointLight(0.0f, 0.89f, 0.0f,
		0.0f, 1.0f,
		0.0f, 18.05f, 50.0f,  //posicion
		1.0f, 0.0f, 0.0f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz para el tablero de pinball
	spotLights[1] = SpotLight(1.0f, 0.0f, 1.0f,
		1.0f, 2.0f, //ambiental, difuso
		5.0f, 60.0f, -30.0f,  //posicion
		0.0f, -5.0f, 0.0f,  //dirección a la que apunta
		1.0f, 0.02f, 0.0f, //ecuacion
		30.0f); //angulo de apertura del cono
	spotLightCount++;

	//luz para los flippers o paletas inferiores
	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 4.0f, //ambiental, difuso
		0.0f, 18.99f, -6.0f,  //posicion
		0.0f, -5.0f, 0.0f,  //dirección a la que apunta
		1.0f, 0.05f, 0.001f, //ecuacion
		45.0f); //angulo de apertura del cono
	spotLightCount++;

	//luz del objeto instanciado muchas veces
	spotLights[3] = SpotLight(0.7922f, 0.9804f, 0.0471f,
		1.0f, 2.0f, //ambiental, difuso
		0.0f, 18.0f, -8.0f, //posicion
		0.0f, -5.0f, 0.0f, //dirección a la que apunta
		1.0f, 0.0f, 0.0f, //
		130.0f); //angulo de apertura del cono
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;
	glm::vec3 posblackhawk = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posAnya = glm::vec3(0.0f, 0.0f, 0.0f);

	movPelota = 0.0f;
	movPelotaOffset = 0.04f;
	rotPelota = 0.0f;
	rotPelotaOffset = 5.0f;
	avanzaPelota = true;

	

	movPalanca = 0.0f;
	movPalancaOffset = 0.02f;
	rotPalancaOffset = 5.0f;
	avanzaPalanca = true;
	
	//---------PARA TENER KEYFRAMES GUARDADOS NO VOLATILES QUE SIEMPRE SE UTILIZARAN SE DECLARAN AQUÍ

	KeyFramePelota[0].movPelota_x = 0.0f;
	//KeyFramePelota[0].movPelota_y = 0.0f;
	KeyFramePelota[0].movPelota_z = 0.0f;
	KeyFramePelota[0].rotacionPelota = -360;


	//Sube sobre el carril del tablero de pinball 
	KeyFramePelota[1].movPelota_x = 0.0f;
	//KeyFramePelota[1].movPelota_y = 0.0f;
	KeyFramePelota[1].movPelota_z = -16.0f;
	KeyFramePelota[1].rotacionPelota = 360;

	//Sale del carril del tablero de pinball y a partir de aquí debe hacer una curva definida por los siguientes keyframes
	KeyFramePelota[2].movPelota_x = -6.0f;
	//KeyFramePelota[2].movPelota_y = 0.0f;
	KeyFramePelota[2].movPelota_z = -15;
	KeyFramePelota[2].rotacionPelota = 360;


	KeyFramePelota[3].movPelota_x = -5.5f;
	//KeyFramePelota[3].movPelota_y = 0.0f;
	KeyFramePelota[3].movPelota_z = -9.5f;
	KeyFramePelota[3].rotacionPelota = 360;

	KeyFramePelota[4].movPelota_x = -7.0f;
	//KeyFramePelota[4].movPelota_y = 0.0f;
	KeyFramePelota[4].movPelota_z = -6.8f;
	KeyFramePelota[4].rotacionPelota = 360;

	KeyFramePelota[5].movPelota_x = -4.0f;
	//KeyFramePelota[5].movPelota_y = 0.0f;
	KeyFramePelota[5].movPelota_z = -6.7f;
	KeyFramePelota[5].rotacionPelota = 360;

	KeyFramePelota[6].movPelota_x = -5.7f;
	//KeyFramePelota[5].movPelota_y = 0.0f;
	KeyFramePelota[6].movPelota_z = 5.0f;
	KeyFramePelota[6].rotacionPelota = 0;

	KeyFramePelota[7].movPelota_x =  0.0f;
	KeyFramePelota[7].movPelota_y = -1.0f;
	KeyFramePelota[7].movPelota_z =  0.0f;
	KeyFramePelota[7].rotacionPelota = 0;

	
	//Animacion de Anya
	KeyFrameAnya[0].movAnya_x = 0.0f;
	KeyFrameAnya[0].movAnya_y = 0.0f;
	KeyFrameAnya[0].movAnya_z = 10.0f;
	KeyFrameAnya[0].rotacionAnya = 40;

	KeyFrameAnya[1].movAnya_x = -3.0f;
	KeyFrameAnya[1].movAnya_y = 0.0f;
	KeyFrameAnya[1].movAnya_z = 12.0f;
	KeyFrameAnya[1].rotacionAnya = -40;

	KeyFrameAnya[2].movAnya_x = -0.0f;
	KeyFrameAnya[2].movAnya_y = 0.0f;
	KeyFrameAnya[2].movAnya_z = 9.0f;
	KeyFrameAnya[2].rotacionAnya = 40;

	KeyFrameAnya[3].movAnya_x = 1.99f;
	KeyFrameAnya[3].movAnya_y = 0.0f;
	KeyFrameAnya[3].movAnya_z = 14.0f;
	KeyFrameAnya[3].rotacionAnya = -40;


	

		printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");
		printf("3.-Presiona L para guardar frame\n4.-Presiona P para habilitar guardar nuevo frame\n5.-Presiona 1 para mover en X\n6.-Presiona 2 para habilitar mover en X");

	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f*deltaTime;

		if (movCoche < 30.0f)
		{
			movCoche -= movOffset * deltaTime;
			//printf("avanza%f \n ",movCoche);
		}
		rotllanta += rotllantaOffset * deltaTime;

		//Si meterMoneda se selecciona (tecla 7) se activa la animación de la moneda, una vez que se mete la moneda, se debe activar la animacion de la pelota
		//Si meterMoneda se selecciona (tecla 7) se activa la animación de la moneda, una vez que se mete la moneda, se debe activar la animacion de la pelota
		if (mainWindow.getMeterMoneda())
		{
			if (avanzaMoneda)
			{
				if (movMoneda > -15.0f)
				{
					movMoneda -= movMonedaOffset * deltaTime;
					//printf("avanza%f \n ",movCoche);
					rotMoneda += rotMonedaOffset * deltaTime;
					if (movMoneda < -9.0f)
					{
						if (avanzaPelota)
						{
							if (movPelota > -10)
							{
								movPelota -= movPelotaOffset * deltaTime;
								//printf("avanza%f \n ",movCoche);
								rotPelota += rotPelotaOffset * deltaTime;
							}
							else
							{
								avanzaPelota = false;
							}
						}

					}
				}
				else
				{
					avanzaMoneda = false;
				}
			}

		}

		//Recibir eventos del usuario
		glfwPollEvents();

		// Para cambiar de cámara
		numCamara = mainWindow.getnumCamara();

		if (numCamara == 0) {
			camera = &cameraLibre;
			camera->keyControl(mainWindow.getsKeys(), deltaTime);
			//printf("x: %f, y: %f, z: %f\n", camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
			camera->mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		else if (numCamara == 1) {
			camera = &cameraIsometrica;
			camera->setCameraDirection(glm::vec3(0.55f, -0.55f, -0.620f));
			//camera->mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
			//printf("x: %f, y: %f, z: %f\n", camera->getCameraDirection().x, camera->getCameraDirection().y, camera->getCameraDirection().z);
		}
		else if (numCamara == 2) {
			camera = &cameraPinball;
			camera->setCameraPosition(glm::vec3(posXpelota + movPelota_x + 5.652f, posYpelota + movPelota_y + 17.172f + 1.0f, posZpelota + movPelota_z + 4.064f - 7.5f));
			camera->setCameraDirection(glm::vec3(0.0f, -0.5f, -0.865f));
			//camera->mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
			//printf("x: %f, y: %f, z: %f\n", camera->getCameraDirection().x, camera->getCameraDirection().y, camera->getCameraDirection().z);
		}


		//-------Para Keyframes
		inputKeyframesPelota(mainWindow.getsKeys());
		inputKeyframesAnya(mainWindow.getsKeys());
		animatePelota();
		animateAnya();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera->calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera->calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera->getCameraPosition().x, camera->getCameraPosition().y, camera->getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera->getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera->getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		tiempoLuz++;
		if (tiempoLuz == 60) {
			apagadoLuz = !apagadoLuz;
			tiempoLuz = 0;
		}
		if (apagadoLuz) {
			shaderList[0].SetPointLights(pointLights, pointLightCount - 1);
		}
		else
			shaderList[0].SetPointLights(pointLights, pointLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
	


		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche-50.0f, 0.5f, -2.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();


		model = glm::mat4(1.0);
		//posblackhawk = glm::vec3(posXavion + movAvion_x, posZavion, posYavion + movAvion_y);
		//model = glm::translate(model, posblackhawk);
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		//model = glm::rotate(model, giroAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//color = glm::vec3(0.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		//--------------------------------------------- AVATAR CON MODELADO GERÁRQUICO ------------------------------------------------------
		model = glm::mat4(1.0);
		posAnya = glm::vec3(posXanya + movAnya_x, posYanya + movAnya_y, posZanya + movAnya_z);
		model = glm::translate(model, posAnya);
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		anya_cuerpo.RenderModel();

		//Cabeza
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		anya_cabeza.RenderModel();

		//Manos
        model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		anya_manos.RenderModel();

		//Pierna derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.423f, 18.599f, 21.697f));
		model = glm::rotate(model, rotacionAnya * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.423f, -18.599f, -21.697f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		anya_pierna_der.RenderModel();

		//Pierna izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.303f, 18.599f, 21.697f));
		model = glm::rotate(model, rotacionAnya * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-1.303f, -18.599f, -21.697f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		anya_pierna_izq.RenderModel();
		 

		//----------------------------------------- TABLERO DE PINBALL ----------------------------------------------
		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cuerpo.RenderModel();
		frente.RenderModel();
		pantalla_marco.RenderModel();;
		base_monedas.RenderModel();
		base_monedas_amarilla.RenderModel();
		moneda_entrada.RenderModel();
		boton_rojo.RenderModel();
		patas.RenderModel();

		//Hacer que la palanca y el resorte hagan su animación, para ello se desplaza la palanca y el resorte se estira (se escala en z), luego se comprime y la canica sale disparada
		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		resorte.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0, movPalanca));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		palanca.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		soporte_resorte.RenderModel();



		//model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//tablero_pinball.RenderModel();

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		base_tablero.RenderModel();

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		base_segunda_capa.RenderModel();

		//obstaculos
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		objeto_animado_ins1.RenderModel();

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		objeto_animado_ins2.RenderModel();

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		objeto_animado_ins3.RenderModel();

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		obstaculo_medio_tubo.RenderModel();

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		obstaculo_rejilla.RenderModel();

		//Mover las paletas con teclas asignables
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.058f, 17.107f, 11.204f));
		model = glm::rotate(model, glm::radians(mainWindow.getAnguloCofre()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-1.058f, -17.107f, -11.204f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		paleta_superior_der.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.982f, 11.204f, 11.232f));
		model = glm::rotate(model, glm::radians(mainWindow.getAnguloCofre()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.982f, -11.204f, -11.232f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		paleta_superior_izq.RenderModel();

		model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(1.961f, 17.154f, 5.963f));
		model = glm::rotate(model, glm::radians(mainWindow.getAnguloCofre()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-1.961f, -17.154f, -5.963f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		paleta_inferior_der.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.786f, 17.154f, 5.963f));
		model = glm::rotate(model, glm::radians(mainWindow.getAnguloCofre()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(1.786f, -17.154f, -5.963f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		paleta_inferior_izq.RenderModel();

		//----------------------------------------------------------------------------------------------------
		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		rampa.RenderModel();

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tornillos_morados.RenderModel();

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tornillos_morados2.RenderModel();



		// ----------------- PELOTA Y ANIMACION ------------------------------
		//animación por keyframes
		model = glm::mat4(1.0);
		//posblackhawk = glm::vec3(posXpelota + movPelota_x + 5.652f, posYpelota + movPelota_y + 17.172f, posZpelota + movPelota_z + 4.064f);
		posblackhawk = glm::vec3(posXpelota + movPelota_x, posYpelota + movPelota_y, posZpelota + movPelota_z);
		model = glm::translate(model, posblackhawk);
		//model = glm::translate(model, glm::vec3(0.0, 0.0f, movPelota));
		model = glm::translate(model, glm::vec3(5.652f, 17.172, 4.064f));
		model = glm::rotate(model, rotacionPelota * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-5.652f, -17.172, -4.064f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		canica.RenderModel();

		//animacion sencilla
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0, 0.0f, movPelota));
		model = glm::translate(model, glm::vec3(5.652f, 17.172, 4.064f));
		model = glm::rotate(model, rotacionPelota * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-5.652f, -17.172, -4.064f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		canica2.RenderModel();

		model = glm::mat4(1.0);
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(13.35f, -3.0f, 3.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0, 0.0f, movMoneda));
		model = glm::rotate(model, rotMoneda * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		moneda.RenderModel();

		

		//---------------------------------------------------------------------------------------------------------------------

		//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[3]->RenderMesh();
		
		//Renderizar casi al final el vidrio
		model = glm::mat4(1.0);
		glEnable(GL_BLEND);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		vidrio.RenderModel();

		//textura con movimiento
		//Importantes porque la variable uniform no podemos modificarla directamente
		toffsetflechau += 0.001;
		toffsetflechav += 0.0;
		//para que no se desborde la variable
		if (toffsetflechau > 1.0)
			toffsetflechau = 0.0;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FlechaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[4]->RenderMesh();




		glDisable(GL_BLEND);
		

		glUseProgram(0);

		mainWindow.swapBuffers(); 
	}

	return 0;
}


void inputKeyframesPelota(bool* keys)
{
	if (keys[GLFW_KEY_SPACE]) // Empezar animacion
	{
		//if (reproduciranimacion < 1)
		//{
		if (playPelota == false && (FrameIndexPelota > 1))
		{
			resetElementsPelota();
			//First Interpolation	
			interpolationPelota();
			playPelota = true;
			playIndexPelota = 0;
			i_curr_stepsPelota = 0;
			//reproduciranimacion++;
			//printf("presiona 0 para habilitar reproducir de nuevo la animaci�n'\n");
			//habilitaranimacion = 0;

		}
		else
		{
			playPelota = false;
		}
		//}
	}
	/*if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1)
		{
			reproduciranimacion = 0;
		}
	}*/

	if (keys[GLFW_KEY_L] ) //Guardar
	{
		if (guardoFrame < 1)
		{
			saveFramePelota();
			/*printf("\n--------- SE GUARDA -----------\n");
			printf("movAvion_x es: %f\n", movAvion_x);
			printf("movAvion_y es: %f\n", movAvion_y);
			printf("movAvion_z es: %f\n", movAvion_z);
			printf("giroAvionY es: %f\n", giroAvionY);
			printf("giroAvionX es: %f\n", giroAvionX);
			printf("presiona R para habilitar guardar otro frame'\n");
			printf("\n-----------------------------\n");*/
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_R])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
		}
	}

	//if (keys[GLFW_KEY_1])
	//{
	//	if (ciclo < 1)
	//	{
	//		//printf("movAvion_x es: %f\n", movAvion_x);
	//		/*movAvion_x += 1.0f;*/
	//	/*	printf("\n movAvion_x es: %f\n", movAvion_x);*/
	//		ciclo++;
	//		ciclo2 = 0;
	//		printf("\n Presiona la tecla 2 para poder habilitar la variable\n");
	//	}

	//}
	//if (keys[GLFW_KEY_2])
	//{
	//	if (ciclo2 < 1)
	//	{
	//		ciclo = 0;
	//		printf("\n Ya puedes modificar tu variable presionando la tecla 1\n");
	//	}
	//}

	if (keys[GLFW_KEY_RIGHT] ) // DECREMENTA Z
	{
		//movPalanca_z -= 1.0f;
		
	}

}

void inputKeyframesAnya(bool* keys)
{
	if (keys[GLFW_KEY_B]) // Empezar animacion
	{
		//if (reproduciranimacion < 1)
		//{
		if (playAnya == false && (FrameIndexAnya > 1))
		{
			resetElementsAnya();
			//First Interpolation	
			interpolationAnya();
			playAnya = true;
			playIndexAnya = 0;
			i_curr_stepsAnya = 0;
			//reproduciranimacion++;
			//printf("presiona 0 para habilitar reproducir de nuevo la animaci�n'\n");
			//habilitaranimacion = 0;

		}
		else
		{
			playAnya = false;
		}
		//}
	}
	/*if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1)
		{
			reproduciranimacion = 0;
		}
	}*/

	if (keys[GLFW_KEY_L]) //Guardar
	{
		if (guardoFrame < 1)
		{
			saveFrameAnya();
			/*printf("\n--------- SE GUARDA -----------\n");
			printf("movAvion_x es: %f\n", movAvion_x);
			printf("movAvion_y es: %f\n", movAvion_y);
			printf("movAvion_z es: %f\n", movAvion_z);
			printf("giroAvionY es: %f\n", giroAvionY);
			printf("giroAvionX es: %f\n", giroAvionX);
			printf("presiona R para habilitar guardar otro frame'\n");
			printf("\n-----------------------------\n");*/
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_R])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
		}
	}

	//if (keys[GLFW_KEY_1])
	//{
	//	if (ciclo < 1)
	//	{
	//		//printf("movAvion_x es: %f\n", movAvion_x);
	//		/*movAvion_x += 1.0f;*/
	//	/*	printf("\n movAvion_x es: %f\n", movAvion_x);*/
	//		ciclo++;
	//		ciclo2 = 0;
	//		printf("\n Presiona la tecla 2 para poder habilitar la variable\n");
	//	}

	//}
	//if (keys[GLFW_KEY_2])
	//{
	//	if (ciclo2 < 1)
	//	{
	//		ciclo = 0;
	//		printf("\n Ya puedes modificar tu variable presionando la tecla 1\n");
	//	}
	//}

	if (keys[GLFW_KEY_RIGHT]) // DECREMENTA Z
	{
		//movPalanca_z -= 1.0f;

	}

}

