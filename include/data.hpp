
#ifndef DATA_HPP
#define DATA_HPP

#include <vector>
#include <string>

#define JOYSTICK_DOWN 0
#define JOYSTICK_X 2
#define JOYSTICK_CIRCLE 4
#define JOYSTICK_TRIANGLE 8
#define JOYSTICK_QUAD 1
#define JOYSTICK_L1 16
#define JOYSTICK_R1 32
#define JOYSTICK_L3 1024
#define JOYSTICK_R3 2048
#define JOYSTICK_START 256
#define JOYSTICK_R2 128
#define JOYSTICK_OPT 512
#define JOYSTICK_L2 64
#define JOYSTICK_ICON_BUTTON 4096
#define JOYSTICK_TOUCH 8192

// Enumeração para tipos de objetos
enum ObjectType
{
    TREE,
    ROCK,
    BOSS,
    HOUSE,
    GRASS,
    WALL,
    ENEMY,
    ITEM,
    NPC,
    PORTAL,
    DOOR,
    WINDOW,
    TABLE,
    CHAIR,
    LAMP,
    ROOF,
    BONFIRE

};

enum class ModelType
{
    GLD,   // Para arquivos GLD
    OBJ,   // Para arquivos OBJ
    GLTF,  // Para arquivos GLTF
    GLB,   // Para arquivos GLB
    CUSTOM // Para tipos personalizados que você pode carregar sem biblioteca externa
};

enum SkillType
{
    ATTACK,
    DEFENSE,
    MAGIC,
    SPEED,
};

enum class AttackType
{
    PHYSICAL,
    FIRE,
    ICE,
    POISON,
    MAGIC
};

struct Attack
{
    AttackType type;
    float power;
};

enum class MapType
{
    MAIN,
    DUNGEON_ONE_LEVEL,
    DUNGEON_TWO_LEVEL,
    DUNGEON_THREE_LEVEL,
    BOSS,
    PARASIDE,
    HOUSE
};
struct Color
{
    float r = 0.5f, g = 0.5f, b = 0.5f;
};
struct GrassPatch
{
    float x, z;   // centro
    float radius; // raio
};

struct TrailPoint
{
    float x, y, z;
};

struct SkillTooltip
{
    bool visible;
    int skillIndex;
    float x, y; // Posição na tela
    float width, height;
    bool showConfirmation;
};

enum STATE_GAME
{
    PLAYING_EXPLORER,
    COMBAT,
    SKILL_TREE,
    GAME_OVER,
    MENU,
    TOP_VIEW_MAP,
    STATE_EXIT,
};

struct Material
{
    Color ambient;   // Cor ambiente
    Color diffuse;   // Cor difusa
    Color specular;  // Cor especular
    float shininess; // Brilho
};

struct SkillNode
{
    float x, y;     // Posição na tela
    float radius;   // Raio do nó
    int skillIndex; // Índice da habilidade
    bool hovering;  // Se o mouse está sobre o nó
};
struct Translation
{
    float x = 0.0f, y = 0.0f, z = 0.0f;
};

struct Rotation
{
    float x = 0.0f, y = 0.0f, z = 1.0f, angulo = 0.0f;
};

struct Scale
{
    float x = 1.0f, y = 1.0f, z = 1.0f;
};




// Button action enum
enum class ACTION_BUTTON
{
    EXIT,
    CREDITS,
    NONE,
    CONFIG,
    CONTROLS,
    RESET_ALL,

    VOLUME_MUSIC_DECREASE, // Controls omnia_poetree_theme.wav, city_of_ingrigues.wav, bury-the-light-deep-within.wav
    VOLUME_MUSIC_INCREASE,

    VOLUME_EFFECTS_DECREASE, // Controls punch.wav, swipe.wav, classiccoin.wav, espacial-attack.wav, etc.
    VOLUME_EFFECTS_INCREASE,

    VOLUME_AMBIENT_DECREASE, // Controls birds_backgorund.wav, walk-on-grass.wav, dark-void-ambience.wav, etc.
    VOLUME_AMBIENT_INCREASE,

    VOLUME_UI_DECREASE, // Controls one_beep.wav, up-habilidade.wav, etc.
    VOLUME_UI_INCREASE
};

// Simple discriminated union for actions
// Button structure
struct Botao
{
    std::string texto;
    float x, y, width, height;
    float r, g, b;
    bool hovering;
    ACTION_BUTTON destino;
};

struct Volume
{
    float ambient = 0.5f;
    float musica = 0.5f;
    float efeitos = 0.5f;
    float UI = 0.5f;
};

/**
 * @struct BMPHeader
 * @brief Estrutura para armazenar o cabeçalho de arquivos BMP.
 */
#pragma pack(push, 1)
typedef struct
{
    unsigned char bfType[2];    /**< @var Tipo do arquivo ("BM" para Bitmap). */
    unsigned int bfSize;        /**< @var Tamanho total do arquivo em bytes. */
    unsigned short bfReserved1; /**< @var Reservado (deve ser 0). */
    unsigned short bfReserved2; /**< @var Reservado (deve ser 0). */
    unsigned int bfOffBits;     /**< @var Offset onde os dados de imagem começam. */
} BMPHeader;

/**
 * @struct BMPInfoHeader
 * @brief Estrutura para armazenar informações sobre a imagem BMP.
 */
typedef struct
{
    unsigned int biSize;         /**< @var Tamanho da estrutura. */
    int biWidth;                 /**< @var Largura da imagem em pixels. */
    int biHeight;                /**< @var Altura da imagem em pixels. */
    unsigned short biPlanes;     /**< @var Número de planos de cor (deve ser 1). */
    unsigned short biBitCount;   /**< @var Número de bits por pixel. */
    unsigned int biCompression;  /**< @var Tipo de compressão. */
    unsigned int biSizeImage;    /**< @var Tamanho da imagem em bytes. */
    int biXPelsPerMeter;         /**< @var Resolução horizontal (pixels por metro). */
    int biYPelsPerMeter;         /**< @var Resolução vertical (pixels por metro). */
    unsigned int biClrUsed;      /**< @var Número de cores na paleta. */
    unsigned int biClrImportant; /**< @var Número de cores importantes. */
} BMPInfoHeader;
#pragma pack(pop)
#endif