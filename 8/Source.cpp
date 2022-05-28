#include <cstdio>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <cmath>
#include <assert.h>
#ifndef MATH_3D_H
#define	MATH_3D_H


struct Vector3f
{
	float x, y, z;

	Vector3f()
	{
	}

	Vector3f(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};


struct Matrix4f
{
	float m[4][4];
};


#endif	/* MATH_3D_H */
GLuint VBO; //ïåðåìåííàÿ äëÿ õðàíåíèÿ óêàçàòåëÿ íà áóôåð âåðøèí
GLuint gWorldLocation; //óêàçàòåëü äëÿ äîñòóïà ê âñåìèðíîé ìàòðèöå

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWorld * vec4(Position, 1.0);                                     \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);                                           \n\
}";

static void RenderSceneCB()
{
	glClear(GL_COLOR_BUFFER_BIT); //î÷èùàåì áóôåð êàäðà

	static float Scale = 0.0f;
	Scale += 0.001f;

	/*Ïîäãîòàâëèâàåì ìàòðèöó 4x4 è çàïîëíÿåì*/
	Matrix4f World;

	/*Ìàñøòàáèðîâàíèå*/
	World.m[0][0] = sinf(Scale); World.m[0][1] = 0.0f;        World.m[0][2] = 0.0f;        World.m[0][3] = 0.0f;
	World.m[1][0] = 0.0f;        World.m[1][1] = cosf(Scale); World.m[1][2] = 0.0f;        World.m[1][3] = 0.0f;
	World.m[2][0] = 0.0f;        World.m[2][1] = 0.0f;        World.m[2][2] = sinf(Scale); World.m[2][3] = 0.0f;
	World.m[3][0] = 0.0f;        World.m[3][1] = 0.0f;        World.m[3][2] = 0.0f;        World.m[3][3] = 1.0f;

	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]); //çàãðó;ftv ìàòðèöó â øåéäåð

	glEnableVertexAttribArray(0); //èíäåêñèðóåì àòðèáóò âåðøèíû
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //îáðàòíî ïðèâÿçûâàåì áóôåð, ïðèãîòàâëèâàÿ åãî äëÿ îòðèñîâêè
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //ãîâîðèì êîíâåéåðó êàê âîñïðèíèìàòü äàííûå âíóòðè áóôåðà

	glDrawArrays(GL_TRIANGLES, 0, 3); //âûçûâàåì ôóíêöèþ äëÿ îòðèñîâêè

	glDisableVertexAttribArray(0); //îòêëþ÷àåì àòðèáóò âåðøèíû

	glutSwapBuffers(); //ìåíÿåì ôîíîâûé áóôåð è áóôåð êàäðà ìåñòàìè
}

static void InitializeGlutCallbacks()
{
	glutDisplayFunc(RenderSceneCB);
	glutIdleFunc(RenderSceneCB);
}

static void CreateVertexBuffer()
{
	Vector3f Vertices[3];
	Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
	Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
	Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	GLuint ShaderObj = glCreateShader(ShaderType);

	/*Îáðàáàòûâàåì îøèáêó*/
	if (ShaderObj == 0)
	{
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	const GLchar* p[1];
	p[0] = pShaderText;

	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);

	glShaderSource(ShaderObj, 1, p, Lengths);
	glCompileShader(ShaderObj);

	GLint success;

	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}

	glAttachShader(ShaderProgram, ShaderObj);
}

static void CompileShaders()
{
	GLuint ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
	AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glUseProgram(ShaderProgram);

	gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
	assert(gWorldLocation != 0xFFFFFFFF);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv); //èíèöèàëèçèðóåì GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // íàñòðàèâàåì íåêîòîðûå îïöèè GLUT

	/*Çàäà¸ì ïàðàìåòðû îêíà*/
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tutorial 08");

	InitializeGlutCallbacks(); //ïðèñîåäèíÿåì ôóíêöèþ RenderSceneCB ê GLUT

	/*Èíèöèàëèçèðóåì GLEW è ïðîâåðÿåì íà îøèáêè*/
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	glClearColor(1.0f, 1.0f, 0.0f, 0.0f); //óñòàíàâëèâàåì öâåò, êîòîðûé áóäåò èñïîëüçîâàí âî âðåìÿ î÷èñòêè áóôåðà êàäðà

	CreateVertexBuffer();

	CompileShaders();

	glutMainLoop(); //ïåðåäà¸ì êîíòðîëü GLUT'ó

	return 0;
}