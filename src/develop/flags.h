#if DEVELOP
#define TEST_DLL false
#define CONNECT_INTERNET true
#define MORE_DETAILS true
#define SLEEP_CRAWL false
#define tempDataPath "tempData"
#define tempDataName tempDataPath
#elif
#define CONNECT_INTERNET true
#define SLEEP_CRAWL true
#endif

#if CONNECT_INTERNET
#else
#define BILIBILI_DATA ".\\testing\\DataFromBilibili.json"
#endif
