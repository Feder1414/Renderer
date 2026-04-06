//
// Created by USUARIO on 12/23/2025.
//

#ifndef GRAFICOS_CAMERA_H
#define GRAFICOS_CAMERA_H


#include "../Classes/Transform.h"
#include "IComponent.h"

//Mouse handler includes glfw3 must be at the bottom
#include "Frustum.h"
#include "Metadata/ComponentPropertiesMetadata.h"
#include "../Classes/MouseHandler.h"


class Camera : public IComponent, public ComponentPropertiesMetadata<Camera>
{
private:
    glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
    float yaw = 0.0f;
    float pitch = 0.0f;
    float m_fov = 45.0f;;
    float m_fovLowerLimit = 1.0f;
    float m_fovUpperLimit = 45.0f;
    float m_nearPlane = 0.1;
    float m_farPlane = 100.0f;
    bool m_viewFrustum = false;

    float m_width, m_height = 100;

    Frustum m_frustum;

    MouseHandler* m_mouseHandler;


    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    float m_movementSPeed = 0.5f;

    void CalculateFrustrum();

public:
    size_t GetComponentId() const override
    {
        return GetComponentMetadataId();
    }

    glm::vec3 GetForwardCam() const;

    void SetMovementSpeed(float speed) { m_movementSPeed = speed; }
    float GetSpeed() const { return m_movementSPeed; }

    const std::unordered_map<std::string, ComponentProperty>& GetComponentMetadata() override
    {
        return GetPropertiesMetadata();
    } ;

    const std::string& GetComponentName() override
    {
        static std::string name = "Camera";
        return name;
    };
    Camera();
    glm::vec3 GetForward() { return m_forward; }
    void ProcessMovement(GLFWwindow* window, float deltaTime);

    void SetMouseHandler(MouseHandler* mouseHandler)
    {
        m_mouseHandler = mouseHandler;
    }

    void UpdateCamRotation(float offsetX, float offsetY);

    glm::mat4 GetViewMatrix() const;

    glm::mat4 GetProjMatrix(float aspectRatio) const;

    void CalculateFrustum(float aspectRatio)
    {
        auto viewMat = GetViewMatrix();
        auto projMat = GetProjMatrix(aspectRatio);
        m_frustum = Frustum(viewMat, projMat);
    };

    const Frustum& GetFrustum() const { return m_frustum; }


    float GetFov() const
    {
        return m_fov;
    }

    void Update()
    {
    };


    float GetNearPlane() const { return m_nearPlane; }

    float GetFarPlane() const { return m_farPlane; }

    void SetNearPlane(float nearPlane) { m_nearPlane = nearPlane; }
    void SetFarPlane(float farPlane) { m_farPlane = farPlane; }


    void SetWidthHeight(float width, float height)
    {
        m_width = width;
        m_height = height;
        CalculateFrustrum();
    }


    void UpdateFOV(double xoffset, double yoffset);
    void SetFov(double fov);
    void SetFov(float fov);

    bool IsInViewFrustum(const AABB* aabb) const;

    bool GetViewFrustum() const {return m_viewFrustum;}
    void SetViewFrustum(bool viewFrustum){m_viewFrustum = viewFrustum;}


    void SetComponentMetadata() override;
};


#endif //GRAFICOS_CAMERA_H
