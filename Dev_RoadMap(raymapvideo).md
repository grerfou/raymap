# RayMapVid - Plan de Développement

Plan détaillé du développement de RayMapVid avec objectifs, commits et tests pour chaque étape.

**Philosophie** : Header-only, simple, zéro fuite mémoire, intégration transparente avec RayMap.

---

## Phase 1 : Foundation (MVP) ✅ COMPLETE

### Étape 1.1 : Architecture de base ✅ COMPLETE
**But** : Créer la structure header-only avec API publique/privée

**Commit** : `feat: initial raymapvid structure with opaque types`

```c
- Type opaque RMV_Video
- Structures publiques (RMV_VideoInfo, RMV_PlaybackState, RMV_HWAccelType)
- Déclarations de fonctions
- Auto-détection RAYMAP_IMPLEMENTATION
- Licence zlib/libpng + LGPL notice FFmpeg
```

**Tests** :
- [x] Compilation sans erreurs
- [x] Inclusion dans projet externe
- [x] Header guards fonctionnels
- [x] Auto-détection RAYMAP_IMPLEMENTATION

**Fichier test** : `01_compilation/`

**Leçons apprises** :
- RMVAPI doit être redéfini dans RAYMAPVID_IMPLEMENTATION
- Auto-détection via `#if defined(RAYMAP_IMPLEMENTATION) && !defined(RAYMAPVID_IMPLEMENTATION)`

---

### Étape 1.2 : Chargement vidéo basique 
**But** : Ouvrir une vidéo, extraire métadonnées, créer texture

**Commit** : `feat: video loading with FFmpeg integration`

```c
- RMV_LoadVideo() / RMV_UnloadVideo()
- avformat_open_input() + avformat_find_stream_info()
- Trouver stream vidéo (AVMEDIA_TYPE_VIDEO)
- Init codec context (avcodec_alloc_context3)
- Créer texture Raylib (GenImage + LoadTexture)
- Allocation buffer RGB (av_image_get_buffer_size)
- Init SwsContext (sws_getContext)
- rmv_CleanupVideo() centralisé
```

**Tests** :
- [x] Test: Load vidéo valide → structure remplie
- [x] Test: Load fichier inexistant → NULL + message erreur
- [x] Test: Load NULL pointer → NULL + message erreur
- [x] Test: Unload NULL → pas de crash
- [x] Test: Double unload → pas de crash
- [x] Test: Valgrind clean (0 leaks)
- [x] Test: ASan clean (0 errors)

**Fichier test** : `02_video_loading/`

**Leçons apprises** :
- Cleanup order important : texture → buffers → contexts → format
- Toujours check NULL avant free
- FFmpeg retourne codes d'erreur négatifs (< 0)

---

### Étape 1.3 : Métadonnées vidéo 
**But** : Extraire et exposer les infos vidéo

**Commit** : `feat: video metadata extraction (resolution, fps, codec, duration)`

```c
- RMV_GetVideoInfo() retourne RMV_VideoInfo
- Extraction résolution (codecCtx->width/height)
- Calcul FPS (av_q2d(stream->r_frame_rate))
- Calcul duration (formatCtx->duration / AV_TIME_BASE)
- Nom codec (avcodec_get_name)
- Format container (formatCtx->iformat->name)
- Détection audio (parcours streams)
```

**Tests** :
- [x] Test: GetInfo sur vidéo valide → valeurs correctes
- [x] Test: GetInfo sur NULL → structure vide (0)
- [x] Test: Resolution matches fichier
- [x] Test: FPS correct (±0.1)
- [x] Test: Duration correcte (±1s)

**Fichier test** : `03_video_metadata/`

---

### Étape 1.4 : Texture accessible 
**But** : Exposer la texture Raylib pour affichage

**Commit** : `feat: expose video texture for rendering`

```c
- RMV_GetVideoTexture() retourne Texture2D
- Validation texture.id > 0
- Texture créée au load (black frame initial)
```

