//
// Created by User on 10/15/2025.
//

#ifndef LEARNVULKANRAII_TIMESTEP_H
#define LEARNVULKANRAII_TIMESTEP_H

namespace LearnVulkanRAII
{
    class Timestep
    {
    public:
        Timestep(float time = 0.0f)
            : m_time(time)
        {}

        operator float() const { return m_time; }

        float getSeconds() const { return m_time; }
        float getMilliseconds() const { return m_time * 1000.0f; }

    private:
        float m_time = 0.0f;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_TIMESTEP_H