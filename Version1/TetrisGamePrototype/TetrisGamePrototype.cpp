/**
* \file    TetrisGamePrototype.h
* \date    2018/02/22
* \author  developer
* \brief   The example SFML tetris game
*/
#include <iostream>
#include <string>
using namespace std;

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics/Sprite.hpp>

// Uygulamaya iliskin sabitler
constexpr uint32_t gIninitalWindowWidth = 480;
constexpr uint32_t gIninitalWindowHeight = 640;
const sf::Vector2f gGridInitialOffset{ 158.0F, 109.0F };

constexpr float    gTetrisTileSizeF = 17.0F;
constexpr uint32_t gTetrisTileSize = 17U;
constexpr uint32_t gTetrisTileCount = 7U;

const string gTitle = "Tetris Oyunu v1.0";

constexpr uint32_t gGridRowCount = 26u;
constexpr uint32_t gGridColCount = 16u;
constexpr float gDelay = 0.3F;

uint32_t gField[gGridRowCount][gGridColCount] = { 0 };

// Tetris taslarini asagidaki gibi bir gride yerlestiriyoruz. x pozisyonu icin % y icin ise /2 kullanilabilir
// 0-1
// 2-3
// 4-5
// 6-7
constexpr uint32_t gFigures[gTetrisTileCount][4] =
{
    1,3,5,7, // I
    2,4,5,7, // Z
    3,5,4,6, // S
    3,5,4,7, // T
    2,3,5,7, // L
    3,5,7,6, // J
    2,3,4,5, // O
};

struct Point
{
    uint32_t x, y;
} a[4], b[4];

// Uygulamaya iliskin kullanacagimiz kaynaklar
sf::Text gameOverText;
sf::RectangleShape textBackgroundRect;
sf::FloatRect gameOverTextRect;

uint32_t score = 0;
sf::Text scoreText;
sf::FloatRect scoreTextRect;

uint32_t lastDuration = 0;
sf::Text durationText;
sf::FloatRect durationTextRect;

sf::Font font;
sf::Texture tileSpriteTexture, backgroundSpriteTexture;
sf::Sprite tilesSprite, backgroundSprite;

int32_t checkAndLoadFromTexture(sf::Texture& texture, const string& name)
{
    if (!texture.loadFromFile(name))
    {
        cout << "Texture yuklenmesinde hata. Texture ismi:" << name << endl;
        return -1;
    }
    else
    {
        return 0;
    }
}

// Gerekli kaynaklari yukleyelim
int32_t loadResources()
{
    // Metinler icin kullanacagimiz font
    if (!font.loadFromFile("Data/sansation.ttf"))
    {
        cout << "Font yuklenmesinde hata!" << endl;
        return -1;
    }

    if (0 != checkAndLoadFromTexture(tileSpriteTexture, "Data/tiles.png"))
    {
         return -1;
    }
    else
    {
        tilesSprite.setTexture(tileSpriteTexture);
    }

    if (0 != checkAndLoadFromTexture(backgroundSpriteTexture, "Data/TetrisBackground.png"))
    {
    return -1;
    }
    else
    {
    backgroundSprite.setTexture(backgroundSpriteTexture);
    }

    return 0;
}

