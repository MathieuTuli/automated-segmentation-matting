#include "deferred_render.hpp"

template<typename Func, typename... Args>
void RegisterForDeferredRendering(Func&& callable, Args&&... args) {
    using Signature = std::function<int(std::decay_t<Args>...)>;
    DeferredRegister<std::decay_t<Args>...>.push_back(DeferredItem<std::decay_t<Args>...>{
            std::make_tuple(std::forward<Args>(args)...), Signature(std::forward<Func>(callable))});
}
