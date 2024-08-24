#pragma once
#include <iostream>
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <deque>
#include <cmath>
using namespace sf;
using namespace std;

typedef Vector2f v2f;
typedef Vector2u v2u;

float float_rand(float min, float max) {
    float scale = rand() / (float)RAND_MAX; /* [0, 1.0] */
    return min + scale * (max - min);      /* [min, max] */
}
float length(v2f vec)
{
    return (float)sqrt((vec.x * vec.x) + (vec.y * vec.y));
}

/*
�������:
1 ��� - ����� 1 ���
*/

class ������� {
public:
    enum ���_������� {
        �������,
        ��������
    };

    v2f �������;            // �������� �������� � ������ ���������
    float ��� = 0.0f;                // ���-�� ���, ��� 0 - ������
    float ��� = 0;                // ���� �������� ����� ������� ������
    float ��������_���;
    float ��������_����;
    float �����_���� = 1.0;
    Color ����;


    bool ���� = false;
};

class ����� : public ������� {
public:
    bool �����;
    ���_������� ���;
    �����(float ���_, ���_������� ���_) {
        ��� = ���_;
        ��� = ���_;
    }
    void ���() {
        if (��� > 0.0f) {
            ��� += 1.0f;
            ��� -= 1.0f;
            �����_���� += float_rand(-0.001, 0.001);
        }
        else {
            ����� = true;
        }
    }
};

class ����� : public ������� {
public:
    int ���_���_������ = 10;
    �����() {
        �����_���� = 1.0;
        ��� = 10000;
        ��������_��� = ���;
        ��������_���� = ��������_���;
    }
    void �������_�������_�����(�����* m, ���_������� ���_) {
        if (��� > ��������_��� * 0.1 and ��� > ���_���_������) {
            ��� -= ���_���_������;
            m = new �����(���_���_������, ���_);
            m->������� = �������;
        }
        m = NULL;
    }
    void ���() {
        if (����) return;
        ���--;
        if (��� < ��������_����) ���++;
        ������� += v2f(float_rand(-�����_����, �����_����), float_rand(-�����_����, �����_����));
    }
};

class ������� : public ������� {
public:
    �����* ����_�����;
    �������() {
        �����_���� = float_rand(1.0, 2.0);
        ��� = float_rand(100, 1000);
        ��������_��� = ���;
        ��������_���� = ��������_���;
    }
    void ���(v2f ���_��_���) {
        if (����) return;
        if (��� <= 0.0f) {
            ��� = 0.0f;
            ���� = true;
            ���� = Color(255, 255, 255);
            return;
        }
        ��� -= 1.0f;
        if (��� < ��������_����) ��� += 1.0f;
        if (��� > ��������_��� / 2) { // ���� � ����� �������� ���
            v2f ������_����������� = ����_�����->������� - �������;
            float ����� = 1.0f / length(������_�����������);
            ������_����������� *= �����;
            ������� += ������_����������� * �����_����;
        }
        else {
            ������� += ���_��_��� * �����_����; //v2f(float_rand(-�����_����, �����_����), float_rand(-�����_����, �����_����));
        }

    }
};

