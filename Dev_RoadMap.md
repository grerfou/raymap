# RayMap - Plan de Développement

Plan détaillé du développement de RayMap avec objectifs, commits et tests pour chaque étape.

---

## Phase 1 : Foundation (MVP)

### Étape 1.1 : Architecture de base  ✅
**But** : Créer la structure header-only avec API publique/privée

**Commit** : `feat: initial library structure with opaque types`
```
- Types opaques (RM_Surface, RM_Calibration)
- Structures publiques (RM_Quad, enums)
- Déclarations de fonctions
- Licence et documentation header
```

**Tests** :
- [ ] Compilation sans erreurs
- [ ] Inclusion dans un projet externe
- [ ] Header guards fonctionnels

---

### Étape 1.2 : Création et destruction de surfaces
**But** : Gestion mémoire et lifecycle des surfaces

**Commit** : `feat: surface creation and destruction with RenderTexture`
```
- RM_CreateSurface()
- RM_DestroySurface()
- Allocation RenderTexture
- Quad par défaut
```

**Tests** :
- [ ] Test: Créer et détruire 1 surface → pas de leak
- [ ] Test: Créer et détruire 100 surfaces → pas de leak
- [ ] Test: RenderTexture valide après création

**Fichier test** : `test_01_surface_lifecycle.c`

---

### Étape 1.3 : Rendu simple (quad basique)
**But** : Afficher du contenu sur une surface non-déformée

**Commit** : `feat: basic quad rendering with texture mapping`
```
- RM_BeginSurface() / RM_EndSurface()
- RM_DrawSurface() avec simple quad
- Mapping UV correct (fix flip RenderTexture)
```

**Tests** :
- [ ] Test: Contenu visible sans déformation
- [ ] Test: Pas d'effet miroir
- [ ] Test: Coordonnées de texture correctes

**Fichier test** : `test_02_simple_render.c`

---

### Étape 1.4 : Gestion des quads
**But** : Permettre la déformation du quad

**Commit** : `feat: quad manipulation (set/get corners)`
```
- RM_SetQuad()
- RM_GetQuad()
- Validation des quads
```

**Tests** :
- [ ] Test: Set quad déformé → affichage correct
- [ ] Test: Get quad retourne valeurs correctes
- [ ] Test: Quad dégénéré géré gracieusement

**Fichier test** : `test_03_quad_manipulation.c`

---

## Phase 2 : Calibration Interactive 

### Étape 2.1 : Calibration basique
**But** : Permettre le déplacement des coins à la souris

**Commit** : `feat: interactive calibration with mouse drag`
```
- RM_BeginCalibration() / RM_EndCalibration()
- RM_UpdateCalibration() (détection clic + drag)
- Structure RM_CalibrationConfig
```

**Tests** :
- [ ] Test: Cliquer sur coin → sélection
- [ ] Test: Glisser coin → déplacement
- [ ] Test: Relâcher souris → fin sélection

**Fichier test** : `test_04_calibration_input.c`

---

### Étape 2.2 : UI de calibration
**But** : Feedback visuel pendant la calibration

**Commit** : `feat: calibration UI with corners, borders, and grid`
```
- RM_DrawCalibration()
- RM_DrawCalibrationCorners()
- RM_DrawCalibrationBorder()
- RM_DrawCalibrationGrid()
```

**Tests** :
- [ ] Test: Coins visibles et cliquables
- [ ] Test: Grille s'affiche correctement
- [ ] Test: Bordure suit le quad
- [ ] Test: Corner actif change de couleur

**Fichier test** : `test_05_calibration_ui.c`

---

### Étape 2.3 : Reset et utilitaires
**But** : Outils de calibration pratiques

**Commit** : `feat: calibration utilities (reset, active corner query)`
```
- RM_ResetQuad()
- RM_GetActiveCorner()
- RM_IsCalibrating()
```

**Tests** :
- [ ] Test: Reset → quad revient à rectangle
- [ ] Test: GetActiveCorner retourne bon index
- [ ] Test: IsCalibrating correct pendant drag

**Fichier test** : `test_06_calibration_utils.c`

---

## Phase 3 : Mesh Warping

### Étape 3.1 : Subdivision bilinéaire
**But** : Remplacer le quad simple par un mesh subdivisé

**Commit** : `feat: bilinear mesh subdivision for smooth warping`
```
- rm_GenerateBilinearMesh()
- Interpolation bilinéaire des vertices
- Résolution configurable
```

**Tests** :
- [ ] Test: Mesh 10x10 → pas de ligne visible au milieu
- [ ] Test: Déformation forte → warp lisse
- [ ] Test: Comparaison avant/après subdivision

**Fichier test** : `test_07_mesh_subdivision.c`