**Tests** :
- [x] Test: GetTexture retourne texture valide
- [x] Test: texture.id > 0
- [x] Test: Dimensions texture = dimensions vidéo
- [x] Test: DrawTexture fonctionne

**Fichier test** : `04_texture_access/`

---

## Phase 2 : Playback Basique 

### Étape 2.1 : État de lecture 
**But** : Gérer les états play/pause/stop

**Commit** : `feat: playback state management (play/pause/stop)`

```c
- RMV_PlayVideo() → state = PLAYING
- RMV_PauseVideo() → state = PAUSED
- RMV_StopVideo() → state = STOPPED, reset time
- RMV_ToggleVideoPause()
- RMV_GetVideoState() retourne RMV_PlaybackState
- RMV_IsVideoPlaying() helper bool
```

**Tests** :
- [x] Test: État initial = STOPPED
- [x] Test: PlayVideo → PLAYING
- [x] Test: PauseVideo → PAUSED
- [x] Test: StopVideo → STOPPED + time=0
- [x] Test: TogglePause alterne correctement
- [x] Test: État persiste après cleanup

**Fichier test** : `05_playback_state/`

---

### Étape 2.2 : Décodage frame-by-frame 
**But** : Implémenter RMV_UpdateVideo() avec décodage FFmpeg

**Commit** : `feat: frame decoding with FFmpeg (av_read_frame loop)`

```c
- RMV_UpdateVideo(video, deltaTime)
- Boucle av_read_frame() pour lire packets
- avcodec_send_packet() / avcodec_receive_frame()
- Gestion EAGAIN (besoin plus de packets)
- Gestion EOF (fin de vidéo)
- Gestion erreurs décodage
- Frame timing basé sur FPS (frameAccumulator)
```

**Tests** :
- [x] Test: UpdateVideo décode frame
- [x] Test: Respect timing FPS
- [x] Test: Skip frames si deltaTime trop grand
- [x] Test: État PLAYING requis pour décoder
- [x] Test: État PAUSED ne décode pas
- [x] Test: Gestion EOF propre

**Fichier test** : `06_frame_decoding/`

**Notes importantes** :
- Utiliser frameAccumulator (membre de struct, pas static)
- Ne décoder que si accumulator >= (1.0 / fps)
- av_packet_unref() après chaque packet

---

### Étape 2.3 : Conversion YUV→RGB et upload texture 
**But** : Convertir frames décodées et mettre à jour texture GPU

**Commit** : `feat: YUV to RGB conversion and texture upload`

```c
- sws_scale() pour conversion format pixel
- Conversion (frame YUV → frameRGB buffer)
- UpdateTexture(texture, rgbBuffer) pour upload GPU
- Validation dimensions avant conversion
```

**Tests** :
- [x] Test: Frame décodée → texture mise à jour
- [x] Test: Contenu visuel correct (pas de couleurs bizarres)
- [x] Test: Pas de tearing/glitches
- [x] Test: Performance acceptable (>30 FPS software decode 720p)

**Fichier test** : `07_texture_update/`

---

### Étape 2.4 : Loop vidéo 
**But** : Rejouer automatiquement en fin de vidéo

**Commit** : `feat: video looping with seek to start`

```c
- RMV_SetVideoLoop(video, bool loop)
- Gestion EOF : si loop, seek(0) + continuer
- Gestion EOF : si !loop, state = STOPPED
- av_seek_frame() pour retour au début
- avcodec_flush_buffers() après seek
- Reset frameAccumulator après seek
```

**Tests** :
- [x] Test: Loop activé → redémarre automatiquement
- [x] Test: Loop désactivé → s'arrête à la fin
- [x] Test: Seek au début fonctionne
- [x] Test: Pas de freeze pendant loop
- [x] Test: Compteur temps reset correct

**Fichier test** : `08_video_loop/`

---

## Phase 3 : Navigation (Seeking)

### Étape 3.1 : Seeking par temps 
**But** : Se déplacer à une position temporelle précise

**Commit** : `feat: seek to specific timestamp`

