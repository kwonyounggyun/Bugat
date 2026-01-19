# Bugat - Modern C++ Server Framework

**Bugat**은 최신 C++20 기능(Coroutines, Concepts)을 적극 활용하여 설계된 고성능 서버 프레임워크입니다.
레거시 구조를 탈피하고, 모던 C++의 강력한 타입 시스템과 비동기 처리 모델을 통해 **안전성(Safety)**과 **성능(Performance)**을 모두 확보하는 것을 목표로 합니다.

## 🏗️ Architecture Design

이 프로젝트는 의존성을 명확히 분리하고 빌드 효율성을 높이기 위해 계층형 아키텍처로 설계되었습니다.

* **Core**: 가장 기초적인 유틸리티, 메모리 관리자(Object Pool), 기본 타입 정의가 포함된 라이브러리입니다.
* **Base**: Core를 기반으로 네트워크 엔진, 스레드 풀, 로깅 시스템 등 서버의 근간이 되는 기능을 제공합니다.
* **GameServer**: Base 위에서 동작하는 실제 게임/콘텐츠 서버 로직을 구현하는 레이어입니다.
    * *Dependency Flow: GameServer -> Base -> Core*

## ⚡ Concurrency & Thread Safety Model

이 프로젝트는 멀티스레드 환경에서의 데이터 경쟁(Race Condition)과 데드락(Deadlock) 문제를 원천적으로 방지하기 위해 **Serialized Execution Model (Actor Pattern)**을 채택했습니다.

### 1. SerializeObject & Job Queue
* 모든 게임 객체(Object)는 `SerializeObject`를 상속받아 고유한 **Job Queue**를 가집니다.
* 특정 객체에 대한 모든 작업(상태 변경, 로직 수행)은 즉시 실행되지 않고, 해당 객체의 큐에 **Job** 형태로 푸시됩니다.

### 2. Context-Based Execution
* **Context**(실행 스레드/워커)는 `SerializeObject`의 큐에 쌓인 작업들을 가져와 순차적으로 실행합니다.
* 이를 통해 단일 객체 내에서는 **마치 싱글 스레드처럼** 동작하므로, 개발자는 복잡한 뮤텍스(Mutex) 관리 없이 비즈니스 로직을 작성할 수 있습니다.

### 3. Safe Object Interaction
* 객체 간 상호작용(A -> B 공격 등) 시, 직접 상대 객체의 메모리를 수정하는 대신 **상대 객체의 큐에 작업을 요청(Enqueue)**하는 방식을 사용합니다.
* 이 방식은 스레드 간 경합을 제거하고 객체의 상태 무결성(Consistency)을 완벽하게 보장합니다.

## ✨ Key Features

### 1. Asynchronous I/O with C++20 Coroutines
콜백 지옥(Callback Hell)을 해결하고 가독성 높은 비동기 코드를 작성하기 위해 **C++20 Coroutines**를 도입했습니다.
* **Custom Task Implementation**: `std::coroutine_handle`의 생명주기와 소유권 이전을 명확히 관리하는 커스텀 `Task` 객체를 구현하여 오버헤드를 최소화했습니다.
* **Zero-Overhead Abstraction**: 비동기 로직을 동기 코드처럼 직관적으로 작성할 수 있습니다.

### 2. High-Performance Memory Management
서버 성능의 핵심인 메모리 관리 효율성을 극대화했습니다.
* **Smart Pointers**: `std::shared_ptr`의 오버헤드를 줄이고 프로젝트 특성에 맞춘 커스텀 스마트 포인터(`TSharedPtr`)를 구현하여 참조 카운팅 성능을 최적화했습니다.
* **Lock-Free Structures**: 락 경합을 최소화하기 위해 Lock-Free Queue를 적용했습니다.
* **Object Pool**: 메모리 할당 부하를 줄이고 Pool부족시 동적으로 증가하도록 설계했습니다.(향후 자동으로 크기를 조절하게 만들예정)

## 🛠️ Tech Stack & Environment

* **Language**: C++23
* **IDE**: Visual Studio 2026
* **Libraries**:
    * Boost (Asio, Redis, MySQL)
    * nlohmann/json (JSON Parsing)
* **Build Settings**:
    * Multi-threaded Static Linking(/MT)
    * Memory Alignment Optimization

## 🚀 Getting Started

### Prerequisites
* Visual Studio 2026 (v145 build tools)
* C++23 Standard support enabled
* Boost Library (Path configuration required)

### Build
1.  Clone the repository.
2.  Open `server.sln` solution file.
3.  Set the build configuration to `Release` / `x64`.
4.  Build the `Core` project first, followed by `Base`, and then `GameServer`.