---

### Étape 3.2 : Résolution dynamique
**But** : Permettre l'ajustement de la qualité du mesh

**Commit** : `feat: dynamic mesh resolution adjustment`
```
- RM_SetMeshResolution()
- RM_GetMeshResolution()
- Régénération automatique du mesh
```

**Tests** :
- [ ] Test: Augmenter résolution → meilleure qualité
- [ ] Test: Diminuer résolution → meilleure performance
- [ ] Test: Résolution min/max respectées

**Fichier test** : `test_08_mesh_resolution.c`

---

### Étape 3.3 : Mode MESH
**But** : Première implémentation de mode de warp

**Commit** : `feat: MESH warp mode with configurable quality`
```
- Enum RM_WarpMode avec RM_WARP_MESH
- RM_SetWarpMode() / RM_GetWarpMode()
- Résolution par défaut (16x16)
```

**Tests** :
- [ ] Test: Mode MESH fonctionne
- [ ] Test: Switch mode régénère mesh
- [ ] Test: Performance acceptable (>60 FPS)

**Fichier test** : `test_09_mesh_mode.c`

---

## Phase 4 : Homographie Perspective

### Étape 4.1 : Mathématiques homographie
**But** : Calculer la matrice 3x3 de transformation projective

**Commit** : `feat: homography matrix computation (DLT algorithm)`
```
- Structure Matrix3x3
- rm_ComputeHomography() (algorithme DLT)
- rm_ApplyHomography()
- Opérations matrices (inverse, déterminant)
```

**Tests** :
- [ ] Test: Quad rectangle → matrice identité
- [ ] Test: Quad déformé → matrice correcte
- [ ] Test: ApplyHomography sur points connus
- [ ] Test: Inverse de matrice correcte

**Fichier test** : `test_10_homography_math.c`

---

### Étape 4.2 : Mode PERSPECTIVE
**But** : Utiliser l'homographie pour le warping

**Commit** : `feat: PERSPECTIVE mode using homography transformation`
```
- RM_WARP_PERSPECTIVE dans enum
- Mesh généré avec homographie
- Résolution haute (32x32) par défaut
```

**Tests** :
- [ ] Test: Mode PERSPECTIVE fonctionne
- [ ] Test: Lignes droites restent droites
- [ ] Test: Comparaison MESH vs PERSPECTIVE
- [ ] Test: Points mappés correctement

**Fichier test** : `test_11_perspective_mode.c`

---

### Étape 4.3 : Comparaison modes
**But** : Valider la différence entre bilinéaire et perspective

**Commit** : `test: visual comparison between MESH and PERSPECTIVE modes`
```
- Test côte-à-côte
- Mesure de distorsion
- Benchmark performance
```

**Tests** :
- [ ] Test: Mire de test (lignes droites)
- [ ] Test: Grille déformée
- [ ] Test: FPS pour chaque mode
- [ ] Test: Qualité vs résolution

**Fichier test** : `test_12_mode_comparison.c`

---

## Phase 5 : Configuration I/O 

### Étape 5.1 : Save/Load texte
**But** : Sauvegarder les configurations de quad

**Commit** : `feat: save and load quad configuration (text format)`
```
- RM_SaveConfig() (format texte)
- RM_LoadConfig()
- Gestion erreurs fichier
```

**Tests** :
- [ ] Test: Sauvegarder quad → fichier créé
- [ ] Test: Charger quad → positions correctes
- [ ] Test: Fichier invalide géré gracieusement
- [ ] Test: Round-trip save→load identique

**Fichier test** : `test_13_config_io.c`

---

### Étape 5.2 : Export JSON (TODO)
**But** : Format standard pour partage et outils externes

**Commit** : `feat: JSON export/import for configuration`
```
- RM_ExportConfigJSON()
- RM_ImportConfigJSON()
- Format JSON structuré
```

**Tests** :
- [ ] Test: Export → JSON valide
- [ ] Test: Import JSON → quad correct
- [ ] Test: Compatibilité avec outils JSON

**Fichier test** : `test_14_json_config.c`

---

## Phase 6 : Utilitaires 

### Étape 6.1 : Géométrie de base
**But** : Fonctions utilitaires pour quads

**Commit** : `feat: utility functions for quad geometry`
```
- RM_PointInQuad()
- RM_GetQuadBounds()
- Tests de collision
```

**Tests** :
- [ ] Test: Point dans quad → true
- [ ] Test: Point hors quad → false
- [ ] Test: Bounds correctes

**Fichier test** : `test_15_geometry_utils.c`

---

### Étape 6.2 : Mapping de points
**But** : Transformer coordonnées entre espaces