```c
- RMV_SeekVideo(video, float timeSeconds)
- Conversion temps → timestamp FFmpeg (AV_TIME_BASE)
- av_seek_frame() avec AVSEEK_FLAG_BACKWARD
- avcodec_flush_buffers() après seek
- Reset frameAccumulator
- Update currentTime
```

**Tests** :
- [ ] Test: Seek milieu vidéo → frame correcte
- [ ] Test: Seek début (0.0) → première frame
- [ ] Test: Seek fin → dernière frame
- [ ] Test: Seek hors limites clamped
- [ ] Test: Précision ±0.5s

**Fichier test** : `09_seek_time/`

---

### Étape 3.2 : Seeking par frame 
**But** : Navigation frame-by-frame précise

**Commit** : `feat: seek to specific frame number`

```c
- RMV_SeekVideoFrame(video, int frame)
- Conversion frame → timestamp (frame / fps)
- Appel RMV_SeekVideo() interne
```

**Tests** :
- [ ] Test: Seek frame 0 → début
- [ ] Test: Seek frame N → frame exacte (±1 frame)
- [ ] Test: Seek frame > total → dernière frame

**Fichier test** : `10_seek_frame/`

---

### Étape 3.3 : Query position actuelle 
**But** : Obtenir position temps/frame courante

**Commit** : `feat: query current playback position`

```c
- RMV_GetVideoTime() retourne currentTime (float seconds)
- RMV_GetVideoFrame() retourne frame actuelle (int)
- Calcul frame = currentTime * fps
```

**Tests** :
- [ ] Test: GetVideoTime retourne temps correct
- [ ] Test: GetVideoFrame retourne frame correcte
- [ ] Test: Valeurs cohérentes après seek
- [ ] Test: Valeurs cohérentes pendant playback

**Fichier test** : `11_position_query/`

---

## Phase 4 : Robustesse 

### Étape 4.1 : Gestion d'erreurs complète 
**But** : Gérer tous les cas limites élégamment

**Commit** : `feat: comprehensive error handling`

```c
- Validation formats vidéo supportés
- Gestion codec non supporté
- Gestion corruption fichier
- Messages TraceLog clairs
- État ERROR si problème critique
- Recovery automatique si possible
```

**Tests** :
- [ ] Test: Fichier corrompu → ERROR + message
- [ ] Test: Codec non supporté → ERROR + message
- [ ] Test: Fichier trop grand → warning
- [ ] Test: Interrupt pendant load → cleanup proper
- [ ] Test: NULL checks sur toutes fonctions

**Fichier test** : `12_error_handling/`

---

### Étape 4.2 : Tests de stress mémoire 
**But** : Valider zéro fuite dans tous les scénarios

**Commit** : `test: memory stress tests (1000+ cycles)`

```c
- Load/unload 1000x
- Playback interrupt
- Seek rapide répété
- Multiple vidéos simultanées
- Valgrind sur tous tests
- ASan sur tous tests
```

**Tests** :
- [ ] Test: 1000 load/unload → 0 leaks
- [ ] Test: Playback 10 min → 0 leaks
- [ ] Test: 100 seeks aléatoires → 0 leaks
- [ ] Test: Interrupt brutal → cleanup OK
- [ ] Test: 4 vidéos simultanées → 0 leaks

**Fichier test** : `13_memory_stress/`

---

### Étape 4.3 : Validation edge cases 
**But** : Tester tous les cas limites

**Commit** : `test: edge cases validation`

```c
- Vidéo 1 frame
- Vidéo très courte (<1s)
- Vidéo très longue (>1h)
- Résolution bizarre (1x1, 10000x1)
- FPS bizarre (1 fps, 240 fps)
- Formats rares (MPEG1, AV1)
```

**Tests** :
- [ ] Test: Vidéo 1 frame
- [ ] Test: Vidéo 0.1s
- [ ] Test: Vidéo 2h
- [ ] Test: 8K vidéo
- [ ] Test: 1 FPS vidéo
- [ ] Test: Tous codecs courants (H264, H265, VP9, AV1, MPEG)

