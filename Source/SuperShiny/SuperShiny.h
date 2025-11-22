#pragma once
#include "../ssge/IGame.h"
#include "../ssge/PassKey.h"
#include "../ssge/MenuSystem.h"
#include "../ssge/IniFile.h"

using namespace ssge;

class ssge::Scene;
class ssge::ScenesAccess;
class ssge::Entity;
class ssge::EntitiesAccess;
class ssge::MenuHeader;

// GAMEDEV: Please forward-declare your scenes here

class SplashScreen;
class TitleScreen;
class VictoryScreen;

// GAMEDEV: Please forward-declare your entities here

class Shiny;
class Orb;
class Bubble;

// GAMEDEV: Your game class here. It must implement IGame

class SuperShiny : public IGame
{
public:
    static SuperShiny makeGame();
    SuperShiny();
    SuperShiny(const SuperShiny& toCopy) = delete;
    SuperShiny(SuperShiny&& toMove) = default;
    ~SuperShiny() = default;

private:
    void syncSettings(StepContext& context) const;
    bool wannaSaveSettings = false;
    bool queriedToQuit = false;
    bool gameWasWon = false;
    bool processingGameVictory = false;

    void _queryQuit(StepContext& context);

public: //TODO: Encapsulate
    // Called once after SDL + engine subsystems are up.
    void init(StepContext& context) override;

    // Called every frame before SceneManager step
    void step(StepContext& context) override;

    // Called after everything is drawn
    void draw(DrawContext& context) override;

    // Called when it's time to clean up before SDL quits
    void cleanUp(PassKey<Engine> pk) override;

    bool saveSettings(StepContext& context) override;

    // The game will save the settings later, don't worry
    void saveSettings() override;

    void queryQuit() override;

    void declareVictory() override;

    bool _joypadGotUnplugged = false;
    void joypadGotUnplugged() override;

    // Get application title
    const char* getApplicationTitle() override;

    int getVirtualWidth() override;

    int getVirtualHeight() override;

    MenuCommandEx onHavingBackedOutOfMenus(PassKey<GameAccess> pk, MenuContext& context) override;

    // Scenes
    class Scenes : public IGameScenes
    {
    public: //TODO: Encapsulate
        Scenes(PassKey<SuperShiny> pk);
        Scenes(const Scenes& toCopy) = delete;
        Scenes(Scenes&& toMove) = delete;
        ~Scenes() = default;

        // GAMEDEV: Create methods that create your Scenes
        // Example: static std::unique_ptr<YourSceneClass> yourSceneClass();

        static std::unique_ptr<SplashScreen> splashScreen();
        static std::unique_ptr<TitleScreen> titleScreen();
        static std::unique_ptr<VictoryScreen> victoryScreen();

        std::unique_ptr<Scene> createScene(PassKey<ScenesAccess> pk, std::string id) override;
        std::string getMainMenuSceneClassID() const override;
    };

    Scenes scenes;
    IGameScenes& getScenes(PassKey<ScenesAccess> pk) override;

    // Entities
    class Entities : public IGameEntities
    {
        // GAMEDEV: Create methods that create your Entities
        // Example: static std::shared_ptr<YourEntityClass> yourEntityClass();

        std::shared_ptr<Shiny> shiny();
        std::shared_ptr<Orb> orb();
        std::shared_ptr<Bubble> bubble();

    public: //TODO: Encapsulate
        Entities(PassKey<SuperShiny> pk);
        Entities(const Entities& toCopy) = delete;
        Entities(Entities&& toMove) = delete;
        ~Entities() = default;


    public:
        std::shared_ptr<Entity> createEntity(PassKey<EntitiesAccess> pk, std::string id) override;
    };

    Entities entities;
    IGameEntities& getEntities(PassKey<EntitiesAccess> pk) override;

    // Sprites
    class Sprites : public IGameSprites
    {
        Sprite::Definition sprdefShiny;
        Sprite::Definition sprdefOrb;
        Sprite::Definition sprdefBubble;

    public: //TODO: Encapsulate
        Sprites(PassKey<SuperShiny> pk);
        Sprites(const Sprites& toCopy) = delete;
        Sprites(Sprites&& toMove) = delete;
        ~Sprites() = default;

        bool load(std::string sprdefId, SDL_Renderer* renderer) override;
        void unload(std::string sprdefId) override;
        const Sprite::Definition* fetchDefinition(const std::string& sprdefId) override;
    private:
        Sprite::Definition* fetchDefinitionNonConst(std::string sprdefId);
    };

    Sprites sprites;
    IGameSprites& getSprites() override;

    class Config;

    class Menus
    {
        friend class SuperShiny;
        MenuHeader mainMenu;
        MenuHeader pauseMenu;
        MenuHeader joypadUnpluggedMenu;
        MenuHeader gameOverMenu;
        MenuHeader levelCompleteMenu;
        MenuHeader victoryMenu;
        MenuHeader highScoreMenu;
        MenuHeader confirmExitProgram;
        MenuHeader creditsMenu;
        MenuHeader creditsMenu2;
        MenuHeader creditsMenu3;
        MenuHeader creditsMenu4;
        MenuHeader creditsMenu5;
        MenuHeader optionsMenu;
        MenuHeader inputConfigMenu;
        MenuHeader volumeControlMenu;
        MenuHeader displaySettingsMenu;
        MenuHeader confirmRestart;
        MenuHeader confirmExitGame;
        MenuHeader confirmAbruptExit;
        MenuHeader levelSelect;
        MenuHeader friendlyReminderReset;
        MenuHeader friendlyReminderExit;

        int levelSelectorInt = 1;

    public:
        void init(SuperShiny::Config& config);
        void refreshHighScoreMenu(MenuContext& context, int direction);
    };

    Menus menus;

    class Config
    {
    public:
        int masterVolume = 100;
        int sfxVolume = 100;
        int musicVolume = 100;
        int resolutionScaleConfig = 1;
        bool fullScreen = false;
        bool integralUpscale = false;

        bool load(IniFile& iniFile);
        bool save(IniFile& iniFile) const;
    };

    Config config;

};
