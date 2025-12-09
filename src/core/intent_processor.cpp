#include "intent_processor.h"
#include <algorithm>
#include <cctype>

namespace kelly {

IntentProcessor::IntentProcessor() = default;

const EmotionNode* IntentProcessor::processWound(const Wound& wound) {
    woundHistory_.push_back(wound);

    std::string desc = wound.description;
    std::transform(desc.begin(), desc.end(), desc.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (desc.find("loss") != std::string::npos ||
        desc.find("grief") != std::string::npos) {
        return engine_.findEmotionByName("grief");
    } else if (desc.find("anger") != std::string::npos ||
               desc.find("rage") != std::string::npos) {
        return engine_.findEmotionByName("rage");
    } else if (desc.find("fear") != std::string::npos ||
               desc.find("anxiety") != std::string::npos) {
        return engine_.findEmotionByName("anxiety");
    }

    return engine_.findEmotionByName("melancholy");
}

std::vector<RuleBreak> IntentProcessor::emotionToRuleBreaks(const EmotionNode& emotion) {
    std::vector<RuleBreak> breaks;

    // High intensity emotions break more rules
    if (emotion.intensity > 0.8f) {
        RuleBreak rb;
        rb.ruleType = "dynamics";
        rb.severity = emotion.intensity;
        rb.description = "Extreme dynamic contrasts";
        rb.musicalImpact["velocity_min"] = 10;
        rb.musicalImpact["velocity_max"] = 127;
        rb.musicalImpact["sudden_changes"] = true;
        breaks.push_back(rb);
    }

    // Negative valence introduces dissonance
    if (emotion.valence < -0.5f) {
        RuleBreak rb;
        rb.ruleType = "harmony";
        rb.severity = std::abs(emotion.valence);
        rb.description = "Dissonant intervals and clusters";
        rb.musicalImpact["allow_dissonance"] = true;
        rb.musicalImpact["cluster_probability"] = std::abs(emotion.valence);
        breaks.push_back(rb);
    }

    // High arousal breaks rhythmic conventions
    if (emotion.arousal > 0.7f) {
        RuleBreak rb;
        rb.ruleType = "rhythm";
        rb.severity = emotion.arousal;
        rb.description = "Irregular rhythms and syncopation";
        rb.musicalImpact["syncopation_level"] = emotion.arousal;
        rb.musicalImpact["irregular_meters"] = true;
        breaks.push_back(rb);
    }

    ruleBreaks_.insert(ruleBreaks_.end(), breaks.begin(), breaks.end());
    return breaks;
}

std::map<std::string, std::any> IntentProcessor::compileMusicalParams(
    const EmotionNode& emotion,
    const std::vector<RuleBreak>& ruleBreaks
) {
    std::map<std::string, std::any> params;
    params["tempo_modifier"] = emotion.musicalAttributes.tempoModifier;
    params["mode"] = emotion.musicalAttributes.mode;
    params["dynamics"] = emotion.musicalAttributes.dynamics;

    for (const auto& rb : ruleBreaks) {
        for (const auto& [key, value] : rb.musicalImpact) {
            params[key] = value;
        }
    }

    return params;
}

IntentResult IntentProcessor::processIntent(const Wound& wound) {
    const EmotionNode* emotion = processWound(wound);
    std::vector<RuleBreak> breaks = emotion ? emotionToRuleBreaks(*emotion) : std::vector<RuleBreak>{};

    IntentResult result;
    result.wound = wound;
    result.emotion = emotion;
    result.ruleBreaks = breaks;
    result.musicalParams = emotion ? compileMusicalParams(*emotion, breaks) : std::map<std::string, std::any>{};

    return result;
}

} // namespace kelly
