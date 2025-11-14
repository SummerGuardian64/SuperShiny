#pragma once
#include "../ssge/IGame.h"
#include "../ssge/PassKey.h"
#include "../ssge/MenuSystem.h"
#include "../ssge/IniFile.h"

class Scene;
class ScenesAccess;
class Entity;
class EntitiesAccess;
class MenuHeader;

// GAMEDEV: Please forward-declare your scenes here

class SplashScreen;
class TitleScreen;

// GAMEDEV: Please forward-declare your entities here

class Shiny;
class Orb;
class Bubble;

// GAMEDEV: Your game class here. It must implement ssge::IGame

class SuperShiny : public ssge::IGame
{
public:
    static SuperShiny makeGame();
    SuperShiny();
    SuperShiny(const SuperShiny& toCopy) = delete;
    SuperShiny(SuperShiny&& toMove) = default;
    ~SuperShiny() = default;

private:
    void syncSettings(ssge::StepContext& context) const;
    bool wannaSaveSettings = false;
    bool queriedToQuit = false;

    void _queryQuit(ssge::StepContext& context);

public: //TODO: Encapsulate
    // Called once after SDL + engine subsystems are up.
    void init(ssge::StepContext& context) override;

    // Called every frame before SceneManager step
    void step(ssge::StepContext& context) override;

    // Called after everything is drawn
    void draw(ssge::DrawContext& context) override;

    // Called when it's time to clean up before SDL quits
    void cleanUp(ssge::PassKey<ssge::Engine> pk) override;

    bool saveSettings(ssge::StepContext& context) override;

    // The game will save the settings later, don't worry
    void saveSettings() override;

    void queryQuit() override;

    // Get application title
    const char* getApplicationTitle() override;

    int getVirtualWidth() override;

    int getVirtualHeight() override;

    ssge::MenuCommandEx onHavingBackedOutOfMenus(ssge::PassKey<ssge::GameAccess> pk, ssge::MenuContext& context) override;

    // Scenes
    class Scenes : public ssge::IGameScenes
    {
    public: //TODO: Encapsulate
        Scenes(ssge::PassKey<SuperShiny> pk);
        Scenes(const Scenes& toCopy) = delete;
        Scenes(Scenes&& toMove) = delete;
        ~Scenes() = default;

        // GAMEDEV: Create methods that create your Scenes
        // Example: static std::unique_ptr<YourSceneClass> yourSceneClass();

        static std::unique_ptr<SplashScreen> splashScreen();
        static std::unique_ptr<TitleScreen> titleScreen();

        std::unique_ptr<ssge::Scene> createScene(ssge::PassKey<ssge::ScenesAccess> pk, std::string id) override;
        std::string getMainMenuSceneClassID() const override;
    };

    Scenes scenes;
    ssge::IGameScenes& getScenes(ssge::PassKey<ssge::ScenesAccess> pk) override;

    // Entities
    class Entities : public ssge::IGameEntities
    {
        // GAMEDEV: Create methods that create your Entities
        // Example: static std::shared_ptr<YourEntityClass> yourEntityClass();

        std::shared_ptr<Shiny> shiny();
        std::shared_ptr<Orb> orb();
        std::shared_ptr<Bubble> bubble();

    public: //TODO: Encapsulate
        Entities(ssge::PassKey<SuperShiny> pk);
        Entities(const Entities& toCopy) = delete;
        Entities(Entities&& toMove) = delete;
        ~Entities() = default;


    public:
        std::shared_ptr<ssge::Entity> createEntity(ssge::PassKey<ssge::EntitiesAccess> pk, std::string id) override;
    };

    Entities entities;
    ssge::IGameEntities& getEntities(ssge::PassKey<ssge::EntitiesAccess> pk) override;

    // Sprites
    class Sprites : public ssge::IGameSprites
    {
        ssge::Sprite::Definition sprdefShiny;
        ssge::Sprite::Definition sprdefOrb;
        ssge::Sprite::Definition sprdefBubble;

    public: //TODO: Encapsulate
        Sprites(ssge::PassKey<SuperShiny> pk);
        Sprites(const Sprites& toCopy) = delete;
        Sprites(Sprites&& toMove) = delete;
        ~Sprites() = default;

        bool load(std::string sprdefId, SDL_Renderer* renderer) override;
        void unload(std::string sprdefId) override;
        const ssge::Sprite::Definition* fetchDefinition(const std::string& sprdefId) override;
    private:
        ssge::Sprite::Definition* fetchDefinitionNonConst(std::string sprdefId);
    };

    Sprites sprites;
    ssge::IGameSprites& getSprites() override;

    class Config;

    class Menus
    {
        friend class SuperShiny;
        ssge::MenuHeader mainMenu;
        ssge::MenuHeader pauseMenu;
        ssge::MenuHeader gameOverMenu;
        ssge::MenuHeader levelCompleteMenu;
        ssge::MenuHeader victoryMenu;
        ssge::MenuHeader highScoreMenu;
        ssge::MenuHeader confirmExitProgram;
        ssge::MenuHeader optionsMenu;
        ssge::MenuHeader inputConfigMenu;
        ssge::MenuHeader volumeControlMenu;
        ssge::MenuHeader displaySettingsMenu;
        ssge::MenuHeader confirmRestart;
        ssge::MenuHeader confirmExitGame;
        ssge::MenuHeader confirmAbruptExit;
        ssge::MenuHeader levelSelect;
        ssge::MenuHeader friendlyReminderReset;
        ssge::MenuHeader friendlyReminderExit;

        int levelSelectorInt = 1;

    public:
        void init(SuperShiny::Config& config);
        static void refreshHighScoreMenu(int direction);
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

        bool load(ssge::IniFile& iniFile);
        bool save(ssge::IniFile& iniFile) const;
    };

    Config config;

};