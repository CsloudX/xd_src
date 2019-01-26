#ifndef _X_UNIT_TEST_HPP
#define _X_UNIT_TEST_HPP

// 注意：测试类是一个特殊的类，千万不要包含其他类，因为其他类需要包含此测试类
#include <iostream>
#include <string>
#include <XGlobal.h>

#if defined(X_OS_WIN32)
#include <windows.h>
#endif

class XUnitTest
{
protected:
	virtual void testEvent() = 0;

public:
    XUnitTest():m_passCount(0),m_failCount(0){ 
        std::wcout.imbue(std::locale(""));
    }
    virtual ~XUnitTest()
	{
	}
    void run()
	{ 
		static std::wstring line = L"============";
		std::wcout << line << L" START " << m_id << L" test " << line << L"\n";
		testEvent();
		std::wcout<<std::endl <<
			"TOTAL: "<<m_passCount+m_failCount <<
			", PASS: "<< m_passCount << 
			", FAIL: " << m_failCount<< 
			", TIME: " << m_time.elapsed() << "ms"<<std::endl;
		std::wcout << line << L"  END  " << m_id << L" test " << line << L"\n";
    }

	void setId(const std::wstring& id) { m_id = id; }
	const std::wstring& id()const { return m_id; }

protected:
	void output(const std::wstring& prefix, const std::wstring& msg, int ms)
	{
		std::wcout<< prefix << L": " << msg<<" ["<<ms<<L"ms]"<<std::endl;
	}
	void verify(bool pass, const std::wstring& msg, int ms)
	{
		setPass(pass);
		output(pass ? PASS : FAIL, msg, ms);
	}
	template <typename T>
	void compare(const T& actual, const T& expected,
		const std::wstring& actualMsg, const std::wstring& expectedMsg,int ms)
	{
		bool pass = (actual == expected);
		setPass(pass);
		output(pass ? PASS : FAIL, 
			actualMsg + std::wstring(pass?L" == ":L" != ") + expectedMsg,
			ms);
	}
	void compare(const double actual, const int expected,
		const std::wstring& actualMsg, const std::wstring& expectedMsg, int ms)
	{
		compare(actual, (double)expected, actualMsg, expectedMsg, ms);
	}
	void setPass(bool pass) { pass ? ++m_passCount : ++m_failCount; }

protected:
	class Time {
	public:
		Time() { start(); }
		void setHMS(int h, int m, int s, int ms = 0)
		{
			m_mds = (h*SECS_PER_HOUR + m*SECS_PER_MIN + s) * 1000 + ms;
		}
		void start() 
		{ 
#if defined(X_OS_WIN32)
			SYSTEMTIME st;
			memset(&st, 0, sizeof(SYSTEMTIME));
			GetLocalTime(&st);
			setHMS(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
			assert(false);
#endif
		}
		int restart() {
			Time t = Time::currentTime();
			int n = msecsTo(t);
			if (n < 0)
				n += MSECS_PER_DAY;
			*this = t;
			return n;
		}
		int elapsed()const {
			int n = msecsTo(Time::currentTime());
			if (n < 0)
				n += MSECS_PER_DAY;
			return n;
		}
		int msecsTo(const Time& t)const {return t.m_mds - m_mds;}
		static Time currentTime() 
		{
			Time t;
			t.start();
			return t;
		}
	private:
		enum {
			SECS_PER_DAY = 86400,
			MSECS_PER_DAY = 86400000,
			SECS_PER_HOUR = 3600,
			MSECS_PER_HOUR = 3600000,
			SECS_PER_MIN = 60,
			MSECS_PER_MIN = 60000,
			TIME_T_MAX = 2145916799,  // int maximum 2037-12-31T23:59:59 UTC
			JULIAN_DAY_FOR_EPOCH = 2440588 // result of julianDayFromDate(1970, 1, 1)
		};
		int m_mds;
	};

private:
	std::wstring m_id;
	Time m_time;
	int m_passCount;
	int m_failCount;
	const std::wstring PASS = L"PASS";
	const std::wstring FAIL = L"FAIL"; 

protected:
	Time m_tmpTime;
};

#define X_RUN_TEST(Cls) {		\
	Cls##Test test;				\
	if(test.id().empty())		\
		test.setId(L#Cls);	\
	test.run();				\
}

#define X_OUTPUT(op)	m_tmpTime.start(); op;output(L"    ",L#op, m_tmpTime.elapsed());
#define X_VERIFY(cond)	{Time t;verify(cond,L#cond,t.elapsed());}
#define X_VERIFY2(cond,msg)	{Time t;verify(cond,msg,t.elapsed());}
#define X_COMPARE(actual, expected)	{Time t;compare(actual,expected,L#actual,L#expected,t.elapsed());}	

#endif //_X_UNIT_TEST_HPP
