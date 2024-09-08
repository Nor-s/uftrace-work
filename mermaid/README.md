# Mermaid.js 

**Mermaid.js?**
- JavaScript based diagramming and charting tool that renders Markdown-inspired text definitions to create and modify diagrams dynamically.

**분석하게된 계기**
- uftrace를 분석하던 도중 Mermaid 렌더링 부분에 이슈가 있음을 발견
- Mermaid 분석 및 이슈 해결해보자는 목표를 세움

## 개발환경 세팅

- [contributting.md](https://github.com/mermaid-js/mermaid/blob/develop/packages/mermaid/src/docs/community/contributing.md)
  1. `git clone`
  2. `node`, `pnpm` 설치
  3. `pnpm env use --global 20`
  4. `pnpm install` (mermaid 프로젝트 루트에서)
  5. `pnpm test` (선택, 전체 유닛테스트 실행)
  6. `develop` branch를 기반으로 `[feature | bug | chore | docs]/[issue number]_[short-description]` 브랜치 생성 (eg. `git checkout -b docs/2910_update-contributing-guidelines`)
  7. `pnpm run dev`
  8.  http://localhost:9000 에 접속하면 `/demos` 에 있는 html 페이지가 보임, 보통 `/dev/example.html` 를 복사해서 자유롭게 테스 
  10.  `packages/mermaid/src` 에서 소스 변경
  11.  테스트 작성 (버그 픽스 > 다시 버그가 일어나지 않도록, `pnpm test`, vitest를 사용, 만약 렌더링 관련이면 `pnpm cypress:open`)


## Mermaid 다이어그램 흐름

```
1. 파서
2. 렌더러
3. detectType
4. 렌더링 트리거
```

- https://github.com/mermaid-js/mermaid/blob/develop/packages/mermaid/src/docs/community/new-diagram-jison.md
- https://github.com/mermaid-js/mermaid/blob/develop/packages/mermaid/src/docs/community/new-diagram.md

위 글을 토대로 flowchar 흐름을 분석 진행


## 이슈: #4390

- https://github.com/mermaid-js/mermaid/issues/4390
    - `"flowchart": {"htmlLabels": false}`
    - "<", ">" 심볼이 제대로 렌더링되지 않는 문제가 발생함
 
 
## **flowchart 이슈 분석**

### **파싱**

```
var render
    diag = await Diagram.fromText(text, { title: processed.title });
```
![image](https://github.com/user-attachments/assets/fb647255-cf60-451f-b275-aa95f9bd20cb)

- `Diagram` 클래스에 text 정보를 넘겨주어 diagram을 생성
  

```
export class Diagram {
// ...
    await parser.parse(text);
    return new Diagram(type, text, db, parser, renderer);
```

- `fromText` 에서는 파서가 존재, text를 파싱하여 텍스트 정보에서 그래프를 그릴 데이터 생성함
- 파서는 jison 을 사용해서 구문분석
- 하지만, 우리의 목표인 htmlLabels 관련 코드 존재 x 파서 부분에서 문제가 발생하는 것은 적다고 생각

### **렌더링**

![image](https://github.com/user-attachments/assets/8ba461ba-6b18-45f5-820f-d70c34c38ddf)

- `renderer.draw` 함수를 통해 렌더링 정보를 생성

![image](https://github.com/user-attachments/assets/eac72fa0-4d3c-4fa8-9117-1adaa937c40d)

- `drawRect` 에서 `labelHelper` 클래스의 도움을 받아 라벨들을 생성하게 되는데 호출스택은 위와 같음

![image](https://github.com/user-attachments/assets/796feaf2-4eef-48be-81bb-4f92063579e6)

- 핵심은 `createText`에서 `useHtmlLabels` 를 통해 텍스트를 어떻게 처리할지 분기한다는 것

### **useHtmlLabels  false  텍스트 흐름**

```
    graph TD
      accTitle: What to buy
      accDescr: Options of what to buy with Christmas money
      A[Christmas] -->|Get money| B(Go shopping)
      B --> C{Let me think}
      C -->|<<> > | D[Laptop >]
      C -->|Two| E[iPhone]
      C -->|Three| F[fa:fa-car Car]

  <script type="module">
    import mermaid from './mermaid.esm.mjs';
    mermaid.initialize({
      theme: 'forest',
      logLevel: 3,
      securityLevel: 'loose',
      flowchart: { curve: 'basis', htmlLabels: false },
    });
  </script>
```

위를 테스트 하면 아래와 같은 다이어그램이 나온다.

![image](https://github.com/user-attachments/assets/16251348-1923-40d4-9e1c-ec41220e3bc4)

![image](https://github.com/user-attachments/assets/b1b02ba2-63bc-4c9c-a9a5-50a2b436d07a)

- 위의 다이어그램에서 `createText`가 생성하는 `tspan`은 위와 같다.

![image](https://github.com/user-attachments/assets/038d7d6c-ff42-478e-a2ff-531dbd9a0bdb)

코드상에서 살펴보면 위와 같이 특수문자 처리된 `&gt;` 가 `textcontent` 로 들어가는 것을 볼 수 있다.


### TextContent ?? 

[관련 블로그, textContent vs innerText vs innerHTML](https://blog.illustudio.co.kr/js-change-content/)

![image](https://github.com/user-attachments/assets/0e9f2213-0f2c-4c08-b108-1cf1adb58d8d)

`textContent` 를 통해 대입하게된다면, 특수문자든, html이든 그대로 렌더링하게 변환해준다는것

![image](https://github.com/user-attachments/assets/7143fe6a-8816-4b1f-9152-8fa4f283d453)


즉, `&gt;` 가 인풋이라면... `&gt;` 가 렌더링 되도록 잘 변환해주어 결국 `&amp;gt;` 로 변환되어진다.



### 해결?

![image](https://github.com/user-attachments/assets/8c4d0bb6-cf10-45ba-891d-1b27d340adc7)

![image](https://github.com/user-attachments/assets/c2bfc454-bcf4-44c8-a9e9-fe82aaee9989)

간단하게 tspan을 생성할 때, `&lt;`, `&gt;` 를 '>',  '<' 로  변경하면 위처럼 정상적으로 렌더링 되는 것을 확인할 수 있다.

그 이전에 변경하게되면 비정상적인 결과가 나오므로 이게 최선의 방법인지 더 분석이 필요하다.