**Fichier test** : `14_edge_cases/`

---

## Phase 5 : Performance (Optionnel) 

### Étape 5.1 : Hardware acceleration 
**But** : Décodage GPU pour 4K/8K

**Commit** : `feat: hardware acceleration support (NVDEC, VAAPI, VideoToolbox)`

```c
- RMV_LoadVideoEx(path, RMV_HWAccelType)
- Détection hardware disponible
- av_hwdevice_ctx_create()
- codecCtx->hw_device_ctx
- Fallback software si échec
- RMV_GetAvailableHWAccel()
- RMV_IsHWAccelActive()
```

**Tests** :
- [ ] Test: NVDEC sur NVIDIA → 4K fluide
- [ ] Test: VAAPI sur Intel/AMD → 1080p fluide
- [ ] Test: VideoToolbox sur macOS → 4K fluide
- [ ] Test: Fallback software si HW fail
- [ ] Test: Benchmark SW vs HW

**Fichier test** : `15_hardware_accel/`

---

### Étape 5.2 : Optimisations 
**But** : Performance maximale

**Commit** : `perf: frame buffering and optimizations`

```c
- Frame queue/buffering
- Éviter allocations répétées
- Pré-decode frames (async)
- Zero-copy où possible
```

**Tests** :
- [ ] Test: Benchmark avant/après
- [ ] Test: 4K@60fps stable
- [ ] Test: CPU usage réduit
- [ ] Test: Latence réduite

**Fichier test** : `16_optimizations/`

---

## Phase 6 : Features Avancées 

### Étape 6.1 : Vitesse de lecture 
**But** : Slow motion / fast forward

**Commit** : `feat: playback speed control`

```c
- RMV_SetVideoSpeed(video, float speed)
- speed = 0.5 → slow motion
- speed = 2.0 → fast forward
- Ajustement frameAccumulator
```

**Tests** :
- [ ] Test: Speed 0.5 → 2x plus lent
- [ ] Test: Speed 2.0 → 2x plus rapide
- [ ] Test: Speed 0.0 → pause
- [ ] Test: Audio désynchronisé (attendu)

**Fichier test** : `17_playback_speed/`

---

### Étape 6.2 : Audio sync (bonus)
**But** : Synchroniser audio et vidéo

**Commit** : `feat: audio synchronization (experimental)`

```c
- Décoder stream audio
- Synchroniser timestamps audio/vidéo
- Utiliser raylib audio
```

**Tests** :
- [ ] Test: Audio joue synchronisé
- [ ] Test: Seek audio + vidéo
- [ ] Test: Pas de drift audio/vidéo

**Fichier test** : `18_audio_sync/`

**Note** : Feature complexe, peut être reportée à v2.0

---

## Phase 7 : Polish & Release 

### Étape 7.1 : Exemples complets 
**But** : Exemples pour tous les use cases

**Commit** : `docs: comprehensive examples`

```
- example_basic.c (load + play)
- example_controls.c (play/pause/seek)
- example_loop.c (loop vidéo)
- example_raymap.c (avec projection mapping)
- example_hwaccel.c (hardware accel)
- example_multiple.c (plusieurs vidéos)
```

**Tests** :
- [ ] Chaque exemple compile
- [ ] Chaque exemple fonctionne
- [ ] Chaque exemple commenté

**Fichiers** : `examples/`

---

### Étape 7.2 : Documentation API 
**But** : Doc complète de l'API

**Commit** : `docs: complete API reference`

```
- API_REFERENCE.md
- Documentation chaque fonction
- Exemples de code
- Notes sur performance
- Licensing (LGPL FFmpeg)
```

**Livrables** :
- [ ] API_REFERENCE.md complet
- [ ] README.md avec quickstart
- [ ] LICENSING_GUIDE.md
- [ ] FAQ.md

---

### Étape 7.3 : Tests CI/CD 
**But** : Automatiser les tests

