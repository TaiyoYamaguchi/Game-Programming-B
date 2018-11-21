#include <windows.h>
#include "glut.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#ifdef min
#undef min
#endif

// �f�t�H���g�J�����錾�̃C���N���[�h
#include "DefaultCamera.h"
// �f�t�H���g�J�����N���X�̃C���X�^���X�𐶐�
DefaultCamera defaultCamera;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
};

/***** obj �t�@�C���ǂݍ��ݗp *****/
#include <vector>
#include <algorithm>
static const char* delim = " /\t\n";
static char* context = nullptr;
int ReadVector3(glm::vec3 &v, char* buf)
{
#define READ_VECTOR3_TOKEN(ELEMENT) tok = strtok_s(buf, delim, &context); \
    if (tok == nullptr) { return -1; } \
    v.ELEMENT = static_cast<float>(std::atof(tok));
	char* tok = nullptr;
	READ_VECTOR3_TOKEN(x);
	READ_VECTOR3_TOKEN(y);
	READ_VECTOR3_TOKEN(z);
	return 0;
}
int ReadVector2(glm::vec2& v, char* buf)
{
#define READ_VECTOR2_TOKEN(ELEMENT) tok = strtok_s(buf, delim, &context); \
    if (tok == nullptr) { return -1; } \
    v.ELEMENT = static_cast<float>(std::atof(tok))
	char* tok = nullptr;
	READ_VECTOR2_TOKEN(x);
	READ_VECTOR2_TOKEN(y);
	return 0;
}

bool LoadObjModel(std::vector<Vertex> &vertices, std::vector<uint32_t>& indices, const char* fileName)
{
	char buf[1024];
	char* tok = nullptr;

	FILE* fin = nullptr;
	fopen_s(&fin, fileName, "r");
	if (fin == nullptr)
	{
		return false;
	}
    vertices.clear();

    std::vector<glm::vec3> position;
    std::vector<glm::vec3> normal;
    std::vector<glm::vec2> texcoord;

	int hr = 0;
	while (std::fgets(buf, 1024, fin) != nullptr)
	{
		tok = strtok_s(buf, delim, &context);
		if (tok == nullptr)
		{
		}
		else if (tok[0] == '#')
		{
			continue;
		}
		else if (strcmp(tok, "mtllib") == 0 || strcmp(tok, "usemtl") == 0)
		{
			continue;
		}
		else if (strcmp(tok, "s") == 0 || strcmp(tok, "g") == 0)
		{
			continue;
		}
		else if (strcmp(tok, "v") == 0)
		{
			glm::vec3 p;
			if (hr = ReadVector3(p, NULL), hr < 0)
			{
				break;
			}
			position.push_back(p);
		}
		else if (strcmp(tok, "vn") == 0)
		{
			glm::vec3 n;
			if (hr = ReadVector3(n, NULL), hr < 0)
			{
				break;
			}
			normal.push_back(n);
		}
		else if (strcmp(tok, "vt") == 0)
		{
			glm::vec2 t;
			if (hr = ReadVector2(t, NULL), hr < 0)
			{
				break;
			}
			texcoord.push_back(t);
		}
		else if (strcmp(tok, "f") == 0)
		{
			std::string str(buf + 2);
			std::vector<long> tmp;
			std::string::size_type offset = 0, prevOffset = 0;
			while (offset = std::min(str.find('\n', prevOffset), std::min(str.find('/', prevOffset), str.find(' ', prevOffset))), offset != std::string::npos)
			{
				if (prevOffset == offset)
				{
					prevOffset = offset + 1;
					tmp.push_back(-1);
				}
				else
				{
					std::string substr = str.substr(prevOffset, offset - prevOffset);
					tmp.push_back(atoi(substr.c_str()) - 1);
					prevOffset = offset + 1;
				}
			}

			switch (tmp.size())
			{
			case 9:
				for (int i = 0; i < 3; ++i)
				{
					long pi = tmp[i * 3 + 0];
					long ti = tmp[i * 3 + 1];
					long ni = tmp[i * 3 + 2];
                    
                    Vertex v;
                    v.position = position[pi];
					if (ni >= 0)
					{
                        v.normal = normal[ni];
					}
                    vertices.push_back(v);
                    indices.push_back(indices.size());
				}
				break;
			default:
				fclose(fin);
				position.clear();
				normal.clear();
                texcoord.clear();
				return false;
				break;
			}
		}
	}
	fclose(fin);
	return true;
}
/*** obj �t�@�C���ǂݍ��ݗp ***/

// OBJ���f����`�悷��֐��F����modelID�Ɏ��ʔԍ��i���L��objModelID�j��n���ƁA�`�悳���
// �}�e���A�����͂��̊֐����ĂԒ��O�ɐݒ肷��
void drawObjModel(GLuint modelID)
{
	glPushMatrix(); // ���Ԃ񂢂�Ȃ����ǔO�̂��߂�push
	{
		// ���[�h����OBJ���f���̕`��
		glCallList(modelID);
	}
	glPopMatrix(); // ����D�O�̂��߂�pop
}

// ���[�h����OBJ���f���̎��ʔԍ���\���ϐ�
GLuint objModelID = 0;