**Commit** : `feat: point mapping between texture and screen space`
```
- RM_MapPoint() (texture → screen)
- RM_UnmapPoint() (screen → texture)
```

**Tests** :
- [ ] Test: Round-trip map→unmap
- [ ] Test: Coins du quad correctement mappés
- [ ] Test: Centre du quad

**Fichier test** : `test_16_point_mapping.c`

---

## Phase 7 : Multi-Surface 

### Étape 7.1 : Support multi-surfaces
**But** : Gérer plusieurs surfaces indépendantes

**Commit** : `feat: multi-surface support for multi-projector setups`
```
- Exemple avec 4 surfaces
- Sélection de surface active
- Sauvegarde/chargement multiple
```

**Tests** :
- [ ] Test: 4 surfaces simultanées
- [ ] Test: Calibration indépendante
- [ ] Test: Performance multi-surface
- [ ] Test: Save/Load toutes surfaces

**Fichier test** : `test_17_multi_surface.c` (déjà créé : `multi_surface.c`)

---

## Phase 8 : Optimisations (TODO)

### Étape 8.1 : Cache et mise à jour conditionnelle
**But** : Ne régénérer le mesh que si nécessaire

**Commit** : `perf: conditional mesh regeneration with dirty flags`
```
- Flags meshNeedsUpdate / homographyNeedsUpdate
- Cache de matrice homographique
- Optimisation DrawSurface
```

**Tests** :
- [ ] Test: Pas de régénération inutile
- [ ] Test: FPS améliorés
- [ ] Benchmark avant/après

**Fichier test** : `test_18_performance_cache.c`

---

### Étape 8.2 : LOD (Level of Detail)
**But** : Adapter automatiquement la résolution

**Commit** : `feat: automatic LOD based on quad size`
```
- Calcul taille quad à l'écran
- Résolution adaptative
- Seuils configurables
```

**Tests** :
- [ ] Test: Grand quad → haute résolution
- [ ] Test: Petit quad → basse résolution
- [ ] Test: FPS stables

**Fichier test** : `test_19_lod_system.c`

---

## Phase 9 : Features Avancées (TODO)

### Étape 9.1 : Edge blending
**But** : Fondu entre surfaces qui se chevauchent

**Commit** : `feat: edge blending for overlapping projections`
```
- Zones de blend configurables
- Alpha gradient automatique
- Calibration overlap
```

**Tests** :
- [ ] Test: 2 surfaces qui se chevauchent
- [ ] Test: Blend smooth sans bande visible
- [ ] Test: Configuration zones de blend

**Fichier test** : `test_20_edge_blending.c`

---

### Étape 9.2 : Masques alpha
**But** : Formes non-rectangulaires

**Commit** : `feat: alpha masks for non-rectangular shapes`
```
- Support masque texture
- Masque par vertex
- Masque procédural
```

**Tests** :
- [ ] Test: Masque circulaire
- [ ] Test: Masque custom texture
- [ ] Test: Performance avec masque

**Fichier test** : `test_21_alpha_masks.c`

---

### Étape 9.3 : Keystone correction automatique
**But** : Détection et correction automatique de perspective

**Commit** : `feat: automatic keystone correction from markers`
```
- Détection de markers
- Calcul auto de homographie
- Assistant de calibration
```

**Tests** :
- [ ] Test: 4 markers → quad correct
- [ ] Test: Markers partiels → estimation
- [ ] Test: Précision correction

**Fichier test** : `test_22_auto_keystone.c`

---

## Phase 10 : Shader Custom (TODO)

### Étape 10.1 : Fragment shader perspective
**But** : Correction perspective pixel-perfect

**Commit** : `feat: perspective correction fragment shader`
```
- Shader GLSL custom
- Calcul homographie dans shader
- Fallback si shader fail
```

**Tests** :
- [ ] Test: Shader compile et fonctionne
- [ ] Test: Qualité supérieure au mesh
- [ ] Test: Performance acceptable
- [ ] Test: Fallback mesh si échec

**Fichier test** : `test_23_perspective_shader.c`

---

### Étape 10.2 : Effets shader
**But** : Effets visuels en temps réel

**Commit** : `feat: real-time shader effects (blur, color correction)`
```
- Blur
- Color grading
- Brightness/Contrast
- Custom shaders utilisateur
```

**Tests** :
- [ ] Test: Chaque effet fonctionne
- [ ] Test: Combinaison d'effets
- [ ] Test: Performance

**Fichier test** : `test_24_shader_effects.c`

---

## Phase 11 : Documentation et Polish (TODO)

### Étape 11.1 : Documentation API complète
**But** : Documenter toutes les fonctions

**Commit** : `docs: comprehensive API documentation`
```
- Doxygen comments
- Guide utilisateur
- Exemples de code
```

