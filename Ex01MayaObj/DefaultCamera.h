#ifndef MAYA_CAMERA_H
#define MAYA_CAMERA_H
#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include "glut.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class DefaultCamera
{
public:
    DefaultCamera()
    {
        eye[0] = 0.0f;
        eye[1] = 0.0f;
        eye[2] = 3.0f;
        center[0] = 0.0f;
        center[1] = 0.0f;
        center[2] = 0.0f;
        dist = 3.0f;
        pitch = 0.0f;
        yaw = 0.0f;

        prevX = 0;
        prevY = 0;
        screenWidth = 1;
        screenHeight = 1;
        isMouseDragging = false;
    }
    ~DefaultCamera()
    {
    }

public:
    void OnMouseButton(int button, int state, int x, int y)
    {
        isMouseDragging = false;

        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            prevX = x;
            prevY = y;
            isMouseDragging = true;
            // �h���[�C��
            if (x > screenWidth / 4 && x < 3 * screenWidth / 4 && y > screenHeight / 4 && y < 3 * screenHeight / 4)
            {
                dist -= 0.5;
            }
        }
        else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        {
            // �h���[�A�E�g
            if (x > screenWidth / 4 && x < 3 * screenWidth / 4 && y > screenHeight / 4 && y < 3 * screenHeight / 4)
            {
                dist += 0.5;
            }
            // �g���b�N
            else if ((x < screenWidth / 4 || x > 3 * screenWidth / 4) && (y > screenHeight / 4 && y < 3 * screenHeight / 4))
            {
                if (x < screenWidth / 4)
                {
                    yaw -= 1.0;
                }
                else
                {
                    yaw += 1.0;
                }
                center[2] = eye[2] - dist * cos(M_PI / 180.0 * yaw) * cos(M_PI / 180.0 * pitch);
                center[0] = eye[0] - dist * sin(M_PI / 180.0 * yaw) * cos(M_PI / 180.0 * pitch);
            }
            // �N���[��
            else if ((x > screenWidth / 4 && x < 3 * screenWidth / 4) && (y < screenHeight / 4 || y > 3 * screenHeight / 4))
            {
                if (y < screenHeight / 4)
                {
                    pitch += 1.0;
                }
                else
                {
                    pitch -= 1.0;
                }
                center[2] = eye[2] - dist * cos(M_PI / 180.0 * pitch) * cos(M_PI / 180.0 * yaw);
                center[0] = eye[0] - dist * cos(M_PI / 180.0 * pitch) * sin(M_PI / 180.0 * yaw);
                center[1] = eye[1] - dist * sin(M_PI / 180.0 * pitch);
            }
        }
        Update();
    }
    void OnMouseMove(int x, int y)
    {
        if (!isMouseDragging)
        {
            return;
        }
        // �^���u��
        if (cos(M_PI * pitch / 180.0) >= 0.0)
        {
            yaw -= 0.5 * (x - prevX);
        }
        else
        {
            yaw += 0.5 * (x - prevX);
        }
        pitch += 0.5 * (y - prevY);

        prevX = x;
        prevY = y;
        Update();
    }
    void OnResize(int w, int h)
    {
        screenWidth = w;
        screenHeight = h;
    }
    void Update()
    {
        eye[0] = center[0] + dist * sin(M_PI / 180.0 * yaw) * cos(M_PI / 180.0 * pitch);
        eye[1] = center[1] + dist * sin(M_PI / 180.0 * pitch);
        eye[2] = center[2] + dist * cos(M_PI / 180.0 * yaw) * cos(M_PI / 180.0 * pitch);
    }
    void SetViewMatrix()
    {
        if (cos(M_PI * pitch / 180.0) >= 0.0)
        {
            gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], 0, 1.0, 0.0);
        }
        else
        {
            gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], 0, -1.0, 0.0);
        }
    }

private:
    //! ���_
    float eye[3];
    //! �����_
    float center[3];
    //! ���_�ƒ����_�̋���
    float dist;
    //! ������]�p�F���[�p
    float yaw;
    //! ������]�p�F�s�b�`�p
    float pitch;

    /*** �ȉ��C�J��������p�ϐ� ***/
    //! �O�t���[���ł̃}�E�Xx���W
    int prevX;
    //! �O�t���[���ł̃}�E�Xy���W
    int prevY;
    //! �X�N���[����
    int screenWidth;
    //! �X�N���[������
    int screenHeight;
    //! ���݃}�E�X�h���b�O�����H
    bool isMouseDragging;
};

#endif // MAYA_CAMER
