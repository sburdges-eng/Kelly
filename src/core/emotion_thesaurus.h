#pragma once

#include <array>
#include <string>

namespace kelly {

struct EmotionThesaurusNode {
    int id;
    std::string name;
    // Additional fields would be here
};

class EmotionThesaurus {
public:
    EmotionThesaurus();
    ~EmotionThesaurus() = default;

    const EmotionThesaurusNode* getNode(int id) const;
    // Additional methods for the 216-node thesaurus

private:
    void initializeThesaurus();
    std::array<EmotionThesaurusNode, 216> nodes_;
};

} // namespace kelly
