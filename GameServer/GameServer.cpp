// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <future>

#pragma region LockRegion
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
        _mutex->lock();
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

            //this_thread::sleep_for(std::chrono::milliseconds(100));
            this_thread::sleep_for(0ms);
            //this_thread::yield();
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
#pragma endregion


#pragma region EventRegion
mutex m1;
queue<int32> q;
HANDLE handle;

void Producer() 
{
    while (true)
    {
        {
            unique_lock<mutex> lock(m1);
            q.push(100);
        }

        ::SetEvent(handle);

        this_thread::sleep_for(10000000ms);
    }
}

void Consumer()
{
    while (true)
    {
		::WaitForSingleObject(handle, INFINITE);

		unique_lock<mutex> lock(m1);
        if (q.empty() == false)
        {
			int32 data = q.front();
			q.pop();
            cout << data << endl;
		}
	}

}
#pragma endregion

#pragma region ConditionVariable
mutex m2;
queue<int32> q2;
//참고) cv는 User-Level Object
condition_variable cv;

void Producer2()
{
    while (true)
    {
        // 1) Lock을 걸고
        // 2) 공유 변수 값을 수정
        // 3) Lock을 풀고
        // 4) 조건 변수를 통해 다른 쓰레드에게 통지
        {
			unique_lock<mutex> lock(m2);
			q.push(100);
		}

        cv.notify_one();// wait중인 쓰레드 중 하나를 깨운다.
	}
}

void Consumer() 
{
    while (true) 
    {
        unique_lock<mutex> lock(m2);
        cv.wait(lock, []() {return q.empty() == false; /*탈출 조건*/});
        // 1) Lock을 잡고
        // 2) 조건 확인
        // 만족 하면 빠져 나와서 이어서 코드를 진행
        // 만족하지 않으면 Lock을 풀고 대기 상태로 전환

        {
			int32 data = q.front();
			q.pop();
			cout << data << endl;
		}
    }
}
#pragma endregion

int main()
{
    //커널 오브젝트
    // Usage Count
    // Signal(파란 불) / Non-Signal(빨간 불) << bool
    // Auto/ Manual << bool
   handle = ::CreateEvent(NULL/*보안 속성*/, FALSE/*bManualReset*/, FALSE/*bInitialState*/, NULL);

   thread t1(Producer);
   thread t2(Consumer);

   t1.join();
   t2.join();

   ::CloseHandle(handle);
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
