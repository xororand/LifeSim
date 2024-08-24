#include <iostream>
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <deque>

#include "agent.h"

using namespace sf;
using namespace std;

typedef Vector2f v2f;
typedef Vector2u v2u;

RenderWindow* m_window; 
VertexArray agents_points;

deque<матка*> матки;
deque<рабочий*> рабочие;
deque<кокон*> коконы;

int стартовое_колво_муравьев = 4000;
int процент_маток = 1;
int количество_маток = (стартовое_колво_муравьев * процент_маток) / 100;
int количество_рабочих = (стартовое_колво_муравьев - количество_маток ) / количество_маток;

bool is_equal_v2f(v2f a, v2f b) {
    if (round(a.x) == round(b.x) and round(a.y) == round(b.y)) return true;
    else return false;
}

void render(float deltaTime) {
    agents_points = VertexArray(Points, матки.size() + рабочие.size() + коконы.size());

    // симуляция маток
    for (int i = 0; i < матки.size(); i++) {
        if (матки[i] == NULL) continue;
        матки[i]->шаг();
        кокон* m = NULL;
        матки[i]->попытка_сделать_кокон(m, муравей::тип_муравья::рабочий);
        if (m != NULL) {
            коконы.push_back(m);
        }
        agents_points[i].color = матки[i]->цвет;
        agents_points[i].position = матки[i]->позиция;
    }
    
    // симуляция рабочих
    for (int i = 0; i < рабочие.size(); i++) {
        if (рабочие[i] == NULL) continue;

        v2f век_до_еды = v2f(0.0, 0.0);
        float расстояние_до_еды = 0.0;

        //подбираем еду
        for (int y = 0; y < рабочие.size(); y++) {
            if (рабочие[y] == NULL or рабочие[i]->умер) continue;

            // подбираем еду
            if (is_equal_v2f(рабочие[y]->позиция, рабочие[i]->позиция) and рабочие[y]->умер and i != y) {
                if (рабочие[i]->еды + рабочие[y]->жир > рабочие[i]->максимум_еды) {
                    int остаток = (рабочие[i]->еды + рабочие[y]->жир) - рабочие[i]->максимум_еды;
                    рабочие[y]->жир = остаток;
                    рабочие[i]->еды = рабочие[i]->максимум_еды;
                } else {
                    рабочие[i]->еды += рабочие[y]->жир;
                    рабочие[y] = NULL;
                    continue;
                }
            }
            
            if (рабочие[y]->умер) {
                v2f вектор_направления = рабочие[y]->позиция - рабочие[i]->позиция;
                float расстояние = length(вектор_направления);
                if (расстояние_до_еды == 0.0) расстояние_до_еды = расстояние;

                if (расстояние_до_еды > расстояние) {
                    вектор_направления *= расстояние;
                    век_до_еды = вектор_направления;
                }
            }
        }

        // движение по полю
        рабочие[i]->шаг(век_до_еды);

        agents_points[матки.size() + i].color = рабочие[i]->цвет;
        agents_points[матки.size() + i].position = рабочие[i]->позиция;
    }

    // симуляция коконов
    for (int i = 0; i < коконы.size(); i++) {
        if (коконы[i] == NULL) continue;

        коконы[i]->шаг();
        if (коконы[i]->готов) {
            switch (коконы[i]->тип) {
                case муравей::рабочий:
                    рабочий* м = new рабочий();
                    м->еды = коконы[i]->еды;
                    рабочие.push_back(м);
                    коконы[i] = NULL;
                    continue;
                //case муравей::королева:
                //
                //    break;
            }
        }
        agents_points[матки.size() + рабочие.size() + i].color = коконы[i]->цвет;
        agents_points[матки.size() + рабочие.size() + i].position = коконы[i]->позиция;
    }

    m_window->draw(agents_points);
}

int main()
{
    setlocale(LC_ALL, "RUS");
    IMGUI_CHECKVERSION();

    // создание маток
    for (int i = 0; i < количество_маток; i++) {
        матка* мтк = new матка();
        мтк->позиция = v2f(rand() % 1920, rand() % 1080);
        мтк->цвет = Color(0.0, rand() % 255 + i, rand() % 255 + i);
        матки.push_back(мтк);
        Color цвет_рабочего = Color(0.0, rand() % 255 + i, rand() % 255 + i);
        for (int r = 0; r < количество_рабочих; r++) {
            рабочий* рбч = new рабочий();
            рбч->цвет = цвет_рабочего;
            рбч->позиция = v2f(rand() % 1920, rand() % 1080);
            рбч->наша_матка = мтк; // привязываем к матке
            рабочие.push_back(рбч);
        }
    }

    RenderWindow window(sf::VideoMode(1920, 1080), "2D GAME", Style::Fullscreen);
    m_window = &window;
    window.setFramerateLimit(144);
    window.setVerticalSyncEnabled(true);

    ImGui::SFML::Init(window);
    ImGui::SFML::UpdateFontTexture();

    Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                //loops.setScreenHeight(event.size.height);
                //loops.setScreenWidth(event.size.width);
            }
        }

        Time deltaTime = deltaClock.restart();

        ImGui::SFML::Update(window, deltaTime);

        window.clear(Color(10.0, 10.0, 10.0, 0));

        render(deltaTime.asSeconds());

        ImGui::SFML::UpdateFontTexture();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}
