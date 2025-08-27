#ifdef API_DLL
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif

#define Nullable /*This can be null*/
#define NotNull /*This can't be null*/
