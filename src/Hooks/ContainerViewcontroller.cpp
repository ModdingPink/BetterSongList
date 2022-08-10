#include "hooking.hpp"
#include "logging.hpp"

#include "custom-types/shared/coroutine.hpp"
#include "HMUI/ContainerViewController.hpp"
#include "HMUI/ViewController.hpp"
#include "UnityEngine/EventSystems/EventSystem.hpp"

#include "UnityEngine/Time.hpp"
#include "System/Array.hpp"
#include "System/Action.hpp"
#include "System/Action_3.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"

// just this because it's so fukin long
template<typename T>
using HashSet = System::Collections::Generic::HashSet_1<T>;
using AnimationLayouterAction = System::Action_3<float, ::ArrayW<::HMUI::ViewController*>, HashSet<::HMUI::ViewController*>*>;
custom_types::Helpers::Coroutine RemoveViewControllersCoroutine(HMUI::ContainerViewController* self, ::ArrayW<::HMUI::ViewController*> viewControllersToRemove, ::System::Action* finishedCallback, AnimationLayouterAction* animationLayouter, bool immediately);

// on PC this is a transpiler on the coroutine enumerator, but we can't do that, so we just make our own coro with the transpiler applied
// from HookSelectedCollection
MAKE_AUTO_HOOK_ORIG_MATCH(ContainerViewController_RemoveViewControllers, &HMUI::ContainerViewController::RemoveViewControllers, void, 
    HMUI::ContainerViewController* self, 
    ArrayW<::HMUI::ViewController*> viewControllers, 
    System::Action* finishedCallback, 
    AnimationLayouterAction* animationLayouter, 
    bool immediately) {
    self->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(RemoveViewControllersCoroutine(self, viewControllers, finishedCallback, animationLayouter, immediately)));
    // not calling ORIG on purpose!
}

custom_types::Helpers::Coroutine RemoveViewControllersCoroutine(HMUI::ContainerViewController* self, ::ArrayW<::HMUI::ViewController*> viewControllersToRemove, ::System::Action* finishedCallback, ::System::Action_3<float, ::ArrayW<::HMUI::ViewController*>, ::System::Collections::Generic::HashSet_1<::HMUI::ViewController*>*>* animationLayouter, bool immediately) {
    if (self->get_isInTransition()) co_return;

    self->set_isInTransition(true);
    for (auto vc : viewControllersToRemove) vc->set_isInTransition(true);

    auto eventSystem = UnityEngine::EventSystems::EventSystem::get_current();
    if (eventSystem) eventSystem->SetSelectedGameObject(nullptr);

    auto viewControllers = self->viewControllers->ToArray();
    // idk if casting an array like that is allowed but it should work like on PC?
    auto viewControllersToRemoveSet = HashSet<::HMUI::ViewController*>::New_ctor(reinterpret_cast<System::Collections::Generic::IEnumerable_1<HMUI::ViewController*>*>(viewControllersToRemove.convert()));
    if (!immediately) {
        const float transitionDuration = 0.4f;
        float elapsedTime = 0.0f;
        while(elapsedTime  < transitionDuration) {
            float arg = elapsedTime / transitionDuration;
            animationLayouter->Invoke(arg, viewControllers, viewControllersToRemoveSet);
            elapsedTime += UnityEngine::Time::get_deltaTime();
            co_yield nullptr;
        }
    }
    animationLayouter->Invoke(1.0f, viewControllers, viewControllersToRemoveSet);

    for (auto vc : viewControllersToRemove) {
        vc->__Deactivate(true, true, false);
        vc->__ResetViewController();
    }
    
    // this._viewControllers = this._viewControllers.Except(viewControllersToRemove).ToList<ViewController>();
    // this VV does the same thing except no need to make a new list
    for (auto vc : viewControllersToRemove) self->viewControllers->Remove(vc);

    self->set_isInTransition(false);
    for (auto vc : viewControllersToRemove) vc->set_isInTransition(false);

    if (finishedCallback) finishedCallback->Invoke();

    co_return;
}
