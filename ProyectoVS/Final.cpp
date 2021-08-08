/*---------------------------------------------------------*/
/* ----------------  Proyecto Final------------ -----------*/
/*-----------------    2021-2   ---------------------------*/
/*------------- Alumno:Hernandez Calderon Fernando---------*/
#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include <mmsystem.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor *monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(-360.0f, 106.0f, -380.0f));
float MovementSpeed = 0.001f;
float lastX = SCR_WIDTH / 1.0f;
float lastY = SCR_HEIGHT / 1.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

// posiciones
float	movAuto_x = 0.0f,
		movAuto_z = 0.0f,
		orienta = 0.0f,
		giroLlantas = 0.0f,
		giroPuerta = 0.0f,
		movPersonaje = 0.0f,
		movPuerta = 0.0f,
		movCajon1 = 0.0f,
		movCajon2 = 0.0f,
		puertaMicro = 0.0f;
bool	animacion = false,
		animacionDos = false,
		carroPflag = false,
		PuertaFlag = false,
		cajon1Flag = false,
		cajon2Flag = false,
		microFlag = false;

void animate(void){
	//Vehículo
	if (animacion){
		if (movAuto_z >= 0 and movAuto_z < 420) {
			movAuto_z += 10.0f;
			giroLlantas += 4.0f;
			movPersonaje += 0.5f;
		}
		else if (movAuto_z >= 420 and movAuto_z <= 440) {
			movAuto_z += 10.0f;
			giroLlantas += 6.0f;
			orienta += 30.0f;
			movPersonaje += 0.5f;
		}
		else {
			movAuto_x += 10.0f;
			giroLlantas += 6.0f;
		}
		movPersonaje += 0.8f;
		if (movPuerta <= 90.0f && PuertaFlag == false) {
			movPuerta += 2.0f;
		}else {
			if (PuertaFlag == false)
				PuertaFlag = true;
			if (movPuerta != 0.0f) 
				movPuerta -= 2.0f;
		}
		if (movCajon2 >= 0 && cajon2Flag == false) {
			movCajon2 += 0.2f;
			if (movCajon2 >= 5.0f) {
				cajon2Flag = true;
			}
		}
		else if (movCajon2 > 0 && cajon2Flag == true) {
			movCajon2 -= 0.2f;
		}
		if (puertaMicro < 90 && microFlag == false) {
			puertaMicro += 3;
			if (puertaMicro >= 90) {
				microFlag = true;
			}
		}
		else if (puertaMicro >= 3 && puertaMicro <= 90) {
			puertaMicro -= 3;
		}
	}

	if (animacionDos) {
		if (giroPuerta < 90 && carroPflag != true) {
			giroPuerta += 3;
		}
		else if (giroPuerta >= 0 && giroPuerta <= 90) {
			giroPuerta -= 3;
		}

		if (movCajon1 >= 0 && cajon1Flag == false) {
			movCajon1+=0.2f;
			if (movCajon1 >= 5.0f) {
				cajon1Flag = true;
			}
		}else if (movCajon1 > 0 && cajon1Flag == true) {
			movCajon1-=0.2f;
		}
	}
}

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	Model piso("resources/objects/piso/piso.obj");
	//Modelos del exterior
	Model Consulado("resources/objects/CasaB/Consulado.obj");
	Model kiosko("resources/objects/kiosko/kiosko.obj");
	Model banca("resources/objects/sillaParque/banca.obj");
	Model Edificio1("resources/objects/edificio1/edificio1C.obj");
	Model Edificio4("resources/objects/edificio4/edificioCuatro.obj");
	Model carroP("resources/objects/LamboPuerta/CarroSinP.obj");
	Model puerta("resources/objects/lamboPuerta/puerta.obj");
	Model llanta("resources/objects/lamboR/Wheel.obj");
	Model carroV("resources/objects/lamboR/carroceriaV.obj");
	Model carroR("resources/objects/lamboR/carroceria.obj");
	Model silla("resources/objects/sillas/Chair.obj");
	Model mesa("resources/objects/mesa/mesa.obj");
	//Departamento donde se creara la habitacion
	Model Depa6("resources/objects/Departamento6/Depto6.obj");
	Model DepaP("resources/objects/Departamento6/puertaMM6.obj");
	Model Corredisa("resources/objects/Departamento6/corredisa.obj");
	//Modelos dentro del departamento
	Model etla("resources/objects/etla/Etla.obj");
	Model rack("resources/objects/Rack/Rack.obj");
	Model bed("resources/objects/bed/BedM.obj");
	Model rata("resources/objects/rata/Mouse.obj");
	Model godzilla("resources/objects/Godzilla/Godzilla.obj");
	Model cocina("resources/objects/cocina/cocina.obj");
	Model puertaM("resources/objects/cocina/puertaMicro.obj");
	Model sofa("resources/objects/sofa/sofa.obj");
	Model mesaCasa("resources/objects/MesaCasa/desk.obj");
	Model cajones("resources/objects/MesaCasa/cajones.obj");
	Model tele("resources/objects/tele/Tele.obj");
	Model retrato("resources/objects/retrato/retrato.obj");
	
	//Animados
	
	ModelAnim kate("resources/objects/kate/kate.dae");
	kate.initShaders(animShader.ID);
	ModelAnim megan("resources/objects/megan/megan.dae");
	megan.initShaders(animShader.ID);
	ModelAnim sentado("resources/objects/Sitting/Sitting.dae");
	sentado.initShaders(animShader.ID);
	ModelAnim aplauso("resources/objects/Sitting Clap/Sitting Clap.dae");
	aplauso.initShaders(animShader.ID);
	ModelAnim salto("resources/objects/salto/Jumping Down.dae");
	salto.initShaders(animShader.ID);
	
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);
		
		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(0.6f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);
		

		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);
	
		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);
		
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-60.0f, 0.0f, 60.0f + movPersonaje));
		model = glm::scale(model, glm::vec3(0.08f));
		animShader.setMat4("model", model);
		kate.Draw(animShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-120.0f, 0.0f, 60.0f - movPersonaje));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.08f));
		animShader.setMat4("model", model);
		megan.Draw(animShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-128.0f, -1.0f, 130.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.08f));
		animShader.setMat4("model", model);
		salto.Draw(animShader);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-320.0f,148.0f,-388));
        model = glm::rotate(model, glm::radians(240.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.09f));
        animShader.setMat4("model", model);
        sentado.Draw(animShader);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-346.0f,148.0f,-406));
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.09f));
        animShader.setMat4("model", model);
        aplauso.Draw(animShader);
		
		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);
		
		//Carga Carro Verde
		model = glm::translate(glm::mat4(1.0f), glm::vec3(12.0f + movAuto_x, 0.0f, 0.0f + movAuto_z));
		tmp = model = glm::rotate(model, glm::radians(orienta), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		carroV.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(8.0f, 3.0f, 13.0f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(1.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);    //Izq delantera

		model = glm::translate(tmp, glm::vec3(-8.0f, 3.0f, 13.0f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(1.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);    //Der delantera

		model = glm::translate(tmp, glm::vec3(-8.0f, 3.0f, -14.5f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);    //Der trasera

		model = glm::translate(tmp, glm::vec3(8.0f, 3.0f, -14.5f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);    //Izq trase
		//--------------------------------------------------------------------------------------------------------------------------

		//Carga Carro Rojo
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-447.0f + movAuto_x, 0.0f, 0.0f + movAuto_z));
		tmp = model = glm::rotate(model, glm::radians(orienta), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		carroR.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(8.0f, 3.0f, 13.0f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);    //Izq delantera

		model = glm::translate(tmp, glm::vec3(-8.0f, 3.0f, 13.0f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);    //Der delantera

		model = glm::translate(tmp, glm::vec3(-8.0f, 3.0f, -14.5f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);    //Der trasera

		model = glm::translate(tmp, glm::vec3(8.0f, 3.0f, -14.5f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);    //Izq trase
		//----------------------------------------------------------------------------------------------------------
		
		//Carro con puerta animada
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, 0.0f, -90.0f ));
		tmp = model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		carroP.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(-9.9f, 6.46f, 9.1f));
		model = glm::rotate(model, glm::radians(movPuerta), glm::vec3(1.0f, 0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		puerta.Draw(staticShader);

		model = glm::translate(tmp, glm::vec3(8.0f, 3.0f, 13.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);    //Izq delantera

		model = glm::translate(tmp, glm::vec3(-8.0f, 3.0f, 13.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);    //Der delantera

		model = glm::translate(tmp, glm::vec3(-8.0f, 3.0f, -14.5f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);    //Der trasera

		model = glm::translate(tmp, glm::vec3(8.0f, 3.0f, -14.5f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);    //Izq trase
		
		//--------------------------------------------------------------------------------------------------------------------------		
		
		// Objetos Parque
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-163.0f, 0.0f, 163.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		kiosko.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-130.0f, -1.0f, 130.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f,0.3f,0.3f));
		staticShader.setMat4("model", model);
		banca.Draw(staticShader);
		
		//-----------------------------------------------------------------------------------------------------------------------------
		// Edificios del escenario
		
		model = glm::translate(glm::mat4(1.0f), glm::vec3(170.0f, -20.0f, 50.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(6.0f));
		staticShader.setMat4("model", model);
		Consulado.Draw(staticShader);	

		model = glm::translate(glm::mat4(1.0f), glm::vec3(172.0f, -5.0f, -480.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		Edificio4.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(244.0f, -5.0f, -480.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		Edificio4.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(316.0f, -5.0f, -480.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		Edificio4.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(388.0f, -5.0f, -480.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		Edificio4.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(460.0f, -5.0f, -480.0f));
		model = glm::scale(model, glm::vec3(3.0f));
		staticShader.setMat4("model", model);
		Edificio4.Draw(staticShader);
		
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-60.0f, 0.0f, -380.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		Edificio1.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(450.0f, 0.0f, 350.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		Edificio1.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(330.0f, 0.0f, 350.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		staticShader.setMat4("model", model);
		Edificio1.Draw(staticShader);
		
//----------------------------------------------------------------------------------------------------------------------------
//Departamento Teoria
		
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-330.0f, 145.0f, -400.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f));
		staticShader.setMat4("model", model);
		mesa.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-345.0f, 145.0f, -410.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		silla.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-318.0f, 145.0f, -388.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(130.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		silla.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-296.0f, 106.5f, -383.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		Corredisa.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-296.0f, 106.5f, -417.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		Corredisa.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-274.0f, 106.0f, -366.0f));
		model = glm::rotate(model, glm::radians(90 + giroPuerta), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		DepaP.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-274.0f, 106.0f, -434.0f));
		model = glm::rotate(model, glm::radians(90 - giroPuerta), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		DepaP.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-290.0f, 0.0f, -400.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		Depa6.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-338.0f, 98.0f, -349.0f));
		model = glm::rotate(model, glm::radians(55.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		etla.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-342.0f, 98.0f, -349.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		silla.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-334.0f, 98.0f, -348.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		silla.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-370.0f, 98.0f, -459.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.008f));
		staticShader.setMat4("model", model);
		bed.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-360.0f, 98.0f, -335.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		rack.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-362.0f, 105.5f, -336.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f));
		staticShader.setMat4("model", model);
		godzilla.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-358.0f, 105.5f, -336.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f));
		staticShader.setMat4("model", model);
		retrato.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-424.0f, 106.0f, -445.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.009f));
		staticShader.setMat4("model", model);
		cocina.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-427.3f, 105.0f, -436.7f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(puertaMicro), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.009f));
		staticShader.setMat4("model", model);
		puertaM.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-410.0f, 105.0f, -440.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		staticShader.setMat4("model", model);
		rata.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-340.0f, 99.0f, -360.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		staticShader.setMat4("model", model);
		sofa.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-396.0f, 98.0f, -465.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f,17.0f,15.0f));
		staticShader.setMat4("model", model);
		mesaCasa.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-396.0f, 98.0f, -465.0f+movCajon1));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f, 17.0f, 15.0f));
		staticShader.setMat4("model", model);
		cajones.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-380.6f, 98.0f, -465.0f + movCajon2));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f, 17.0f, 15.0f));
		staticShader.setMat4("model", model);
		cajones.Draw(staticShader);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-340.0f, 97.95f, -440.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.09f,0.12f,0.1f));
		staticShader.setMat4("model", model);
		tele.Draw(staticShader);
//------------------------------------------------------------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;

	//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		animacion = true;
		carroPflag = false;
	}

	if (key == GLFW_KEY_O && action == GLFW_PRESS)
		animacionDos = true;

	if (key == GLFW_KEY_R && action == GLFW_PRESS) { //Con la tecla R reiniciamos la animación
		movAuto_z = 0.0f;
		movAuto_x = 0.0f;
		movPersonaje = 0.0f;
		giroPuerta = 0.0f;
		orienta = 0.0f;
		movCajon1 = 0.0f;
		movCajon2 = 0.0f;
		puertaMicro = 0.0f;
		animacion = false;
		carroPflag = false;
		PuertaFlag = false;
		animacionDos = false;
		cajon1Flag = false;
		cajon2Flag = false;
		microFlag = false;
	}
}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}

	// glfw: whenever the mouse moves, this callback is called
	// -------------------------------------------------------
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	// ----------------------------------------------------------------------
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		camera.ProcessMouseScroll(yoffset);
	}

