#include "StepContext.h"

// We include engine headers here — only the .cpp sees the raw types.
// Headers remain free of raw public Program* signatures.
#include "Program.h"
#include "SceneManager.h"
#include "Scene.h"

namespace ssge {

    // -----------------------------
    // SceneManagerStepContext impl
    // -----------------------------
    SceneManagerStepContext::SceneManagerStepContext(
        ssge::Program* program,
        ssge::Engine* engine
    )
        : program(program)
        , scenes(engine->getSceneManager())
    {
    }

    // Program wrapper
    SceneManagerStepContext::Program::Program(ssge::Program* p)
        : actual(p)
    {
    }

    void SceneManagerStepContext::Program::exit() {
        if (actual) actual->requestExit();
    }

    // Scenes wrapper
    SceneManagerStepContext::Scenes::Scenes(ssge::SceneManager* mgr)
        : actual(mgr)
    {
    }

    void SceneManagerStepContext::Scenes::changeScene(std::unique_ptr<ssge::Scene> newScene) {
        if (actual) actual->changeScene(std::move(newScene));
    }

    // Private helper that SceneManager (friend) calls to get a SceneStepContext.
    // Implemented via the ContextFactory.
    SceneStepContext SceneManagerStepContext::sceneStepContext() const {
        return ContextFactory::createSceneStepContext(*this);
    }

    // -----------------------------
    // SceneStepContext impl
    // -----------------------------
    SceneStepContext::SceneStepContext(ssge::Program* prog, ssge::SceneManager* mgr)
        : program(prog)
        , scenes(mgr)
    {
    }

    SceneStepContext::Program::Program(ssge::Program* p)
        : actual(p)
    {
    }

    void SceneStepContext::Program::exit() {
        if (actual) actual->requestExit();
    }

    SceneStepContext::Scenes::Scenes(ssge::SceneManager* mgr)
        : actual(mgr)
    {
    }

    void SceneStepContext::Scenes::changeScene(std::unique_ptr<ssge::Scene> newScene) {
        if (actual) actual->changeScene(std::move(newScene));
    }

    // -----------------------------
    // ContextFactory impl
    // -----------------------------
    SceneStepContext ContextFactory::createSceneStepContext(const SceneManagerStepContext& smctx) {
        // Access the private internals of the wrappers (friend)
        ssge::Program* progPtr = smctx.program.actual;
        ssge::SceneManager* mgrPtr = smctx.scenes.actual;

        // Construct SceneStepContext (private ctor, but this factory is a friend)
        return SceneStepContext(progPtr, mgrPtr);
    }

} // namespace ssge
