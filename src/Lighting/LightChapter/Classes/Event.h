//
// Created by USUARIO on 2/6/2026.
//

#ifndef GRAFICOS_EVENT_H
#define GRAFICOS_EVENT_H

#include <queue>
#include <functional>

template <typename... Args>


class Event
{
    int ids = 0;
    using HandlerFunction = std::function<void(Args...)>;

    struct Handler
    {
        int id;
        HandlerFunction handler;
    };

    std::deque<Handler> handlers;

public:
    // Event& operator +=(HandlerFunction& t)
    // {
    //     Handler handler = {.id = ids, .handler = t};
    //
    //     handlers.push_front(handler);
    //     ids += 1;
    //     return this;
    // }

    int AddListener(HandlerFunction t)
    {
        Handler handler = {.id = ids, .handler = t};

        handlers.push_front(handler);
        ids += 1;
        return handler.id;
    }

    void RemoveListener(int id)
    {
        auto it = std::remove_if(handlers.begin(), handlers.end(), [id](const Handler& h) { return h.id == id; });
        handlers.erase(it, handlers.end());
    }


    void Notify(Args&... args)
    {
        for (auto& handler : handlers)
        {
            handler.handler(args...);
        }
    }
};


#endif //GRAFICOS_EVENT_H
