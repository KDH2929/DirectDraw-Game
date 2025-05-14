#include "DebugManager.h"
#include <direct.h>
#include <time.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <codecvt>

// 생성자: 로그 파일 초기화
DebugManager::DebugManager() {
    InitializeLogFile();
}

DebugManager::~DebugManager() {
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
}

void DebugManager::InitializeLogFile() {
    // "Log" 디렉토리 생성
    const char* logDir = "Log";
    _mkdir(logDir);

    // 현재 날짜와 시간을 기반으로 로그 파일 이름 생성
    time_t now = time(NULL);
    struct tm localTime;
    localtime_s(&localTime, &now);

    char fileName[256];
    sprintf_s(fileName, sizeof(fileName), "%s/Log_%04d-%02d-%02d_%02d-%02d-%02d.txt",
        logDir,
        localTime.tm_year + 1900,
        localTime.tm_mon + 1,
        localTime.tm_mday,
        localTime.tm_hour,
        localTime.tm_min,
        localTime.tm_sec);

    logFilePath = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(fileName);

    // wide file open for UTF-16
    m_logFile.open(logFilePath, std::ios::out | std::ios::app);
    if (!m_logFile.is_open()) {
        std::wcerr << L"Failed to open log file: " << logFilePath << std::endl;
    }
}

void DebugManager::AddOnScreenMessage(const std::string& msg, float duration) {
    DebugMessage newMsg;
    newMsg.text = msg;
    newMsg.duration = duration * 1000.0f; // seconds -> ms
    newMsg.elapsed = 0.0f;
    m_messages.push_back(newMsg);
}

void DebugManager::LogMessage(const std::wstring& msg) {
    std::lock_guard<std::mutex> lock(logMutex);

    // 디버그 출력
    OutputDebugStringW(msg.c_str());
    OutputDebugStringW(L"\n");

    // 콘솔 출력
    std::wcout << msg << std::endl;

    // 파일 출력
    if (m_logFile.is_open()) {
        m_logFile << msg << std::endl;
    }
}

void DebugManager::Update(float deltaTime) {
    for (auto it = m_messages.begin(); it != m_messages.end(); ) {
        it->elapsed += deltaTime;
        if (it->elapsed > it->duration)
            it = m_messages.erase(it);
        else
            ++it;
    }
}

void DebugManager::RenderMessages() {
    ImGui::Begin("Debug Messages");
    for (const auto& msg : m_messages) {
        ImGui::Text("%s", msg.text.c_str());
    }
    ImGui::End();
}
