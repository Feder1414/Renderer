//
// Created by USUARIO on 4/2/2026.
//

#ifndef GRAFICOS_VIEWPORTWIDGET_H
#define GRAFICOS_VIEWPORTWIDGET_H
#include "Event.h"
#include "IWidget.h"


class ViewPort;


class ViewportWidget : public IWidget
{
public:
    void Render() override;
    bool MouseInViewport() const;

private:
    Event<> m_onViewportSizeChanged;
    unsigned int m_width;
    unsigned int m_height;

    ViewPort* m_viewPort;
};


#endif //GRAFICOS_VIEWPORTWIDGET_H
