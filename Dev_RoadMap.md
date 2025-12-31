# RayMap - Plan de Développement

Plan détaillé du développement de RayMap avec objectifs, commits et tests pour chaque étape.

**Note** : Ce plan suit l'ordre de développement réel. Certaines étapes sont réorganisées pour prioriser les fonctionnalités critiques.

---

## Phase 1 : Foundation (MVP) ✅ EN COURS

### Étape 1.1 : Architecture de base ✅ COMPLÈTE
**But** : Créer la structure header-only avec API publique/privée

**Commit** : `feat: initial library structure with opaque types`
```
- Types opaques (RM_Surface, RM_Calibration)
- Structures publiques (RM_Quad, enums)
- Déclarations de fonctions
- Licence et documentation header
```

**Tests** :
- [x] Compilation sans erreurs
- [x] Inclusion dans un projet externe
- [x] Header guards fonctionnels

**Fichier test** : `01_compilation/`

---

### Étape 1.2 : Création et destruction de surfaces ✅ COMPLÈTE
**But** : Gestion mémoire et lifecycle des surfaces

**Commit** : `feat: surface creation and destruction with RenderTexture`
```
- RM_CreateSurface()
- RM_DestroySurface()
- Allocation RenderTexture
- Quad par défaut
- Fix RMAPI pour linkage correct
```

**Tests** :
- [x] Test: Créer et détruire 1 surface → pas de leak
- [x] Test: Créer et détruire 100 surfaces → pas de leak
- [x] Test: RenderTexture valide après création

**Fichier test** : `02_surface_lifecycle/`

**Leçons apprises** :
- RMAPI doit être redéfini dans le bloc RAYMAP_IMPLEMENTATION
- `#undef RMAPI` puis `#define RMAPI` (vide) pour l'implémentation

---

### Étape 1.3 : Rendu simple (quad basique) ✅ COMPLÈTE
**But** : Afficher du contenu sur une surface non-déformée

**Commit** : `feat: basic quad rendering with texture mapping`
```
- RM_BeginSurface() / RM_EndSurface()
- RM_DrawSurface() avec DrawTexturePro
- Mapping UV correct (fix flip RenderTexture)
```

**Tests** :
- [x] Test: Contenu visible sans déformation
- [x] Test: Pas d'effet miroir
- [x] Test: Coordonnées de texture correctes

**Fichier test** : `03_simple_render/`

**Leçons apprises** :
- DrawTexturePro plus fiable que rlgl pour quads simples
- RenderTexture nécessite flip vertical (source.height négatif)
- rlgl nécessite rlDrawRenderBatchActive() pour fonctionner

---

### Étape 1.4 : Gestion des quads ✅ COMPLÈTE
**But** : Permettre la déformation du quad (API seulement)

**Commit** : `feat: quad manipulation (set/get corners)`
```
- RM_SetQuad()
- RM_GetQuad()
- RM_GetSurfaceSize()
```

**Tests** :
- [x] Test: Set quad déformé → valeurs changent
- [x] Test: Get quad retourne valeurs correctes
- [x] Test: Animation du quad fonctionne

**Fichier test** : `04_quad_manipulation/`

**Note importante** : 
- DrawTexturePro dessine uniquement des rectangles (pas de vraie déformation)
- La vraie déformation nécessite un mesh subdivisé (Étape 3.1)
- Les coins bottomRight sont ignorés par DrawTexturePro
- **C'est normal et attendu** - résolu à l'étape suivante

---

## Phase 3 : Mesh Warping 🔄 PRIORISÉ

> **Décision** : Phase 3 déplacée avant Phase 2 (Calibration) pour résoudre le problème de déformation des quads. La calibration sera plus utile une fois le vrai warping fonctionnel.

### Étape 3.1 : Subdivision bilinéaire 🔄 EN COURS
**But** : Remplacer le quad simple par un mesh subdivisé

**Commit** : `feat: bilinear mesh subdivision for smooth warping`
```
- rm_GenerateBilinearMesh()
- Interpolation bilinéaire des vertices
- Résolution configurable (16x16 par défaut)
- Remplacement de DrawTexturePro par DrawMesh
```

**Tests** :
- [ ] Test: Mesh 16x16 → pas de ligne visible au milieu
- [ ] Test: Déformation forte → warp lisse
- [ ] Test: Les 4 coins indépendants fonctionnent
- [ ] Test: Comparaison avant/après subdivision

**Fichier test** : `05_mesh_subdivision/`

**Pourquoi maintenant** :
- Résout le problème de DrawTexturePro (rectangles uniquement)
- Permet la vraie déformation de quads
- Nécessaire avant la calibration interactive

---

### Étape 3.2 : Résolution dynamique
**But** : Permettre l'ajustement de la qualité du mesh

**Commit** : `feat: dynamic mesh resolution adjustment`
```
- RM_SetMeshResolution()
- RM_GetMeshResolution()
- Régénération automatique du mesh
- Flag meshNeedsUpdate
```

**Tests** :
- [ ] Test: Augmenter résolution → meilleure qualité
- [ ] Test: Diminuer résolution → meilleure performance
- [ ] Test: Résolution min/max respectées (4x4 à 64x64)

**Fichier test** : `06_mesh_resolution/`

---

### Étape 3.3 : Modes de warp
**But** : Implémenter les deux modes (MESH et PERSPECTIVE)

