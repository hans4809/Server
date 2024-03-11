// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>

//atomic : All-Or-Nothing
atomic<int32> sum = 0; //병목 현상 일으킬 수 있음
//Mutual Exclusive (상호베타적)
mutex m;

// RAII (Resource Acquistion Is Initialization)
template<typename T>
class LockGuard 
{
public:
    LockGuard(T& m) 
    {
        _mutex = &m;
        _mutext->lock();
    }

    ~LockGuard()
    {
        _mutex->unlock();
    }
private:
    T* _mutex;
};

class SpinLock 
{
public:
    void lock() 
    {
        // CAS (Compare-And-Swap)
        bool expected = false;
        bool desired = true;

        //CAS 의사 코드
        //if (_locked == expected) 
        //{
        //    expected = _locked;
        //    _locked = desired;
        //    return true;
        //}
        //else 
        //{
        //    expected = _locked;
        //    return false;
        //}

        while (_locked.compare_exchange_strong(expected, desired) == false)
        {
            expected = false;
        }

    }

    void unlock()
    {
        _locked.store(false);
    }
private:
    atomic<bool> _locked = false;
};
void HelloThread() 
{
    m.lock();

    cout << "Hello Thread" << endl;

    m.unlock();
}


int main()
{
    HelloWorld();
    
    std::thread t;

    auto id1 = t.get_id();
    t = std::thread(HelloThread);

    int32 count = t.hardware_concurrency(); // CPU 코어 개수
    auto id2 = t.get_id();// 쓰레드마다 id

    t.detach();// std::thread 객체에서 실제 쓰레드를 분리

    if(t.joinable())
        t.join();

    cout << "Hello Main" << endl;

}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