// Gerekli ilklendirmeleri gerceklestir
int32_t initialize()
{
    if (0 != loadResources())
    {
        return -1;
    }

    // Oyun bitis mesaji
    gameOverText.setPosition({ gIninitalWindowWidth / 2, gIninitalWindowHeight / 2 });
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(20);
    gameOverText.setFillColor(sf::Color::Green);
    gameOverText.setString("                         Oyun Bitti!\nTekrar baslamak icin Space Bar'a basiniz");

    // Metini merkezlemek icin
    gameOverTextRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(gameOverTextRect.left + gameOverTextRect.width / 2.0f, 
        gameOverTextRect.top + gameOverTextRect.height / 2.0f);
    
    // Metin arka plani
    textBackgroundRect = sf::RectangleShape{ sf::Vector2f{ gameOverTextRect.width + 10, gameOverTextRect.height + 10} };
    textBackgroundRect.setFillColor(sf::Color(0, 0, 0, 200));
    textBackgroundRect.setPosition({ gIninitalWindowWidth / 2, gIninitalWindowHeight / 2 });
    textBackgroundRect.setOrigin(gameOverTextRect.left + gameOverTextRect.width / 2.0f,
        gameOverTextRect.top + gameOverTextRect.height / 2.0f);

    // Puan
    scoreText.setPosition({ 84, 153});
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setStyle(sf::Text::Bold);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString("0");
    
    // Metini merkezlemek icin
    scoreTextRect = scoreText.getLocalBounds();
    scoreText.setOrigin(scoreTextRect.left + scoreTextRect.width / 2.0f,
        scoreTextRect.top + scoreTextRect.height / 2.0f);

    // Sure
    durationText.setPosition({ 84, 233 });
    durationText.setFont(font);
    durationText.setCharacterSize(24);
    durationText.setStyle(sf::Text::Bold);
    durationText.setFillColor(sf::Color::White);
    durationText.setString("0");

    // Metini merkezlemek icin
    durationTextRect = durationText.getLocalBounds();
    durationText.setOrigin(durationTextRect.left + durationTextRect.width / 2.0f,
        durationTextRect.top + durationTextRect.height / 2.0f);

    return 0;
}

bool check()
{
    for (int32_t i = 0; i < 4; ++i)
    {
        if (a[i].x < 0 
            ||
            a[i].x >= gGridColCount
            ||
            a[i].y >= gGridRowCount)
        {
            return false;
        }
        else if (gField[a[i].y][a[i].x])
        {
            return false;
        }
    }

    return true;
}

void clearTable()
{
    for (int i = 0; i < gGridRowCount; i++)
    {
        for (int j = 0; j < gGridColCount; j++)
        {
            gField[i][j] = 0;
        }
    }    
}

// Mevcut tetris tasinin rengi
uint32_t gCurrentTetrisFigureColor;

void newTile()
{
    // Yeni tetris figur rengi
    gCurrentTetrisFigureColor = 1 + rand() % gTetrisTileCount;

    int n = rand() % gTetrisTileCount;
    for (int i = 0; i < 4; i++)
    {
        // Son bolme ve cikarma islemi ortadan tasin gelmesi icin
        a[i].x = gFigures[n][i] % 2 + gGridColCount / 2 - 1;
        a[i].y = gFigures[n][i] / 2;
    }
}

