#pragma once

#include <string>
#include <vector>
#include <map>
#include <any>
#include "emotion_engine.h"

namespace kelly {

enum class IntentPhase {
    Wound,
    Emotion,
    RuleBreak
};

struct Wound {
    std::string description;
    float intensity;
    std::string source;
};

struct RuleBreak {
    std::string ruleType;  // e.g., "harmony", "rhythm", "dynamics"
    float severity;        // 0.0 to 1.0
    std::string description;
    std::map<std::string, std::any> musicalImpact;
};

struct IntentResult {
    Wound wound;
    const EmotionNode* emotion;
    std::vector<RuleBreak> ruleBreaks;
    std::map<std::string, std::any> musicalParams;
};

class IntentProcessor {
public:
    IntentProcessor();
    ~IntentProcessor() = default;

    const EmotionNode* processWound(const Wound& wound);
    std::vector<RuleBreak> emotionToRuleBreaks(const EmotionNode& emotion);
    IntentResult processIntent(const Wound& wound);

private:
    std::map<std::string, std::any> compileMusicalParams(
        const EmotionNode& emotion,
        const std::vector<RuleBreak>& ruleBreaks
    );

    EmotionEngine engine_;
    std::vector<Wound> woundHistory_;
    std::vector<RuleBreak> ruleBreaks_;
};

} // namespace kelly
