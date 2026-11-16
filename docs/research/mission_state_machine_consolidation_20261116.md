# C++ 상태머신 저장소 정리 (2026-11-16)

## 유지 저장소
`gxmzung/px4-fault-aware-mission-verification`: C++20 event-driven mission, CommandGuard, IClock, simulated vehicle interface, telemetry health, failsafe, scenario fault injection, Catch2/CTest CI.

## Archive 후보
`gxmzung/mission-state-machine-cpp`: 15-file C++17 educational prototype with hard-coded sequential `next()` state transitions and independent simple sensor thresholds.

## 실질적으로 이관한 기능
- 원본 Makefile, 실행 소스 5개, 헤더 4개, 예시 데이터, 문서 및 테스트 노트 **전부 원형 보존**.
- 각 원본 blob SHA와 이관본 SHA의 일치 검증.
- 원본 C++17 데모를 별도 실행하고 현재 실제 출력 흐름을 고정.
- 중심 UAVMV `src/`와 `include/uavmv/` 내부 소스는 전혀 수정하지 않음.

## 차이 및 위험
- 단순 상태 자동 전진은 명시적 event/guard 기반 임무 검증을 대체하지 못함.
- Failsafe 판단 임계치는 구조와 기준 모두 다르므로 병합하지 않음.
- **기존 데모는 완주 지점에서 루프를 종료해 마지막 저전압 시료를 처리하지 않음**: Failsafe 성공 실증이라고 표기 금지.
- 원본 프로그램은 하드웨어/실기체 검증 근거 없음.

## 종료 조건
- 원본 15개 Git blob 일치 및 역사적 콘솔 데모 검증.
- 중심 저장소 CMake/CTest CI 정상 유지.
- 추가 공개 패키지/소비자 의존성 확인 후 원본 Archive 권장. 영구 삭제 금지.