int main()
{   
    srand(static_cast<uint32_t>(0));

    // Uygulamaya ilişkin pencereyi oluşturalım
    sf::RenderWindow window(sf::VideoMode(gIninitalWindowWidth, gIninitalWindowHeight, 32), gTitle.c_str(), sf::Style::Titlebar | sf::Style::Close);

    // Ilklendirmeleri gerceklestir
    if (0 != initialize())
    {
        cout << "Ilklendirmelerde hata olustu!" << endl;
        return -1;
    }
    
    // X hareket
    int32_t dx = 0;

    // Su anki tetris figuru dondurulmesi
    bool rotate = false;

    // Oyun bitti mi kontrolü
    bool isGameOver = false;

    float timer = 0.0f;
    float delay = gDelay;
    sf::Clock clock;

    // Oyun zamanini gostermek icin kullanacagim saat
    sf::Clock gameDurationClock;

    // Yeni tetris tasini olustur
    newTile();

    while (window.isOpen())
    {
        // Gecen zamani hesaplayalim
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        // Pencere olayi nesnesi
        sf::Event event;

        while (window.pollEvent(event))
        {
            // Pencere kapandi ya da esc ye basildi
            if ((event.type == sf::Event::Closed)
                ||
                ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
            {
                window.close();
                break;
            }

            // Space key tuşuna basıldı
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
            {
                if (true == isGameOver)
                {
                    cout << "Space Bar'a basildi" << endl;

                    isGameOver = false;

                    // Tahtayi temizleyelim
                    clearTable();

                    // Yeni tetris tasini olustur
                    newTile();

                    gameDurationClock.restart();
                }
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Up)
                {
                    rotate = true;
                }
                else if (event.key.code == sf::Keyboard::Left)
                {
                    dx = -1;
                }
                else if (event.key.code == sf::Keyboard::Right)
                {
                    dx = 1;
                }
            }        
        }

        // Oyun bitmedi ise oyun mantigini kosturmaya devam edelim
        if (false == isGameOver)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                delay = 0.05f;

            // Tetris tas hareket ettirme
            for (int32_t i = 0; i < 4; ++i)
            {
                b[i] = a[i];
                a[i].x += dx;
            }

            if (!check())
            {
                for (int32_t i = 0; i < 4; ++i)
                {
                    a[i] = b[i];
                }
            }

            // Tetris tas dondurme
            if (true == rotate)
            {
                // Dondurme referans noktasi
                Point p = a[1];

                for (int32_t i = 0; i < 4; ++i)
                {
                    int32_t x = a[i].y - p.y;
                    int32_t y = a[i].x - p.x;

                    a[i].x = p.x - x;
                    a[i].y = p.y + y;
                }

                if (!check())
                    for (int i = 0; i < 4; i++)
                    {
                        a[i] = b[i];
                    }
            }

            // Dikey hareket
            if (timer > delay)
            {
                for (int32_t i = 0; i < 4; ++i)
                {
                    b[i] = a[i];
                    a[i].y++;
                }

                // Ilgili tasin hareketi durur ve sahada ilgili yerler o renge boyanir
                if (!check())
                {
                    for (int i = 0; i < 4; i++)
                    {
                        gField[b[i].y][b[i].x] = gCurrentTetrisFigureColor;
                    }

                    // Yeni tetris tasini olustur
                    newTile();
                }

                timer = 0;
            }

            // Herhangi bir satir doldu mu kontrolü
            int k = gGridRowCount - 1;
            for (int i = gGridRowCount - 1; i > 0; i--)
            {
                int count = 0;
                for (int j = 0; j < gGridColCount; j++)
                {
                    if (gField[i][j])
                        count++;

                    gField[k][j] = gField[i][j];
                }

                // Hala boslar var
                if (count < gGridColCount)
                    k--;
                else
                {
                    // Satir getti, puani ekle
                    ++score;
                }
            }

            // Oyun bitti mi, kontrolü
            for (int i = 0; i < gGridColCount; i++)
            {
                if (gField[0][i] || gField[1][i])
                {
                    isGameOver = true;
                    break;
                }
            }
        }

        dx = 0;
        rotate = false;
        delay = gDelay;

        // Cizim ile ilgili kisimlar
        // Pencereyi temizle
        window.clear(sf::Color::White);
        window.draw(backgroundSprite);

        // Daha once tahtaya inen taslar
        for (int i = 0; i < gGridRowCount; i++)
        {
            for (int j = 0; j < gGridColCount; j++)
            {
                if (gField[i][j] == 0)
                    continue;

                tilesSprite.setTextureRect(sf::IntRect(gField[i][j] * gTetrisTileSize, 0, gTetrisTileSize, gTetrisTileSize));
                tilesSprite.setPosition(j * gTetrisTileSizeF, i * gTetrisTileSizeF);

                // Tahta baslangic noktasi
                tilesSprite.move(gGridInitialOffset);
                window.draw(tilesSprite);
            }
        }

        // Su an asagi inen tas
        for (int i = 0; i < 4; i++)
        {
            tilesSprite.setTextureRect(sf::IntRect(gCurrentTetrisFigureColor * gTetrisTileSize, 0, gTetrisTileSize, gTetrisTileSize));
            tilesSprite.setPosition(a[i].x * gTetrisTileSizeF, a[i].y * gTetrisTileSizeF);
            
            // Tahta baslangic noktasi
            tilesSprite.move(gGridInitialOffset);
            window.draw(tilesSprite);
        }

        // Oyun bitis metin gosterimi
        if (true == isGameOver)
        {
            window.draw(textBackgroundRect);
            window.draw(gameOverText);
        }

        // Puan gosterimi
        scoreText.setString(std::to_string(score));
        window.draw(scoreText);

        // Zaman gosterimi
        // Oyun bitince zaman ilerlemesin
        if (false == isGameOver) 
        {
            lastDuration = static_cast<uint32_t>(gameDurationClock.getElapsedTime().asSeconds());
            durationText.setString(std::to_string(lastDuration));
        }
        window.draw(durationText);

        // Butun her seyi gosterelim
        window.display();
    }

    return 0;
}

