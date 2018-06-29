#include <memory>

#include <stdio.h>
#include <string.h>

#include "Orbs.h"

extern const char *vertexshader, *fragmentshader;

Orbs::Orbs(int w, int h, int count)
{
	attributes.reset(new float[count * 4]);
	init_extensions();

	world.left = -8.0f;
	world.right = 8.0f;
	world.bottom = 4.5f;
	world.top = -4.5f;

	// opengl settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// shaders
	const unsigned vshader = glCreateShader(GL_VERTEX_SHADER);
	const unsigned fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vshader, 1, &vertexshader, NULL);
	glShaderSource(fshader, 1, &fragmentshader, NULL);
	glCompileShader(vshader);
	glCompileShader(fshader);
	int success = 0;
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if(success == 0)
	{
		char buffer[2000];
		glGetShaderInfoLog(vshader, 2000, NULL, buffer);
		throw std::runtime_error(std::string("vertex shader:\n") + buffer);
	}
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if(success == 0)
	{
		char buffer[2000];
		glGetShaderInfoLog(fshader, 2000, NULL, buffer);
		throw std::runtime_error(std::string("fragment shader:\n") + buffer);
	}
	program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glUseProgram(program);

	uniform.size = get_uniform("size");
	glUniform1f(uniform.size, Orb::SIZE);

	float matrix[16];
	initortho(matrix, world.left, world.right, world.bottom, world.top, -1.0f, 1.0f);
	uniform.projection = get_uniform("projection");
	glUniformMatrix4fv(uniform.projection, 1, false, matrix);

	load_texture();

	const float verts[] =
	{
		-0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, 0.5f, (1.0f / 6.0f), 0.0f,
		0.5f, -0.5f, (1.0f / 6.0f), 1.0f
	};

	const unsigned int indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vbo_attribute);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float) * 4, NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(float) * 4, (void*)(sizeof(float) * 2));

	glBindBuffer(GL_ARRAY_BUFFER, vbo_attribute);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 4 * sizeof(float), NULL);
	glVertexAttribPointer(3, 1, GL_FLOAT, false, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glVertexAttribPointer(4, 1, GL_FLOAT, false, 4 * sizeof(float), (void*)(3 * sizeof(float)));
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count * 4, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	// generate the orbs
	for(int i = 0; i < count; ++i)
		orb_list.push_back({});
}

void Orbs::step()
{
	int index = 0;
	for(Orb &orb : orb_list)
	{
		orb.x += orb.xv;
		orb.y += orb.yv;
		orb.rot += orb.rotv;

		for(Orb &o : orb_list)
		{
			if(&o == &orb)
				continue;

			orb.checkcollision(o);
		}

		if(orb.x + Orb::SIZE > world.right)
		{
			orb.x = world.right - Orb::SIZE;
			orb.xv = -orb.xv;
		}
		else if(orb.x < world.left)
		{
			orb.x = world.left;
			orb.xv = -orb.xv;
		}
		if(orb.y + Orb::SIZE > world.bottom)
		{
			orb.y = world.bottom - Orb::SIZE;
			orb.yv = -orb.yv;
		}
		else if(orb.y < world.top)
		{
			orb.y = world.top;
			orb.yv = -orb.yv;
		}

		attributes[index + 0] = orb.x;
		attributes[index + 1] = orb.y;
		attributes[index + 2] = orb.rot;
		attributes[index + 3] = orb.texture / 6.0f;
		index += 4;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo_attribute);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * orb_list.size() * 4, attributes.get());
}

void Orbs::render() const
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, orb_list.size());
}

void Orbs::stop()
{
	glDeleteProgram(program);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vbo_attribute);
	glDeleteVertexArrays(1, &vao);
	glDeleteTextures(1, &texture);
}

void Orbs::add()
{
	attributes.reset(new float[(orb_list.size() + 1) * 4]);
	orb_list.push_back({});
	glBindBuffer(GL_ARRAY_BUFFER, vbo_attribute);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * orb_list.size() * 4, NULL, GL_DYNAMIC_DRAW);
}

void Orbs::remove()
{
	if(orb_list.size() < 1)
		return;

	orb_list.erase(orb_list.begin());
	glBindBuffer(GL_ARRAY_BUFFER, vbo_attribute);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * orb_list.size() * 4, NULL, GL_DYNAMIC_DRAW);
}

