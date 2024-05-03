#ifndef DEFERRED_RENDER_H
#define DEFERRED_RENDER_H

#include <vector>
#include <functional>

template <class... T>
struct DeferredItem {
    std::tuple<T...> args;
    std::function<int(T...)> callable;
};

template <class... T>
std::vector<DeferredItem<T...>> DeferredRegister;

template<typename Func, typename... Args>
void RegisterForDeferredRendering(Func&& callable, Args&&... args);

#endif
