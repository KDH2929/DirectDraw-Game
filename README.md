# DirectDraw2D_Game

**개발 기간** : 3주  
**개발 인원** : 2인팀

[유튜브 영상](https://youtu.be/iQB3RYAKHIA?si=Pz4x_eIdLN7srZDO)

<br>

---
### std::async 기반 비동기 로딩 과정에서의 문제
- [비동기 로딩 과정에서 로그 메세지가 깨지던 현상](https://wandering-rumba-865.notion.site/26eaba645d32808ab1ccf49f04927223)
- [Scene 전환 시 프로그램이 종류되던 문제](https://wandering-rumba-865.notion.site/Scene-1a9aba645d328042a577d510c35a7bf7)
- [Dangling 포인터 문제](https://wandering-rumba-865.notion.site/1a9aba645d3280d4b8cae88041aa0135)

<br>

---

https://github.com/megayuchi/PixelHorizon
유영천님의 DirectDraw 엔진 소스코드를 기반으로 구현된 프로젝트입니다.<br><br>


- [유영천님 DirectDraw 강의영상](https://www.youtube.com/live/xrYT5l5mYR8?si=bQJBaLkkqmvZMAW9)
- [DirectDraw 개인공부](https://wandering-rumba-865.notion.site/DirectDraw-197aba645d3280978479fd0b73652cac?pvs=74)

<br>

---

- **충돌처리부분은 관련된 서적과 인터넷자료들을 참고**하였습니다.<br>

- **타일맵 처리 부분은 제 동생이 구현해주었습니다.**

- **클래스 초안과 일부소스코드(UI, DebugManager 등)들을 생성형AI를 활용하였습니다.**

---

<br>

유영천님의 DirectDraw 엔진 소스코드에는 스프라이트 렌더 기능이 없었기에 아래와 같이 엔진코드를 일부 수정하였습니다. 

<br>

- [DirectDraw에서 스프라이트 렌더 구현 시 고려했던 점](https://wandering-rumba-865.notion.site/DirectDraw-Sprite-281aba645d32809caec0e045554bba0a)

<br>

---

<br>

## 구현 목록

- **기본 시스템**
  - InputManager
  - SceneManager (비동기 Scene 로딩) 
  - 카메라

<br>

- **게임 오브젝트**
  - 캐릭터 & 몬스터
  - Sprite 애니메이션

<br>

- **충돌 처리**
  - Raycast (Slab Method)
  - AABB 충돌 검출
  - OBB 충돌 검출
    
<br>





