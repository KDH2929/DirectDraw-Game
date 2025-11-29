# DirectDraw2D_Game

[유튜브 영상](https://youtu.be/iQB3RYAKHIA?si=Pz4x_eIdLN7srZDO)

<br>

---

https://github.com/megayuchi/PixelHorizon
유영천님의 DirectDraw 엔진 소스코드를 기반으로 구현된 프로젝트<br><br>



- [유영천님 DirectDraw 강의영상](https://www.youtube.com/live/xrYT5l5mYR8?si=bQJBaLkkqmvZMAW9)
- [DirectDraw 개인공부](https://wandering-rumba-865.notion.site/DirectDraw-197aba645d3280978479fd0b73652cac?pvs=74)
- [공부한 2D 충돌처리](https://wandering-rumba-865.notion.site/2D-1b2aba645d3280b19597fb3ae25a0a92)

<br>

---

### std::async 기반 비동기 로딩 과정에서의 문제해결과정
- [비동기 로딩 과정에서 로그 메세지가 깨지던 현상](https://wandering-rumba-865.notion.site/26eaba645d32808ab1ccf49f04927223)
- [Scene 전환 시 프로그램이 종류되던 문제](https://wandering-rumba-865.notion.site/Scene-1a9aba645d328042a577d510c35a7bf7)
- [Dangling 포인터 문제](https://wandering-rumba-865.notion.site/1a9aba645d3280d4b8cae88041aa0135)

<br>



---

## 구현 목록

- **기본 시스템**
  - 기존 Game 클래스와 GameObject 클래스 리팩토링
  - InputManager
  - SceneManager (비동기 Scene 로딩) 
  - 카메라

<br>

- **게임 오브젝트**
  - 캐릭터 & 몬스터
  - Sprite 애니메이션
  - FSM (Idle, Move, Jump, Dash, DashAttack, ComboAttack(연속공격))

<br>

- **충돌 처리**
  - Raycast (Slab Method)
  - AABB 충돌 검출
  - OBB 충돌 검출
    
<br>