// �E�B���h�E�T�C�Y�ύX���ɌĂ΂��֐�
void resize(int w, int h)
{
	// �r���[�|�[�g�ϊ��F�E�B���h�E�S�̂��r���[�|�[�g�ɂ���
	glViewport(0, 0, w, h);
	// ���e�ϊ��s��̑�����J�n
	glMatrixMode(GL_PROJECTION);
	// ���e�ϊ��s���P�ʍs��ɏ�����
	glLoadIdentity();
	// �������e�s���ݒ�
	gluPerspective(30.0, (double)w / (double)h, 1.0e-2, 1.0e3);

	// �E�B���h�E���T�C�Y���ɂ̓J�����ɐV�����X�N���[���T�C�Y��o�^����
	defaultCamera.OnResize(w, h);
}

void mouse_button(int button, int state, int x, int y)
{
	// �}�E�X�������ꂽ�Ƃ��ɂ́C���̏����J�����ɑ���
	// �J�����p�����[�^�̕ύX���@�̓J�������g�����߂�
	defaultCamera.OnMouseButton(button, state, x, y);
}

void mouse_drag(int x, int y)
{
	// �}�E�X���h���b�O���ꂽ�Ƃ��ɂ́C���̏����J�����ɑ���
	// �J�����p�����[�^�̕ύX���@�̓J�������g�����߂�
	defaultCamera.OnMouseMove(x, y);
}

// ���ݎ�����b�P�ʁi���������_�`���j�Ŏ擾����֐��Dwindows�̍����\�^�C�}�[�𗘗p
double queryTimeInSeconds()
{
	LARGE_INTEGER freq, count;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&count);
	return (double)count.QuadPart / (double)freq.QuadPart;
}

void idle(void)
{
	glutPostRedisplay();
}

// �`��֐�
void display(void)
{
	// �E�B���h�E�h��Ԃ��@�{�@Z�o�b�t�@�̃N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// ���f�����O�ϊ��Ǝ���ϊ��s��̑�����J�n
	glMatrixMode(GL_MODELVIEW);
	// ���f�����O�ϊ��s��Ǝ���ϊ��s���P�ʍs��ɏ�����
	glLoadIdentity();
	// ����ϊ��s���ݒ�F�@�f�t�H���g�J�����̓����œK�؂ɏ��������
	defaultCamera.SetViewMatrix();
	// ���ݎ�����b�P�ʂŎ擾
	double msec = queryTimeInSeconds();

	// ���C�g�ʒu
	float lightPosition[4] = { 0.0f, 0.0f, 2.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	// �g�U���ˌ�
	float lightDiffuse[3] = { 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	// ���ʔ��ˌ�
	float lightSpecular[3] = { 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	// ����
	float lightAmbient[3] = { 0.25f, 0.25f, 0.25f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

	glPushMatrix();
	{
		// +Y������ɕ������̉�]
		glRotated(-msec * 5.0, 0, 1.0, 0);

		// ����
		float ambient[4] = { 0.25f, 0.25f, 0.25f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		// �g�U����
		float diffuse[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		// ���ʔ���
		float specular[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		// ���������i���C�g�Ɋ֌W�Ȃ����f�����g�����P�x�j
		float emission[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);

		// ���[�h����OBJ���f���̕`��
		drawObjModel(objModelID);
	}
	glPopMatrix();

	// ���̃t���[���̕`��
	glutIdleFunc(idle);
	// �`��o�b�t�@�̐؂�ւ�
	glutSwapBuffers();
}

// main�֐�
int main(int argc, char* argv[])
{
	// �E�B���h�E�T�C�Y
	glutInitWindowSize(640, 480);
	// �E�B���h�E�ʒu
	glutInitWindowPosition(300, 300);
	// GLUT�����OpenGL�̏�����
	glutInit(&argc, argv);
	// �f�B�X�v���C�\�����[�h�ݒ�@�F�@�t���[���o�b�t�@��RGBA�t�H�[�}�b�g�Ƃ��C����Z�o�b�t�@�i�[�x�o�b�t�@�j�����p
	// ����ɃA�j���[�V�����Ή��̂��߁C�_�u���o�b�t�@�̎g�p���w��
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	// �V�����E�B���h�E����
	glutCreateWindow(argv[0]);
	// �`��֐��̓o�^
	glutDisplayFunc(display);
	// �E�B���h�E�T�C�Y�ύX���ɌĂ΂��֐���o�^
	glutReshapeFunc(resize);
	// �}�E�X�̃{�^���������ꂽ�Ƃ��ɌĂяo���֐��̓o�^
	glutMouseFunc(mouse_button);
	// �}�E�X�h���b�O���ɌĂяo���֐��̓o�^
	glutMotionFunc(mouse_drag);
	// �E�B���h�E�h��Ԃ��F
	glClearColor(0.0, 0.2, 0.0, 1.0);
	// Z�o�b�t�@�@�ɂ��B�ʏ����̗L����
	glEnable(GL_DEPTH_TEST);
	// ���C�e�B���O������L����
	glEnable(GL_LIGHTING);
	// �[���Ԗڂ̃��C�g��_��
	glEnable(GL_LIGHT0);

	// OBJ���f���̃��[�h
	objModelID = LoadObjModel("./model.obj");

	// GLUT���C�����[�v
	glutMainLoop();
	// Z�o�b�t�@�@�ɂ��B�ʏ����̖�����
	glDisable(GL_DEPTH_TEST);

	return 0;
}
