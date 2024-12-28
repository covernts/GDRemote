#pragma once

namespace gdws::Modifiers {
    enum class ModifierType {
        Checkbox,
        Button
    };
    
    class Modifier {
    private:
        std::string m_name;
        bool m_enabled;
        ModifierType m_type;
    
    public:
        Modifier(std::string name, bool enabled, ModifierType type) : m_name(name), m_enabled(enabled), m_type(type) {}
        
        std::string getName() const { return m_name; }
        bool getEnabled() const { return m_enabled; }
        void setEnabled(bool enabled) { m_enabled = enabled; }
        ModifierType getType() const { return m_type; }

        virtual void buttonCallback() {}
    };

    class ButtonModifier : public Modifier {
    private:
        std::function<void()> m_callback;
    
    public:
        ButtonModifier(std::string name, std::function<void()> callback) : Modifier(name, false, ModifierType::Button), m_callback(callback) {}

        void buttonCallback() override {
            m_callback();
        }
    };

    extern std::vector<Modifier*> m_mods;

    bool isEnabled(const std::string& name);
}