void Orbs::load_texture()
{
	unsigned short width, height;
	const unsigned char *raw;
#ifdef _WIN32
	HRSRC res = FindResource(GetModuleHandle(NULL), "#1", "TARGA");
	if(res == NULL)
		throw std::runtime_error("FindResource failure");
	HGLOBAL handle = LoadResource(GetModuleHandle(NULL), res);
	if(handle == NULL)
		throw std::runtime_error("LoadResource failure");
	unsigned char *data = (unsigned char*)LockResource(handle);
	if(data == NULL)
		throw std::runtime_error("could not find the orb resource");
	memcpy(&width, data + 12, sizeof(width));
	memcpy(&height, data + 14, sizeof(height));
	raw = (unsigned char*)data + 18;
#else
	FILE *file = fopen("orb.tga", "rb");
	if(file == NULL)
		throw std::runtime_error("could not find orb.tga");

	fseek(file, 12, SEEK_SET);
	fread(&width, 1, sizeof(width), file);
	fread(&height, 1, sizeof(height), file);
	fseek(file, 18, SEEK_SET);
	const int data_size = width * height * 4;
	std::unique_ptr<unsigned char[]> data(new unsigned char[data_size]);
	fread(data.get(), 1, data_size, file);
	raw = data.get();
	fclose(file);
#endif // _WIN32

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, raw);
}

void *Orbs::getproc(const char *name)
{
	void *fn = SDL_GL_GetProcAddress(name);

	if(fn == NULL)
		throw std::runtime_error(std::string("could not load function address \"") + name + "\"");

	return fn;
}

int Orbs::get_uniform(const char *name) const
{
	const int u = glGetUniformLocation(program, name);
	if(u == -1)
		throw std::runtime_error(std::string("could get uniform variable \"") + name + "\"");

	return u;
}

void Orbs::init_extensions()
{
	glCreateShader = (decltype(glCreateShader))getproc("glCreateShader");
	glShaderSource = (decltype(glShaderSource))getproc("glShaderSource");
	glCompileShader = (decltype(glCompileShader))getproc("glCompileShader");
	glGetShaderiv = (decltype(glGetShaderiv))getproc("glGetShaderiv");
	glGetShaderInfoLog = (decltype(glGetShaderInfoLog))getproc("glGetShaderInfoLog");
	glAttachShader = (decltype(glAttachShader))getproc("glAttachShader");
	glLinkProgram = (decltype(glLinkProgram))getproc("glLinkProgram");
	glDeleteShader = (decltype(glDeleteShader))getproc("glDeleteShader");
	glCreateProgram = (decltype(glCreateProgram))getproc("glCreateProgram");
	glUseProgram = (decltype(glUseProgram))getproc("glUseProgram");
	glDeleteProgram = (decltype(glDeleteProgram))getproc("glDeleteProgram");
	glGenVertexArrays = (decltype(glGenVertexArrays))getproc("glGenVertexArrays");
	glGenBuffers = (decltype(glGenBuffers))getproc("glGenBuffers");
	glBindVertexArray = (decltype(glBindVertexArray))getproc("glBindVertexArray");
	glBindBuffer = (decltype(glBindBuffer))getproc("glBindBuffer");
	glBufferData = (decltype(glBufferData))getproc("glBufferData");
	glVertexAttribPointer = (decltype(glVertexAttribPointer))getproc("glVertexAttribPointer");
	glEnableVertexAttribArray = (decltype(glEnableVertexAttribArray))getproc("glEnableVertexAttribArray");
	glDeleteVertexArrays = (decltype(glDeleteVertexArrays))getproc("glDeleteVertexArrays");
	glDeleteBuffers = (decltype(glDeleteBuffers))getproc("glDeleteBuffers");
	glGetUniformLocation = (decltype(glGetUniformLocation))getproc("glGetUniformLocation");
	glUniformMatrix4fv = (decltype(glUniformMatrix4fv))getproc("glUniformMatrix4fv");
	glVertexAttribDivisor = (decltype(glVertexAttribDivisor))getproc("glVertexAttribDivisor");
	glUniform1f = (decltype(glUniform1f))getproc("glUniform1f");
	glDrawElementsInstanced = (decltype(glDrawElementsInstanced))getproc("glDrawElementsInstanced");
	glBufferSubData = (decltype(glBufferSubData))getproc("glBufferSubData");
}

void Orbs::initortho(float *matrix,float left,float right,float bottom,float top,float znear,float zfar)
{
	matrix[0]=2.0f/(right-left);
	matrix[1]=0.0f;
	matrix[2]=0.0f;
	matrix[3]=0.0f;
	matrix[4]=0.0f;
	matrix[5]=2.0f/(top-bottom);
	matrix[6]=0.0f;
	matrix[7]=0.0f;
	matrix[8]=0.0f;
	matrix[9]=0.0f;
	matrix[10]=-2.0f/(zfar-znear);
	matrix[11]=0.0f;
	matrix[12]=-((right+left)/(right-left));
	matrix[13]=-((top+bottom)/(top-bottom));
	matrix[14]=-((zfar+znear)/(zfar-znear));
	matrix[15]=1.0f;
}