**Tests** :
- [ ] Doxygen génère docs sans erreur
- [ ] Tous les exemples compilent
- [ ] Guide complet et clair

---

### Étape 11.2 : Exemples supplémentaires
**But** : Couvrir tous les cas d'usage

**Commit** : `examples: add comprehensive usage examples`
```
- Exemple basique
- Exemple multi-projecteur
- Exemple avec vidéo
- Exemple intégration game
```

**Tests** :
- [ ] Tous les exemples compilent
- [ ] Exemples fonctionnent sur toutes plateformes

---

### Étape 11.3 : Tests automatisés
**But** : CI/CD et tests unitaires

**Commit** : `test: automated test suite with CI`
```
- Tests unitaires complets
- GitHub Actions CI
- Tests cross-platform
```

**Tests** :
- [ ] CI passe sur Linux/Mac/Windows
- [ ] Coverage > 80%
- [ ] Tests rapides (<1min)

---

## Phase 12 : Extensions (Futur)

### Étape 12.1 : Support 3D
**But** : Projection sur objets 3D

**Commit** : `feat: 3D surface mapping (cubes, spheres, custom meshes)`
```
- RM_CreateSurface3D()
- Mapping sur mesh 3D
- Caméra virtuelle
```

**Tests** :
- [ ] Test: Projection sur cube
- [ ] Test: Projection sur sphère
- [ ] Test: Mesh custom

**Fichier test** : `test_25_3d_mapping.c`

---

### Étape 12.2 : Entrée vidéo
**But** : Support de flux vidéo en temps réel

**Commit** : `feat: video input support (webcam, files, streams)`
```
- Intégration video decoder
- Streaming temps réel
- Performance optimisée
```

**Tests** :
- [ ] Test: Fichier vidéo
- [ ] Test: Webcam
- [ ] Test: Stream réseau
- [ ] Test: 60 FPS minimum

**Fichier test** : `test_26_video_input.c`

---

### Étape 12.3 : Network sync
**But** : Synchronisation multi-machine

**Commit** : `feat: network synchronization for distributed setups`
```
- Protocole sync
- Master/slave setup
- Time sync précis
```

**Tests** :
- [ ] Test: 2 machines synchronisées
- [ ] Test: Latence < 16ms
- [ ] Test: Récupération après déconnexion

**Fichier test** : `test_27_network_sync.c`

---

## Récapitulatif des Phases

| Phase | Statut | Commits | Tests |
|-------|--------|---------|-------|
| 1. Foundation |  🔄 Done | 4 | 4 |
| 2. Calibration |  🔄 Done | 3 | 3 |
| 3. Mesh Warping |  🔄 Done | 3 | 3 |
| 4. Homographie |  🔄 Done | 3 | 3 |
| 5. Configuration I/O |  🔄 Partial | 2 | 2 |
| 6. Utilitaires |  🔄 Partial | 2 | 2 |
| 7. Multi-Surface |  🔄 Done | 1 | 1 |
| 8. Optimisations | 🔄 TODO | 0/2 | 0/2 |
| 9. Features Avancées | 🔄 TODO | 0/3 | 0/3 |
| 10. Shader Custom | 🔄 TODO | 0/2 | 0/2 |
| 11. Documentation | 🔄 TODO | 0/3 | 0/3 |
| 12. Extensions | 🔄 Future | 0/3 | 0/3 |

**Total actuel** : 1 commits  | 0 tests 

---

## Convention de Commits

```
feat:     Nouvelle fonctionnalité
fix:      Correction de bug
perf:     Amélioration performance
refactor: Refactoring sans changement fonctionnel
test:     Ajout ou modification de tests
docs:     Documentation
style:    Formatage, style
chore:    Maintenance, build
```

---

## Checklist Release v1.0

- [ ] Phase 1-7 complètes
- [ ] Tous les tests passent
- [ ] Documentation API complète
- [ ] 3 exemples minimum
- [ ] README avec quickstart
- [ ] LICENSE file
- [ ] CHANGELOG.md
- [ ] Tag git v1.0.0

---

## Checklist Release v2.0 (Future)

- [ ] Shader custom fonctionnel
- [ ] Edge blending
- [ ] Auto-keystone
- [ ] Support 3D
- [ ] 90% test coverage
- [ ] Multi-platform CI

---

## Notes

- **Tests visuels** : Beaucoup de tests nécessitent validation visuelle (warping, blending)
- **Performance** : Target 60 FPS minimum sur hardware moyen
- **Compatibilité** : Tester sur Linux, macOS, Windows
- **Raylib version** : Minimum 5.0, tester avec latest

---

Généré le : 2024-12-30
Version RayMap : 1.1.0