**Commit** : `feat: warp modes with mesh/perspective options`
```
- RM_SetWarpMode() / RM_GetWarpMode()
- Mode MESH : interpolation bilinéaire
- Mode PERSPECTIVE : préparation homographie (stub)
- Résolution par défaut selon mode
```

**Tests** :
- [ ] Test: Mode MESH fonctionne
- [ ] Test: Switch entre modes
- [ ] Test: Performance acceptable (>60 FPS)

**Fichier test** : `07_warp_modes/`

---

## Phase 2 : Calibration Interactive 🔄 APRÈS MESH

> **Décision** : Phase 2 déplacée après Phase 3. La calibration est plus utile une fois qu'on peut voir la vraie déformation.

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

**Fichier test** : `08_calibration_input/`

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

**Fichier test** : `09_calibration_ui/`

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
- [ ] Test: Reset → quad revient à rectangle centré
- [ ] Test: GetActiveCorner retourne bon index
- [ ] Test: IsCalibrating correct pendant drag

**Fichier test** : `10_calibration_utils/`

---

## Phase 4 : Homographie Perspective 🔄 TODO

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

**Fichier test** : `11_homography_math/`

---

### Étape 4.2 : Mode PERSPECTIVE complet
**But** : Utiliser l'homographie pour le warping

**Commit** : `feat: PERSPECTIVE mode using homography transformation`
```
- Implémentation complète de rm_ComputeHomography()
- Mesh généré avec homographie (pas bilinéaire)
- Résolution haute (32x32) par défaut
```

**Tests** :
- [ ] Test: Mode PERSPECTIVE fonctionne
- [ ] Test: Lignes droites restent droites
- [ ] Test: Comparaison MESH vs PERSPECTIVE
- [ ] Test: Points mappés correctement

**Fichier test** : `12_perspective_mode/`

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

**Fichier test** : `13_mode_comparison/`

---

## Phase 5 : Configuration I/O 🔄 TODO

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

**Fichier test** : `14_config_io/`

---

### Étape 5.2 : Export JSON
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

**Fichier test** : `15_json_config/`

---

## Phase 6 : Utilitaires 🔄 TODO

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

**Fichier test** : `16_geometry_utils/`

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

**Fichier test** : `17_point_mapping/`

---

## Phase 7 : Multi-Surface 🔄 TODO

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

**Fichier test** : `18_multi_surface/`

---

## Phases suivantes (TODO)

Les phases 8-12 restent inchangées :
- **Phase 8** : Optimisations (cache, LOD)
- **Phase 9** : Features avancées (edge blending, masques, keystone auto)
- **Phase 10** : Shader custom (perspective pixel-perfect, effets)
- **Phase 11** : Documentation et polish
- **Phase 12** : Extensions (3D, vidéo, network sync)

---

## Récapitulatif des Phases

| Phase                      | Statut    | Commits | Tests |
|----------------------------|-----------|---------|-------|
| 1. Foundation              | ✅ 4/4    | 4/4     | 4/4   |
| 3. Mesh Warping (PRIORISÉ) | 🔄 0/3    | 0/3     | 0/3   |
| 2. Calibration             | 🔄 0/3    | 0/3     | 0/3   |
| 4. Homographie             | 🔄 0/3    | 0/3     | 0/3   |
| 5. Configuration I/O       | 🔄 0/2    | 0/2     | 0/2   |
| 6. Utilitaires             | 🔄 0/2    | 0/2     | 0/2   |
| 7. Multi-Surface           | 🔄 0/1    | 0/1     | 0/1   |
| 8-12.                      | 🔄 Future | 0/13    | 0/13  |

**Progression** : 4/31 étapes complètes (13%)

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

## Leçons Apprises (Important !)

### Header-only library
- `RMAPI` doit être redéfini dans `RAYMAP_IMPLEMENTATION`
- Structure : `#undef RMAPI` puis `#define RMAPI` (vide)
- Sinon : erreurs de linkage "référence indéfinie"

### RenderTexture
- **Toujours flippée verticalement** en OpenGL
- Solution : `source.height` négatif dans DrawTexturePro
- Ou inverser les coordonnées V dans rlgl

### DrawTexturePro vs rlgl
- **DrawTexturePro** : Simple, fiable, mais RECTANGLES SEULEMENT
- **rlgl** : Quads déformés possibles, mais nécessite `rlDrawRenderBatchActive()`
- **DrawMesh** : Solution finale pour quads déformés

### Ordre de développement
- ✅ **Mesh warping AVANT calibration** : On voit la vraie déformation
- ❌ Calibration avant mesh : On calibre un rectangle, frustrant

---

## Checklist Release v1.0

- [ ] Phases 1-7 complètes
- [ ] Tous les tests passent
- [ ] Documentation API complète
- [ ] 3 exemples minimum
- [ ] README avec quickstart
- [ ] LICENSE file
- [ ] CHANGELOG.md
- [ ] Tag git v1.0.0

---

## Notes Techniques

- **Tests visuels** : Beaucoup de tests nécessitent validation visuelle
- **Performance target** : 60 FPS minimum sur hardware moyen
- **Compatibilité** : Tester sur Linux, macOS, Windows
- **Raylib version** : Minimum 5.0, tester avec latest
- **Mesh resolution** : 16x16 par défaut = bon compromis qualité/performance

---

Dernière mise à jour : 2024-12-31  
Version RayMap : 0.3.0 (Phase 1 complète, Phase 3 en cours)
