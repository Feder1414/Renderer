//
// Created by USUARIO on 2/8/2026.
//

#ifndef GRAFICOS_IWIDGET_H
#define GRAFICOS_IWIDGET_H
#include <string>


class IWidget
{
protected:
    std::string m_windowName;

public:
    [[nodiscard]] const std::string& GetWindowName() const { return m_windowName; }

    explicit IWidget (const std::string& windowsName) : m_windowName(windowsName) {}
    virtual ~IWidget() = default;
    virtual void Render() = 0;


};


#endif //GRAFICOS_IWIDGET_H
