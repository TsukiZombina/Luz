#include <iostream>
#include <GL/glew.h>
#include <vector>
#include <GLFW/glfw3.h>
#include <fstream>

#include "util.h"
#include "camera.h"
#include "control.h"
#include "perspective.h"

using std::cout;
using std::endl;

const int WIDTH = 512, HEIGHT = 512;

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL App",
		nullptr, nullptr);

	if (window == nullptr)
	{
		cout << "Error creando la ventana de GLFW" << endl;

		return 1;
	}

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;

	GLenum result = glewInit();

	if (result != GLEW_OK)
	{
		cout << "Error inicializando GLEW" << endl;
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	GLuint shaderProgram = linkShaderProgram();

	std::vector<GLfloat> vertices;
	readVector(vertices, "vertices.txt");

	std::vector<GLuint> indices;
	readVector(indices, "indices.txt");

	std::vector<GLfloat> verticesPlane;
	readVector(verticesPlane, "verticesPlane.txt");

	CameraInfo camera;
	camera.position.set(0.0f, 0.0f,  3.0f);
	camera.target.set(0.0f, 0.0f, -1.0f);
	camera.up.set(0.0f, 1.0f,  0.0f);
	camera.mouseX = WIDTH / HEIGHT;
	camera.mouseY = WIDTH / HEIGHT;

	glfwSetWindowUserPointer(window, (GLvoid*)&camera);

	SquareMatrix<GLfloat, 4> model;
	SquareMatrix<GLfloat, 4> view;
	SquareMatrix<GLfloat, 4> projection;
	SquareMatrix<GLfloat, 4> modelView;
	
	model.Identity();
	
	MakePerspective(45.0f, WIDTH / HEIGHT, 0.1f, 100.0f, projection);

	GLuint vao[2], vbo[2], ebo;

	//Tetrahedron
	glGenVertexArrays(2, vao);
		glGenBuffers(2, vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao[0]);
				
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLfloat), &indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Plane
		glBindVertexArray(vao[1]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, verticesPlane.size() * sizeof(GLfloat), &verticesPlane[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Vector3D<float> newTarget;
		newTarget = camera.position + camera.target;
		MakeView(camera.position, newTarget, camera.up, view);
		
		Matrix<GLfloat, 4, 4> modelViewProjection;

		// Dibuja
		glUseProgram(shaderProgram);

		GLfloat radius = 2.0f;
		Vector3D<GLfloat> diffuseLight(radius * sinf(glfwGetTime()), radius * cosf(glfwGetTime()), 2.0f);

		Vector3D<GLfloat> diffuseLight(radius * sinf(glfwGetTime()), radius * cosf(glfwGetTime()), 2.0f);

		glUniform4f(glGetUniformLocation(shaderProgram, "diffuseLightPosition"), diffuseLight(0), diffuseLight(1), diffuseLight(2), 1.0);
		glUniform4f(glGetUniformLocation(shaderProgram, "spacularLightPosition"), diffuseLight(0), diffuseLight(1), diffuseLight(2), 1.0);

		model.Identity();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_TRUE, model.GetArray());
		modelViewProjection = projection * (view * model);

		GLint location = glGetUniformLocation(shaderProgram, "modelViewProjection");
		glUniformMatrix4fv(location, 1, GL_TRUE, modelViewProjection.GetArray());
	
		glBindVertexArray(vao[0]);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		model.Identity();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_TRUE, model.GetArray());
		modelViewProjection = projection * (view * model);
		location = glGetUniformLocation(shaderProgram, "modelViewProjection");
		glUniformMatrix4fv(location, 1, GL_TRUE, modelViewProjection.GetArray());

		glBindVertexArray(vao[1]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(2, vao);
	glDeleteBuffers(2, vbo);
	glDeleteBuffers(1, &ebo);
	glfwTerminate();

	return 0;
}