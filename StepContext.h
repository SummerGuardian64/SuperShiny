#pragma once
#include "SuperShiny.h"
#include <memory>

// StepContext.h
// Capability-style Step Contexts for ssge
// - No raw ssge::Program* leaked to general code
// - Small, focused ContextFactory is the only intermediary that can
//   access the private internals and mint downstream contexts.

namespace ssge {
    // Forward engine types (do NOT expose raw Program* publicly)
    class Program;          // real engine Program (defined elsewhere)
    class SceneManager;     // the manager that owns scenes
    class Scene;            // abstract scene base

    // Forward context classes
    class SceneManagerStepContext;
    class SceneStepContext;

    // Small internal factory: allowed to construct downstream contexts.
    // Only ContextFactory is allowed to peer into wrapper internals.
    class ContextFactory {
    public:
        // Create a SceneStepContext from a SceneManagerStepContext (friend-only access)
        static SceneStepContext createSceneStepContext(const SceneManagerStepContext& smctx);
    };

    // -----------------------
    // SceneManagerStepContext
    // -----------------------
    // Created *only* by ssge::Program (friend). Exposes safe wrappers as members so
    // user code can use `ctx.program.exit()` and `ctx.scenes.changeScene(...)`.
    class SceneManagerStepContext {
    public:
        // -------- Program wrapper (exposes allowed Program functions) --------
        class Program {
        public:
            void exit();

        private:
            explicit Program(ssge::Program* p);
            ssge::Program* actual = nullptr;   // private: only wrappers/factory can see

            friend class SceneManagerStepContext;
            friend class ::ssge::ContextFactory; // factory may read .actual
        };

        // -------- Scenes wrapper (exposes allowed SceneManager functions) --------
        class Scenes {
        public:
            void changeScene(std::unique_ptr<ssge::Scene> newScene);

        private:
            explicit Scenes(ssge::SceneManager* mgr);
            ssge::SceneManager* actual = nullptr;

            friend class SceneManagerStepContext;
            friend class ::ssge::ContextFactory; // factory may read .actual
        };

        // public sugar
        Program program;
        Scenes scenes;

    private:
        // Only ssge::Program constructs this (declared as friend)
        SceneManagerStepContext(ssge::Program* prog);

        // Provide SceneManager a way to obtain a SceneStepContext, but keep the method private:
        // SceneManager is friend so it can call this.
        SceneStepContext sceneStepContext() const; // implemented via ContextFactory

        // Strict lifetime rules: no copying allowed.
        SceneManagerStepContext(const SceneManagerStepContext&) = delete;
        SceneManagerStepContext& operator=(const SceneManagerStepContext&) = delete;

        // Move exists but is private so only friends can move it into locals if needed (return-from-factory).
        SceneManagerStepContext(SceneManagerStepContext&&) = default;
        SceneManagerStepContext& operator=(SceneManagerStepContext&&) = delete;

        // Trusted classes:
        friend class ::ssge::Program;      // engine may create this
        friend class ::ssge::SceneManager; // allowed to call sceneStepContext()
        friend class ::ssge::ContextFactory;
    };

    // -----------------------
    // SceneStepContext
    // -----------------------
    // Created only by ContextFactory (and requested by SceneManager). Exposes
    // the same small wrappers so Scene code can do `context.program.exit()`, etc.
    class SceneStepContext {
    public:
        class Program {
        public:
            void exit();

        private:
            explicit Program(ssge::Program* p);
            ssge::Program* actual = nullptr;

            friend class SceneStepContext;
            friend class ::ssge::ContextFactory;
        };

        class Scenes {
        public:
            void changeScene(std::unique_ptr<ssge::Scene> newScene);

        private:
            explicit Scenes(ssge::SceneManager* mgr);
            ssge::SceneManager* actual = nullptr;

            friend class SceneStepContext;
            friend class ::ssge::ContextFactory;
        };

        // public sugar for scene authors
        Program program;
        Scenes scenes;

    private:
        // Only ContextFactory may construct it (private ctor).
        SceneStepContext(ssge::Program* prog, ssge::SceneManager* mgr);

        // No copying, move only for the few trusted friends (private)
        SceneStepContext(const SceneStepContext&) = delete;
        SceneStepContext& operator=(const SceneStepContext&) = delete;

        SceneStepContext(SceneStepContext&&) = default; // private
        SceneStepContext& operator=(SceneStepContext&&) = delete;

        // Only SceneManager and ContextFactory are allowed to move/create these
        friend class ::ssge::SceneManager;
        friend class ::ssge::ContextFactory;
    };

} // namespace ssge