**Commit** : `ci: GitHub Actions for automated testing`

```yaml
- Build sur Linux/macOS/Windows
- Tests Valgrind (Linux)
- Tests ASan (tous OS)
- Tests avec vidéos réelles
```

**Livrables** :
- [ ] .github/workflows/ci.yml
- [ ] Tests passent sur 3 OS
- [ ] Badge CI dans README

---

## Récapitulatif des Phases

| Phase                    | Statut       | Étapes  | Tests   |
|--------------------------|--------------|---------|---------|
| 1. Foundation            | ⏸️ COMPLETE  | 4/4     | 17/17   |
| 2. Playback Basique      | ⏸️ EN COURS  | 0/4     | 0/16    |
| 3. Navigation (Seeking)  | ⏸️ TODO      | 0/3     | 0/9     |
| 4. Robustesse            | ⏸️ TODO      | 0/3     | 0/17    |
| 5. Performance           | ⏸️ TODO      | 0/2     | 0/8     |
| 6. Features Avancées     | ⏸️ TODO      | 0/2     | 0/7     |
| 7. Polish & Release      | ⏸️ TODO      | 0/3     | 0/5     |

**Progression globale** : 1/21 étapes (19%)  
**Tests validés** : 17/79 (22%)

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

### Code
- [ ] Phases 1-4 complètes (MVP fonctionnel)
- [ ] Phase 5 optionnelle (HW accel bonus)
- [ ] Tous les tests passent
- [ ] Valgrind clean (0 leaks)
- [ ] ASan clean (0 errors)
- [ ] Compilation sans warnings (-Wall -Wextra)

### Documentation
- [ ] README.md avec quickstart
- [ ] API_REFERENCE.md complète
- [ ] LICENSING_GUIDE.md (zlib + LGPL FFmpeg)
- [ ] 3+ exemples fonctionnels
- [ ] CHANGELOG.md
- [ ] FAQ.md

### Release
- [ ] VERSION dans header (1.0.0)
- [ ] Tag git v1.0.0
- [ ] Release notes
- [ ] Binaires FFmpeg (optionnel)

---

## Priorités Développement

### MUST HAVE (P0) - Bloquant v1.0
1. ✅ Phase 1 (Foundation)
2. ⏸️ Phase 2 (Playback Basique)
3. ⏸️ Phase 3 (Seeking)
4. ⏸️ Phase 4 (Robustesse)

### SHOULD HAVE (P1) - Important
1. ⏸️ Phase 5.1 (Hardware Accel)
2. ⏸️ Phase 6.1 (Speed Control)
3. ⏸️ Phase 7 (Documentation)

### NICE TO HAVE (P2) - Bonus
1. ⏸️ Phase 5.2 (Optimisations)
2. ⏸️ Phase 6.2 (Audio Sync)

---

## Metrics de Qualité

### Performance Targets
- **1080p@60fps** : Software decode minimum
- **4K@60fps** : Hardware decode minimum
- **Seek latency** : < 100ms
- **Memory usage** : < 100MB pour 1080p
- **Frame drops** : < 1% à 60 FPS

### Code Quality
- **Warnings** : 0 avec -Wall -Wextra
- **Leaks** : 0 (Valgrind)
- **Errors** : 0 (ASan)
- **Test coverage** : > 80% des fonctions

---

## Notes Techniques

### FFmpeg
- **Minimum version** : 4.4
- **Codecs requis** : H.264, H.265, VP9 (minimum)
- **Format pixel** : RGB24 pour simplicité
- **API style** : Moderne (avcodec_send_packet/receive_frame)

### Raylib
- **Minimum version** : 5.0
- **Texture format** : PIXELFORMAT_UNCOMPRESSED_R8G8B8
- **Update** : UpdateTexture() pour upload GPU

### Timing
- **Frame pacing** : Accumulator basé sur deltaTime
- **Sync** : FPS vidéo (pas VSync écran)
- **Drift** : Acceptable pour v1.0 (audio sync v2.0)

---


