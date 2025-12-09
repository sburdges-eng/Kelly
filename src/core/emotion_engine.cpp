#include "emotion_engine.h"
#include <cmath>
#include <algorithm>
#include <array>

namespace kelly {

EmotionEngine::EmotionEngine() {
    initializeEmotions();
}

void EmotionEngine::initializeEmotions() {
    // 8 categories × 9 variants × 3 intensity levels = 216 nodes
    // Variants: (name, valence, arousal)
    struct Variant { const char* name; float valence; float arousal; };

    std::array<std::pair<EmotionCategory, std::array<Variant, 9>>, 8> categoryVariants = {{
        {EmotionCategory::Joy, {{
            {"euphoria", 1.0f, 1.0f}, {"ecstasy", 0.95f, 0.95f}, {"elation", 0.85f, 0.9f},
            {"delight", 0.8f, 0.7f}, {"happiness", 0.7f, 0.6f}, {"contentment", 0.7f, 0.3f},
            {"serenity", 0.6f, 0.2f}, {"satisfaction", 0.5f, 0.4f}, {"cheerfulness", 0.6f, 0.5f}
        }}},
        {EmotionCategory::Sadness, {{
            {"grief", -0.9f, 0.7f}, {"despair", -0.95f, 0.6f}, {"sorrow", -0.8f, 0.5f},
            {"melancholy", -0.6f, 0.3f}, {"gloom", -0.5f, 0.4f}, {"disappointment", -0.4f, 0.3f},
            {"loneliness", -0.6f, 0.2f}, {"nostalgia", -0.3f, 0.2f}, {"wistfulness", -0.2f, 0.15f}
        }}},
        {EmotionCategory::Anger, {{
            {"rage", -0.8f, 1.0f}, {"fury", -0.85f, 0.95f}, {"wrath", -0.75f, 0.9f},
            {"hostility", -0.6f, 0.7f}, {"resentment", -0.5f, 0.5f}, {"annoyance", -0.4f, 0.5f},
            {"irritation", -0.35f, 0.45f}, {"frustration", -0.45f, 0.55f}, {"bitterness", -0.5f, 0.4f}
        }}},
        {EmotionCategory::Fear, {{
            {"terror", -0.9f, 1.0f}, {"panic", -0.85f, 0.95f}, {"horror", -0.8f, 0.9f},
            {"dread", -0.7f, 0.7f}, {"anxiety", -0.5f, 0.8f}, {"worry", -0.4f, 0.6f},
            {"unease", -0.3f, 0.5f}, {"apprehension", -0.35f, 0.55f}, {"nervousness", -0.3f, 0.65f}
        }}},
        {EmotionCategory::Surprise, {{
            {"amazement", 0.6f, 0.95f}, {"astonishment", 0.5f, 0.9f}, {"awe", 0.4f, 0.7f},
            {"wonder", 0.5f, 0.6f}, {"shock", -0.1f, 0.9f}, {"startle", 0.0f, 0.85f},
            {"bewilderment", -0.1f, 0.6f}, {"confusion", -0.2f, 0.5f}, {"curiosity", 0.3f, 0.5f}
        }}},
        {EmotionCategory::Disgust, {{
            {"revulsion", -0.9f, 0.8f}, {"loathing", -0.85f, 0.7f}, {"abhorrence", -0.8f, 0.75f},
            {"contempt", -0.6f, 0.5f}, {"aversion", -0.5f, 0.45f}, {"distaste", -0.4f, 0.35f},
            {"dislike", -0.3f, 0.3f}, {"disapproval", -0.35f, 0.4f}, {"repugnance", -0.7f, 0.6f}
        }}},
        {EmotionCategory::Trust, {{
            {"admiration", 0.8f, 0.5f}, {"adoration", 0.85f, 0.6f}, {"devotion", 0.75f, 0.55f},
            {"faith", 0.7f, 0.4f}, {"confidence", 0.6f, 0.5f}, {"reliance", 0.5f, 0.35f},
            {"acceptance", 0.4f, 0.3f}, {"respect", 0.55f, 0.4f}, {"appreciation", 0.5f, 0.45f}
        }}},
        {EmotionCategory::Anticipation, {{
            {"eagerness", 0.7f, 0.85f}, {"excitement", 0.75f, 0.9f}, {"hope", 0.6f, 0.6f},
            {"expectation", 0.4f, 0.55f}, {"vigilance", 0.1f, 0.7f}, {"interest", 0.35f, 0.5f},
            {"optimism", 0.65f, 0.55f}, {"yearning", 0.2f, 0.6f}, {"impatience", -0.1f, 0.7f}
        }}}
    }};

    constexpr float intensityLevels[3] = {1.0f, 0.6f, 0.3f};
    int nodeId = 0;

    for (const auto& [category, variants] : categoryVariants) {
        for (const auto& v : variants) {
            for (float intensity : intensityLevels) {
                std::string suffix = (intensity == 1.0f) ? "" : (intensity == 0.6f ? "_mid" : "_low");
                nodes_[nodeId] = EmotionNode{
                    nodeId,
                    std::string(v.name) + suffix,
                    category,
                    intensity,
                    v.valence * intensity,
                    v.arousal * intensity,
                    {},
                    {1.0f + (v.arousal * intensity - 0.5f) * 0.5f, v.valence > 0 ? "major" : "minor", intensity}
                };
                ++nodeId;
            }
        }
    }
}

const EmotionNode* EmotionEngine::getEmotion(int emotionId) const {
    auto it = nodes_.find(emotionId);
    return (it != nodes_.end()) ? &it->second : nullptr;
}

const EmotionNode* EmotionEngine::findEmotionByName(const std::string& name) const {
    for (const auto& [id, node] : nodes_) {
        if (node.name == name) {
            return &node;
        }
    }
    return nullptr;
}

float EmotionEngine::calculateDistance(const EmotionNode& a, const EmotionNode& b) const {
    float dv = a.valence - b.valence;
    float da = a.arousal - b.arousal;
    float di = a.intensity - b.intensity;
    return std::sqrt(dv * dv + da * da + di * di);
}

std::vector<const EmotionNode*> EmotionEngine::getNearbyEmotions(int emotionId, float threshold) const {
    const EmotionNode* source = getEmotion(emotionId);
    if (!source) {
        return {};
    }
    
    std::vector<const EmotionNode*> nearby;
    for (const auto& [id, node] : nodes_) {
        if (id == emotionId) continue;
        
        float distance = calculateDistance(*source, node);
        if (distance < threshold) {
            nearby.push_back(&node);
        }
    }
    
    return nearby;
}

} // namespace kelly
