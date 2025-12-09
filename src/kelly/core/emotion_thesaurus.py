"""Core emotion processing and thesaurus."""
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass
from enum import Enum


class EmotionCategory(Enum):
    """Primary emotion categories."""
    JOY = "joy"
    SADNESS = "sadness"
    ANGER = "anger"
    FEAR = "fear"
    SURPRISE = "surprise"
    DISGUST = "disgust"
    TRUST = "trust"
    ANTICIPATION = "anticipation"


@dataclass
class EmotionNode:
    """Represents a node in the 216-node emotion thesaurus."""
    id: int
    name: str
    category: EmotionCategory
    intensity: float  # 0.0 to 1.0
    valence: float  # -1.0 (negative) to 1.0 (positive)
    arousal: float  # 0.0 (calm) to 1.0 (excited)
    related_emotions: List[int]
    musical_attributes: Dict[str, any]


class EmotionThesaurus:
    """
    216-node emotion thesaurus for mapping emotions to musical properties.
    
    The thesaurus organizes emotions in a hierarchical structure with
    dimensions of valence, arousal, and intensity.
    """
    
    def __init__(self) -> None:
        """Initialize the emotion thesaurus."""
        self.nodes: Dict[int, EmotionNode] = {}
        self._initialize_thesaurus()
    
    def _initialize_thesaurus(self) -> None:
        """Initialize the 216-node emotion network.

        Structure: 8 categories × 3 intensity levels × 9 variants = 216 nodes.
        """
        # Emotion variants per category (9 variants each)
        emotion_variants: Dict[EmotionCategory, List[Tuple[str, float, float]]] = {
            EmotionCategory.JOY: [
                ("euphoria", 1.0, 1.0),
                ("ecstasy", 0.95, 0.95),
                ("elation", 0.85, 0.9),
                ("delight", 0.8, 0.7),
                ("happiness", 0.7, 0.6),
                ("contentment", 0.7, 0.3),
                ("serenity", 0.6, 0.2),
                ("satisfaction", 0.5, 0.4),
                ("cheerfulness", 0.6, 0.5),
            ],
            EmotionCategory.SADNESS: [
                ("grief", -0.9, 0.7),
                ("despair", -0.95, 0.6),
                ("sorrow", -0.8, 0.5),
                ("melancholy", -0.6, 0.3),
                ("gloom", -0.5, 0.4),
                ("disappointment", -0.4, 0.3),
                ("loneliness", -0.6, 0.2),
                ("nostalgia", -0.3, 0.2),
                ("wistfulness", -0.2, 0.15),
            ],
            EmotionCategory.ANGER: [
                ("rage", -0.8, 1.0),
                ("fury", -0.85, 0.95),
                ("wrath", -0.75, 0.9),
                ("hostility", -0.6, 0.7),
                ("resentment", -0.5, 0.5),
                ("annoyance", -0.4, 0.5),
                ("irritation", -0.35, 0.45),
                ("frustration", -0.45, 0.55),
                ("bitterness", -0.5, 0.4),
            ],
            EmotionCategory.FEAR: [
                ("terror", -0.9, 1.0),
                ("panic", -0.85, 0.95),
                ("horror", -0.8, 0.9),
                ("dread", -0.7, 0.7),
                ("anxiety", -0.5, 0.8),
                ("worry", -0.4, 0.6),
                ("unease", -0.3, 0.5),
                ("apprehension", -0.35, 0.55),
                ("nervousness", -0.3, 0.65),
            ],
            EmotionCategory.SURPRISE: [
                ("amazement", 0.6, 0.95),
                ("astonishment", 0.5, 0.9),
                ("awe", 0.4, 0.7),
                ("wonder", 0.5, 0.6),
                ("shock", -0.1, 0.9),
                ("startle", 0.0, 0.85),
                ("bewilderment", -0.1, 0.6),
                ("confusion", -0.2, 0.5),
                ("curiosity", 0.3, 0.5),
            ],
            EmotionCategory.DISGUST: [
                ("revulsion", -0.9, 0.8),
                ("loathing", -0.85, 0.7),
                ("abhorrence", -0.8, 0.75),
                ("contempt", -0.6, 0.5),
                ("aversion", -0.5, 0.45),
                ("distaste", -0.4, 0.35),
                ("dislike", -0.3, 0.3),
                ("disapproval", -0.35, 0.4),
                ("repugnance", -0.7, 0.6),
            ],
            EmotionCategory.TRUST: [
                ("admiration", 0.8, 0.5),
                ("adoration", 0.85, 0.6),
                ("devotion", 0.75, 0.55),
                ("faith", 0.7, 0.4),
                ("confidence", 0.6, 0.5),
                ("reliance", 0.5, 0.35),
                ("acceptance", 0.4, 0.3),
                ("respect", 0.55, 0.4),
                ("appreciation", 0.5, 0.45),
            ],
            EmotionCategory.ANTICIPATION: [
                ("eagerness", 0.7, 0.85),
                ("excitement", 0.75, 0.9),
                ("hope", 0.6, 0.6),
                ("expectation", 0.4, 0.55),
                ("vigilance", 0.1, 0.7),
                ("interest", 0.35, 0.5),
                ("optimism", 0.65, 0.55),
                ("yearning", 0.2, 0.6),
                ("impatience", -0.1, 0.7),
            ],
        }

        intensity_levels = [1.0, 0.6, 0.3]  # high, medium, low

        node_id = 0
        for category, variants in emotion_variants.items():
            for name, valence, arousal in variants:
                for intensity in intensity_levels:
                    self.nodes[node_id] = EmotionNode(
                        id=node_id,
                        name=name if intensity == 1.0 else f"{name}_{'mid' if intensity == 0.6 else 'low'}",
                        category=category,
                        intensity=intensity,
                        valence=valence * intensity,
                        arousal=arousal * intensity,
                        related_emotions=[],
                        musical_attributes={
                            "tempo_modifier": 1.0 + (arousal * intensity - 0.5) * 0.5,
                            "mode": "major" if valence > 0 else "minor",
                            "dynamics": intensity,
                        },
                    )
                    node_id += 1

        # Build related_emotions links for nearby emotions
        for nid, node in self.nodes.items():
            node.related_emotions = [
                other.id for other in self.get_nearby_emotions(nid, threshold=0.35)
            ]
    
    def get_emotion(self, emotion_id: int) -> Optional[EmotionNode]:
        """Get emotion node by ID."""
        return self.nodes.get(emotion_id)
    
    def find_emotion_by_name(self, name: str) -> Optional[EmotionNode]:
        """Find emotion by name."""
        for node in self.nodes.values():
            if node.name.lower() == name.lower():
                return node
        return None
    
    def get_nearby_emotions(
        self, emotion_id: int, threshold: float = 0.3
    ) -> List[EmotionNode]:
        """
        Find emotions near the given emotion in emotional space.
        
        Args:
            emotion_id: ID of the source emotion
            threshold: Maximum distance in emotional space
            
        Returns:
            List of nearby emotion nodes
        """
        source = self.get_emotion(emotion_id)
        if not source:
            return []
        
        nearby = []
        for node in self.nodes.values():
            if node.id == emotion_id:
                continue
            
            # Calculate emotional distance
            distance = (
                (source.valence - node.valence) ** 2 +
                (source.arousal - node.arousal) ** 2 +
                (source.intensity - node.intensity) ** 2
            ) ** 0.5
            
            if distance < threshold:
                nearby.append(node)
        
        return nearby
