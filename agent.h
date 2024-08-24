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
Условия:
1 шаг - минус 1 еда
*/

class муравей {
public:
    enum тип_муравья {
        рабочий,
        королева
    };

    v2f позиция;            // задается случайно в начале симуляции
    float еды = 0.0f;                // кол-во еды, при 0 - смерть
    float жир = 0;                // жира запасено после будущей смерти
    float максимум_еды;
    float максимум_жира;
    float длина_шага = 1.0;
    Color цвет;


    bool умер = false;
};

class кокон : public муравей {
public:
    bool готов;
    тип_муравья тип;
    кокон(float еды_, тип_муравья тип_) {
        еды = еды_;
        тип = тип_;
    }
    void шаг() {
        if (жир > 0.0f) {
            еды += 1.0f;
            жир -= 1.0f;
            длина_шага += float_rand(-0.001, 0.001);
        }
        else {
            готов = true;
        }
    }
};

class матка : public муравей {
public:
    int еды_для_кокона = 10;
    матка() {
        длина_шага = 1.0;
        еды = 10000;
        максимум_еды = еды;
        максимум_жира = максимум_еды;
    }
    void попытка_сделать_кокон(кокон* m, тип_муравья тип_) {
        if (еды > максимум_еды * 0.1 and еды > еды_для_кокона) {
            еды -= еды_для_кокона;
            m = new кокон(еды_для_кокона, тип_);
            m->позиция = позиция;
        }
        m = NULL;
    }
    void шаг() {
        if (умер) return;
        еды--;
        if (жир < максимум_жира) жир++;
        позиция += v2f(float_rand(-длина_шага, длина_шага), float_rand(-длина_шага, длина_шага));
    }
};

class рабочий : public муравей {
public:
    матка* наша_матка;
    рабочий() {
        длина_шага = float_rand(1.0, 2.0);
        еды = float_rand(100, 1000);
        максимум_еды = еды;
        максимум_жира = максимум_еды;
    }
    void шаг(v2f век_до_еды) {
        if (умер) return;
        if (еды <= 0.0f) {
            еды = 0.0f;
            умер = true;
            цвет = Color(255, 255, 255);
            return;
        }
        еды -= 1.0f;
        if (жир < максимум_жира) жир += 1.0f;
        if (еды > максимум_еды / 2) { // идем к матке передать еды
            v2f вектор_направления = наша_матка->позиция - позиция;
            float длина = 1.0f / length(вектор_направления);
            вектор_направления *= длина;
            позиция += вектор_направления * длина_шага;
        }
        else {
            позиция += век_до_еды * длина_шага; //v2f(float_rand(-длина_шага, длина_шага), float_rand(-длина_шага, длина_шага));
        }

    }
};

